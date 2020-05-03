#ifndef GAME_H
#define GAME_H
#include <bitset>
#include <string>
#include <math.h>
#include <cstdlib>
#include <bits/stdc++.h>
#include <map>
#include <iostream>
#include <chrono>

using namespace std;

const string WHITE = "white";
const string BLACK = "black";

struct Move{
    short current;
    short target;
    int score;
    Move *next;
    Move(){
        this->next = nullptr;
    }
    Move(int c, int t){
        this->current = c;
        this->target = t;
    }
    Move(short c, short t, int s){
        this->current = c;
        this->target = t;
        this->score = s;
    }
};

class game
{
    public:
        game();
        game(const game&);
        //game(char[65])
        virtual ~game();
        bitset<64> get_occupied(string);
        bitset<64> get_occupied();
        bitset<64> get_true_target_area(int, string);
        bitset<64> get_true_target_area(string);
        bitset<64> get_pseudo_target_area(int);
        bitset<64> get_pseudo_target_area(string);
        vector<Move> get_all_moves(string, bool, bool=false);
        int check_game_over(string);
        short moves_since_last_capture;
        void set_occupied(string, int, bool);
        bool make_move(short, short, string, bool, bool);
        bool is_check(string);
        bool is_capture(short, short, string);
        bool promote_pawn(int, char, string, bool);
        void update_status(string);
        int en_passant;
        bool game_over;
        bool checked;
        array<bool, 2> white_castle;
        array<bool, 2> black_castle;
        string winner;
        string game_board;
        int pawn_promotion;
        bitset<64> white_occupied;
        bitset<64> black_occupied;
        array <bitset<64>, 64> target_areas;

};


void disp_board(bitset<64>);
Move call_ai(game, string, short);

// For debugging purposes only, Delete in production
bitset<64> get_valid_moves(string, int, string);
vector<int> get_true_pos(bitset<64>);
int heuristic(game, string, string);
string reverse_player(string);
void disp_board(string);


#endif // GAME_H
