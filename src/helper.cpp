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

double Double(double min, double max) {
  std::uniform_real_distribution<double> dist(min, max);
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
                                              "Andromeda anthem",
                                              "Electric Eclipse",
                                              "Digital Dreams",
                                              "Quantum Leap of Faith",
                                              "Echoes of Eternity",
                                              "Whispers of the Cosmos",
                                              "The Last Goodbye",
                                              "Digital Dust",
                                              "Stardust Serenade",
                                              "Cosmic Waves",
                                              "Neon Dreams",
                                              "Cybernetic Odyssey",
                                              "Lost in Hyperspace",
                                              "Fractured Frequencies",
                                              "The Algorithm of Love",
                                              "Electric Heartbeat",
                                              "Binary Serenade",
                                              "The Glitch in the Heart",
                                              "Virtual Vibrations",
                                              "Spectral Symphony",
                                              "Crystal Code",
                                              "Fractured Rhythms",
                                              "Quantum Code",
                                              "Cosmic Code",
                                              "Stardust Serenity",
                                              "Echoes of Eternity",
                                              "Whispers of the Cosmos",
                                              "Digital Dawn",
                                              "Cybernetic Uprising",
                                              "Neon Nights",
                                              "Nebula Nights",
                                              "Galactic Groove",
                                              "Supernova Soul",
                                              "Black Hole Blues",
                                              "Milky Way Melody",
                                              "Andromeda anthem",
                                              "Electric Eclipse",
                                              "Digital Dreams",
                                              "Quantum Leap of Faith",
                                              "Echoes of Eternity",
                                              "Whispers of the Cosmos",
                                              "The Last Goodbye",
                                              "Digital Dust",
                                              "Stardust Serenade",
                                              "Cosmic Waves",
                                              "Neon Dreams",
                                              "Cybernetic Odyssey",
                                              "Lost in Hyperspace",
                                              "Fractured Frequencies",
                                              "The Algorithm of Love",
                                              "Electric Heartbeat",
                                              "Binary Serenade",
                                              "The Glitch in the Heart",
                                              "Virtual Vibrations",
                                              "Spectral Symphony",
                                              "Crystal Code",
                                              "Fractured Rhythms",
                                              "Quantum Code",
                                              "Cosmic Code",
                                              "Stardust Serenity",
                                              "Echoes of Eternity",
                                              "Whispers of the Cosmos",
                                              "Digital Dawn",
                                              "Cybernetic Uprising",
                                              "Neon nights",
                                              "Nebula nights",
                                              "Galactic Groove",
                                              "Supernova Soul",
                                              "Black Hole Blues",
                                              "Milky Way Melody",
                                              "Andromeda anthem",
                                              "Electric Eclipse",
                                              "Digital Dreams",
                                              "Quantum Leap of Faith",
                                              "Echoes of Eternity",
                                              "Whispers of the Cosmos",
                                              "The Last Goodbye",
                                              "Digital Dust",
                                              "Stardust Serenade",
                                              "Cosmic Waves",
                                              "Neon Dreams",
                                              "Cybernetic Odyssey",
                                              "Lost in Hyperspace",
                                              "Fractured Frequencies",
                                              "The Algorithm of Love",
                                              "Electric Heartbeat",
                                              "Binary Serenade",
                                              "The Glitch in the Heart",
                                              "Virtual Vibrations",
                                              "Spectral Symphony",
                                              "Crystal Code",
                                              "Fractured Rhythms",
                                              "Quantum Code",
                                              "Cosmic Code",
                                              "Stardust Serenity",
                                              "Echoes of Eternity",
                                              "Whispers of the Cosmos",
                                              "Digital Dawn",
                                              "Cybernetic Uprising",
                                              "Neon nights",
                                              "Nebula nights",
                                              "Galactic Groove",
                                              "Supernova Soul",
                                              "Black Hole Blues",
                                              "Milky Way Melody",
                                              "Andromeda anthem",
                                              "Electric Eclipse",
                                              "Digital Dreams",
                                              "Quantum Leap of Faith",
                                              "Echoes of Eternity",
                                              "Whispers of the Cosmos",
                                              "The Last Goodbye",
                                              "Digital Dust",
                                              "Stardust Serenade",
                                              "Cosmic Waves",
                                              "Neon Dreams",
                                              "Cybernetic Odyssey",
                                              "Lost in Hyperspace",
                                              "Fractured Frequencies",
                                              "The Algorithm of Love",
                                              "Electric Heartbeat",
                                              "Binary Serenade",
                                              "The Glitch in the Heart",
                                              "Virtual Vibrations",
                                              "Spectral Symphony",
                                              "Crystal Code",
                                              "Fractured Rhythms",
                                              "Quantum Code",
                                              "Cosmic Code",
                                              "Stardust Serenity",
                                              "Echoes of Eternity",
                                              "Whispers of the Cosmos",
                                              "Digital Dawn",
                                              "Cybernetic Uprising",
                                              "Neon nights",
                                              "Nebula nights",
                                              "Galactic Groove",
                                              "Supernova Soul",
                                              "Black Hole Blues",
                                              "Milky Way Melody",
                                              "Andromeda anthem",
                                              "Electric Eclipse",
                                              "Digital Dreams",
                                              "Quantum Leap of Faith",
                                              "Echoes of Eternity",
                                              "Whispers of the Cosmos",
                                              "The Last Goodbye",
                                              "Digital Dust",
                                              "Stardust Serenade",
                                              "Cosmic Waves",
                                              "Neon Dreams",
                                              "Cybernetic Odyssey",
                                              "Lost in Hyperspace",
                                              "Fractured Frequencies",
                                              "The Algorithm of Love",
                                              "Electric Heartbeat",
                                              "Binary Serenade",
                                              "The Glitch in the Heart",
                                              "Virtual Vibrations",
                                              "Spectral Symphony",
                                              "Crystal Code",
                                              "Fractured Rhythms",
                                              "Quantum Code",
                                              "Cosmic Code",
                                              "Stardust Serenity",
                                              "Echoes of Eternity",
                                              "Whispers of the Cosmos",
                                              "Digital Dawn",
                                              "Cybernetic Uprising",
                                              "Neon nights",
                                              "Nebula nights",
                                              "Galactic Groove",
                                              "Supernova Soul",
                                              "Black Hole Blues",
                                              "Milky Way Melody",
                                              "Andromeda anthem"};
  return songNames[Random::Int(0, static_cast<int>(songNames.size()) - 1)];
}

std::string GetAlbumGenre() {
  std::vector<std::string> genres = {
      "Pop",       "Rock",  "Hip-Hop",     "R&B",     "Jazz",
      "Classical", "Other", "Electronic",  "Country", "Folk",
      "Metal",     "Indie", "Experimental"};

  // calculate most used genre in album
  int popCount = 0;
  int rockCount = 0;
  int hipHopCount = 0;
  int rnbCount = 0;
  int jazzCount = 0;
  int classicalCount = 0;
  int otherCount = 0;
  int electronicCount = 0;
  int countryCount = 0;
  int folkCount = 0;
  int metalCount = 0;
  int indieCount = 0;
  int experimentalCount = 0;

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
    } else if (genres[i] == "Other") {
      otherCount++;
    } else if (genres[i] == "Electronic") {
      electronicCount++;
    } else if (genres[i] == "Country") {
      countryCount++;
    } else if (genres[i] == "Folk") {
      folkCount++;
    } else if (genres[i] == "Metal") {
      metalCount++;
    } else if (genres[i] == "Indie") {
      indieCount++;
    } else if (genres[i] == "Experimental") {
      experimentalCount++;
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
  } else if (mostUsed == classicalCount) {
    return "Classical";
  } else if (mostUsed == otherCount) {
    return "Other";
  } else if (mostUsed == electronicCount) {
    return "Electronic";
  } else if (mostUsed == countryCount) {
    return "Country";
  } else if (mostUsed == folkCount) {
    return "Folk";
  } else if (mostUsed == metalCount) {
    return "Metal";
  } else if (mostUsed == indieCount) {
    return "Indie";
  } else if (mostUsed == experimentalCount) {
    return "Experimental";
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
