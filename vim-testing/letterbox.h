#pragma once

#include <SFML/Graphics.hpp>

//https://en.sfml-dev.org/forums/index.php?topic=16876.0
sf::View getLetterboxView(sf::View view, int windowWidth, int windowHeight);
