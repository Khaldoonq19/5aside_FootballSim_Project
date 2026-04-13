#include "Match.h"
#include <algorithm>

Match::Match(const Team& home, const Team& away)
    : m_home(home), m_away(away) {
}

std::shared_ptr<const Goalkeeper> Match::findGoalkeeper(const std::vector<std::shared_ptr<const Player>>& five) const {
    for (const std::shared_ptr<const Player>& player : five) {
        std::shared_ptr<const Goalkeeper> gk = std::dynamic_pointer_cast<const Goalkeeper>(player); // RTTI
        if (gk) {
            return gk;
        }
    }
    return nullptr;
}

std::shared_ptr<const OutfieldPlayer> Match::pickRandomScorer(
    const std::vector<std::shared_ptr<const Player>>& five, std::mt19937& rng) const {
    std::vector<std::shared_ptr<const OutfieldPlayer>> outs;
    for (const std::shared_ptr<const Player>& player : five) {
        std::shared_ptr<const OutfieldPlayer> outfieldPlayer =
            std::dynamic_pointer_cast<const OutfieldPlayer>(player); // RTTI
        if (outfieldPlayer) outs.push_back(outfieldPlayer);
    }
    if (outs.empty()) return nullptr;

    std::uniform_int_distribution<int> d(0, (int)outs.size() - 1);
    return outs[d(rng)];
}

int Match::sampleGoals(int attack, int oppDefense, bool homeAdv,
    std::mt19937& rng, const std::function<double(double)>& lambdaAdjust) {
    double base = 1.0 + (attack - oppDefense) * 0.04;
    if (homeAdv) base += 0.25;

    base = std::max(0.2, std::min(4.5, base));
    base = lambdaAdjust(base); // style adjustment via std::function
    base = std::max(0.1, std::min(6.0, base));

    std::poisson_distribution<int> pois(base);
    int g = pois(rng);

    g = std::max(0, std::min(12, g));
    return g;
}

Score Match::play(std::mt19937& rng, std::ostream& log,
    const std::function<double(double)>& lambdaAdjust) {
    Score s;

    const int hAtk = m_home.attackStrength();
    const int hDef = m_home.defenseStrength();
    const int aAtk = m_away.attackStrength();
    const int aDef = m_away.defenseStrength();

    std::vector<std::shared_ptr<const Player>> hFive = m_home.selectFive();
    std::vector<std::shared_ptr<const Player>> aFive = m_away.selectFive();

    std::shared_ptr<const Goalkeeper> homeGK = findGoalkeeper(hFive);
    std::shared_ptr<const Goalkeeper> awayGK = findGoalkeeper(aFive);

    int hDefAdj = hDef + (homeGK ? 6 : -3);
    int aDefAdj = aDef + (awayGK ? 6 : -3);

    log << "MATCH: " << m_home.name() << " vs " << m_away.name() << "\n";
    log << "  Home (Atk " << hAtk << ", Def " << hDefAdj << (homeGK ? ", GK YES" : ", GK NO") << ")\n";
    log << "  Away (Atk " << aAtk << ", Def " << aDefAdj << (awayGK ? ", GK YES" : ", GK NO") << ")\n";

    int homeRaw = sampleGoals(hAtk, aDefAdj, true, rng, lambdaAdjust);
    int awayRaw = sampleGoals(aAtk, hDefAdj, false, rng, lambdaAdjust);

    // NEW: Clear polymorphic behaviour output (GK saves depend on GK reflex)
    std::function<int(const std::shared_ptr<const Goalkeeper>&, int)> computeSaves =
        [&](const std::shared_ptr<const Goalkeeper>& gk, int rawGoals) -> int {
        if (!gk || rawGoals <= 0) return 0;
        int maxSaves = std::min(3, rawGoals);

        // reflex 55..95 -> roughly 0.20..0.70 save prob
        double p = (gk->reflex() - 45) / 100.0;
        p = std::max(0.10, std::min(0.80, p));

        std::binomial_distribution<int> bin(maxSaves, p);
        return bin(rng);
        };

    int awaySaves = computeSaves(awayGK, homeRaw);
    int homeSaves = computeSaves(homeGK, awayRaw);

    s.home = std::max(0, homeRaw - awaySaves);
    s.away = std::max(0, awayRaw - homeSaves);

    if (awayGK && awaySaves > 0) {
        log << "  " << awayGK->name() << " (GK) makes " << awaySaves << " big save(s)!\n";
    }
    if (homeGK && homeSaves > 0) {
        log << "  " << homeGK->name() << " (GK) makes " << homeSaves << " big save(s)!\n";
    }

    // NEW: scorers commentary (outfield finishing)
    if (s.home > 0) {
        log << "  Goals for " << m_home.name() << ":\n";
        for (int k = 0; k < s.home; ++k) {
            std::shared_ptr<const OutfieldPlayer> scorer = pickRandomScorer(hFive, rng);
            if (scorer) {
                log << "    - " << scorer->name() << " finishes clinically!\n";
            }
            else {
                log << "    - A scramble in the box... it goes in!\n";
            }
        }
    }
    if (s.away > 0) {
        log << "  Goals for " << m_away.name() << ":\n";
        for (int k = 0; k < s.away; ++k) {
            std::shared_ptr<const OutfieldPlayer> scorer = pickRandomScorer(aFive, rng);
            if (scorer) {
                log << "    - " << scorer->name() << " rifles it home!\n";
            }
            else {
                log << "    - A deflection wrong-foots everyone!\n";
            }
        }
    }

    // Small extra randomness: last-minute drama
    std::bernoulli_distribution drama(0.10);
    if (drama(rng)) {
        std::bernoulli_distribution who(0.5);
        if (who(rng)) s.home += 1; else s.away += 1;
        log << "  Drama! Late goal swings the score.\n";
    }

    log << "  FINAL: " << m_home.name() << " " << s.home
        << " - " << s.away << " " << m_away.name() << "\n";

    return s;
}

