#pragma once
#include "Piece.h"
class Queen : public Piece
{
public:
	Queen(char _symbol);
	~Queen();
	bool check_legal_move(Square* to, Board& board);
};

