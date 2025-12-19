#include "../headers/helper.h"
#include "../headers/config.h"

#include <random>
#include <string>
#include <vector>


// --- HELPER FUNCTIONS ---

// --- MODERN RANDOM ENGINE ---
namespace Random {

std::mt19937 mt(std::random_device{}());

double Normal(double mean, double stdDev) {
  std::normal_distribution<double> dist(mean, stdDev);
  return dist(mt);
}

// Returns integer between min and max (inclusive)
int Int(int min, int max) {
  std::uniform_int_distribution<int> dist(min, max);
  return dist(mt);
}

bool Chance(double probability01) {
  std::bernoulli_distribution dist(probability01);
  return dist(mt);
}
} // namespace Random

std::string GenerateSongName() {
  const std::vector<std::string> songNames = {"Take Me On",
                                              "I'm So Tired",
                                              "Neon Lights",
                                              "Code Monkey",
                                              "Who am i",
                                              "Feeling you",
                                              "Hello World",
                                              "Memory Leak",
                                              "Enough of you",
                                              "Is there anything left",
                                              "The Algorithm",
                                              "Binary Love",
                                              "Electric Dreams",
                                              "Midnight City",
                                              "Starlight Serenade",
                                              "Pixel Heart",
                                              "Lost in the Code",
                                              "Digital Rain",
                                              "Synthetic Soul",
                                              "Echoes of Tomorrow",
                                              "Quantum Leap",
                                              "Cybernetic Love",
                                              "The Glitch in the Matrix",
                                              "The major",
                                              "Virtual Embrace",
                                              "Sunset Boulevard",
                                              "Whispers in the Dark",
                                              "Ocean Drive",
                                              "City of Stars",
                                              "Broken Melodies",
                                              "Fading Memories",
                                              "Crimson Skies",
                                              "Silent Echoes",
                                              "Golden Hour",
                                              "Rainy Day Blues",
                                              "Summer Breeze",
                                              "Winter's Lullaby",
                                              "Autumn Leaves",
                                              "Spring Awakening",
                                              "Distant Shores",
                                              "Lost in Translation",
                                              "Paper Thin Walls",
                                              "Velvet Touch",
                                              "Crystal Clear",
                                              "Shadow Play",
                                              "Dream Weaver",
                                              "Cosmic Dust",
                                              "Stardust Symphony",
                                              "Galactic Groove",
                                              "Nebula Nights",
                                              "Supernova Soul",
                                              "Black Hole Blues",
                                              "Milky Way Melody",
                                              "Andromeda Anthem"};
  return songNames[Random::Int(0, static_cast<int>(songNames.size()) - 1)];
}

std::string GetAlbumGenre() {
  std::vector<std::string> genres = {"Pop",  "Rock",      "Hip-Hop", "R&B",
                                     "Jazz", "Classical", "Other"};

  // calculate most used genre in album
  int popCount = 0;
  int rockCount = 0;
  int hipHopCount = 0;
  int rnbCount = 0;
  int jazzCount = 0;
  int classicalCount = 0;
  int otherCount = 0;

  for (int i = 0; i < 5; i++) {
    if (genres[i] == "Pop") {
      popCount++;
    } else if (genres[i] == "Rock") {
      rockCount++;
    } else if (genres[i] == "Hip-Hop") {
      hipHopCount++;
    } else if (genres[i] == "R&B") {
      rnbCount++;
    } else if (genres[i] == "Jazz") {
      jazzCount++;
    } else if (genres[i] == "Classical") {
      classicalCount++;
    } else {
      otherCount++;
    }
  }

  int mostUsed =
      std::max({popCount, rockCount, hipHopCount, rnbCount, jazzCount});

  if (mostUsed == popCount) {
    return "Pop";
  } else if (mostUsed == rockCount) {
    return "Rock";
  } else if (mostUsed == hipHopCount) {
    return "Hip-Hop";
  } else if (mostUsed == rnbCount) {
    return "R&B";
  } else if (mostUsed == jazzCount) {
    return "Jazz";
  } else {
    return "Unknown";
  }
}

double GetRecommendedPrice(double quality, bool IsAlbum) {
  double result;
  if (!IsAlbum) {
    result = EconomyConfig::BASE_PRICE +
             (quality * EconomyConfig::PRICE_PER_QUALITY);
  } else if (IsAlbum) {
    result = EconomyConfig::ALBUM_BASE_PRICE +
             (quality * EconomyConfig::PRICE_PER_QUALITY_ALBUM);
  }
  return result;
}
