#include "Pawn.h"
#include "Square.h"
#include "Board.h"


Pawn::Pawn(char _symbol): Piece(_symbol)
{
	if (symbol == 'P') img.loadFromFile("img/wP.png");
	else img.loadFromFile("img/bP.png");
	img_size = img.getSize();
	sprite.setTexture(img);
}

Pawn::~Pawn()
{
}

bool Pawn::check_legal_move(Square* to, Board& board)
{
	//Check destination square occupied by own color
	bool return_val = false;
	if (to->piece != nullptr && to->piece->is_white == is_white) return false;
	int dx = to->collumn - current_square->collumn;
	int dy = to->row - current_square->row;
	//White pawns//////////////////////////////////////////////
	if (is_white)
	{
		if (dx == 0 && to->piece != nullptr) return false;
		else if (dx == 0 && get_square(current_square->collumn, current_square->row + 1, board.squares)->piece != nullptr) return false;
		else if (has_moved && dx == 0) return_val = (dy == 1);
		else if (!has_moved && dx == 0)
		{
			return_val = (dy == 1 || dy == 2);
			if (dy == 2)
			{
				board.en_passant_target_square = get_square(current_square->collumn, current_square->row + 1, board.squares);
				en_passant = true;
				std::get<0>(board.en_passant_status) = this;
				std::get<1>(board.en_passant_status) = 0;
			}
		}
		else if (abs(dx) == 1 && dy == 1)
		{
			if (to->piece != nullptr) return_val = true;
			//else if(board.en_passant_target_square != nullptr && board.en_passant_target_square == to)
			//{
				else if (get_square(current_square->collumn + dx,
					current_square->row, board.squares)->piece != nullptr &&
				get_square(current_square->collumn + dx,
					current_square->row, board.squares)
				->piece->get_en_passant() == true)
			{
				captured_with_en_passant = true;
				return_val = true;
			}
		}
	}
	//Black pawns//////////////////////////////////////////////////////////
	else
	{
		if (dx == 0 && to->piece != nullptr) return false;
		else if (dx == 0 && get_square(current_square->collumn, current_square->row - 1, board.squares)->piece != nullptr) return false;
		else if (has_moved && dx == 0) return_val = (dy == -1);
		else if (!has_moved && dx == 0)
		{
			return_val = (dy == -1 || dy == -2);
			if (dy == -2)
			{
				board.en_passant_target_square = get_square(current_square->collumn, current_square->row - 1, board.squares);
				en_passant = true;
				std::get<0>(board.en_passant_status) = this;
				std::get<1>(board.en_passant_status) = 0;
			}
		}
		else if (abs(dx) == 1 && dy == -1)
		{
			if (to->piece != nullptr) return_val = true;
			//else if (board.en_passant_target_square != nullptr && board.en_passant_target_square == to)
			//{
			else if (get_square(current_square->collumn + dx,
				current_square->row, board.squares)->piece != nullptr &&
				get_square(current_square->collumn + dx,
					current_square->row, board.squares)
				->piece->get_en_passant() == true)
			{
				captured_with_en_passant = true;
				return_val = true;
				
			}
		}
	}
	if (return_val == true) has_moved = true;
	return return_val;
}