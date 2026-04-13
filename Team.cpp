#include "Team.h"
#include <algorithm>

Team::Team(std::string name) : m_name(std::move(name)) {}

void Team::addPlayer(std::unique_ptr<Player> p) {
    if (!p) return;
    m_players.push_back(std::move(p));
}

std::vector<const Player*> Team::selectFive() const {
    std::vector<const Player*> chosen;
    chosen.reserve(5);

    // Prefer a GK if any
    for (const auto& up : m_players) {
        if (up && up->position() == Position::Goalkeeper) {
            chosen.push_back(up.get());
            break;
        }
    }

    // Fill remaining by highest rating
    std::vector<const Player*> outfield;
    outfield.reserve(m_players.size());

    for (const auto& up : m_players) {
        if (!up) continue;
        if (chosen.size() == 1 && up.get() == chosen[0]) continue;
        outfield.push_back(up.get());
    }

    std::sort(outfield.begin(), outfield.end(),
        [](const Player* a, const Player* b) { return a->rating() > b->rating(); });

    for (const Player* p : outfield) {
        if (chosen.size() >= 5) break;
        chosen.push_back(p);
    }

    return chosen;
}

int Team::attackStrength() const {
    auto five = selectFive();
    int sum = 0;
    for (auto* p : five) sum += p->attackImpact();
    return sum;
}

int Team::defenseStrength() const {
    auto five = selectFive();
    int sum = 0;
    for (auto* p : five) sum += p->defenseImpact();
    return sum;
}

void Team::printRoster(std::ostream& os) const {
    os << "Team: " << m_name << "\n";
    for (const auto& p : m_players) {
        os << "  - " << *p << "\n";
    }
}

std::ostream& operator<<(std::ostream& os, const Team& t) {
    os << t.name();
    return os;
}
