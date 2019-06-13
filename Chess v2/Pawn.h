#pragma once
#include "Piece.h"
class Pawn : public Piece
{
public:
	Pawn(char _symbol);
	~Pawn();

	bool check_legal_move(Square* to, Board& board);
	bool en_passant = false;
	bool has_moved = false;
	bool captured_with_en_passant = false;
	bool get_en_passant() { return en_passant; }
	void reset_en_passant() { en_passant = false; }
	void reset_captured_with_en_passant() { captured_with_en_passant = false; }
	bool get_if_piece_captured_with_en_passant() { return captured_with_en_passant; }
};

