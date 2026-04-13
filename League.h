#pragma once
#include <vector>
#include <string>
#include <random>
#include <iostream>
#include <functional>
#include <memory>
#include "Team.h"
#include "Match.h"

struct TableRow {
    std::string team;
    int played{ 0 }, won{ 0 }, drawn{ 0 }, lost{ 0 };
    int gf{ 0 }, ga{ 0 }, gd{ 0 };
    int pts{ 0 };
};

// EXTRA OPERATOR OVERLOADING (beyond <<):
// Sort table rows by points, then GD, then GF
inline bool operator<(const TableRow& a, const TableRow& b) {
    if (a.pts != b.pts) return a.pts < b.pts;
    if (a.gd != b.gd)  return a.gd < b.gd;
    return a.gf < b.gf;
}

enum class SimStyle {
    Normal = 1,
    Defensive = 2,
    Chaotic = 3
};

class League {
public:
    explicit League(std::string name);

    void generateDefault10Teams(std::mt19937& rng);

    size_t teamCount() const noexcept { return m_teams.size(); }
    const Team& getTeam(size_t i) const { return *m_teams.at(i); }
    Team& getTeam(size_t i) { return *m_teams.at(i); }

    // Simulation style control (uses std::function internally)
    void setSimStyle(SimStyle style);
    SimStyle simStyle() const noexcept { return m_style; }

    void simulateRandomMatch(std::mt19937& rng, std::ostream& log);
    void simulateSeason(std::mt19937& rng, std::ostream& log);

    void printTeams(std::ostream& os) const;
    void printTable(std::ostream& os) const;

    void addCustomPlayerToTeam(size_t teamIndex, std::shared_ptr<Player> p);

private:
    std::string m_name;
    std::vector<std::shared_ptr<Team>> m_teams;
    std::vector<TableRow> m_table;

    // Uses std::function to adjust goal expectancy (advanced aspect)
    SimStyle m_style{ SimStyle::Normal };
    std::function<double(double)> m_goalLambdaAdjust;

    void initTable();
    void applyResult(const Team& home, const Team& away, const Score& s);
    TableRow* findRow(const std::string& teamName);
};



