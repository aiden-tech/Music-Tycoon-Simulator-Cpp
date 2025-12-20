#pragma once

#include <SFML/System/Time.hpp>

struct EconomyConfig {
  static constexpr double STREAM_PAYOUT_RATE = 0.004;
  static constexpr double BASE_PRICE = 0.69;
  static constexpr double ALBUM_BASE_PRICE = 1.99;
  static constexpr double PRICE_PER_QUALITY = 0.10;
  static constexpr double PRICE_PER_QUALITY_ALBUM = 0.05;
  static constexpr float REP_CYCLE = 5.0f;         // 5 seconds
  static constexpr float ECONOMY_TICK_RATE = 0.2f; // 5Hz
  static constexpr float PRICE_ELASTICITY = 2.5;
  static constexpr sf::Time SONG_LIFETIME = sf::seconds(300.0f);  // 5 Minutes
  static constexpr sf::Time ALBUM_LIFETIME = sf::seconds(500.0f); // 8 Minutes
};

template <typename T>
constexpr const T &clamp(const T &v, const T &lo, const T &hi) {
  return (v < lo) ? lo : ((hi < v) ? hi : v);
}
