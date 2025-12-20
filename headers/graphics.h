#pragma once

#include "album.h"
#include "gamestate.h"
#include "player.h"
#include "song.h"

#include <SFML/Graphics.hpp>
#include <SFML/System/Time.hpp>

#include <deque>

struct EventLog {
  std::deque<std::string> logs;

  // Create the timer here so it "lives" as long as the EventLog does
  std::shared_ptr<float> ticktimer = std::make_shared<float>(0.0f);

  void Add(const std::string &message);
  void DrawWindow(sf::Time dt, std::shared_ptr<float> tickTimer);
};

std::string BeginDropDownMenu(bool IsOpen);

void DrawStudioWindow(Player &player, std::vector<Song> &songsMade,
                      std::vector<Song> &songsReleased,
                      std::vector<Album> &albumsReleased);

void DrawActionsWindow(Player &player);

void DrawAnalyticsWindow(const std::vector<Song> &songsReleased,
                         const std::vector<Album> &albumsReleased);

void DrawUpgradeWindow(const char *title, Player &player,
                       std::vector<std::pair<std::string, double>> &items);

void DrawMainMenu(GameState &state, Player &player);

extern EventLog gameLog; // Global instance