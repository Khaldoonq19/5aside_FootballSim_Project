#include <iostream>
#include <fstream>
#include <limits>
#include <random>
#include <string>
#include <chrono>
#include <sstream>

#include "League.h"
#include "Player.h"

static void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

static bool readYesNo(const std::string& prompt)
{
    while (true)
    {
        std::cout << prompt << " (y/n): ";
        std::string s;
        if (!(std::cin >> s)) return false;

        if (s == "y" || s == "Y") return true;
        if (s == "n" || s == "N") return false;

        std::cout << "Please type y or n.\n";
    }
}

static int askInt(const std::string& prompt, int lo, int hi) {
    while (true) {
        std::cout << prompt;
        int v;
        if (std::cin >> v) {
            clearInput();
            if (v >= lo && v <= hi) return v;
        }
        else {
            clearInput();
        }
        std::cout << "Invalid input. Enter a number in [" << lo << ", " << hi << "].\n";
    }
}

static std::string askLine(const std::string& prompt) {
    std::cout << prompt;
    std::string s;
    std::getline(std::cin, s);
    while (s.empty()) {
        std::cout << "Please enter something.\n" << prompt;
        std::getline(std::cin, s);
    }
    return s;
}

static std::shared_ptr<Player> createPlayerInteractive() {
    std::cout << "\n=== Create Player ===\n";
    std::string name = askLine("Player name: ");

    int pos = askInt("Position (1=Goalkeeper, 2=Outfield): ", 1, 2);
    int rating = askInt("Overall rating (1..100): ", 1, 100);

    if (pos == 1) {
        int reflex = askInt("GK reflex (1..100): ", 1, 100);
        return std::make_shared<Goalkeeper>(name, rating, reflex);
    }
    else {
        int fin = askInt("Finishing (1..100): ", 1, 100);
        int tack = askInt("Tackling (1..100): ", 1, 100);
        return std::make_shared<OutfieldPlayer>(name, rating, fin, tack);
    }
}

static const char* styleName(SimStyle s) {
    switch (s) {
    case SimStyle::Defensive: return "Defensive";
    case SimStyle::Chaotic:   return "Chaotic";
    case SimStyle::Normal:
    default: return "Normal";
    }
}

static void printMenu(SimStyle currentStyle) {
    std::cout <<
        "==============================\n"
        "  5-A-SIDE LEAGUE SIMULATOR\n"
        "==============================\n"
        "Current match style: " << styleName(currentStyle) << "\n"
        "1) Show teams\n"
        "2) Show league table\n"
        "3) Show a team's roster\n"
        "4) Create a player and add to a team\n"
        "5) Simulate ONE random match (two random teams)\n"
        "6) Simulate FULL season (double round-robin)\n"
        "7) Save last output log to file\n"
        "8) Set simulation style (Normal/Defensive/Chaotic)\n"
        "0) Exit\n";
}

int main() {
    // Internal time-based seed (not printed)
    unsigned int now = static_cast<unsigned int>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count()
        );
    std::mt19937 rng(now);

    League league("European 5s League");
    league.generateDefault10Teams(rng);

    std::string lastLog;

    std::cout << "Welcome to the 5-a-side simulator!\n\n";

    while (true) {
        printMenu(league.simStyle());
        int choice = askInt("Choose an option: ", 0, 8);

        if (choice == 0) {
            std::cout << "Goodbye.\n";
            break;
        }

        if (choice == 1) {
            league.printTeams(std::cout);
        }
        else if (choice == 2) {
            league.printTable(std::cout);
        }
        else if (choice == 3) {
            league.printTeams(std::cout);
            int idx = askInt("Enter team index to view roster: ", 0, (int)league.teamCount() - 1);
            std::cout << "\n";
            league.getTeam((size_t)idx).printRoster(std::cout);
            std::cout << "\n";
        }
        else if (choice == 4) {
            league.printTeams(std::cout);
            int idx = askInt("Add player to team index: ", 0, (int)league.teamCount() - 1);

            std::shared_ptr<Player> p = createPlayerInteractive();
            std::cout << "Created: " << *p << "\n";
            league.addCustomPlayerToTeam((size_t)idx, std::move(p));
            std::cout << "Player added.\n\n";
        }
        else if (choice == 5) {
            std::ostringstream oss;
            oss << "=== Random Match ===\n";
            league.simulateRandomMatch(rng, oss);
            lastLog = oss.str();
            std::cout << lastLog;
        }
        else if (choice == 6) {
            std::cout << "\n=== Simulate Season ===\n";

            // Ask for 3 user inputs
            int gamesPerTeam = askInt("Enter games each team plays (2-38): ", 2, 38);
            int homeAdvantage = askInt("Home advantage % (0-100): ", 0, 100);
            bool includePlayoffs = readYesNo("Include playoffs after season?");

            std::ostringstream oss;
            oss << "=== Simulating Season ===\n";
            oss << "Games per team: " << gamesPerTeam << "\n";
            oss << "Home advantage: " << homeAdvantage << "%\n";
            oss << "Playoffs: " << (includePlayoffs ? "Yes" : "No") << "\n\n";

            league.simulateSeason(rng, oss);
            oss << "\n";
            lastLog = oss.str();

            std::cout << lastLog;
            league.printTable(std::cout);
        }
        else if (choice == 7) {
            if (lastLog.empty()) {
                std::cout << "No log available yet. Simulate a match/season first.\n\n";
                continue;
            }
            const std::string filename = "simulation_log.txt";
            std::ofstream out(filename, std::ios::out | std::ios::trunc);
            if (!out) {
                std::cout << "Failed to write " << filename << "\n\n";
                continue;
            }
            out << lastLog;
            std::cout << "Saved log to " << filename << "\n\n";
        }
        else if (choice == 8) {
            std::cout << "\n=== Set Simulation Style ===\n";
            std::cout << "1) Normal (balanced)\n";
            std::cout << "2) Defensive (fewer goals)\n";
            std::cout << "3) Chaotic (more goals)\n";
            int s = askInt("Choose style: ", 1, 3);

            league.setSimStyle(static_cast<SimStyle>(s));
            std::cout << "Style set to: " << styleName(league.simStyle()) << "\n\n";
        }
    }

    return 0;
}

