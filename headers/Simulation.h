#pragma once

#include "album.h"
#include "player.h"
#include "song.h"

#include <SFML/Graphics.hpp>

void UpdateFanbase(Player &player, int streams, double songQuality,
                   double hype);

std::pair<float, std::string_view>
GetMarketTrend(std::shared_ptr<float> tickTimer);

bool UpdateReputation(Player &player, const std::vector<Song> &songs,
                      const std::vector<Album> &albums,
                      const std::shared_ptr<float> &deltaTimePtr);

void SimulateEconomy(std::vector<Song> &songs, std::vector<Album> &albums,
                     Player &player, sf::Time dt,
                     std::shared_ptr<float> tickTimer);
