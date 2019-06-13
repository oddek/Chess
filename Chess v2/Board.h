#pragma once
#include "Square.h"
#include <vector>
#include <tuple>
#include <algorithm>
#include "Piece.h"
#include "Rook.h"
#include "Pawn.h"
#include "King.h"
#include "Queen.h"
#include "Bishop.h"
#include "Knight.h"

class Board
{
public:
	Board();
	~Board();
	//Color to play
	bool board_flipped = false;
	//Functions/////////////////////////////////
	void update_sprites();
	bool move_piece(std::string move);
	bool legal_move(Square* from, Square* to);
	bool check_own_king_in_check(bool white);
	char check_game_over();
	bool stalemate();
	//Board sprites////////////////////////
	sf::Sprite get_board_sprite();
	//Square and piece data//////////
	std::vector<Square*> squares;
	std::vector<Piece*> pieces;
	//Save all moves in algabraic notation
	std::vector<std::string> moves_made;
	//En passant som faktisk brukes.
	std::tuple<Piece*, int> en_passant_status;
	//States//////////////////////////////
	bool white_to_play = true;
	std::string castle_status = "KQkq";
	int moves_since_capture = 0;
	int moves;
	//Used only with saving and loading state of position
	Square* en_passant_target_square;
	std::vector<std::string> states;
	void update_current_state();
	void save_state();
	std::string current_state;
	void print_state();
	void load_last_state(char ch);
	//std::string start_pos = "4r1k1/p4ppp/N2p4/1q6/1P2bP2/3QP1P1/6rP/R4K2 b - - 6 27";
	std::string start_pos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	int board_size = 800;
	int square_size = 100;
	int pixel_offset = 50;
private:
	//Board Image///////////////////
	sf::Image board_img;
	sf::Texture board_texture;
	sf::Sprite board_sprite;
	
	Piece* black_king;
	Piece* white_king;
	Square* get_square(char collumn, int row);

	bool capture_in_move = false;

	Square* get_infront_en_passant_target();

	sf::Color black_color{ 255, 243, 224 };
	sf::Color white_color{ 220, 118, 51 };
		
	
};

