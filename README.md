# Music Tycoon Simulator (C++)

A simple 2D music tycoon where you play as an artist who makes songs in a chosen genre and slowly grows over time by upgrading skills and studio tools. The project is written in C++ and uses CMake, SFML, and Dear ImGui (ImGui-SFML).


---

Table of contents
- About
- Key features
- Build requirements
- Build & run
- Project layout & sources
- How to play
- Contributing
- Roadmap
- Contact

About
-----
Music Tycoon Simulator puts you in the role of an artist. Choose a genre, create and release songs, and grow your career by investing in personal skill upgrades and studio tool upgrades. Progression happens over time as you manage production and growth decisions.

Key features (what the repo implements)
-----
- Single-player progression: make songs, release them, and improve your artist.
- Upgrade mechanics for skills and studio tools.
- Desktop application built with SFML for rendering/audio and Dear ImGui for UI (ImGui-SFML integration).

Build requirements
-----
The repository's CMake configuration specifies the following requirements (these are taken directly from the project's CMakeLists.txt):

- CMake minimum required: 3.28
- C++ standard: C++20
- Network access at configure time is required if relying on CMake FetchContent (the project fetches SFML, Dear ImGui, and ImGui-SFML automatically).
- A C++20-capable compiler (examples: recent GCC, Clang, or MSVC).

Notes about dependencies
- The CMake configuration fetches:
  - SFML v3.0.2 from https://github.com/SFML/SFML
  - Dear ImGui v1.91.5 from https://github.com/ocornut/imgui
  - ImGui-SFML v3.0 from https://github.com/SFML/imgui-sfml
- The build links ImGui-SFML::ImGui-SFML and SFML::Graphics / SFML::Window / SFML::System.
- If you prefer to use system-installed SFML/ImGui instead of FetchContent, update the CMakeLists.txt accordingly.

Build & run
-----
The project produces an executable target named `MusicTycoonApp` and places runtime binaries in the CMake binary directory under `bin/` (CMAKE_RUNTIME_OUTPUT_DIRECTORY).

Typical out-of-source CMake workflow:

1. Clone the repository
   git clone https://github.com/aiden-tech/Music-Tycoon-Simulator-Cpp.git
   cd Music-Tycoon-Simulator-Cpp

2. Create and enter a build directory:
   mkdir -p build
   cd build

3. Configure with CMake:
   cmake .. -DCMAKE_BUILD_TYPE=Release

   - This project uses FetchContent to download SFML, ImGui and ImGui-SFML during configure if those libraries are not already available.
   - If your environment cannot download dependencies during configure, install SFML and ImGui system-wide and adjust CMakeLists.txt or your CMake cache accordingly.

4. Build:
   cmake --build . --config Release

5. Run:
   - The produced executable will be at:
     ./bin/MusicTycoonApp   (on Linux / macOS)
     .\bin\MusicTycoonApp.exe (on Windows)
   - If your build tool or OS places binaries elsewhere, run the produced executable accordingly.

Project layout & sources
-----
Sources referenced in the CMake configuration (these filenames are in the repository and used to build the executable):

- src/main.cpp
- src/song.cpp
- src/album.cpp
- src/graphics.cpp
- src/simulation.cpp
- src/helper.cpp
- src/player.cpp

CMakeLists notes:
- The project sets `CMAKE_CXX_STANDARD` to 20 and `CMAKE_EXPORT_COMPILE_COMMANDS ON`.
- Runtime binaries are written to `${CMAKE_BINARY_DIR}/bin`.
- ImGui integration is done via ImGui-SFML and the CMake config ensures ImGui-SFML uses the fetched SFML.

How to play (concise, non-speculative)
-----
- You play as an artist who picks a genre and writes/releases songs.
- You improve over time by upgrading personal skills and studio tools.
- The core loop is: create songs → release/promote → earn progression → spend on upgrades → repeat.

(Controls, UI specifics, save/load behavior, and exact metrics are intentionally not expanded here to avoid inventing details. Document controls or gameplay parameters in-game or by adding a short HOWTO.md if you want those included.)

Contributing
-----
Contributions are welcome. A suggested workflow:

1. Fork the repository.
2. Create a branch: git checkout -b feat/your-feature
3. Implement changes and test them locally.
4. Open a pull request describing what you changed and why.

If you want a CONTRIBUTING.md or specific code style rules, add them to the repository and mention them here.

Roadmap 
-----
Planing too add a shop into the game, add more song genres, improve balancing, better game loop, improve ui, and just add too everything in general

Contact
-----
Created by Aiden (aiden-tech). For questions open an issue in this repository.

---
