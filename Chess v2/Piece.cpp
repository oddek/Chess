#include "Piece.h"
#include "Square.h"
#include "Board.h"



Piece::Piece()
{
	
}

Piece::Piece(char _symbol): symbol(_symbol)
{
	//Setting color
	if (symbol > 96) is_white = false;
	else is_white = true;
}


Piece::~Piece()
{
}

Square* Piece::get_square(char collumn, int row, std::vector<Square*>& squares)
{
	for (auto i : squares)
		if (i->collumn == collumn && i->row == row) return i;
	return nullptr;
	
}
