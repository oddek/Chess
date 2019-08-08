#include <SFML/Graphics.hpp>
#include <Windows.h>
#include "Board.h"
#include "Stockfish.h"



//En passant i Board.cpp og Pawn.cpp bør forbedres. FUnker greit nå, men kan forenkles.


//Functions/////////////////////////////////////////////////////////
Square* get_square_from_pixels(Board& board, sf::Vector2i m_pos);
int map_onto(int x, int a, int b, int c, int d);
void get_stockfish_move(Board& board);
void update_window(sf::RenderWindow& window, Board& board, std::vector<sf::RectangleShape*> shapes, std::vector<sf::Text*> texts);
bool check_game_over(sf::RenderWindow& window, Board& board, sf::Text& eog_text);
//MAIN////////////////////////////////////////////////////////////////

int main()
{
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	//Player color
	bool player_is_white = true;
	bool game_over = false;
	//Setting up board///////////////////////////////////////////////
	Board board;
	board.current_state = board.start_pos;
	board.update_sprites();
	//Text////////////////////////////////////////////////////////////
	std::vector<sf::Text*> texts;
	std::vector<sf::RectangleShape*> shapes;
	//Init stockfish
	char stock_path[] = "stockfish.exe";
	ConnectToEngine(stock_path);
	//Variables/////////////////////////////////////////////////////
	bool move = false;
	int current_index = 0;
	float dx = 0;
	float dy = 0;
	int button_width = 200;
	int button_height = 100;
	int button_text_size = 15;
	bool player1_human = true;
	bool player2_human = true;
	bool show_eval = true;
	//Setting up window//////////////////////////////////////////////
	sf::RenderWindow window(sf::VideoMode(1450, 1000), "Chess!", sf::Style::Titlebar | sf::Style::Close);
	sf::Image icon;
	icon.loadFromFile("img/bN.png");
	window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
	//Setting up textfont/////////////////////////////////////////////
	sf::Font font;
	if (!font.loadFromFile("OpenSans-Light.ttf"))
	{
		return 1;
	}
	sf::Font summitfont;
	if (!summitfont.loadFromFile("SummitLight-Display.ttf"))
	{
		return 1;
	}
	//HEADLINE TEXT/////////////////////////////////////////////////////
	sf::Text headline("CHESS", summitfont, 100);
	headline.setFillColor(sf::Color::White);
	//headline.setStyle(sf::Text::Bold | sf::Text::Underlined);
	sf::FloatRect textRect = headline.getLocalBounds();
	headline.setOrigin(textRect.left + textRect.width / 2.0f,
		textRect.top + textRect.height / 2.0f);
	headline.setPosition(sf::Vector2f((window.getSize().x - (window.getSize().x - board.board_size-board.pixel_offset)/2), 150));
	texts.push_back(&headline);
	//NEW GAME-button////////////////////////////////////////////////////////
	sf::RectangleShape new_button(sf::Vector2f(button_width, button_height));
	new_button.setOrigin(button_width/2, button_height/2);
	new_button.setFillColor(sf::Color::Black);
	new_button.setPosition(sf::Vector2f(window.getSize().x - (window.getSize().x - board.board_size-board.pixel_offset)*3 / 4, 400));
	shapes.push_back(&new_button);
	
	sf::Text new_game_text("New game", font, button_text_size);
	new_game_text.setFillColor(sf::Color::White);

	sf::FloatRect new_rectBounds = new_button.getGlobalBounds();
	sf::FloatRect new_textBounds = new_game_text.getGlobalBounds();

	new_game_text.setPosition(
		new_rectBounds.left + (new_rectBounds.width / 2) - (new_textBounds.width / 2),
		new_rectBounds.top + (new_rectBounds.height / 2) - (new_textBounds.height / 2));

	texts.push_back(&new_game_text);

	//UNDO-button/////////////////////////////////////////////////////////
	sf::RectangleShape undo_button(sf::Vector2f(button_width, button_height));
	undo_button.setOrigin(button_width/2, button_height/2);
	undo_button.setFillColor(sf::Color::Black);
	undo_button.setPosition(sf::Vector2f(window.getSize().x - (window.getSize().x - board.board_size-board.pixel_offset) / 4, 400));
	shapes.push_back(&undo_button);

	sf::Text undo_text("Undo move", font, button_text_size);
	undo_text.setFillColor(sf::Color::White);

	sf::FloatRect undo_rectBounds = undo_button.getGlobalBounds();
	sf::FloatRect undo_textBounds = undo_text.getGlobalBounds();

	undo_text.setPosition(
		undo_rectBounds.left + (undo_rectBounds.width / 2) - (undo_textBounds.width / 2),
		undo_rectBounds.top + (undo_rectBounds.height / 2) - (undo_textBounds.height / 2));

	texts.push_back(&undo_text);
	//Player1 button//////////////////////////////////////////////////////////////////////
	sf::RectangleShape player1_button(sf::Vector2f(button_width, button_height));
	player1_button.setOrigin(button_width / 2, button_height / 2);
	player1_button.setFillColor(sf::Color::Black);
	player1_button.setPosition(sf::Vector2f(window.getSize().x - (window.getSize().x - board.board_size-board.pixel_offset)*3 / 4, 600));
	shapes.push_back(&player1_button);

	sf::Text player1_text("White:\nHuman", font, button_text_size);
	player1_text.setFillColor(sf::Color::White);

	sf::FloatRect player1_rectBounds = player1_button.getGlobalBounds();
	sf::FloatRect player1_textBounds = player1_text.getGlobalBounds();

	player1_text.setPosition(
		player1_rectBounds.left + (player1_rectBounds.width / 2) - (player1_textBounds.width / 2),
		player1_rectBounds.top + (player1_rectBounds.height / 2) - (player1_textBounds.height / 2));

	texts.push_back(&player1_text);
	//Player2 button//////////////////////////////////////////////////////////////////////
	sf::RectangleShape player2_button(sf::Vector2f(button_width, button_height));
	player2_button.setOrigin(button_width / 2, button_height / 2);
	player2_button.setFillColor(sf::Color::Black);
	player2_button.setPosition(sf::Vector2f(window.getSize().x - (window.getSize().x - board.board_size-board.pixel_offset) / 4, 600));
	shapes.push_back(&player2_button);

	sf::Text player2_text("Black:\nHuman", font, button_text_size);
	player2_text.setFillColor(sf::Color::White);

	sf::FloatRect player2_rectBounds = player2_button.getGlobalBounds();
	sf::FloatRect player2_textBounds = player2_text.getGlobalBounds();

	player2_text.setPosition(
		player2_rectBounds.left + (player2_rectBounds.width / 2) - (player2_textBounds.width / 2),
		player2_rectBounds.top + (player2_rectBounds.height / 2) - (player2_textBounds.height / 2));

	texts.push_back(&player2_text);
	//FLIPBOARD-button/////////////////////////////////////
	sf::RectangleShape flip_board_button(sf::Vector2f(button_width, button_height));
	flip_board_button.setOrigin(button_width / 2, button_height / 2);
	flip_board_button.setFillColor(sf::Color::Black);
	flip_board_button.setPosition(sf::Vector2f(window.getSize().x - (window.getSize().x - board.board_size-board.pixel_offset)*3 / 4, 800));
	shapes.push_back(&flip_board_button);

	sf::Text flip_board_text("Flip board", font, button_text_size);
	flip_board_text.setFillColor(sf::Color::White);

	sf::FloatRect flip_board_rectBounds = flip_board_button.getGlobalBounds();
	sf::FloatRect flip_board_textBounds = flip_board_text.getGlobalBounds();

	flip_board_text.setPosition(
		flip_board_rectBounds.left + (flip_board_rectBounds.width / 2) - (flip_board_textBounds.width / 2),
		flip_board_rectBounds.top + (flip_board_rectBounds.height / 2) - (flip_board_textBounds.height / 2));

	texts.push_back(&flip_board_text);
	//Show_hide eval button
	sf::RectangleShape show_eval_button(sf::Vector2f(button_width, button_height));
	show_eval_button.setOrigin(button_width / 2, button_height / 2);
	show_eval_button.setFillColor(sf::Color::Black);
	show_eval_button.setPosition(sf::Vector2f(window.getSize().x - (window.getSize().x - board.board_size-board.pixel_offset) / 4, 800));
	shapes.push_back(&show_eval_button);

	sf::Text show_eval_text("Show//hide\nevaluation", font, button_text_size);
	show_eval_text.setFillColor(sf::Color::White);

	sf::FloatRect show_eval_rectBounds = show_eval_button.getGlobalBounds();
	sf::FloatRect show_eval_textBounds = show_eval_text.getGlobalBounds();

	show_eval_text.setPosition(
		show_eval_rectBounds.left + (show_eval_rectBounds.width / 2) - (show_eval_textBounds.width / 2),
		show_eval_rectBounds.top + (show_eval_rectBounds.height / 2) - (show_eval_textBounds.height / 2));

	texts.push_back(&show_eval_text);
	//End of game text//////////////////////////////////////
	sf::Text eog_text;
	eog_text.setFont(summitfont);
	eog_text.setCharacterSize(50);
	eog_text.setFillColor(sf::Color::Red);
	eog_text.setStyle(sf::Text::Bold | sf::Text::Underlined);
	texts.push_back(&eog_text);
	//Evaltext////////////////////////////////////////////////////////////
	sf::Text eval_text{ std::to_string(get_eval(board.current_state)), font, 30 };
	eval_text.setFillColor(sf::Color::White);
	sf::FloatRect eval_textRect = eval_text.getLocalBounds();
	eval_text.setOrigin(eval_textRect.left + eval_textRect.width / 2.0f,
		eval_textRect.top + eval_textRect.height / 2.0f);
	eval_text.setPosition(sf::Vector2f(board.board_size/2  + board.pixel_offset, board.board_size + 100 ));
	if (show_eval) eval_text.setString(std::to_string(get_eval(board.current_state)) + "\nBest move: " + getNextMove(board.current_state));
	texts.push_back(&eval_text);
	//Main LOOP//////////////////////////////////////////////////////
	while (window.isOpen())
	{	
		//Mouse_position
		sf::Vector2i m_pos = sf::Mouse::getPosition(window);
		sf::Event event;
		while (window.pollEvent(event))
		{
			//Close window/////////////////////////////////////////////
			if (event.type == sf::Event::Closed)
			{
				window.close();
				return 0;
			}
			//RESIZE window///////////////////////////////////////////
			if (event.type == sf::Event::Resized)
			{
				sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
				window.setView(sf::View(visibleArea));
			}
			
			//Drag and drop pieces////////////////////////////////////////
			//Left_button////////////////////////////////////////////////
			if (event.type == sf::Event::MouseButtonPressed)
				if (event.key.code == sf::Mouse::Left)
				{//New button push
					if (new_button.getGlobalBounds().contains(m_pos.x, m_pos.y))
					{
						board.load_last_state('s');
						game_over = false;
						board.update_sprites();
						eog_text.setString("");
						
					}
					//Undo button push
					if (undo_button.getGlobalBounds().contains(m_pos.x, m_pos.y))
					{
						board.load_last_state('a');
						board.update_sprites();
					}
					//Whitehuman button push
					if (player1_button.getGlobalBounds().contains(m_pos.x, m_pos.y))
					{
						player1_human = !player1_human;
						if (player1_human) player1_text.setString("White:\nHuman");
						else player1_text.setString("White:\nStockfish");
					}
					//Blackhuman button push
					if (player2_button.getGlobalBounds().contains(m_pos.x, m_pos.y))
					{
						player2_human = !player2_human;
						if (player2_human) player2_text.setString("Black:\nHuman");
						else player2_text.setString("Black:\nStockfish");
					}
					//FLip board
					if (flip_board_button.getGlobalBounds().contains(m_pos.x, m_pos.y))
					{
						board.board_flipped = !board.board_flipped;
						board.load_last_state('c');
						board.update_sprites();
					}
					//Show/hide eval
					if (show_eval_button.getGlobalBounds().contains(m_pos.x, m_pos.y))
					{
						show_eval = !show_eval;
						if (show_eval) eval_text.setString(std::to_string(get_eval(board.current_state)) + "\nBest move: " + getNextMove(board.current_state));
						else (eval_text.setString(""));
					}
					//Checking if piece was pressed
					for (int i = 0; i < board.pieces.size(); i++)
					{
						if (board.pieces[i]->sprite.getGlobalBounds().contains(m_pos.x, m_pos.y))//piece_sprites[i].getGlobalBounds().contains(m_pos.x, m_pos.y))
						{
							move = true;
							current_index = i;
							dx = m_pos.x - board.pieces[i]->sprite.getPosition().x;//piece_sprites[i].getPosition().x;
							dy = m_pos.y - board.pieces[i]->sprite.getPosition().y;//piece_sprites[i].getPosition().y;
						}
					}
				}
			//Right_button - Prints FENstates and stuff to console /////////////////////////
			if (event.type == sf::Event::MouseButtonPressed)
				if (event.key.code == sf::Mouse::Right)
				{
					//for (auto i : board.states) std::cout << i << std::endl;
					//std::cout << "current: " << std::endl;
					board.print_state();

					board.update_current_state();
					//std::cout << "new updated state: " << std::endl;
					board.print_state();
					std::string stock_move = getNextMove(board.current_state);
					/*if (board.move_piece(stock_move))
					{
						board.save_state();
						board.update_current_state();
						//if(!board.check_king_in_check(!is_white))
					}*/
					//std::cout << "Stock_move: " << stock_move << std::endl;
					board.update_sprites();
				}
			//Releasing left button////////////////////////////////////////////////
			if (event.type == sf::Event::MouseButtonReleased)
				if (event.key.code == sf::Mouse::Left)
				{
					if (move)
					{
						move = false;
						Square* new_square = get_square_from_pixels(board, m_pos);
						if (new_square != nullptr && new_square != board.pieces[current_index]->current_square
							&& board.white_to_play == board.pieces[current_index]->is_white)
						{
							std::string move = "";
							//Legger til current square
							move += (board.pieces[current_index]->current_square->collumn + 32);
							move += (board.pieces[current_index]->current_square->row + 48);
							//Legger til ny square
							move += (new_square->collumn + 32);
							move += (new_square->row + 48);
							//std::cout << "Move: " << move << std::endl;
							if (board.move_piece(move))
							{
								board.save_state();
								board.white_to_play = !board.white_to_play;
								board.update_current_state();
								board.update_sprites();
								update_window(window, board, shapes, texts);
								if (check_game_over(window, board, eog_text)) game_over = true;
								//if(!board.check_king_in_check(!is_white))
								if (show_eval) eval_text.setString(std::to_string(get_eval(board.current_state)) + "\nBest move: " + getNextMove(board.current_state));
							}
						}
						board.update_sprites();
						update_window(window, board, shapes, texts);

					}
				}

			//Keyboard Z, UNDO. Loading last state.
			if (event.type == sf::Event::KeyPressed)
				if (event.key.code == sf::Keyboard::Z)
				{
					board.load_last_state('a');
					board.update_sprites();
				}
		}
		//Moving piece///////////////////////////////////////////////////////////////
		if (move)
		{
			board.pieces[current_index]->sprite.setPosition(m_pos.x - dx, m_pos.y - dy);
		}
		//Stockfish
		if (!game_over && ((board.white_to_play && !player1_human) || !board.white_to_play && !player2_human))
		{
			//std::cout << "current: " << std::endl;
			board.update_current_state();
			board.print_state();
			std::string stock_move = getNextMove(board.current_state);
			if (board.move_piece(stock_move))
			{
				board.save_state();
				board.white_to_play = !board.white_to_play;
				board.update_current_state();
				if (check_game_over(window, board, eog_text)) game_over = true;
				//if(!board.check_king_in_check(!is_white))
				if (show_eval) eval_text.setString(std::to_string(get_eval(board.current_state)) + "\nBest move: " + getNextMove(board.current_state));
			}
			//std::cout << "Stock_move: " << stock_move << std::endl;
			board.update_sprites();

		}
		//Drawing to window////////////////////////////////////////////
		update_window(window, board, shapes, texts);
	}
	return 0;
}

Square* get_square_from_pixels(Board& board, sf::Vector2i m_pos)
{
	int upper[8] = { 799, 699, 599, 499, 399, 299, 199, 99 };
	int lower[8] = { 700, 600, 500, 400, 300, 200, 100, 0 };
	for (int i = 0; i < 8; i++)
	{
		upper[i] += board.pixel_offset;
		lower[i] += board.pixel_offset;
	}
	char col = 'A';
	int row = 0;
	if (!board.board_flipped)
	{
		col = (m_pos.x - board.pixel_offset) / 100 + 65;
		std::cout << col << std::endl;
		row = 0;
		for (int i = 0; i < 8; i++)
		{
			if (m_pos.y <= upper[i] && m_pos.y >= lower[i]) row = i + 1;
		}
		std::cout << row << std::endl;
	}
	else
	{
		row = ((m_pos.y - board.pixel_offset) / 100) + 1;
		col = 'A';
		for (int i = 0; i < 8; i++)
		{
			if (m_pos.x <= upper[i] && m_pos.x >= lower[i]) col += i;
		}
	}
	for (int i = 0; i < board.squares.size(); i++)
	{
		if (board.squares[i]->collumn == col && board.squares[i]->row == row) return board.squares[i];
	}
	return nullptr;
}

int map_onto(int x, int a, int b, int c, int d)
{
	return (x - a)*((d - c) / (b - a)) + c;
}

void get_stockfish_move(Board& board)
{
	
}

void update_window(sf::RenderWindow& window, Board& board, std::vector<sf::RectangleShape*> shapes, std::vector<sf::Text*> texts)
{
	window.clear(sf::Color(0x251805ff));
	window.draw(board.get_board_sprite());
	for (auto i : board.pieces) window.draw(i->sprite);
	for (auto i : shapes) window.draw(*i);
	for (auto i : texts) window.draw(*i);
	window.display();
}

bool check_game_over(sf::RenderWindow& window, Board& board, sf::Text& eog_text)
{
	char status = board.check_game_over();
	switch (status)
	{
	case 'f':
		return false;
	case 'c':
		//Display checkmate
		eog_text.setString("CheckMate");
		break;

	case 'i':
		//Display 'Game drawn by insufficient material
		eog_text.setString("Game drawn by\ninsufficient material");
		break;

	case 'd':
		//Display game draw by fifty move rule
		eog_text.setString("Game drawn by\nfifty move rule");
		break;
	case 's':
		//Display stalemate
		eog_text.setString("Game drawn by\nstalemate");
		break;
	}
	//std::cout << "Game over: " << status << std::endl;


	sf::FloatRect textRect = eog_text.getLocalBounds();
	eog_text.setOrigin(textRect.left + textRect.width / 2.0f,
		textRect.top + textRect.height / 2.0f);
	eog_text.setPosition(sf::Vector2f((board.board_size) / 2.0f + board.pixel_offset, (board.board_size) / 2.0f + board.pixel_offset));
	
	return true;
}