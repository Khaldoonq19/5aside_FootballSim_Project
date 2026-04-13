#include "Team.h"
#include <algorithm>

Team::Team(std::string name) : m_name(std::move(name)) {}

void Team::addPlayer(std::shared_ptr<Player> p) {
    if (!p) return;
    m_players.push_back(std::move(p));
}

std::vector<std::shared_ptr<const Player>> Team::selectFive() const {
    std::vector<std::shared_ptr<const Player>> chosen;
    chosen.reserve(5);

    // Prefer a GK if any
    for (const std::shared_ptr<Player>& player : m_players) {
        if (player && player->position() == Position::Goalkeeper) {
            chosen.push_back(player);
            break;
        }
    }

    // Fill remaining by highest rating
    std::vector<std::shared_ptr<const Player>> outfield;
    outfield.reserve(m_players.size());

    for (const std::shared_ptr<Player>& player : m_players) {
        if (!player) continue;
        if (chosen.size() == 1 && player == chosen[0]) continue;
        outfield.push_back(player);
    }

    std::sort(outfield.begin(), outfield.end(),
        [](const std::shared_ptr<const Player>& a, const std::shared_ptr<const Player>& b) {
            return a->rating() > b->rating();
        });

    for (const std::shared_ptr<const Player>& player : outfield) {
        if (chosen.size() >= 5) break;
        chosen.push_back(player);
    }

    return chosen;
}

int Team::attackStrength() const {
    std::vector<std::shared_ptr<const Player>> five = selectFive();
    int sum = 0;
    for (const std::shared_ptr<const Player>& player : five) sum += player->attackImpact();
    return sum;
}

int Team::defenseStrength() const {
    std::vector<std::shared_ptr<const Player>> five = selectFive();
    int sum = 0;
    for (const std::shared_ptr<const Player>& player : five) sum += player->defenseImpact();
    return sum;
}

void Team::printRoster(std::ostream& os) const {
    os << "Team: " << m_name << "\n";
    for (const std::shared_ptr<Player>& player : m_players) {
        os << "  - " << *player << "\n";
    }
}

std::ostream& operator<<(std::ostream& os, const Team& t) {
    os << t.name();
    return os;
}
