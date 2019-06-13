#pragma once
#include <vector>
#include <iostream>
#include <SFML/Graphics.hpp>

class Square;

class Board;

class Piece
{
public:
	Piece();
	Piece(char _symbol);
	virtual ~Piece() = 0;
	virtual bool check_legal_move(Square* to, Board& board) = 0;
	char symbol;
	virtual bool get_en_passant() { return false; }
	virtual bool get_if_piece_captured_with_en_passant() { return false; }
	virtual void reset_en_passant() {}
	virtual void reset_captured_with_en_passant(){}
	virtual char check_if_castled() { return 'n'; }
	virtual void reset_castled() {}
	Square* current_square;
	std::vector<Square*> legal_squares;
	bool is_white;
	sf::Sprite sprite;
	sf::Vector2u img_size;
protected:
	sf::Texture img;

	Square* get_square(char collumn, int row, std::vector<Square*>& squares);
	
	
	
	

};

