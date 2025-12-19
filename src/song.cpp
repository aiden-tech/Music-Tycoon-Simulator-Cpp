#include "../headers/song.h"

Song::Song(std::string _name, std::string _artist, double _price,
           double _quality, int _currentFans)
    : name(std::move(_name)), artist(std::move(_artist)), price(_price),
      quality(_quality) {
  // Initial Hype: Base 1.0 + Tiny boost from existing fans
  this->hype = 1.0 + (_currentFans * 0.0005);
}