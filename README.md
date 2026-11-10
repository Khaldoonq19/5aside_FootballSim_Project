# 5-a-Side Football League Simulator

An individual second-year C++ university project by Khaldoon Qasim at Sheffield Hallam University. It models players, teams, matches and a ten-team league in a console application.

## Try it in one minute

Requires a C++17-capable compiler. Build from the repository root:

```bash
c++ -std=c++17 -Wall -Wextra -pedantic main.cpp Player.cpp Team.cpp Match.cpp League.cpp Types.cpp -o football_sim
./football_sim
```

Choose `1` to view teams, `5` to simulate a match, `2` to inspect the table and `0` to exit. A [captured terminal demonstration](docs/demo-session.txt) shows those steps. Results vary because the program uses a time-based random seed.

The C++17 build and this scripted demonstration were checked on 10 November 2026. This was a smoke check, not a complete automated test suite.

## Features and code to explore

- [Player hierarchy](Player.h): abstract Player class with Goalkeeper and OutfieldPlayer implementations.
- [Team model](Team.h): groups players and exposes team-level behaviour.
- [Match simulation](Match.cpp): rating-based, probabilistic outcomes.
- [League management](League.cpp): default teams, match results and league standings.
- [Interactive menu](main.cpp): input handling, player creation, match/season simulation and log export.
- Simulation-style options: Normal, Defensive and Chaotic.

## Design choices

The domain is split into Player, Team, Match and League classes so that input handling is separate from the main football models. Virtual methods let goalkeeper and outfield players contribute differently to the simulation. Standard-library random distributions produce varying results; league rows track results and standings.

## Current limitations and next improvements

The season menu currently asks for games-per-team, home-advantage and playoff values, but these are printed to the log rather than passed into simulateSeason. The season implementation uses a fixed ten-team double round-robin. These inputs should be wired into a configuration object or removed to avoid implying they change the simulation.

A configurable random seed and automated checks for table consistency would make results easier to reproduce and validate. The simulator is an educational model, not a predictor of real football results.

## Academic context

Developed for second-year Advanced Programming. The repository demonstrates object-oriented C++ and a complete interactive program; grade evidence is separate from the source code.
