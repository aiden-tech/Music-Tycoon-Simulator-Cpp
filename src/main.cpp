#include "../headers/album.h"
#include "../headers/config.h"
#include "../headers/graphics.h"
#include "../headers/player.h"
#include "../headers/simulation.h"
#include "../headers/song.h"

#include "imgui-SFML.h"
#include <SFML/Graphics.hpp>
#include <SFML/System/Time.hpp>

#include <optional>
#include <string>
#include <sys/stat.h>
#include <vector>

// --- MAIN ---
int main() {
  // SFML 3.0 Window Creation
  sf::RenderWindow window(sf::VideoMode({1280, 800}), "Music Tycoon 2025");
  window.setFramerateLimit(60);

  if (!ImGui::SFML::Init(window))
    return -1;

  Player player("The Architect");
  std::vector<Song> songsMade;
  std::vector<Song> songsReleased;
  std::vector<Album> albumsReleased; // Correctly initialized

  sf::Clock deltaClock;
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

    // --- Logic ---
    SimulateEconomy(songsReleased, albumsReleased, player, dt);

    // Clean up old songs
    std::erase_if(songsReleased, [](const Song &s) {
      // Convert sf::Time to float seconds
      return s.lifeTime >= EconomyConfig::SONG_LIFETIME.asSeconds();
    });

    // Clean up old albums
    std::erase_if(albumsReleased, [](const Album &a) {
      // You already have ALBUM_LIFETIME defined in your config! Use it:
      return a.lifeTime >= EconomyConfig::ALBUM_LIFETIME.asSeconds();

      // OR if you want the "Double song lifetime" logic:
      // return a.lifeTime >= (EconomyConfig::SONG_LIFETIME.asSeconds() * 2.0f);
    });

    // --- UI ---

    // FIX: Added 'albumsReleased' as the 4th argument
    DrawStudioWindow(player, songsMade, songsReleased, albumsReleased);

    // FIX: You likely want to pass albums to Analytics to see their
    // performance If your DrawAnalyticsWindow doesn't support this yet,
    // you'll need to update its parameters too
    DrawAnalyticsWindow(songsReleased, albumsReleased);

    DrawUpgradeWindow("Skills", player, player.skills);
    DrawUpgradeWindow("Studio Gear", player, player.studio_tools);
    gameLog.DrawWindow();

    window.clear(sf::Color(25, 25, 30));
    ImGui::SFML::Render(window);
    window.display();
  }

  ImGui::SFML::Shutdown();
  return 0;
}