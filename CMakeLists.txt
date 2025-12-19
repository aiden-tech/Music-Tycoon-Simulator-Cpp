cmake_minimum_required(VERSION 3.28)
project(MusicTycoon LANGUAGES CXX)

# Use C++20 consistently
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Export compilation database for clangd / tooling
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)

# 1. Fetch SFML 3.0.2
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)
FetchContent_MakeAvailable(SFML)

# 2. Fetch Dear ImGui
# We need the source code for ImGui so ImGui-SFML can compile it.
FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.91.5 # 'docking' is recommended, or use 'v1.91.5'
    GIT_SHALLOW    ON
)
FetchContent_MakeAvailable(imgui)

# 3. Fetch ImGui-SFML
# IMPORTANT: Tell ImGui-SFML where the ImGui source is.
set(IMGUI_DIR ${imgui_SOURCE_DIR})
# IMPORTANT: Tell ImGui-SFML to use the SFML we just fetched, not look for a system install.
set(IMGUI_SFML_FIND_SFML OFF)

FetchContent_Declare(imgui-sfml
    GIT_REPOSITORY https://github.com/SFML/imgui-sfml.git
    # IMPORTANT: Must use v3.0 or higher to work with SFML 3.0
    GIT_TAG        v3.0
    GIT_SHALLOW    ON
)
FetchContent_MakeAvailable(imgui-sfml)

# Add all your new .cpp files here
add_executable(MusicTycoonApp
    src/main.cpp
    src/song.cpp
    src/album.cpp
    src/graphics.cpp
    src/simulation.cpp
    src/helper.cpp
    src/player.cpp

)
target_compile_features(MusicTycoonApp PRIVATE cxx_std_20)

# 4. Link Libraries
# ImGui-SFML::ImGui-SFML automatically links SFML::Graphics and SFML::Window for you.
target_link_libraries(MusicTycoonApp PRIVATE ImGui-SFML::ImGui-SFML SFML::Graphics SFML::Window SFML::System)