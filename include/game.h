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
#include <random>
#include <fstream>

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
    bool operator == (const Move &obj){
        if(this->current == obj.current && this->target == obj.target)
            return true;
        return false;
    }
};

class game
{
    static map<char, short> init_map(){
        map<char, short> index;
        index['P'] = 1; /// White pawn
        index['p'] = index['P'] + 56; /// Black pawn
        index['N'] = index['p'] + 56; /// White knight
        index['n'] = index['N'] + 64; /// Black knight
        index['B'] = index['n'] + 64; /// White bishop
        index['b'] = index['B'] + 64; /// Black bishop
        index['R'] = index['b'] + 64; /// White Rook
        index['r'] = index['R'] + 64; /// Black Rook
        index['Q'] = index['r'] + 64; /// White Queen
        index['q'] = index['Q'] + 64; /// Black Queen
        index['K'] = index['q'] + 64; /// White King
        index['k'] = index['K'] + 64; /// Black King
        return index;
    }
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
        vector<Move> get_all_moves(string, bool, bool=false, short=-1);
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

        /// TT members
        void initial_zobrist_hash(string);
        void update_zobrist_val(short, short, char, char);
        unsigned long long zobrist_val;
        static map<char, short> TT_INDEXES;
        static short tt_castle_start_index;
        static short tt_en_passant_start;

};



void disp_board(bitset<64>);
void initialize_prn(bool=false);
Move call_ai(game, string, short);

// For debugging purposes only, Delete in production
long heuristic(game, string, string);
bitset<64> get_valid_moves(string, int, string);
vector<int> get_true_pos(bitset<64>);
string reverse_player(string);
void disp_board(string);


#endif // GAME_H
