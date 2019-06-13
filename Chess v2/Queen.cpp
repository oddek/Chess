#include "Queen.h"
#include "Square.h"
#include "Board.h"


Queen::Queen(char _symbol): Piece(_symbol)
{
	if (symbol == 'Q') img.loadFromFile("img/wQ.png");
	else img.loadFromFile("img/bQ.png");
	img_size = img.getSize();
	sprite.setTexture(img);
}


Queen::~Queen()
{
}

bool Queen::check_legal_move(Square* to, Board& board)
{
	//Check destination square occupied by own color
	if (to->piece != nullptr && to->piece->is_white == is_white) return false;
	//Check same square
	if (to == current_square) return true;
	//Straight horizontal or vertical movement
	if (current_square->collumn == to->collumn)
	{
		for (int i = std::min(current_square->row, to->row) + 1; i < std::max(current_square->row, to->row); i++)
			if (get_square(current_square->collumn, i, board.squares)->piece != nullptr)
				return false;
		return true;
	}
	else if (current_square->row == to->row)
	{
		for (char ch = std::min(current_square->collumn, to->collumn) + 1; ch < std::max(current_square->collumn, to->collumn); ch++)
			if (get_square(ch, current_square->row, board.squares)->piece != nullptr)
				return false;
		return true;
	}
	//Checking if abs(dy/dx)==1 (if theyre on the same diagonal):
	else if (abs(to->collumn - current_square->collumn) == abs(to->row - current_square->row))
		//Checking if piece is blocking path to destination square:
	{
		int i = current_square->row;
		int ch = current_square->collumn;
		while (abs(ch - to->collumn) > 1 && abs(i - to->row) > 1)
		{
			if (ch < to->collumn) ch++;
			else ch--;
			if (i < to->row) i++;
			else i--;
			if (get_square(ch, i, board.squares)->piece != nullptr) return false;
		}
		return true;
	}
	else return false;
}
