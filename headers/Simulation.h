#pragma once

#include "album.h"
#include "player.h"
#include "song.h"


#include <SFML/Graphics.hpp>

void UpdateFanbase(Player &player, int streams, double songQuality,
                   double hype);

void SimulateEconomy(std::vector<Song> &songs, std::vector<Album> &albums,
                     Player &player, sf::Time dt);