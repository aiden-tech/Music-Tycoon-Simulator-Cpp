#pragma once

#include <string>

// --- MODERN RANDOM ENGINE ---
namespace Random {

double Normal(double mean, double stdDev);

// Returns integer between min and max (inclusive)
int Int(int min, int max);
double Double(double min, double max);

bool Chance(double probability01);
} // namespace Random

// --- HELPER FUNCTIONS ---

std::string GenerateSongName();

std::string GetAlbumGenre();

double GetRecommendedPrice(double quality, bool IsAlbum);
