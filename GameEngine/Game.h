﻿#pragma once
#include "../Board/Board.h"
#include <algorithm>
#include <stdlib.h>
#include <time.h>

class Player
{
private:
	std::list<Figure*> player_figures;
public:
	Player() {}
	void set_player_figures(std::list<Figure*> const&);
	std::list<Figure*> get_player_figures() const;
	Figure* get_king() const;
	void del_figure(Figure* fig_to_del);
	void replace_figure(Figure* fig_to_add, Figure* current_figure);
	void clear_player(); // usuwa figury gracza
};


class Game
{
private:
	Player player_white;
	Player player_black;
	Board board;
	bool is_white_turn;
	std::list<std::vector<int>> restrict_king_positions();
	bool castling_left_conditions(int pos_x, int pos_y) const;
	bool castling_right_conditions(int pos_x, int pos_y) const;
	void create_figures();
	std::list<std::vector<int>> get_positions_beetween(std::vector<int> const& checking_pos, std::vector<int> const& king_pos) const;
	std::string convert_x_coordinate(int) const;
	std::string convert_y_coordinate(int) const;
public:
	std::list<Figure*> get_checking_figures() const;
	Game();
	~Game();
	std::vector<int> generate_random_move();
	void capture_figure(int x, int y);
	bool check_stalemate_condition();
	bool check_win_condition();
	void change_turn();
	bool check_promote_pawn(Figure* pawn);
	std::vector<int> do_castling(int new_x, int new_y);
	void make_move(Figure*, int, int);
	std::list<std::vector<int>> get_castling_positions() const;
	Board get_board();
	bool get_current_player() const; // sprawdz ktory kolor ma ruch bialy = true, czarny false
	std::list<std::vector<int>> get_allowed_moves() const; //zwraca możliwe ruchy dla szachowanego gracza
	Player get_player()const; // zwraca gracza który ma teraz turę
	Player get_enemy_player()const; // zwraca gracza który nie ma tury  
	std::vector<Figure*> get_possible_checking_figures();
	std::list<std::vector<int>> get_final_moves_for_figure(Figure*); // zwraca ostateczne możliwe ruchy figury uwzględniająć możliwego szacha króla
	std::string get_record(Figure*, int x, int y) const;
	void restart_game(); //przywraca stan początkowy gry
};
