#pragma once
#include "Piece.h"
class King : public Piece
{
public:
	King(char _symbol);
	~King();
	char castled = 'n';
	bool check_legal_move(Square* to, Board& board);
	bool check_castle(Square* to, Board& board);
	char check_if_castled() { return castled; }
	void reset_castled() { castled = 'n'; }
};

