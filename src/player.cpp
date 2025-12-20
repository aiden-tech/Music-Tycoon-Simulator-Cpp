#include "../headers/player.h"
#include "../headers/graphics.h"
#include "../headers/helper.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <format>
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
  // 1. Safety check
  if (songQualities.empty())
    return 0.0;

  // 2. Create a copy and sort descending (C++20 ranges)
  std::vector<double> sortedSongs = songQualities;
  std::ranges::sort(sortedSongs, std::greater<double>());

  // 3. Weighted Average (The "Hit Single" Effect)
  // Realism: Listeners value the "Highs" of an album more than the "Lows".
  // We use a gentle decay so the top 3-4 songs drive the score,
  // but the 'tail' still matters.
  double weightedSum = 0.0;
  double totalWeight = 0.0;

  for (size_t i = 0; i < sortedSongs.size(); ++i) {
    // Decay: 100%, 85%, 72%... floor at 25% weight.
    // This ensures even the last track contributes 25% of its value to the
    // average.
    double weight = std::max(0.25, std::pow(0.85, static_cast<double>(i)));
    weightedSum += sortedSongs[i] * weight;
    totalWeight += weight;
  }

  // This is our starting point.
  // If inputs are [10, 20, 10, 20, 15], this will naturally settle around ~16.
  double baseQuality = weightedSum / totalWeight;

  // 4. Cohesion (Standard Deviation)
  // Realism: An album with wild quality swings (90, 10, 90, 10) feels
  // disjointed. An album of all 50s feels cohesive.
  double sum = std::accumulate(sortedSongs.begin(), sortedSongs.end(), 0.0);
  double mean = sum / static_cast<double>(sortedSongs.size());

  double sqDiffSum = 0.0;
  for (double q : sortedSongs) {
    double diff = q - mean;
    sqDiffSum += diff * diff;
  }

  // Variance logic
  double variance =
      (sortedSongs.size() > 1)
          ? sqDiffSum / static_cast<double>(sortedSongs.size() - 1)
          : 0.0;
  double stdev = std::sqrt(std::max(0.0, variance));

  // Cohesion Logic:
  // - High Stdev (e.g. > 15) -> Penalty (Disjointed)
  // - Low Stdev (e.g. < 5)  -> Bonus (Consistent flow)
  // Note: We scale this impact. It matters less for low-quality albums.
  // A consistent 15/100 album is still just a 15, not a 20.
  double cohesionModifier = 0.0;
  if (stdev < 5.0) {
    cohesionModifier = 2.0; // Small bonus for tight consistency
  } else if (stdev > 15.0) {
    // Penalty scales with how "good" the album tries to be.
    // If base is 80 and stdev is 20, penalty is noticeable (-4).
    // If base is 15 and stdev is 5 (irrelevant), penalty is tiny.
    cohesionModifier = -(stdev - 15.0) * 0.25;
  }

  // 5. Dynamic Filler Penalty
  // Realism: "Filler" isn't just "Bad songs". It's songs that ruin the specific
  // album. If the Average is 15, a 10 IS NOT FILLER. It fits the vibe. If the
  // Average is 80, a 40 IS FILLER.
  double fillerPenalty = 0.0;

  // We only check for filler if the album claims to be decent (> 40 avg)
  // This PROTECTS your low-quality 10-20 album from being penalized.
  if (sortedSongs.size() >= 4 && baseQuality > 40.0) {
    double fillerThreshold =
        baseQuality * 0.6; // Filler is 60% of average or less
    int fillerCount = 0;

    for (double q : sortedSongs) {
      if (q < fillerThreshold)
        ++fillerCount;
    }

    // Only punish if it's a significant portion of the album (> 25%)
    if (fillerCount > (sortedSongs.size() * 0.25)) {
      fillerPenalty = fillerCount * 2.0; // -2 per filler track
    }
  }

  // 6. Length Bonus (The "LP" Effect)
  // Realism: Critics respect a 12-track project more than a 6-track one,
  // provided the quality holds up.
  double lengthBonus = 0.0;
  if (sortedSongs.size() >= 10)
    lengthBonus = 2.0;
  if (sortedSongs.size() >= 14)
    lengthBonus = 3.5;

  // 7. Calculate Final
  double finalQuality =
      baseQuality + cohesionModifier + lengthBonus - fillerPenalty;

  // 8. Soft Caps (Diminishing Returns)
  // It is exponentially harder to go from 90->95 than 50->55.
  if (finalQuality > 90.0) {
    finalQuality = 90.0 + (finalQuality - 90.0) * 0.5;
  }

  return std::clamp(finalQuality, 1.0, 100.0);
}

double Player::Busk(double requestedTime) {
  auto moneyMade = 0.0;
  auto timeSpent = 0.0;
  auto energyCost = 0.0;

  // 1. Determine the appropriate duration and energy cost based on the
  // requested time
  if (requestedTime >= 120.0) {
    timeSpent = 120.0;
    energyCost = 25.0;
  } else if (requestedTime >= 90.0) {
    timeSpent = 90.0;
    energyCost = 15.0;
  } else if (requestedTime >= 60.0) {
    timeSpent = 60.0;
    energyCost = 10.0;
  } else if (requestedTime >= 30.0) {
    timeSpent = 30.0;
    energyCost = 5.0;
  } else {
    // Handle cases where time is less than 30 mins
    gameLog.Add("Please use game slider to set time busking (min 30 mins).");
    return money; // Return existing money, make no changes
  }

  // 2. Check if the player has enough energy for the determined action
  if (Energy >= energyCost) {
    Energy -= energyCost;
    auto luck = Random::Double(0.1, 0.3);
    moneyMade = timeSpent * luck * 2.0;
    gameLog.Add("Busked for " + std::format("{:.2f}", requestedTime) +
                " minutes.");
  } else {
    gameLog.Add("Not enough energy to busk for " + std::to_string(timeSpent) +
                " mins!");
  }

  // 3. Update total money and return the new total
  money += moneyMade;
  return money;
}

double Player::Rest() {
  Energy += 100.0;
  return Energy;
}