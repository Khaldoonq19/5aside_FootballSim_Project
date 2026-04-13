#include "League.h"
#include "Types.h"
#include <algorithm>
#include <array>
#include <iomanip>

League::League(std::string name)
    : m_name(std::move(name)) {
    setSimStyle(SimStyle::Normal);
}

void League::setSimStyle(SimStyle style) {
    m_style = style;

    // std::function used to adjust the Poisson lambda in Match simulation
    switch (m_style) {
    case SimStyle::Defensive:
        m_goalLambdaAdjust = [](double lambda) { return lambda * 0.78; };
        break;
    case SimStyle::Chaotic:
        m_goalLambdaAdjust = [](double lambda) { return lambda * 1.28; };
        break;
    case SimStyle::Normal:
    default:
        m_goalLambdaAdjust = [](double lambda) { return lambda; };
        break;
    }
}

static int rInt(std::mt19937& rng, int lo, int hi) {
    std::uniform_int_distribution<int> d(lo, hi);
    return d(rng);
}

static std::string randomSurname(std::mt19937& rng) {
    static const std::array<std::string, 20> names = {
        "Khan","Smith","Jones","Brown","Ali","Taylor","Wilson","Singh","Walker","Evans",
        "Johnson","White","Hall","Martin","Clarke","Lewis","Young","King","Wright","Scott"
    };
    return names[rInt(rng, 0, (int)names.size() - 1)];
}

static std::string randomFirst(std::mt19937& rng) {
    static const std::array<std::string, 20> first = {
        "Alex","Sam","Jamie","Chris","Morgan","Riley","Casey","Jordan","Taylor","Avery",
        "Noah","Leo","Mason","Owen","Ethan","Mia","Zoe","Ivy","Luca","Kai"
    };
    return first[rInt(rng, 0, (int)first.size() - 1)];
}

void League::generateDefault10Teams(std::mt19937& rng) {
    m_teams.clear();
    m_table.clear();

    static const std::array<std::string, 10> teamNames = {
        "Arsenal",
        "Chelsea",
        "Liverpool",
        "Man City",
        "Man UTD",
        "Spurs",
        "Barcelona",
        "Real Madrid",
        "Bayern Munich",
        "Juventus"
    };

    for (int i = 0; i < 10; ++i) {
        std::shared_ptr<Team> team = std::make_shared<Team>(teamNames[i]);

        // Squad (8 players): 1-2 GK + outfield
        int gkCount = rInt(rng, 1, 2);
        for (int g = 0; g < gkCount; ++g) {
            std::string nm = randomFirst(rng) + " " + randomSurname(rng);
            int rating = rInt(rng, 55, 90);
            int reflex = rInt(rng, 55, 95);
            team->addPlayer(std::make_shared<Goalkeeper>(nm, rating, reflex));
        }

        for (int p = 0; p < 8 - gkCount; ++p) {
            std::string nm = randomFirst(rng) + " " + randomSurname(rng);
            int rating = rInt(rng, 50, 92);
            int fin = rInt(rng, 45, 95);
            int tack = rInt(rng, 45, 95);
            team->addPlayer(std::make_shared<OutfieldPlayer>(nm, rating, fin, tack));
        }

        m_teams.push_back(team);
    }

    initTable();
}

void League::initTable() {
    m_table.clear();
    m_table.reserve(m_teams.size());
    for (const std::shared_ptr<Team>& team : m_teams) {
        TableRow r;
        r.team = team->name();
        m_table.push_back(r);
    }
}

TableRow* League::findRow(const std::string& teamName) {
    for (TableRow& r : m_table) {
        if (r.team == teamName) return &r;
    }
    return nullptr;
}

void League::applyResult(const Team& home, const Team& away, const Score& s) {
    TableRow* h = findRow(home.name());
    TableRow* a = findRow(away.name());
    if (!h || !a) return;

    h->played++; a->played++;
    h->gf += s.home; h->ga += s.away;
    a->gf += s.away; a->ga += s.home;
    h->gd = h->gf - h->ga;
    a->gd = a->gf - a->ga;

    if (s.home > s.away) { h->won++; a->lost++; h->pts += 3; }
    else if (s.home < s.away) { a->won++; h->lost++; a->pts += 3; }
    else { h->drawn++; a->drawn++; h->pts += 1; a->pts += 1; }
}

void League::simulateRandomMatch(std::mt19937& rng, std::ostream& log) {
    if (m_teams.size() < 2) return;

    std::uniform_int_distribution<int> d(0, (int)m_teams.size() - 1);
    int i = d(rng);
    int j = d(rng);
    while (j == i) j = d(rng);

    Match m(*m_teams[i], *m_teams[j]);
    Score s = m.play(rng, log, m_goalLambdaAdjust);
    applyResult(*m_teams[i], *m_teams[j], s);
    log << "\n";
}

void League::simulateSeason(std::mt19937& rng, std::ostream& log) {
    if (m_teams.size() < 2) return;

    initTable();

    log << "=== Simulating season: " << m_name << " (10 teams, double round-robin) ===\n";
    log << "Style: " << (m_style == SimStyle::Normal ? "Normal" :
        m_style == SimStyle::Defensive ? "Defensive" : "Chaotic")
        << "\n\n";

    for (size_t i = 0; i < m_teams.size(); ++i) {
        for (size_t j = i + 1; j < m_teams.size(); ++j) {
            {
                Match m(*m_teams[i], *m_teams[j]);
                Score s = m.play(rng, log, m_goalLambdaAdjust);
                applyResult(*m_teams[i], *m_teams[j], s);
                log << "\n";
            }
            {
                Match m(*m_teams[j], *m_teams[i]);
                Score s = m.play(rng, log, m_goalLambdaAdjust);
                applyResult(*m_teams[j], *m_teams[i], s);
                log << "\n";
            }
        }
    }

    log << "=== Season complete ===\n\n";
}

void League::printTeams(std::ostream& os) const {
    os << "Teams in " << m_name << ":\n";
    for (size_t i = 0; i < m_teams.size(); ++i) {
        os << "  [" << i << "] " << m_teams[i]->name() << "\n";
    }
}

void League::printTable(std::ostream& os) const {
    std::vector<TableRow> rows = m_table;

    // Uses operator< we overloaded in League.h
    std::sort(rows.begin(), rows.end());
    std::reverse(rows.begin(), rows.end());

    os << "TABLE: " << m_name << "\n";
    os << padRight("Team", 22)
        << " P  W  D  L  GF GA GD  Pts\n";
    os << "-----------------------------------------------\n";

    for (const TableRow& r : rows) {
        os << padRight(r.team, 22)
            << std::setw(2) << r.played << " "
            << std::setw(2) << r.won << " "
            << std::setw(2) << r.drawn << " "
            << std::setw(2) << r.lost << " "
            << std::setw(2) << r.gf << " "
            << std::setw(2) << r.ga << " "
            << std::setw(3) << r.gd << " "
            << std::setw(3) << r.pts << "\n";
    }
    os << "\n";
}

void League::addCustomPlayerToTeam(size_t teamIndex, std::shared_ptr<Player> p) {
    if (teamIndex >= m_teams.size() || !p) return;
    m_teams[teamIndex]->addPlayer(std::move(p));
}

