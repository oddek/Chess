#include "Knight.h"
#include "Square.h"
#include "Board.h"



Knight::Knight(char _symbol) : Piece(_symbol)
{
	if (symbol == 'N') img.loadFromFile("img/wN.png");
	else img.loadFromFile("img/bN.png");
	img_size = img.getSize();
	sprite.setTexture(img);
}


Knight::~Knight()
{
}

bool Knight::check_legal_move(Square* to, Board& board)
{
	//Check destination square occupied by own color
	if (to->piece != nullptr && to->piece->is_white == is_white) return false;

	//Check valid move pattern
	int dx = abs(current_square->collumn - to->collumn);
	int dy = abs(current_square->row - to->row);
	
	if((dx == 1 && dy == 2) || (dy == 1 && dx == 2)) return true;
	return false;
}