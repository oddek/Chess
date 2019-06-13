#include "Board.h"

Board::Board()
{
	states.push_back(start_pos);
	//Draw Board//////////////////////////////////////////////////////////
	board_img.create(board_size, board_size);
	bool white = true;
	for (int i = 1; i < board_size+1; i++)
	{
		for (int j = 1; j < board_size+1; j++)
		{
			if (j % square_size == 0) white = !white;
			if (white) board_img.setPixel(j, i, black_color);
			else board_img.setPixel(j, i, white_color);
		}
		if (i % 100 == 0) white = !white;
	}
	board_texture.loadFromImage(board_img);
	
	board_sprite.setTexture(board_texture);
	board_sprite.setPosition(pixel_offset, pixel_offset);
	en_passant_status = std::make_tuple<Piece*, int>(nullptr, 0);
	
	load_last_state('s');
}

Board::~Board()
{
}

sf::Sprite Board::get_board_sprite()
{
	return board_sprite;
}

void Board::update_sprites()
{
	for (int i = 0; i < pieces.size(); i++)
	{
		pieces[i]->sprite.setPosition
		(pieces[i]->current_square->center.x - (pieces[i]->img_size.x / 2),
			pieces[i]->current_square->center.y - (pieces[i]->img_size.y / 2));
	}
}

//Takes algabraic notation
bool Board::move_piece(std::string move)
{
	//Decomposing string with algebra notation
	Square* from = nullptr;
	Square* to = nullptr;
	for (int i = 0; i < squares.size(); i++)
	{
		//32 and 48 because ascii
		if (squares[i]->collumn == move[0] -32 && squares[i]->row == move[1] - 48)
			from = squares[i];
		if (squares[i]->collumn == move[2] - 32 && squares[i]->row == move[3] - 48)
			to = squares[i];
	}
	//Main check if move is legal
	if (!legal_move(from, to)) return false;
	//Check castle and move rooks if necessary
	if (from->piece->check_if_castled() != 'n')
	{
		Square* R_from = nullptr;
		Square* R_to = nullptr;
		switch (from->piece->check_if_castled())
		{
		case 'K':
			R_from = get_square('H', 1);
			R_to = get_square('F', 1);
			break;
		case 'Q':
			R_from = get_square('A', 1);
			R_to = get_square('D', 1);
			break;
		case 'k':
			R_from = get_square('H', 8);
			R_to = get_square('F', 8);
			break;
		case 'q':
			R_from = get_square('A', 8);
			R_to = get_square('D', 8);
			break;
		}
		R_from->piece->current_square = R_to;
		R_to->piece = R_from->piece;
		R_from->piece = nullptr;

		from->piece->reset_castled();
	}
	//Capturing piece, if square occupied
	if (to->piece != nullptr)
	{
		for (int i = 0; i < pieces.size(); i++)
		{
			if (pieces[i] == to->piece) pieces.erase(pieces.begin() + i);
		}
		//Checking that captured piece isnt pointed to by en_passant_status
		if (std::get<0>(en_passant_status))
		{
			en_passant_target_square = nullptr;
			std::get<0>(en_passant_status) = nullptr;
			std::get<1>(en_passant_status) = 0;
		}
		moves_since_capture = -1;
		delete to->piece;
	}
	//Updating pieces and squares after move.
	to->piece = from->piece;
	from->piece = nullptr;
	to->piece->current_square = to;
	//Checking if the piece moved captured another with en passant
	if (to->piece->get_if_piece_captured_with_en_passant())
	{
		for (int i = 0; i < pieces.size(); i++)
		{
			if (pieces[i] == std::get<0>(en_passant_status))
			{
				pieces.erase(pieces.begin() + i);
				std::get<0>(en_passant_status)->current_square->piece = nullptr;
				to->piece->reset_captured_with_en_passant();
				
				delete std::get<0>(en_passant_status);
				std::get<0>(en_passant_status) = nullptr;
				//delete get_infront_en_passant_target()->piece;
				en_passant_target_square = nullptr;
				
				capture_in_move = true;
			}
		}
	}
	//Handles deletion of en passant status if it hasnt been taken advantage of
	if (std::get<0>(en_passant_status)) 
	{
		std::get<1>(en_passant_status)++;
		if (std::get<1>(en_passant_status) > 1)
		{
			en_passant_target_square = nullptr;
			std::get<0>(en_passant_status)->reset_en_passant();
			std::get<0>(en_passant_status) = nullptr;
			std::get<1>(en_passant_status) = 0;
		}
	}
	//Promote pawn if necessary
	if ((to->piece->symbol == 'P' && to->row == 8) || (to->piece->symbol == 'p' && to->row == 1))
	{
		Piece* p = (to->piece->symbol == 'p') ? new Queen('q') : new Queen('Q');
		p->current_square = to;
		for (int i = 0; i < pieces.size(); i++) if (pieces[i] == to->piece) pieces.erase(pieces.begin() + i);
		delete to->piece;
		to->piece = p;
		pieces.push_back(p);
	}
	//Remove castle flags if necessery:
	char s = to->piece->symbol;
	switch (s)
	{
	case 'K':
		castle_status.erase(std::remove(castle_status.begin(), castle_status.end(), 'K'));
		castle_status.erase(std::remove(castle_status.begin(), castle_status.end(), 'Q'));
		break;
	case 'R':
		if(from->collumn == 'A') castle_status.erase(std::remove(castle_status.begin(), castle_status.end(), 'Q'));
		if(from->collumn == 'H') castle_status.erase(std::remove(castle_status.begin(), castle_status.end(), 'K'));
		break;
	case 'k':
		castle_status.erase(std::remove(castle_status.begin(), castle_status.end(), 'k'));
		castle_status.erase(std::remove(castle_status.begin(), castle_status.end(), 'q'));
		break;
	case 'r':
		if (from->collumn == 'A') castle_status.erase(std::remove(castle_status.begin(), castle_status.end(), 'q'));
		if (from->collumn == 'H') castle_status.erase(std::remove(castle_status.begin(), castle_status.end(), 'k'));
		break;
	}
	//Pushing back algabraic move to vector
	
	//Checking if own king is in check, rolls state back to "current" if so.
	if (check_own_king_in_check(white_to_play))
	{
		load_last_state('c');
		return false;
	}
	//increasing move counters
	if (!to->piece->is_white) moves++;
	moves_since_capture++;
	if (capture_in_move || to->piece->symbol == 'p' || to->piece->symbol == 'P')
	{
		moves_since_capture = 0;
		capture_in_move = false;
	}
	moves_made.push_back(move);
	
	return true;
}

bool Board::legal_move(Square* from, Square* to)
{
	if (from == nullptr || to == nullptr || from->piece == nullptr) return false;
	return from->piece->check_legal_move(to, *this);
}

bool Board::check_own_king_in_check(bool white)
{
	//std::cout << "Sjekker for sjakk!" << std::endl;
	for (auto i : pieces)
	{
		if (!white && i->check_legal_move(white_king->current_square, *this))
		{
			//std::cout << "King in check on square: " << black_king->current_square->collumn << white_king->current_square->row << std::endl;
			//std::cout << "In check by: " << i->symbol << std::endl;
			return true;
		}
		if (white && i->check_legal_move(black_king->current_square, *this))
		{
			//std::cout << "King in check on square: " << black_king->current_square->collumn << black_king->current_square->row << std::endl;
			//std::cout << "In check by: " << i->symbol << std::endl;
			return true;
		}
			
	}
	//std::cout << "Ikke i sjakk" << std::endl;
	return false;
}

//Save state to FEN string and push back to vector of states
void Board::update_current_state()
{
	std::string state = "";
	int i = 0;
	int emptys = 0;
	for (int collumn = 1; collumn < 9; collumn++)
	{
		for (int row = 8; row > 0; row--)
		{
			if (squares[i]->piece != nullptr)
			{
				if (emptys != 0)
				{
					state += (std::to_string(emptys));
					emptys = 0;
				}
				state.push_back(squares[i]->piece->symbol);
			}
			if (squares[i]->piece == nullptr) emptys++;
			
			i++;
		}
		if (emptys != 0) state += std::to_string(emptys);
		emptys = 0;
		if(collumn != 8) state.push_back('/');
	}
	state += ((white_to_play) ? " w " : " b ");
	if (castle_status.empty()) state += "- ";
	else state += castle_status + " ";
	if (en_passant_target_square == nullptr) state += "-";
	else
	{
		state.push_back(en_passant_target_square->collumn + 32);
		state += std::to_string(en_passant_target_square->row);
	}
	state += " " + std::to_string(moves_since_capture) + " " + std::to_string(moves);
	current_state = state;
}

void Board::save_state()
{
	states.push_back(current_state);
}
//Passing 'c' reloads current state
void Board::load_last_state(char ch)
{
	std::string last_state;
	switch (ch)
	{
	case 'c':
		last_state = current_state;
		break;
	case 's':
		last_state = start_pos;
		break;
	default:
		if (states.empty()) last_state = start_pos;
		else
		{
			last_state = states.back();
			states.pop_back();
		}
		break;
	}
	
	//std::cout << "\nLoading following state: " << last_state << std::endl;
	for (auto i : squares) delete i;
	for (auto i : pieces) delete i;
	squares.clear();
	pieces.clear();


	//Creating squares//////////////////////////////////////////////////////////////
	if (!board_flipped)
	{
		sf::Vector2f cur_pos(pixel_offset + square_size/2, pixel_offset + square_size/2);
		for (int i = 8; i > 0; i--)
		{
			for (char col = 'A'; col < 73; col++)
			{
				Square* s = new Square(col, i, cur_pos);
				squares.push_back(s);
				cur_pos.x += square_size;
			}
			cur_pos.x = pixel_offset + square_size/2;
			cur_pos.y += square_size;
		}
	}
	else
	{
		sf::Vector2f cur_pos(pixel_offset + board_size - square_size/2, pixel_offset + board_size-square_size/2);
		for (int i = 8; i > 0; i--)
		{
			for (char col = 'A'; col < 73; col++)
			{
				Square* s = new Square(col, i, cur_pos);
				squares.push_back(s);
				cur_pos.x -= square_size;
			}
			cur_pos.x = board_size + pixel_offset - square_size/2;
			cur_pos.y -= square_size;
		}
	}

	//Denne er altfor klønete, må finne en bedre måte.
	//Create pieces///////////////////////////////////////////////////////
	int square_index = 0;
	int i = 0;
	while(last_state[i] != ' ')
	{
		char ch = last_state[i];

		switch (ch)
		{
		case '/':
			break;
		case '1':
			square_index += 1;
			break;
		case '2':
			square_index += 2;
			break;
		case '3':
			square_index += 3;
			break;
		case '4':
			square_index += 4;
			break;
		case '5':
			square_index += 5;
			break;
		case '6':
			square_index += 6;
			break;
		case '7':
			square_index += 7;
			break;
		case '8':
			square_index += 8;
			break;
			//White pieces
		case 'r':
		{
			Piece* p = new Rook(ch);
			p->current_square = squares[square_index];
			squares[square_index]->piece = p;
			pieces.push_back(p);
			square_index++;
			break;
		}
		case 'n':
		{
			Piece* p = new Knight(ch);
			p->current_square = squares[square_index];
			squares[square_index]->piece = p;
			pieces.push_back(p);
			square_index++;
			break;
		}
		case 'b':
		{
			Piece* p = new Bishop(ch);
			p->current_square = squares[square_index];
			squares[square_index]->piece = p;
			pieces.push_back(p);
			square_index++;
			break;
		}
		case 'q':
		{
			Piece* p = new Queen(ch);
			p->current_square = squares[square_index];
			squares[square_index]->piece = p;
			pieces.push_back(p);
			square_index++;
			break;
		}
		case 'k':
		{
			Piece* p = new King(ch);
			p->current_square = squares[square_index];
			squares[square_index]->piece = p;
			pieces.push_back(p);
			square_index++;
			white_king = p;
			break;
		}
		case 'p':
		{
			Piece* p = new Pawn(ch);
			p->current_square = squares[square_index];
			squares[square_index]->piece = p;
			pieces.push_back(p);
			square_index++;
			break;
		}
		//Black pieces/////////////////////////////////////////////////////////
		case 'R':
		{
			Piece* p = new Rook(ch);
			p->current_square = squares[square_index];
			squares[square_index]->piece = p;
			pieces.push_back(p);
			square_index++;
			break;
		}
		case 'N':
		{
			Piece* p = new Knight(ch);
			p->current_square = squares[square_index];
			squares[square_index]->piece = p;
			pieces.push_back(p);
			square_index++;
			break;
		}
		case 'B':
		{
			Piece* p = new Bishop(ch);
			p->current_square = squares[square_index];
			squares[square_index]->piece = p;
			pieces.push_back(p);
			square_index++;
			break;
		}
		case 'Q':
		{
			Piece* p = new Queen(ch);
			p->current_square = squares[square_index];
			squares[square_index]->piece = p;
			pieces.push_back(p);
			square_index++;
			break;
		}
		case 'K':
		{
			Piece* p = new King(ch);
			p->current_square = squares[square_index];
			squares[square_index]->piece = p;
			pieces.push_back(p);
			square_index++;
			black_king = p;
			break;
		}
		case 'P':
		{
			Piece* p = new Pawn(ch);
			p->current_square = squares[square_index];
			squares[square_index]->piece = p;
			pieces.push_back(p);
			square_index++;
			break;
		}
		case 'w':
			break;
		default:
			break;
		}
		i++;
	}
	i++;
	white_to_play = (last_state[i] == 'w');
	i += 2;
	int n = 0;
	while (last_state[i+n] != ' ') n++;
	if(n != i) castle_status = last_state.substr(i, n);
	i += n+1;
	if (last_state[i] == '-')
	{
		en_passant_target_square = nullptr;
		std::get<0>(en_passant_status) = nullptr;
		std::get<1>(en_passant_status) = 0;
		i += 2;
	}
	else
	{
		en_passant_target_square = get_square(toupper(last_state[i]), (int)last_state[i + 1] + 42);
		i += 3;
	}
	if (get_infront_en_passant_target())
	{
		std::get<0>(en_passant_status) = get_infront_en_passant_target()->piece;
		std::get<1>(en_passant_status) = 1;
	}
	n = 0;
	while (last_state[i+n] != ' ') n++;
	moves_since_capture = std::stoi(last_state.substr(i, n));
	i += n+1;
	n = 0;
	while (last_state[i+n] != ' ') n++;
	moves = std::stoi(last_state.substr(i, n));

	current_state = last_state;
}

Square* Board::get_square(char collumn, int row)
{
	for (auto i : squares)
		if (i->collumn == collumn && i->row == row) return i;
	return nullptr;

}

Square* Board::get_infront_en_passant_target()
{
	if (en_passant_target_square == nullptr) return nullptr;
	if (en_passant_target_square->row == 6)
	{
		return get_square(en_passant_target_square->collumn, 5);
	}
	else return get_square(en_passant_target_square->collumn, 4);
}

void Board::print_state()
{
	std::cout << current_state << std::endl;
}

char Board::check_game_over()
{
	//Fifty move rule
	if (moves_since_capture >= 50) return 'd';
	//Checking insufficient material
	if (pieces.size() < 3) return 'i';
	if (pieces.size() < 4)
	{
		std::string symbols;
		for (auto i : pieces) symbols.push_back(i->symbol);
		if (symbols.find('R') == -1 && symbols.find('r') == -1 &&
			symbols.find('Q') == -1 && symbols.find('q') == -1 &&
			symbols.find('P') == -1 && symbols.find('p') == -1)
			return 'i';
	}
	if (pieces.size() < 5)
	{
		std::string symbols;
		for (auto i : pieces) symbols.push_back(i->symbol);
		if (symbols.find('b') != -1 && symbols.find('B') != -1 ||
			symbols.find('b') != -1 && symbols.find('N') != -1 ||
			symbols.find('n') != -1 && symbols.find('B') != -1 ||
			symbols.find('N') != -1 && symbols.find('N') != -1)
			return 'i';

			
	}
	/*
	//Sjekker stale og checkmate:
	for(auto from : squares)
	{
		if (from->piece == nullptr || from->piece->is_white != white_to_play) continue;
		for (auto to : squares)
		{
			//if (s == i->current_square) continue;
			std::string move = "";

			//Legger til current square
			move += (from->collumn + 32);
			move += (from->row + 48);
			//Legger til ny square
			move += (to->collumn + 32);
			move += (to->row + 48);
			std::cout << "Testing current move: " << move << std::endl;
			if (move_piece(move))
			{
				load_last_state('c');
				return 'f';
			}
		}
	}*/

	for (char col = 'a'; col <= 'h'; col++)
	{
		for (int row = 1; row <= 8; row++)
		{
			if (get_square(col-32, row)->piece == nullptr || get_square(col-32, row)->piece->is_white != white_to_play) continue;

			for (char col2 = 'a'; col2 <= 'h'; col2++)
			{
				for (int row2 = 1; row2 <= 8; row2++)
				{
					std::string move = "";
					move += col + std::to_string(row) + col2 + std::to_string(row2);
					//std::cout << "Testing current move: " << move << std::endl;
					if (move_piece(move))
					{
						load_last_state('c');
						return 'f';
					}
				}
			}
		}
	}

	if (check_own_king_in_check(white_to_play)) return 'c';
	return 's';
}