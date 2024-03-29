﻿#include "Game.h"

Game::Game()
{
	is_white_turn = true;
	board = Board();
	this->create_figures();

}

Game::~Game()
{
	auto my_board = this->get_board().get_board();
	for (auto itr_x = my_board.begin(); itr_x != my_board.end(); ++itr_x)
	{
		for (auto itr_y = (*itr_x).begin(); itr_y != (*itr_x).end(); ++itr_y)
		{
			delete* itr_y;
		}
	}
}

Board Game::get_board()
{
	return board;
}

Player Game::get_player()const
{
	if (is_white_turn)
	{
		return player_white;
	}
	else
	{
		return player_black;
	}
}

Player Game::get_enemy_player()const
{
	if (is_white_turn)
	{
		return player_black;
	}
	else
	{
		return player_white;
	}
}

std::vector<Figure*> Game::get_possible_checking_figures()
{
	std::vector<Figure*> get_possible_checking_figures;
	Player current_player = get_player();
	Board board_backup = board;
	std::list<Figure*> figures_to_check = current_player.get_player_figures();
	for (auto iter = figures_to_check.begin(); iter != figures_to_check.end(); ++iter)
	{
		auto temp_board = board.get_board();
		temp_board[(*iter)->get_position()[0]][(*iter)->get_position()[1]] = nullptr;
		board.swap_board(temp_board);
		auto checking_figures = get_checking_figures();
		if (checking_figures.size() == 1)
		{
			for (auto iter = checking_figures.begin(); iter != checking_figures.end(); ++iter)
			{
				get_possible_checking_figures.push_back((*iter));
			}
		}
		board.swap_board(board_backup.get_board());
	}
	return get_possible_checking_figures;
}

void Game::restart_game()
{
	auto my_board = this->get_board().get_board();
	for (auto itr_x = my_board.begin(); itr_x != my_board.end(); ++itr_x)
	{
		for (auto itr_y = (*itr_x).begin(); itr_y != (*itr_x).end(); ++itr_y)
		{
			delete* itr_y;
		}
	}

	board.clear_board();
	player_black.clear_player();
	player_white.clear_player();
	create_figures();
}

void Game::make_move(Figure* current_figure, int new_x, int new_y)
{

	if (board.get_figure(new_x, new_y) != nullptr)
	{
		capture_figure(new_x, new_y);
		board.move_figure(current_figure, new_x, new_y);
	}
	else
	{
		board.move_figure(current_figure, new_x, new_y);
	}
}

void Game::capture_figure(int x, int y)
{
	Figure* captured = board.get_figure(x, y);
	if (captured->get_color() == 0)//czarne
	{
		player_black.del_figure(captured);
	}
	else
	{
		player_white.del_figure(captured);
	}
}

bool Game::check_stalemate_condition()
{
	auto enemy_figures = get_player().get_player_figures();
	if (get_checking_figures().size() == 0)
	{
		for (auto itr_fig = enemy_figures.begin(); itr_fig != enemy_figures.end(); ++itr_fig)
		{
			if (get_final_moves_for_figure((*itr_fig)).size() != 0)
			{
				return false;
			}

		}
		return true;
	}
	return false;
}

bool Game::check_win_condition()
{
	auto enemy_figures = get_player().get_player_figures();
	for (auto itr_fig = enemy_figures.begin(); itr_fig != enemy_figures.end(); ++itr_fig)
	{
		if (get_final_moves_for_figure((*itr_fig)).size() != 0)
		{
			return false;
		}

	}
	return true;
}

std::list<Figure*> Game::get_checking_figures() const
{
	Player cur_player = get_player();
	Player ene_player = get_enemy_player();

	std::list<Figure*> checking_figures;

	std::vector<int> king_pos = cur_player.get_king()->get_position();

	auto enemy_figures = ene_player.get_player_figures();
	for (auto itr_fig = enemy_figures.begin(); itr_fig != enemy_figures.end(); ++itr_fig)
	{
		std::list<std::vector<int>> figure_move_positions = board.get_free_positions_for_figure(*itr_fig);
		auto itr_to_add = std::find(figure_move_positions.begin(), figure_move_positions.end(), king_pos);
		if (itr_to_add != figure_move_positions.end())
		{
			checking_figures.push_back(*itr_fig);
		}
	}
	return checking_figures;
}

std::list<std::vector<int>> Game::get_final_moves_for_figure(Figure* my_figure)
{
	auto checking_figures = get_checking_figures();
	auto allowed_player_moves = get_allowed_moves();
	auto figure_positions = board.get_free_positions_for_figure(my_figure);
	std::list<std::vector<int>> final_positions = {};

	if (checking_figures.size() == 0)
	{
		auto possible_checking_figures = get_possible_checking_figures();
		for (auto iter = possible_checking_figures.begin(); iter != possible_checking_figures.end(); ++iter)
		{
			std::vector<int> checking_position = (*iter)->get_position();
			std::vector<int> king_position = get_player().get_king()->get_position();
			auto positions_beetween = get_positions_beetween(checking_position, king_position);
			std::vector<Figure*> figures_between;
			for (auto itr = positions_beetween.begin(); itr != positions_beetween.end(); ++itr)
			{
				if (board.get_figure((*itr)[0], (*itr)[1]) != nullptr)
				{
					figures_between.push_back(board.get_figure((*itr)[0], (*itr)[1]));
				}
			}
			if (figures_between.size() == 1 && figures_between[0] == my_figure)
			{
				for (auto itr = positions_beetween.begin(); itr != positions_beetween.end(); ++itr)
				{
					if (std::find(figure_positions.begin(), figure_positions.end(), (*itr)) != figure_positions.end())
					{
						final_positions.push_back((*itr));
					}
				}

				if (std::find(figure_positions.begin(), figure_positions.end(), (*iter)->get_position()) != figure_positions.end())
				{
					final_positions.push_back((*iter)->get_position());
				}
				return final_positions;
			}
		}
	}


	if (my_figure->get_type() != "K")
	{
		if (checking_figures.size() > 1)
		{
			return final_positions;
		}

		if (allowed_player_moves.size() != 0)
		{
			for (auto itr = allowed_player_moves.begin(); itr != allowed_player_moves.end(); ++itr)
			{
				if (std::find(figure_positions.begin(), figure_positions.end(), *itr) != figure_positions.end())
				{
					final_positions.push_back(*itr);
				}
			}
			return final_positions;
		}
		else
		{
			return figure_positions;
		}

	}
	else
	{
		final_positions = restrict_king_positions();

		return final_positions;
	}
}


std::list<std::vector<int>> Game::get_allowed_moves() const //zwraca możliwe ruchy dla szachowanego gracza
{

	std::list<Figure*> checking_figures = get_checking_figures(); // figury przeciwne szachujące
	std::vector<int> king_pos = (get_player().get_king())->get_position(); // król szachowany
	std::list<std::vector<int>> allowed_moves = {};

	if (checking_figures.size() == 1)
	{
		std::vector<int> checking_pos = (*(checking_figures.begin()))->get_position();
		auto positions_between = get_positions_beetween(checking_pos, king_pos);
		allowed_moves.insert(allowed_moves.end(), positions_between.begin(), positions_between.end());
		allowed_moves.push_back(checking_pos);
	}
	return allowed_moves;
}


std::list<std::vector<int>> Game::restrict_king_positions() //zrwaca listę pól na które może ruszyć się król, ze względu na przeciwne figury // przyjmuje grasza szachującego
{
	Player checked_player = get_player();
	Player enem_player = get_enemy_player();


	Figure* king = checked_player.get_king();
	std::list<std::vector<int>> king_free_positions = board.get_free_positions_for_figure(king);

	std::list<Figure*> enemy_figures = enem_player.get_player_figures();

	for (auto itr = enemy_figures.begin(); itr != enemy_figures.end(); ++itr) // usuwa pozycje na któr może się ruszyć król, gdzy przeciwna figura ma bicie na to pole
	{
		std::list<std::vector<int>> attacking_positions;
		if ((*itr)->get_type() == "P")
			attacking_positions = board.get_strike_positions_for_pawn(*itr);
		else
			attacking_positions = board.get_free_positions_for_figure(*itr);
		for (auto itr_attack = attacking_positions.begin(); itr_attack != attacking_positions.end(); ++itr_attack)
		{
			auto itr_del = std::find(king_free_positions.begin(), king_free_positions.end(), *itr_attack);
			if (itr_del != king_free_positions.end())
			{
				king_free_positions.remove(*itr_del);
			}
		}
	}

	auto checking_figures = get_checking_figures();
	auto temp_board = board.get_board();
	temp_board[king->get_position()[0]][king->get_position()[1]] = nullptr;
	for (auto itr = checking_figures.begin(); itr != checking_figures.end(); ++itr) // usuwa możliwość ruchu króla w lini szacha 
	{
		std::list<std::vector<int>> attacking_positions = (*itr)->get_possible_positions();
		for (auto itr_attack = attacking_positions.begin(); itr_attack != attacking_positions.end(); ++itr_attack)
		{
			auto itr_del = std::find(king_free_positions.begin(), king_free_positions.end(), *itr_attack);

			if (itr_del != king_free_positions.end())
			{
				king_free_positions.remove(*itr_del);
			}
		}
	}

	Board backup_board = board;
	int backup_kin_x = king->get_position()[0];
	int backup_kin_y = king->get_position()[1];
	bool backup_first_move_status = king->get_first_move();

	std::list<std::vector<int>> final_positions = king_free_positions;

	for (auto itr = king_free_positions.begin(); itr != king_free_positions.end(); ++itr)
	{
		auto temp_board_3 = board;
		temp_board_3.move_figure(king, (*itr)[0], (*itr)[1]);

		board.swap_board(temp_board_3.get_board());

		if (size(get_checking_figures()) >= 1)
		{
			if (itr != king_free_positions.end())
			{
				final_positions.remove(*itr);
			}
		}
	}
	board.move_figure(king, backup_kin_x, backup_kin_y);
	if (backup_first_move_status == true)
	{
		king->set_first_move();
	}
	board.swap_board(backup_board.get_board());
	return final_positions;
}



void Game::create_figures()
{
	//czarne
	Figure* bishop_l_b = new Bishop(0, 2, 0);
	Figure* bishop_r_b = new Bishop(0, 5, 0);
	Figure* rook_l_b = new Rook(0, 0, 0);
	Figure* rook_r_b = new Rook(0, 7, 0);
	Figure* knight_l_b = new Knight(0, 1, 0);
	Figure* knight_r_b = new Knight(0, 6, 0);
	Figure* king_b = new King(0, 4, 0);
	Figure* queen_b = new Queen(0, 3, 0);
	Figure* pawn_b_1 = new Pawn(0, 0, 1);
	Figure* pawn_b_2 = new Pawn(0, 1, 1);
	Figure* pawn_b_3 = new Pawn(0, 2, 1);
	Figure* pawn_b_4 = new Pawn(0, 3, 1);
	Figure* pawn_b_5 = new Pawn(0, 4, 1);
	Figure* pawn_b_6 = new Pawn(0, 5, 1);
	Figure* pawn_b_7 = new Pawn(0, 6, 1);
	Figure* pawn_b_8 = new Pawn(0, 7, 1);
	std::list <Figure*> black{ bishop_l_b, bishop_r_b, rook_l_b, rook_r_b, knight_l_b, knight_r_b, king_b, queen_b,
	pawn_b_1, pawn_b_2, pawn_b_3, pawn_b_4, pawn_b_5, pawn_b_6, pawn_b_7, pawn_b_8 };

	board.set_starting_postions(black);
	player_black.set_player_figures(black);

	//Biale
	Figure* bishop_l_w = new Bishop(1, 2, 7);
	Figure* bishop_r_w = new Bishop(1, 5, 7);
	Figure* rook_l_w = new Rook(1, 0, 7);
	Figure* rook_r_w = new Rook(1, 7, 7);
	Figure* knight_l_w = new Knight(1, 1, 7);
	Figure* knight_r_w = new Knight(1, 6, 7);
	Figure* king_w = new King(1, 4, 7);
	Figure* queen_w = new Queen(1, 3, 7);
	Figure* pawn_w_1 = new Pawn(1, 0, 6);
	Figure* pawn_w_2 = new Pawn(1, 1, 6);
	Figure* pawn_w_3 = new Pawn(1, 2, 6);
	Figure* pawn_w_4 = new Pawn(1, 3, 6);
	Figure* pawn_w_5 = new Pawn(1, 4, 6);
	Figure* pawn_w_6 = new Pawn(1, 5, 6);
	Figure* pawn_w_7 = new Pawn(1, 6, 6);
	Figure* pawn_w_8 = new Pawn(1, 7, 6);
	std::list < Figure*> white{ bishop_l_w, bishop_r_w, rook_l_w, rook_r_w, knight_l_w, knight_r_w, king_w, queen_w,
	pawn_w_1, pawn_w_2, pawn_w_3, pawn_w_4, pawn_w_5, pawn_w_6, pawn_w_7, pawn_w_8 };

	board.set_starting_postions(white);
	player_white.set_player_figures(white);
}


bool Game::castling_left_conditions(int pos_x, int pos_y) const
{
	if (board.get_figure(pos_x - 4, pos_y) == nullptr)
		return false;
	else
	{
		Figure* corner_figure = board.get_figure(pos_x - 4, pos_y);
		if ((corner_figure->get_type() == "R") && (corner_figure->get_first_move() == true))
			return true;
		return false;
	}
}

bool Game::castling_right_conditions(int pos_x, int pos_y) const
{
	if (board.get_figure(pos_x + 3, pos_y) == nullptr)
		return false;
	else
	{
		Figure* corner_figure = board.get_figure(pos_x + 3, pos_y);
		if ((corner_figure->get_type() == "R") && (corner_figure->get_first_move() == true))
			return true;
		return false;
	}
}

std::list<std::vector<int>> Game::get_castling_positions() const
{
	int pos_x = 4;
	int pos_y = 0;
	if (is_white_turn == true)
	{
		pos_x = 4;
		pos_y = 7;
	}
	std::list<std::vector<int>> castling_positions;
	if (get_player().get_king()->get_first_move() == true)
	{
		if ((castling_left_conditions(pos_x, pos_y) == true) && (board.get_figure(pos_x - 1, pos_y) == nullptr) && (board.get_figure(pos_x - 2, pos_y) == nullptr)
			&& board.get_figure(pos_x - 3, pos_y) == nullptr)
		{
			castling_positions.push_back({ pos_x - 2, pos_y });
		}
		if ((castling_right_conditions(pos_x, pos_y) == true) && (board.get_figure(pos_x + 1, pos_y)) == nullptr && (board.get_figure(pos_x + 2, pos_y) == nullptr))
		{
			castling_positions.push_back({ pos_x + 2, pos_y });
		}
	}
	return castling_positions;
}

bool Game::get_current_player() const
{
	return is_white_turn;
}

void Game::change_turn()
{
	if (is_white_turn == true)
	{
		is_white_turn = false;
	}
	else
	{
		is_white_turn = true;
	}
}

bool Game::check_promote_pawn(Figure* pawn)
{
	if (is_white_turn == true)
	{
		if (pawn->get_position()[1] == 0)
		{
			Figure* promoted_queen = new Queen(1, pawn->get_position()[0], 0); // tutaj trzeba ogarnac wstawianie figury jesli bicie na ukos to bedzie inny x
			player_white.replace_figure(promoted_queen, pawn);
			board.replace_figure(pawn, promoted_queen);
			return true;
		}
		return false;
	}
	else
	{
		if (pawn->get_position()[1] == 7)
		{
			Figure* promoted_queen = new Queen(0, pawn->get_position()[0], 7);
			player_black.replace_figure(promoted_queen, pawn);
			board.replace_figure(pawn, promoted_queen);
			return true;
		}
		return false;
	}


}

std::vector<int> Game::do_castling(int new_x, int new_y)
{
	std::vector<int> rook_coords;
	Figure* king;
	if (is_white_turn == true)
	{
		king = player_white.get_king();
	}
	else
	{
		king = player_black.get_king();
	}

	if (king->get_position()[0] == new_x + 2)
	{
		rook_coords.push_back(0);
		rook_coords.push_back(new_y);
		rook_coords.push_back(3);
		rook_coords.push_back(king->get_position()[1]);
		board.move_figure(board.get_figure(0, new_y), 3, new_y);
	}
	if (king->get_position()[0] == new_x - 2)
	{
		rook_coords.push_back(7);
		rook_coords.push_back(new_y);
		rook_coords.push_back(5);
		rook_coords.push_back(king->get_position()[1]);
		board.move_figure(board.get_figure(7, new_y), 5, new_y);
	}
	return rook_coords;
}


std::list<std::vector<int>> Game::get_positions_beetween(std::vector<int> const& checking_pos, std::vector<int> const& king_pos) const
{
	std::list<std::vector<int>> positions_between;

	int king_x = king_pos[0];
	int king_y = king_pos[1];
	int checking_x = checking_pos[0];
	int checking_y = checking_pos[1];


	if ((king_x == checking_x) && (checking_y < king_y)) //pion w górę
	{
		king_y--;
		while (king_y > checking_y)
		{
			std::vector<int> point = { king_x, king_y };
			positions_between.push_back(point);
			king_y--;
		}
	}

	if ((king_x == checking_x) && (checking_y > king_y)) //pion w dół
	{
		king_y++;
		while (king_y < checking_y)
		{
			std::vector<int> point = { king_x, king_y };
			positions_between.push_back(point);
			king_y++;
		}
	}

	if ((checking_x > king_x) && (checking_y == king_y)) //poziom w prawo
	{
		king_x++;
		while (king_x < checking_x)
		{
			std::vector<int> point = { king_x, king_y };
			positions_between.push_back(point);
			king_x++;
		}
	}

	if ((checking_x < king_x) && (checking_y == king_y)) //poziom w lewo
	{
		king_x--;
		while (king_x > checking_x)
		{
			std::vector<int> point = { king_x, king_y };
			positions_between.push_back(point);
			king_x--;
		}
	}

	if ((checking_x > king_x) && (checking_y < king_y)) // prawy ukos w górę
	{
		king_x++;
		king_y--;
		while (king_x < checking_x && king_y > checking_y)
		{
			std::vector<int> point = { king_x, king_y };
			positions_between.push_back(point);
			king_x++;
			king_y--;
		}
	}

	if ((checking_x > king_x) && (checking_y > king_y)) // prawy ukos w dół
	{
		king_x++;
		king_y++;
		while (king_x < checking_x && king_y < checking_y)
		{
			std::vector<int> point = { king_x, king_y };
			positions_between.push_back(point);
			king_x++;
			king_y++;
		}
	}

	if ((checking_x < king_x) && (checking_y < king_y)) // lewy ukos w górę
	{
		king_x--;
		king_y--;
		while (king_x > checking_x && king_y > checking_y)
		{
			std::vector<int> point = { king_x, king_y };
			positions_between.push_back(point);
			king_x--;
			king_y--;
		}
	}

	if ((checking_x < king_x) && (checking_y > king_y)) // lewy ukos w dół
	{
		king_x--;
		king_y++;
		while (king_x > checking_x && king_y < checking_y)
		{
			std::vector<int> point = { king_x, king_y };
			positions_between.push_back(point);
			king_x--;
			king_y++;
		}
	}

	return positions_between;
}


std::string Game::get_record(Figure* my_figure, int new_x, int new_y) const
{
	std::string text;
	if (get_current_player())
		text += "[B]: ";
	else
		text += "[C]: ";

	text += my_figure->get_type();
	text += " ";
	text += convert_x_coordinate(my_figure->get_position()[0]);
	text += convert_y_coordinate(my_figure->get_position()[1]);
	text += "-->";
	text += convert_x_coordinate(new_x);
	text += convert_y_coordinate(new_y);

	if (board.get_figure(new_x, new_y) != nullptr)
		text += " (bicie)";

	if ((abs(my_figure->get_position()[0] - new_x) > 1) && (my_figure->get_type() == "K"))
		text += " (roszada)";
	text += "\n";
	return text;
}

std::string Game::convert_x_coordinate(int x) const
{
	std::string text;
	switch (x)
	{
	case 0: text += "A";
		break;
	case 1: text += "B";
		break;
	case 2: text += "C";
		break;
	case 3: text += "D";
		break;
	case 4: text += "E";
		break;
	case 5: text += "F";
		break;
	case 6: text += "G";
		break;
	case 7: text += "H";
		break;
	}
	return text;
}


std::string Game::convert_y_coordinate(int y) const
{
	std::string text;
	switch (y)
	{
	case 0: text += "8";
		break;
	case 1: text += "7";
		break;
	case 2: text += "6";
		break;
	case 3: text += "5";
		break;
	case 4: text += "4";
		break;
	case 5: text += "3";
		break;
	case 6: text += "2";
		break;
	case 7: text += "1";
		break;
	}
	return text;
}

std::vector<int> Game::generate_random_move()
{
	std::vector<int> chosen_moves;
	auto figures_list = get_player().get_player_figures();
	srand(time(NULL));
	int number;
	std::list<std::vector<int>> moves;
	std::list<Figure*>::iterator iter;
	do
	{
		number = rand() % figures_list.size();
		iter = figures_list.begin();
		std::advance(iter, number);
		moves = get_final_moves_for_figure(*iter);

	} while (get_final_moves_for_figure(*iter).size() == 0);

	chosen_moves.push_back((*iter)->get_position()[0]);
	chosen_moves.push_back((*iter)->get_position()[1]);
	number = rand() % moves.size();
	auto itr2 = moves.begin();
	std::advance(itr2, number);
	chosen_moves.push_back((*itr2)[0]);
	chosen_moves.push_back((*itr2)[1]);
	return chosen_moves;
}


///////////////////////////////////////// PLAYER /////////////////////////////////////////////

void Player::set_player_figures(std::list<Figure*> const& figures_list)
{
	player_figures = figures_list;
}

std::list<Figure*> Player::get_player_figures() const
{
	return player_figures;
}

Figure* Player::get_king() const
{
	for (auto itr = player_figures.begin(); itr != player_figures.end(); itr++)
	{
		if ((*itr)->get_type() == "K")
		{
			return *itr;
		}
	}
	return nullptr;
}


void Player::del_figure(Figure* fig_to_del)
{
	auto to_del = std::find(player_figures.begin(), player_figures.end(), fig_to_del);
	player_figures.erase(to_del);
}

void Player::replace_figure(Figure* fig_to_add, Figure* current_figure)
{
	std::replace(player_figures.begin(), player_figures.end(), current_figure, fig_to_add);
}

void Player::clear_player()
{
	player_figures.clear();
}