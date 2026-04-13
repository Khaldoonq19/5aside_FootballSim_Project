#pragma once
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "Player.h"

class Team {
public:
    explicit Team(std::string name);

    const std::string& name() const noexcept { return m_name; }

    // Owns players with shared ownership so players are passed around as smart pointers.
    void addPlayer(std::shared_ptr<Player> p);

    // 5-a-side: we will pick a squad of 5 (prefers 1 GK if present)
    std::vector<std::shared_ptr<const Player>> selectFive() const;

    int attackStrength() const;
    int defenseStrength() const;

    void printRoster(std::ostream& os) const;

private:
    std::string m_name;
    std::vector<std::shared_ptr<Player>> m_players;
};

std::ostream& operator<<(std::ostream& os, const Team& t);
