#pragma once

#include "album.h"
#include "player.h"
#include "song.h"

#include <SFML/Graphics.hpp>
#include <SFML/System/Time.hpp>

#include <deque>

struct EventLog {
  std::deque<std::string> logs;

  void Add(const std::string &message);

  void DrawWindow();
};

std::string BeginDropDownMenu(bool IsOpen);

void DrawStudioWindow(Player &player, std::vector<Song> &songsMade,
                      std::vector<Song> &songsReleased,
                      std::vector<Album> &albumsReleased);

void DrawAnalyticsWindow(const std::vector<Song> &songsReleased,
                         std::vector<Album> &albumsReleased);

void DrawUpgradeWindow(const char *title, Player &player,
                       std::vector<std::pair<std::string, double>> &items);

extern EventLog gameLog; // Global instance