#pragma once
#include <SFML/Graphics.hpp>
class Piece;

class Square
{
public:
	Square();
	Square(char _collumn, int _row, sf::Vector2f _center);
	~Square();
	sf::Vector2f center;
	char collumn;
	int row;
	Piece* piece;
};

