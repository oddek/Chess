#include "Square.h"
#include "Piece.h"



Square::Square()
{
}

Square::Square(char _collumn, int _row, sf::Vector2f _center): collumn(_collumn), row(_row), center(_center)
{}

Square::~Square()
{
}
