#include "Rook.h"
#include "Square.h"
#include "Board.h"

Rook::Rook(char _symbol): Piece(_symbol)
{
	//Setting sprite;
	if (symbol == 'R') img.loadFromFile("img/wR.png");
	else img.loadFromFile("img/bR.png");
	img_size = img.getSize();
	sprite.setTexture(img);
}


Rook::~Rook()
{
}

bool Rook::check_legal_move(Square* to, Board& board)
{
	//Check destination square occupied by own color
	if (to->piece != nullptr && to->piece->is_white == is_white) return false;
	//Check if path is blocked
	if (current_square->collumn == to->collumn)
	{
		for(int i = std::min(current_square->row, to->row) + 1; i < std::max(current_square->row, to->row); i++)
			if (get_square(current_square->collumn, i, board.squares)->piece != nullptr)
				return false;
		return true;
	}
	else if (current_square->row == to->row)
	{
		for(char ch = std::min(current_square->collumn, to->collumn) + 1; ch < std::max(current_square->collumn, to->collumn); ch++)
			if (get_square(ch, current_square->row, board.squares)->piece != nullptr)
				return false;
		return true;
	}
	else return false;
}
