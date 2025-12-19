#include "../headers/song.h"

Song::Song(std::string _name, std::string _artist, std::string _genre,
           double _quality, int _fans, double _price)
    : name(_name), artist(_artist), genre(_genre), quality(_quality),
      fansAtRelease(_fans), price(_price) {
  // Calculate initial hype based on quality and existing fans
  hype = 1.0 + (quality / 100.0) + (fansAtRelease / 50000.0);
}