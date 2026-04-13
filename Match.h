#pragma once
#include <random>
#include <string>
#include <iostream>
#include <functional>
#include "Team.h"
#include "Types.h"

class Match {
public:
    Match(const Team& home, const Team& away);

    // NEW: pass a std::function to adjust goal expectancy (simulation style)
    Score play(std::mt19937& rng, std::ostream& log,
        const std::function<double(double)>& lambdaAdjust);

    const Team& home() const noexcept { return m_home; }
    const Team& away() const noexcept { return m_away; }

private:
    const Team& m_home;
    const Team& m_away;

    int sampleGoals(int attack, int oppDefense, bool homeAdv,
        std::mt19937& rng, const std::function<double(double)>& lambdaAdjust);

    // NEW: polymorphic commentary helpers
    const Goalkeeper* findGoalkeeper(const std::vector<const Player*>& five) const;
    const OutfieldPlayer* pickRandomScorer(const std::vector<const Player*>& five, std::mt19937& rng) const;
};

