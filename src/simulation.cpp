

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
  // 1. Safety & Triviality Check
  if (streams <= 0)
    return;

  // -------------------------------------------------------------------------
  // A. REPUTATION DYNAMICS
  // -------------------------------------------------------------------------
  double repChange = 0.0;
  if (songQuality >= 75.0) {
    repChange = (songQuality - 70.0) * 0.005 * hype;
  } else if (songQuality <= 45.0) {
    repChange = -(50.0 - songQuality) * 0.02 * hype;
  }
  player.reputation = std::clamp(player.reputation + repChange, 0.0, 1000.0);

  // -------------------------------------------------------------------------
  // B. MARKET SATURATION (Calculated early for use in conversion)
  // -------------------------------------------------------------------------
  // As you get bigger, it becomes harder to convince the remaining population.
  double saturation = 1.0;
  if (player.fans > 1000000) {
    double logFans = std::log10(static_cast<double>(player.fans));
    // Logistic damping: 1M fans -> ~0.33 multiplier
    saturation = std::clamp(1.0 / (logFans - 3.0), 0.01, 1.0);
  }

  // -------------------------------------------------------------------------
  // C. CONVERSION LOGIC (The Fix)
  // -------------------------------------------------------------------------
  double conversionChance = 0.0;

  // FIX: Lower threshold from 50.0 to 10.0 so beginners can gain fans
  if (songQuality > 10.0) {
    // Base chance (0.2%) + Curve based on quality
    // Quality 20 -> ~0.2%
    // Quality 90 -> ~3.0%
    conversionChance =
        0.002 + (std::pow((songQuality - 10.0) / 90.0, 2.5) * 0.035);
  }

  // Reputation Bonus
  double trustFactor = 1.0 + (player.reputation / 200.0);

  // Calculate theoretical fans (Float precision)
  double theoreticalNewFans =
      streams * conversionChance * trustFactor * saturation;

  // FIX: Probabilistic Rounding
  // Separate the whole number from the decimal
  int newFans = static_cast<int>(theoreticalNewFans);
  double remainder = theoreticalNewFans - newFans;

  // Roll for the remainder. If theoretical is 0.4, there is a 40% chance to get
  // 1 fan.
  if (Random::Double(0.0, 1.0) < remainder) {
    newFans += 1;
  }

  // -------------------------------------------------------------------------
  // D. VIRAL MECHANICS
  // -------------------------------------------------------------------------
  if (hype > 2.0 && songQuality > 80.0) {
    // 0.2% chance per tick
    if (Random::Chance(0.002)) {
      int viralSpike = static_cast<int>(streams * Random::Double(0.5, 2.0));
      newFans += viralSpike;
      gameLog.Add("VIRAL SENSATION! " + std::to_string(viralSpike) +
                  " new fans!");
    }
  }

  // -------------------------------------------------------------------------
  // E. BACKLASH (Reactionary Churn)
  // -------------------------------------------------------------------------
  int angryFans = 0;
  if (player.fans > 1000 && songQuality < 15.0) {
    double disappointmentRate = (40.0 - songQuality) * 0.0005;
    angryFans = static_cast<int>(player.fans * disappointmentRate);
  }

  // -------------------------------------------------------------------------
  // F. APPLY FINAL VALUES
  // -------------------------------------------------------------------------
  player.fans += newFans;
  player.fans = std::max(0, player.fans - angryFans);
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
                     std::shared_ptr<float> globalClock) {

  if ((songs.empty() && albums.empty()) || !globalClock)
    return;

  // -------------------------------------------------------------------------
  // 1. GLOBAL TIME & MARKET TRACKING
  // -------------------------------------------------------------------------

  // Increment the Global Clock (used for Market Trends ~45s cycles)
  *globalClock += dt.asSeconds();

  // Update Lifetime for all items
  for (auto &song : songs)
    song.lifeTime += dt.asSeconds();
  for (auto &album : albums)
    album.lifeTime += dt.asSeconds();

  // Check Market Trend (Once per frame to ensure UI updates, but logic changes
  // slowly) We pass the globalClock to the trend manager.
  auto [trendMultiplier, trendingGenre] = GetMarketTrend(globalClock);

  // -------------------------------------------------------------------------
  // 2. ECONOMY TICK ACCUMULATOR
  // -------------------------------------------------------------------------
  // We use a local static accumulator so we don't reset the GlobalClock
  // which is needed for the 45-second trend cycles.
  static float economyAccumulator = 0.0f;
  economyAccumulator += dt.asSeconds();

  // If we haven't reached the "End of Day" (Tick Rate), exit.
  if (economyAccumulator < EconomyConfig::ECONOMY_TICK_RATE) {
    return;
  }

  // Reset accumulator but keep the overshoot for time precision
  economyAccumulator -= EconomyConfig::ECONOMY_TICK_RATE;

  // -------------------------------------------------------------------------
  // 3. HELPER: REALISTIC STREAM ALGORITHM
  // -------------------------------------------------------------------------
  // Defines how media performs based on real-world factors.
  auto CalculatePerformance =
      [&](double quality, double hype, double price, double lifeTime,
          const std::string &genre,
          bool isAlbum) -> std::tuple<int, int, double> {
    // A. Market Trend Impact (The "Zeitgeist" Factor)
    // If trending, acts as a multiplier on DISCOVERY, not just cash.
    double trendBonus = 1.0;
    if (genre == trendingGenre) {
      trendBonus = 1.0 + trendMultiplier; // e.g., 1.5x to 3.0x visibility
    }

    // B. The "Freshness" Curve (Exponential Decay)
    // New releases spike hard, then stabilize.
    // Albums stay fresh longer (decay divisor 200.0 vs 120.0).
    double decaySpeed = isAlbum ? 200.0 : 120.0;
    // Quality slows down decay (Great songs stay relevant).
    double qualityPreservation = std::max(1.0, quality / 20.0);
    double ageFactor =
        std::exp(-(lifeTime / (decaySpeed * qualityPreservation)));

    // C. Price Elasticity (Demand Curve)
    // People tolerate high prices for High Quality/Hype, but punish it for low
    // quality.
    double recommendedPrice = GetRecommendedPrice(quality, isAlbum);
    double priceRatio = price / std::max(0.01, recommendedPrice);
    // If price is too high (>1.5x fair), demand creates a cliff drop.
    double demandMod =
        (priceRatio > 1.5)
            ? std::pow(priceRatio, -3.0)
            : std::pow(priceRatio, -EconomyConfig::PRICE_ELASTICITY);

    // D. Listener Logic (The Core Simulation)

    // 1. Fan Reach: Not all fans see the content.
    // Higher reputation = higher reach.
    double reachPercent = std::clamp(player.reputation / 1000.0, 0.05, 0.40);
    double activeFanListeners = player.fans * reachPercent * hype;

    // 2. Organic Discovery (Viral Potential)
    // Non-linear: 90 quality is 4x better than 45, not 2x.
    double qualityPower = std::pow(quality / 10.0, 2.5);
    double viralBase = Random::Normal(isAlbum ? 150.0 : 50.0, 15.0);
    double organicListeners =
        viralBase * qualityPower * trendBonus * ageFactor * hype;

    // Total Daily Streams
    double totalListeners = activeFanListeners + organicListeners;

    // Reputation Multiplier (Global fame boost)
    double repBoost =
        1.0 + (std::log10(std::max(1.0, player.reputation)) * 0.1);

    int streams = static_cast<int>(totalListeners * demandMod * repBoost);

    // Determine "Guaranteed" streams (The long tail)
    // Even dead songs get 1-5 streams a day if they are in the catalog.
    if (streams < 5 && lifeTime > 0)
      streams = Random::Int(0, 2);

    // E. Sales Conversion (The Funnel)
    // Harder to sell than stream. Requires high engagement (Hype + Quality).
    double baseConversion = isAlbum ? (1.0 / 1000.0) : (1.0 / 600.0);
    double salesChance = baseConversion * (quality / 50.0) * demandMod;

    // Binomial distribution approximation for sales
    int sales = 0;
    if (streams > 0) {
      // Simple optimization for large numbers
      sales = static_cast<int>(streams * salesChance);
    }

    // F. Decay Calculation (Next Day's Hype)
    // Hype decays naturally, but sales/streams regenerate it slightly (Word of
    // Mouth).
    double naturalDecay = (quality > 85.0) ? 0.995 : 0.97;
    double tractionRestoration = (streams > 1000) ? 0.005 : 0.0;
    double nextHype = hype * (naturalDecay + tractionRestoration);

    return {streams, sales, nextHype};
  };

  // -------------------------------------------------------------------------
  // 4. EXECUTE SIMULATION (Songs)
  // -------------------------------------------------------------------------
  for (auto &song : songs) {
    if (song.hype <= 0.001f) {
      song.dailyStreams = 0;
      continue; // Dead song
    }

    auto [streams, sales, nextHype] = CalculatePerformance(
        song.quality, song.hype, song.price, song.lifeTime, song.genre, false);

    // Apply Financials
    double revenue =
        (streams * EconomyConfig::STREAM_PAYOUT_RATE) + (sales * song.price);
    player.money += revenue;

    // Apply Stats
    song.dailyStreams = streams;
    song.totalStreams += streams;
    song.totalSales += sales;
    song.earnings += revenue;
    song.hype = std::clamp(nextHype, 0.0, 10.0); // Soft cap hype

    // Feedback Loop: Good performance grows fans
    UpdateFanbase(player, streams, song.quality, song.hype);
  }

  // -------------------------------------------------------------------------
  // 5. EXECUTE SIMULATION (Albums)
  // -------------------------------------------------------------------------
  for (auto &album : albums) {
    if (album.hype <= 0.001f) {
      album.dailyStreams = 0;
      continue;
    }

    auto [streams, sales, nextHype] = CalculatePerformance(
        album.quality, album.hype, album.price, album.lifeTime, "Album", true);

    double revenue =
        (streams * EconomyConfig::STREAM_PAYOUT_RATE) + (sales * album.price);
    player.money += revenue;

    album.dailyStreams = streams;
    album.totalStreams += streams;
    album.totalSales += sales;
    album.earnings += revenue;
    album.hype = std::clamp(nextHype, 0.0, 10.0);

    // Albums have a stronger effect on fanbase retention than singles
    UpdateFanbase(player, streams, album.quality, album.hype);
  }

  // -------------------------------------------------------------------------
  // 6. REALISTIC PLAYER CHURN & SCANDALS
  // -------------------------------------------------------------------------

  // A. Natural Churn (Boredom)
  // The bigger you are, the harder it is to keep everyone.
  // Small artists (0-10k) lose almost no one. Massive stars lose 0.1% daily.
  double churnRate = 0.0;
  if (player.fans > 100000)
    churnRate = 0.0005;
  else if (player.fans > 10000)
    churnRate = 0.0002;

  // B. Inactivity Penalty
  // If user has no active songs (total streams low), churn increases.
  // We calculate total active streams for today first.
  long totalDailyStreams = 0;
  for (const auto &s : songs)
    totalDailyStreams += s.dailyStreams;

  if (totalDailyStreams < 100 && player.fans > 500) {
    churnRate *= 2.0; // Fans leave if you are silent
  }

  int lostFans = static_cast<int>(player.fans * churnRate);

  // C. Scandal Event (Random Bad Luck)
  // Realism: Scandals are rare (0.1% chance per day), but impactful.
  if (player.fans > 1000 && Random::Chance(0.001)) {
    int scandalLoss =
        static_cast<int>(player.fans * Random::Double(0.02, 0.05));
    lostFans += scandalLoss;
    gameLog.Add("SCANDAL: Bad press caused " + std::to_string(scandalLoss) +
                " fans to leave!");
  }

  player.fans = std::max(0, player.fans - lostFans);
}