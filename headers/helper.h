#pragma once

#include <string>

// --- HELPER FUNCTIONS ---

// --- MODERN RANDOM ENGINE ---
namespace Random {

double Normal(double mean, double stdDev);

// Returns integer between min and max (inclusive)
int Int(int min, int max);

bool Chance(double probability01);
} // namespace Random

std::string BeginDropDownMenu(bool IsOpen);

std::string GenerateSongName();

std::string GetAlbumGenre();

double GetRecommendedPrice(double quality, bool IsAlbum);
