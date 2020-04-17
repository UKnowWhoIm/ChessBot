#include "game.h"
/*
game::game(char board[64])
{
    //constructor
    game_board = board
    for(int i=0; i < 64; i++){
        if(board[i] != 'f' &&
            (board[i]) == WHITE)
            white_occupied[i] = 1;
        else if(board[i] != 'f' &&
                    (board[i]) == BLACK)
            black_occupied[i] = 1;
    }

}
*/

game::game(){
    //constructor
    char board[65] = "rnbqkbnrppppppppffffffffffffffffffffffffffffffffPPPPPPPPRNBQKBNR";
    strcpy(this->game_board, board);
    for(int i=0; i < 64; i++){
        if(this->game_board[i] != 'f' && get_player(this->game_board[i]) == WHITE)
            white_occupied[64-i] = 1;
        else if(this->game_board[i] != 'f' && get_player(this->game_board[i]) == BLACK)
            black_occupied[64-i] = 1;
    }
}

bitset<64> game::get_occupied(string player){
    // occupied by a specific player
    if(player == WHITE)
        return this->white_occupied;
    return this->black_occupied;
}

bitset<64> game::get_occupied(){
    // All occupied cols
    return this->black_occupied | this->white_occupied;
}

bitset<64> game::get_pseudo_target_area(int pos){
    // Get pseudo target area of a particular piece
    return this->target_area.at(pos);
}

bitset<64> game::get_pseudo_target_area(string player){
    // Get pseudo target area of a player
    bitset<64> area (0);
    int *posns = get_true_pos(this->get_occupied(player));
    for(;*posns != -1; posns++)
        area |= this->target_area.at(*posns);
    return area;
}

bitset<64> game::get_true_target_area(int pos, string player){
    // Get true target area of piece
    return this->target_area.at(pos) & ~this->get_occupied(player);
}

bitset<64> game::get_true_target_area(string player){
    // Get True target area of player
    return this->get_pseudo_target_area(player) & ~this->get_occupied(player);
}

bool game::is_check(string player){
    int king_pos = find_king(this->game_board, player);
    return this->get_true_target_area(player)[king_pos];
}

bool game::make_move(int current, int target, string player, bool _reverse=true){
    bitset<64> move_board(0);

    move_board[63-current] = 1;
    move_board[63-target] = 1;

    // These are the steps that should be reversed to restore game to previous state
    this->game_board[target] = this->game_board[current];
    this->game_board[current] = 'f';
    if(player == WHITE){
        this->white_occupied[63 - current] = 0;
        this->white_occupied[63 - target] = 1;
    }
    else{
        this->black_occupied[63 - current] = 0;
        this->black_occupied[63 - target] = 1;
    }
    map<int, bitset<64>,Comparer> old_target_area;
    old_target_area.insert(this->target_area.begin(), this->target_area.end());
    this->target_area[target] = get_valid_moves(this->game_board, target, player);

    map<int, bitset<64>,Comparer>::iterator itr;
    for(itr = this->target_area.begin(); itr != this->target_area.end(); itr++)
        if((itr->second & move_board).any())
            //update target_area
            this->target_area[itr->first] = get_valid_moves(this->game_board, itr->first, get_player(this->game_board[itr->first]));
    bool checked = this->is_check(player);
    if(_reverse || checked){
        this ->target_area.insert(old_target_area.begin(), old_target_area.end());
        this ->game_board[current] = this->game_board[target];
        this->game_board[target] = 'f';
        if(player == WHITE){
            this->white_occupied[63 - current] = 1;
            this->white_occupied[63 - target] = 0;
        }
        else{
            this->black_occupied[63 - current] = 1;
            this->black_occupied[63 - target] = 0;
        }
        return checked;
    }
    return true;
}

game::~game()
{
    //destructor
}
