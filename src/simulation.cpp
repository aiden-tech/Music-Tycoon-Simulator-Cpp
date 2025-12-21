

#include "../headers/Simulation.h"
#include "../headers/album.h"
#include "../headers/config.h"
#include "../headers/graphics.h"
#include "../headers/helper.h"
#include "../headers/player.h"
#include "../headers/song.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <random>
#include <string>

// --- CORE SIMULATION LOGIC ---
void UpdateFanbase(Player &player, int streams, double songQuality,
                   double hype) {
  // --- A. Base Conversion (Getting new listeners) ---
  // A "Good" song (70+) converts 1% of listeners.
  // A "Masterpiece" (95+) converts 2.5%.
  // A "Bad" song (<40) converts 0%.
  double qualityFactor =
      std::max(0.0, (songQuality - 40.0) / 40.0); // Normalized 0.0 to 1.5
  double baseConversionRate = 0.008;              // 0.8% base chance

  // --- B. Saturation Logic (Diminishing Returns) ---
  // It's easy to get your first 1,000 fans. Hard to get your 100 millionth.
  double saturation = 1.0;
  if (player.fans > 1000) {
    // Logarithmic curve: slows down growth significantly as you get huge
    saturation = 1.0 / (std::log10(player.fans) - 1.5);
    // Example: 10k fans -> saturation ~0.4
    // Example: 1M fans  -> saturation ~0.2
  }
  // Clamp saturation so it never breaks math (min 0.05)
  saturation = std::clamp(saturation, 0.05, 1.0);

  // --- C. Calculate New Fans ---
  // Formula: Streams * (Base Rate * QualityBonus) * MarketHype * Saturation
  double rawNewFans = streams * (baseConversionRate * qualityFactor) *
                      std::max(0.5, hype) * saturation;
  int newFans = static_cast<int>(rawNewFans);

  // --- D. The "Viral" Lottery ---
  // Small chance for massive explosive growth (TikTok/Shorts effect)
  // Higher hype = higher chance.
  if (hype > 0.5 && Random::Chance(0.001 * hype)) { // 0.1% chance per tick
    int viralSpike = static_cast<int>(streams * (songQuality / 10.0));
    newFans += viralSpike;
    // Add a distinct notification so the player knows WHY they jumped
    gameLog.Add("VIRAL HIT! Gained " + std::to_string(viralSpike) +
                " fans instantly!");
  }

  // --- E. Churn (Losing Fans) ---
  // You always lose a tiny bit of fans (people lose interest).
  // Low quality releases accelerate this.
  double churnRate =
      0.0001 / player.reputation; // 0.01% natural daily churn / rep factor
  if (songQuality < 40.0)
    churnRate += 0.005 / player.reputation; // 0.5% penalty for bad songs
  if (hype < 0.1)
    churnRate += 0.001 / player.reputation; // Penalty for being inactive

  int lostFans = static_cast<int>(player.fans * churnRate);

  // --- F. Apply & Ensure Safety ---
  player.fans += newFans;
  player.fans -= lostFans;
  if (player.fans < 0)
    player.fans = 0;
}

// Returns true if an update occurred (useful for triggering UI sounds/visuals)
bool UpdateReputation(Player &player, const std::vector<Song> &songs,
                      const std::vector<Album> &albums,
                      const std::shared_ptr<float> &deltaTimePtr) {

  // 1. Safety Checks
  if (!deltaTimePtr)
    return false;

  // 2. Accumulate Time
  // We strictly READ from the shared pointer. We do not reset it.
  player.repUpdateAccumulator += *deltaTimePtr;

  // 3. Check Cycle
  if (player.repUpdateAccumulator >= EconomyConfig::REP_CYCLE) {

    // Reset local accumulator, keeping the remainder for time precision
    // (e.g., if we overshoot by 0.01s, we keep it for the next cycle)
    player.repUpdateAccumulator -= EconomyConfig::REP_CYCLE;

    // Optimization: Early exit if inventory is empty
    if (songs.empty() && albums.empty()) {
      player.reputation = 0.0;
      return true;
    }

    // 4. Calculate using C++20 Ranges (Clean & readable)
    // We project the 'quality' member and sum it up.

    // Sum Songs
    double songScore = 0.0;
    for (const auto &song : songs) {
      songScore += song.quality;
    }

    // Sum Albums
    double albumScore = 0.0;
    for (const auto &album : albums) {
      albumScore += album.quality;
    }

    // 5. Apply Logic
    // Combine, normalize (divide by 100), and clamp
    double rawReputation = (songScore + albumScore) / 100.0;

    player.reputation = std::clamp(rawReputation, 0.0, 5.0);

    return true; // Indicate that values changed
  }

  return false; // No update this frame
}

std::pair<float, std::string_view>
GetMarketTrend(std::shared_ptr<float> tickTimer) {
  // 1. Safety check
  if (!tickTimer) {
    return {1.0f, "Neutral"};
  }

  // 2. Persistent State
  static bool initialized = false;
  static float currentMultiplier = 1.0f;
  static size_t currentGenreIndex =
      0; // Store index, faster than string comparison

  // Static constant data
  static const std::vector<std::string> genres = {
      "Pop", "Rock", "Hip-Hop", "R&B", "Jazz", "Classical", "Other"};

  // Modern Random Number Generation (Standard C++)
  // Initialized once (static)
  static std::mt19937 gen(std::random_device{}());

  constexpr float TREND_DURATION = 45.0f;

  // Helper lambda to pick a new multiplier
  auto PickMultiplier = [&]() -> float {
    // NOTE: Your original code used Normal(0.5, 0.1).
    // This results in a value around 0.5. If this is a "Bonus",
    // usually you want values > 1.0.
    // If you intended a nerf, keep it as is.
    // Below preserves your exact original logic:
    return std::max(0.1f, (float)Random::Normal(0.5, 0.1));
  };

  // 3. First Run Initialization
  if (!initialized) {
    std::uniform_int_distribution<size_t> dist(0, genres.size() - 1);
    currentGenreIndex = dist(gen);
    currentMultiplier = PickMultiplier();
    initialized = true;
  }

  // 4. Update Logic
  if (*tickTimer >= TREND_DURATION) {
    // Soft Reset: Preserve overshoot to maintain time accuracy
    *tickTimer -= TREND_DURATION;

    // Pick a NEW genre index (ensure it is different)
    std::uniform_int_distribution<size_t> dist(0, genres.size() - 1);
    size_t newIndex = currentGenreIndex;

    while (newIndex == currentGenreIndex) {
      newIndex = dist(gen);
    }
    currentGenreIndex = newIndex;

    // Pick new multiplier
    currentMultiplier = PickMultiplier();

    // C++20 std::format - cleaner and faster than stringstream
    std::string logMsg =
        std::format("Market Shift! Trending: {} (+{:.2f}x bonus)",
                    genres[currentGenreIndex], currentMultiplier);

    gameLog.Add(logMsg);
  }

  // Return string_view (no copy) + current value
  return {currentMultiplier, genres[currentGenreIndex]};
}

void SimulateEconomy(std::vector<Song> &songs, std::vector<Album> &albums,
                     Player &player, sf::Time dt,
                     std::shared_ptr<float> tickTimer) {

  if (songs.empty() && albums.empty())
    return;

  // 1. Update lifetimes (every frame)
  for (auto &song : songs)
    song.lifeTime += dt.asSeconds();
  for (auto &album : albums)
    album.lifeTime += dt.asSeconds();

  // 2. IMPORTANT: Get market trend ONCE per frame, not per song
  // This prevents the "Reset Bug"
  auto [trendMultiplier, trendingGenre] = GetMarketTrend(tickTimer);

  // 3. Increment the shared timer
  *tickTimer += dt.asSeconds();

  // 4. Run Economics at fixed tick rate
  if (*tickTimer >= EconomyConfig::ECONOMY_TICK_RATE) {

    for (auto &song : songs) {
      if (song.hype <= 0.01f) {
        song.dailyStreams = 0;
        continue;
      }

      // Calculation Logic
      double fairPrice = GetRecommendedPrice(song.quality, false);
      double priceRatio = fairPrice / std::max(0.01, (double)song.price);
      double demandMultiplier = std::clamp(
          std::pow(priceRatio, EconomyConfig::PRICE_ELASTICITY), 0.0, 3.0);

      // Market Trend logic
      float activeBonus = 1.0f;
      if (song.genre == trendingGenre) {
        activeBonus += trendMultiplier; // Bonus boost
      }

      double baseDiscovery = Random::Normal(50.0, 15.0);
      double fanReach = player.fans * 0.05;
      double potentialListeners = (baseDiscovery + fanReach) * song.hype;
      double retention = std::clamp(song.quality / 10.0, 0.1, 1.2);
      double repMultiplier = Random::Double(0.0, player.reputation) / 100.0;

      int actualStreams =
          static_cast<int>(potentialListeners * retention * demandMultiplier *
                           activeBonus * repMultiplier);

      // Sales
      double saleProb =
          (1.0 / 800.0) * demandMultiplier * (song.quality / 10.0);
      int newSales = static_cast<int>(actualStreams * saleProb);
      if (newSales < 0)
        newSales = 0;

      // Financials
      double streamRev = actualStreams * EconomyConfig::STREAM_PAYOUT_RATE;
      double salesRev = newSales * song.price;
      player.money += (streamRev + salesRev);

      // Stats Update
      song.dailyStreams = actualStreams;
      song.totalStreams += actualStreams;
      song.totalSales += newSales;
      song.earnings += (streamRev + salesRev);

      // Feedback & Decay
      UpdateFanbase(player, actualStreams, song.quality, song.hype);

      double decay =
          (song.quality > 70.0) ? 0.99 : 0.96; // Higher quality lasts longer
      song.hype *= decay;
    }

    // --- PART B: ALBUMS SIMULATION ---
    for (auto &album : albums) {
      if (album.hype <= 0.01f) {
        album.dailyStreams = 0;
        continue;
      }

      double fairPrice = GetRecommendedPrice(album.quality, true);
      double priceRatio = fairPrice / std::max(0.01, album.price);
      double demandMultiplier = std::clamp(
          std::pow(priceRatio, EconomyConfig::PRICE_ELASTICITY), 0.0, 3.0);

      // Albums have higher discovery and fan reach than singles
      double baseDiscovery = Random::Normal(100.0, 30.0);
      double fanReach = player.fans * 0.12;
      double potentialListeners = (baseDiscovery + fanReach) * album.hype;
      double retention = std::clamp(album.quality / 10.0, 0.1, 1.2);

      int actualStreams =
          static_cast<int>(potentialListeners * retention * demandMultiplier);

      // Album sales are harder to get than song sales
      double saleProb =
          (1.0 / 1200.0) * demandMultiplier * (album.quality / 10.0);
      int newSales = static_cast<int>(actualStreams * saleProb);

      // Financials
      double streamRev = actualStreams * EconomyConfig::STREAM_PAYOUT_RATE;
      double salesRev = newSales * album.price;
      player.money += (streamRev + salesRev);

      // Stats Update
      album.dailyStreams = actualStreams;
      album.totalStreams += actualStreams;
      album.totalSales += newSales;
      album.earnings += (streamRev + salesRev);

      // Feedback & Decay
      UpdateFanbase(player, actualStreams, album.quality, album.hype);

      // Albums decay slightly slower than singles (more staying power)
      double decay = (album.quality > 70.0) ? 0.995 : 0.98;
      album.hype *= decay;
    }

    // ADD THIS AT THE BOTTOM OF THE TICK BLOCK:
    // Natural Churn (only once per tick, not once per song)
    double baseChurn = (player.fans > 100000) ? 0.001 : 0.0005;
    int lostFans = static_cast<int>(player.fans * baseChurn);

    // Scandal logic moved here too so it only triggers once per tick
    if (Random::Chance(0.002)) { // Reduced probability since it's per-tick now
      int scandalLoss = static_cast<int>(player.fans * 0.03);
      player.fans -= scandalLoss;
      gameLog.Add("Scandal: Fans are leaving!");
    }

    player.fans = std::max(0, player.fans - lostFans);

    *tickTimer = 0.0f;
  }
}
