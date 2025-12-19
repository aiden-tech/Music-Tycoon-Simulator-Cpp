#include "../headers/player.h"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

// 1. STABLE UI CALCULATION
double Player::GetBaseQuality() const {
  // --- 1. HANDLE EMPTY STATE ---
  if (skills.empty())
    return 5.0;

  // --- 2. EXTRACT & SORT SKILLS ---
  // We want the player's best skills to matter more than their worst.
  std::vector<double> skillLevels;
  for (const auto &[name, level] : skills) {
    skillLevels.push_back(level);
  }
  std::sort(skillLevels.begin(), skillLevels.end(), std::greater<double>());

  // --- 3. CALCULATE SKILL POWER (The "Core") ---
  // Instead of a flat average, we use a weighted top-heavy calculation:
  // Top skill: 50% weight
  // 2nd skill: 30% weight
  // 3rd skill: 20% weight
  // This represents "Primary Talent" vs "Supporting Skills" (e.g., Vocals vs
  // Mixing)
  double skillPower = 0;
  if (skillLevels.size() >= 1)
    skillPower += skillLevels[0] * 0.50;
  if (skillLevels.size() >= 2)
    skillPower += skillLevels[1] * 0.30;
  if (skillLevels.size() >= 3)
    skillPower += skillLevels[2] * 0.20;
  else if (skillLevels.size() == 1)
    skillPower /= 0.50; // Normalize if they only have 1 skill
  else if (skillLevels.size() == 2)
    skillPower /= 0.80; // Normalize if they only have 2

  // --- 4. CALCULATE TOOL IMPACT (The "Polish") ---
  // Tools shouldn't be 30% of the total quality (that's too much).
  // In reality, a $10,000 mic makes a great singer sound better,
  // but it won't make a bad singer sound good.
  double toolSum = 0;
  for (const auto &[name, level] : studio_tools) {
    // Logarithmic scaling: The jump from No Mic to Cheap Mic is huge.
    // The jump from Expensive Mic to Legendary Mic is small.
    toolSum += std::log1p(level * 10.0);
  }

  // Normalize ToolSum to a 0.0 - 1.0 range (Assuming ~4-5 key tools)
  double toolFactor = std::clamp(toolSum / 12.0, 0.0, 1.0);

  // --- 5. THE FINAL MERGE (Realistic Weighting) ---
  // Skill is the base. Tools provide a "Polish Bonus" of up to 15 points.
  // If SkillPower is 0-100, we scale it to 85, then add up to 15 from tools.

  // Assuming skillLevels are 0.0 to 100.0.
  // If your skills are 0.0 to 1.0, multiply skillPower by 100 first.
  double baseSkillScore = (skillPower);

  // The "Studio Ceiling": Low quality tools cap your potential.
  // Even if you are a god, recording on a phone (0 tools) limits you.
  double studioCeiling = 0.70 + (toolFactor * 0.30); // Range: 0.7 to 1.0

  double finalBase = (baseSkillScore * studioCeiling);

  // Add a tiny bit of "Equipment Depth" (0 to 5 points max)
  finalBase += (toolFactor * 5.0);

  // --- 6. FINAL OUTPUT ---
  // 5.0 is the floor for a human making "noise".
  return std::clamp(finalBase, 5.0, 100.0);
}

// 2. RANDOMIZED RECORDING CALCULATION
double Player::CalcQuality() const {
  static std::mt19937 gen(std::random_device{}());

  double base = GetBaseQuality();

  // Tighter Luck: +/- 5 points instead of 8.
  // Low level players shouldn't "accidentally" make a masterpiece.
  std::normal_distribution<double> luckDist(0.0, 5.0);
  double finalQuality = base + luckDist(gen);

  // Viral Roll: Only happens if the song is already "decent" (e.g., > 20)
  // This prevents a 0-skill player from going viral with a broken song.
  std::uniform_real_distribution<double> viralRoll(0.0, 100.0);
  if (base > 20.0 && viralRoll(gen) > 99.0) {
    finalQuality += 20.0;
  }

  return std::clamp(finalQuality, 1.0, 100.0);
}

// 3. ALBUM AGGREGATION
double Player::CalcAlbumQuality(const std::vector<double> &songQualities) {
  if (songQualities.empty())
    return 0.0;

  // Sort descending
  std::vector<double> sortedSongs = songQualities;
  std::sort(sortedSongs.begin(), sortedSongs.end(), std::greater<double>());

  // 1. Weighted Average (Top songs carry the album)
  double weightedSum = 0;
  double totalWeight = 0;
  for (size_t i = 0; i < sortedSongs.size(); ++i) {
    double weight = std::pow(0.6, i); // Sharp drop off for worse songs
    weightedSum += sortedSongs[i] * weight;
    totalWeight += weight;
  }
  double baseQuality = weightedSum / totalWeight;

  // 2. Consistency (Flow) Bonus
  // We need to calculate standard deviation to see how "consistent" the album
  // is
  double sum = std::accumulate(sortedSongs.begin(), sortedSongs.end(), 0.0);
  double mean = sum / sortedSongs.size();
  double sq_sum = std::inner_product(sortedSongs.begin(), sortedSongs.end(),
                                     sortedSongs.begin(), 0.0);
  double stdev =
      std::sqrt(std::max(0.0, (sq_sum / sortedSongs.size()) - (mean * mean)));

  // IMPORTANT FIX: The flow bonus must scale with the quality.
  // An album of all 1/100 songs is "consistent" but shouldn't get a +5 bonus.
  double qualityScale = baseQuality / 100.0;
  double flowModifier = (10.0 - stdev) * 0.5 * qualityScale;

  // 3. Filler Penalty
  double fillerPenalty = 0;
  if (sortedSongs.size() >= 4) {
    int fillerCount = 0;
    for (double q : sortedSongs)
      if (q < 20.0)
        fillerCount++;
    if (fillerCount > (sortedSongs.size() * 0.4))
      fillerPenalty = 10.0;
  }

  double finalAlbumQuality = baseQuality + flowModifier - fillerPenalty;

  // 4. Hard Diminishing Returns (Make it very hard to get a 90+)
  if (finalAlbumQuality > 80.0) {
    finalAlbumQuality = 80.0 + (finalAlbumQuality - 80.0) * 0.4;
  }

  return std::clamp(finalAlbumQuality, 1.0, 100.0);
}