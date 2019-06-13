#include "King.h"
#include "Square.h"
#include "Board.h"


King::King(char _symbol): Piece(_symbol)
{
	if (symbol == 'K') img.loadFromFile("img/wK.png");
	else img.loadFromFile("img/bK.png");
	img_size = img.getSize();
	sprite.setTexture(img);
}


King::~King()
{
}

bool King::check_legal_move(Square* to, Board& board)
{
	//Check destination square occupied by own color
	if (to->piece != nullptr && to->piece->is_white == is_white) return false;
	//Check Castle
	if (check_castle(to, board)) return true;
	

	//Check if square is just one step away
	int dx = abs(current_square->collumn - to->collumn);
	int dy = abs(current_square->row - to->row);
	if (current_square->collumn == to->collumn) return (dy == 1) ? true : false;
	else if (current_square->row == to->row) return (dx == 1) ? true : false;
	else if (dx + dy == 2) return true;
	else return false;
}

bool King::check_castle(Square* to, Board& board)
{
	//White kingside
	if (is_white && (to == get_square('G', 1, board.squares)) && ((board.castle_status.find('K') != -1)))
	{
		if (get_square('F', 1, board.squares)->piece != nullptr) return false;
		if (get_square('G', 1, board.squares)->piece != nullptr) return false;
		for (auto i : board.pieces)
		{
			if (!i->is_white && i->check_legal_move(get_square('G', 1, board.squares), board)) return false;
			if (!i->is_white && i->check_legal_move(get_square('F', 1, board.squares), board)) return false;
			if (!i->is_white && i->check_legal_move(get_square('E', 1, board.squares), board)) return false;
		}
		castled = 'K';
		return true;
	}
	//White queenside
	else if (is_white && (to == get_square('C', 1, board.squares)) && ((board.castle_status.find('Q') != -1)))
	{
		if (get_square('B', 1, board.squares)->piece != nullptr) return false;
		if (get_square('C', 1, board.squares)->piece != nullptr) return false;
		if (get_square('D', 1, board.squares)->piece != nullptr) return false;
		
		for (auto i : board.pieces)
		{
			if (!i->is_white && i->check_legal_move(get_square('C', 1, board.squares), board)) return false;
			if (!i->is_white && i->check_legal_move(get_square('D', 1, board.squares), board)) return false;
			if (!i->is_white && i->check_legal_move(get_square('E', 1, board.squares), board)) return false;
		}
		castled = 'Q';
		return true;
	}
	//Black Queenside
	else if (!is_white && (to == get_square('C', 8, board.squares)) && ((board.castle_status.find('q') != -1)))
	{
		if (get_square('B', 8, board.squares)->piece != nullptr) return false;
		if (get_square('C', 8, board.squares)->piece != nullptr) return false;
		if (get_square('D', 8, board.squares)->piece != nullptr) return false;
		for (auto i : board.pieces)
		{
			if (i->is_white && i->check_legal_move(get_square('C', 8, board.squares), board)) return false;
			if (i->is_white && i->check_legal_move(get_square('D', 8, board.squares), board)) return false;
			if (i->is_white && i->check_legal_move(get_square('E', 8, board.squares), board)) return false;
		}
		castled = 'q';
		return true;
	}
	//Black Kingside
	else if (!is_white && (to == get_square('G', 8, board.squares)) && ((board.castle_status.find('k') != -1)))
	{
		if (get_square('F', 8, board.squares)->piece != nullptr) return false;
		if (get_square('G', 8, board.squares)->piece != nullptr) return false;
		for (auto i : board.pieces)
		{
			if (i->is_white && i->check_legal_move(get_square('G', 8, board.squares), board)) return false;
			if (i->is_white && i->check_legal_move(get_square('F', 8, board.squares), board)) return false;
			if (i->is_white && i->check_legal_move(get_square('E', 8, board.squares), board)) return false;
		}
		castled = 'k';
		return true;
	}
	return false;
}