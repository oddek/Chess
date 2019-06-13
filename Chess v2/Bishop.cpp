#include "Bishop.h"
#include "Square.h"
#include "Board.h"



Bishop::Bishop(char _symbol) : Piece(_symbol)
{
	if (symbol == 'B') img.loadFromFile("img/wB.png");
	else img.loadFromFile("img/bB.png");
	img_size = img.getSize();
	sprite.setTexture(img);
}


Bishop::~Bishop()
{
}

bool Bishop::check_legal_move(Square* to, Board& board)
{
	//Check destination square occupied by same color:
	if (to->piece != nullptr && to->piece->is_white == is_white) return false;
	//Checking if abs(dy/dx)==1 (if theyre on the same diagonal):
	if (to != current_square &&(abs(to->collumn - current_square->collumn) != abs(to->row - current_square->row))) return false;
	//Checking if piece is blocking path to destination square:
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