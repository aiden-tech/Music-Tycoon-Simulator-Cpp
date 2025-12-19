#pragma once

#include "song.h"

#include <string>
#include <vector>

struct Album {
  std::string name;
  std::string artist;
  std::string genre;
  std::vector<Song> tracks; // This needs to be initialized!
  double price;
  double quality;
  double hype;

  // Stats
  int dailyStreams = 0;
  int totalStreams = 0;
  int totalSales = 0;
  double earnings = 0.0;
  float lifeTime = 0.0f;

  Album(std::string _name, std::string _artist, std::string _genre,
        std::vector<Song> _tracks, double _quality, int _currentFans,
        double _price);
};