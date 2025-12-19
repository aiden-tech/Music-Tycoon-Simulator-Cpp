#pragma once

#include <string>

struct Song {
  std::string name;
  std::string artist;
  std::string genre;
  double price;
  double quality;
  double hype; // 1.0 = Max, 0.0 = Dead
  int fansAtRelease;

  // Stats
  int dailyStreams = 0;
  int totalStreams = 0;
  int totalSales = 0;
  double earnings = 0.0;
  float lifeTime = 0.0f;

  Song(std::string _name, std::string _artist, std::string _genre,
       double _quality, int _fans, double _price);
};