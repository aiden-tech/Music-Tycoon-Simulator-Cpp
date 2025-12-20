#include "../headers/album.h"
#include "../headers/config.h"
#include "../headers/graphics.h"
#include "../headers/player.h"
#include "../headers/simulation.h"
#include "../headers/song.h"

#include "imgui-SFML.h"

#include <SFML/Graphics.hpp>
#include <SFML/System/Time.hpp>

#include <memory>
#include <optional>
#include <string>
#include <vector>

int main() {
  // SFML 3.0 Window Creation
  sf::RenderWindow window(sf::VideoMode({1800, 800}), "Music Tycoon 2025");
  window.setFramerateLimit(60);

  if (!ImGui::SFML::Init(window))
    return -1;

  // Game Variables
  GameState currentState = GameState::MainMenu;

  // 2. CREATE TIMER (Before the loop)
  auto globalTickTimer = std::make_shared<float>(0.0f);

  Player player("The Architect");
  std::vector<Song> songsMade;
  std::vector<Song> songsReleased;
  std::vector<Album> albumsReleased;

  sf::Clock deltaClock;

  // 3. LOG INITIALIZATION
  gameLog.Add("Engine Initialized.");

  while (window.isOpen()) {
    // SFML 3.0 Event Polling
    while (const std::optional event = window.pollEvent()) {
      ImGui::SFML::ProcessEvent(window, *event);
      if (event->is<sf::Event::Closed>()) {
        window.close();
      }
    }

    sf::Time dt = deltaClock.restart();
    ImGui::SFML::Update(window, dt);

    // --- GAME STATE MACHINE ---
    switch (currentState) {
    case GameState::MainMenu: {
      DrawMainMenu(currentState, player);
      break;
    }

    case GameState::Playing: {
      // --- Logic ---
      // 4. CALL SIMULATION (Pass global timer)
      SimulateEconomy(songsReleased, albumsReleased, player, dt,
                      globalTickTimer);

      bool updated = UpdateReputation(player, songsReleased, albumsReleased,
                                      globalTickTimer);

      // Clean up old songs (C++20 erase_if)
      std::erase_if(songsReleased, [](const Song &s) {
        return s.lifeTime >= EconomyConfig::SONG_LIFETIME.asSeconds();
      });

      // Clean up old albums
      std::erase_if(albumsReleased, [](const Album &a) {
        return a.lifeTime >= EconomyConfig::ALBUM_LIFETIME.asSeconds();
      });

      // --- Drawing UI ---
      DrawStudioWindow(player, songsMade, songsReleased, albumsReleased);
      DrawAnalyticsWindow(songsReleased, albumsReleased);

      DrawUpgradeWindow("Skills", player, player.skills);
      DrawUpgradeWindow("Studio Gear", player, player.studio_tools);
      DrawActionsWindow(player);

      // 5. DRAW LOG WINDOW (Fixed typo: gamelog -> gameLog)
      gameLog.DrawWindow(dt, globalTickTimer);

      break;
    }
    }

    // Render everything
    window.clear(sf::Color(25, 25, 30));
    ImGui::SFML::Render(window);
    window.display();
  }

  ImGui::SFML::Shutdown();
  return 0;
}