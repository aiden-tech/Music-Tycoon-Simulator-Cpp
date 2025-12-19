#include "../headers/album.h"

Album::Album(std::string _name, std::string _artist, std::string _genre,
             std::vector<Song> _tracks, double _quality, int _currentFans,
             double _price = 9.99)
    : name(std::move(_name)), artist(std::move(_artist)),
      genre(std::move(_genre)),
      tracks(std::move(_tracks)), // Correctly move the vector into the struct
      price(_price), quality(_quality) {

  // Initial Hype logic
  this->hype = 1.0 + (_currentFans * 0.001);
}
