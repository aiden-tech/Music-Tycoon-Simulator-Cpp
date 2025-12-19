#pragma once

#include <string>
#include <utility>
#include <vector>

struct Player {
  std::string name;
  int fans = 100;
  double money = 50.0;

  // Skill Name, Level
  std::vector<std::pair<std::string, double>> skills = {
      {"Voice", 1.0},     {"Producing", 0.3}, {"Writing", 0.9},
      {"Recording", 0.5}, {"Mixing", 0.3},    {"Mastering", 0.3}};

  // Tool Name, Level (Refactored to double for math consistency)
  std::vector<std::pair<std::string, double>> studio_tools = {
      {"Mic", 1.0},       {"Live Mixer", 0.3},   {"Soundboard", 0.9},
      {"Acoustics", 0.5}, {"Audio Editor", 0.3}, {"Computer", 0.3}};

  Player(std::string n) : name(std::move(n)) {}

  // 1. STABLE UI CALCULATION
  double GetBaseQuality() const;

  // 2. RANDOMIZED RECORDING CALCULATION
  double CalcQuality() const;

  // 3. ALBUM AGGREGATION
  double CalcAlbumQuality(const std::vector<double> &songQualities);
};