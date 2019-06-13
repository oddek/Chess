#pragma once
#include "Piece.h"
class Bishop : public Piece
{
public:
	Bishop(char _symbol);
	~Bishop();
	bool check_legal_move(Square* to, Board& board);
};

