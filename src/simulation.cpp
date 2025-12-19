

#include "../headers/Simulation.h"
#include "../headers/album.h"
#include "../headers/config.h"
#include "../headers/graphics.h"
#include "../headers/helper.h"
#include "../headers/player.h"
#include "../headers/song.h"

#include <algorithm>
#include <cmath>

// --- CORE SIMULATION LOGIC ---
void UpdateFanbase(Player &player, int streams, double songQuality,
                   double hype) {
  // 1. Logarithmic Scaling (Market Saturation)
  // As you get more fans, it becomes harder to reach "new" people.
  // We use a saturation factor to slow down growth at high numbers.
  double saturationFactor = 1.0;
  if (player.fans > 100000) {
    // This makes growth feel slower once you hit 100k, 1M, etc.
    saturationFactor = 1.0 / (1.0 + std::log10(player.fans / 10000.0));
  }

  // 2. Sophisticated Conversion logic
  // Conversion is now heavily influenced by Quality.
  // High quality (80+) builds "Superfans", low quality (<30) actually hurts
  // your brand.
  double qualityMult =
      (songQuality - 40.0) / 40.0; // 100 = 1.5x, 40 = 0x, 0 = -1.0x

  // Calculate new fans based on streams, hype, and quality
  // We use a smaller base rate (0.01) to keep numbers from exploding too fast
  double conversionRate = 0.01 * std::max(0.1, hype) * qualityMult;
  int netChange = static_cast<int>(streams * conversionRate * saturationFactor);

  // 3. The "Viral Moment" (The TikTok Factor)
  // 0.5% chance to have a song go viral, regardless of quality (though quality
  // helps)
  if (Random::Chance(0.0001 * hype)) { // Only high-hype songs likely go viral
    int viralFans = static_cast<int>(streams * (songQuality / 100.0) * 2.0);
    player.fans += viralFans;
    gameLog.Add("A song went viral!");
  }

  // 4. Natural Churn (Retention)
  // Fans leave if you are inactive (low hype) or if your music is bad.
  double baseChurn = 0.0005; // 0.05% natural decay
  if (songQuality < 35.0)
    baseChurn += 0.002; // Penalty for "selling out" or bad quality
  if (hype < 0.2)
    baseChurn += 0.001; // Penalty for being forgotten

  int lostFans = static_cast<int>(player.fans * baseChurn);

  // Apply the changes
  player.fans += netChange;
  player.fans = std::max(0, player.fans - lostFans);
}

void SimulateEconomy(std::vector<Song> &songs, std::vector<Album> &albums,
                     Player &player, sf::Time dt) {
  // Check if there is anything to simulate
  if (songs.empty() && albums.empty())
    return;

  // 1. Always update age (framerate independent)
  for (auto &song : songs) {
    song.lifeTime += dt.asSeconds();
  }
  for (auto &album : albums) {
    album.lifeTime += dt.asSeconds();
  }

  static float tickTimer = 0.0f;
  tickTimer += dt.asSeconds();

  // 2. Run Economics at fixed tick rate
  if (tickTimer >= EconomyConfig::ECONOMY_TICK_RATE) {

    // --- PART A: SINGLES SIMULATION ---
    for (auto &song : songs) {
      if (song.hype <= 0.01f) {
        song.dailyStreams = 0; // Song is "dead" on charts
        continue;
      }

      double fairPrice = GetRecommendedPrice(song.quality, false);
      double priceRatio = fairPrice / std::max(0.01, (double)song.price);
      double demandMultiplier = std::clamp(
          std::pow(priceRatio, EconomyConfig::PRICE_ELASTICITY), 0.0, 3.0);

      // Discovery & Streams
      double baseDiscovery = Random::Normal(50.0, 15.0);
      double fanReach = player.fans * 0.05;
      double potentialListeners = (baseDiscovery + fanReach) * song.hype;
      double retention = std::clamp(song.quality / 10.0, 0.1, 1.2);

      int actualStreams =
          static_cast<int>(potentialListeners * retention * demandMultiplier);

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

    tickTimer = 0.0f;
  }
}