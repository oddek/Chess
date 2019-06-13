#pragma once
#include "Piece.h"
class Knight : public Piece
{
public:
	Knight(char _symbol);
	~Knight();
	bool check_legal_move(Square* to, Board& board);
};

