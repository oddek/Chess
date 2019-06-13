#pragma once
#include "Piece.h"

class Rook : public Piece
{
public:
	Rook(char _symbol);
	~Rook();
	bool check_legal_move(Square* to, Board& board);

};

