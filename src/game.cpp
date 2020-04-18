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
bool return_true(int i){
    // Since NULL is just 0,  this is a hotfix for Updation arr initialization
    return true;
}

struct Updation{
    int updation;
    int times;
};

int* get_true_pos(bitset<64> board){
    // Find the true values in a bitboard
    int *posns = (int*)malloc(sizeof(int)*17);
    int k=0;
    for(int i=0;i<64;i++){
        if(board[63-i])
            posns[k++] = i;
    }
    // Set last element to -1 to indicate arr has ended
    posns[k] = -1;
    return posns;
}

string get_player(char piece){
    if(piece == 'f')
            return "";
    if(piece > 96)
        return BLACK;
    return WHITE;
}

bool can_go_left(int pos){
    return pos % 8 != 0;
}

bool can_go_right(int pos){
    return pos % 8 != 7;
}

int how_many_times_can_go_left(int pos){
    return pos % 8;
}

int how_many_times_can_go_right(int pos){
    return 7 - pos % 8;
}


string reverse_player(string player){
    if(player == WHITE)
        return BLACK;
    return WHITE;
}

bitset<64> move_generator(char board[65], int pos, Updation updation[4], int valid_updations){
    int pos_temp;
    bitset<64> area (0);
    for(int i=0, k=0; i < valid_updations; i++){
        pos_temp = pos + updation[i].updation;
        for(k=0;pos_temp >= 0 && pos_temp <= 63; pos_temp += updation[i].updation, k++){
            if(updation[i].times == k)
                break;
            area[63-pos_temp] = 1;
            if(board[pos_temp] != 'f')
                // Current col is occupied
                break;
        }
    }
    return area;
}



int find_king(char board[65], string player){
    char target;
    if(player == WHITE)
        target = 'K';
    else
        target = 'k';
    for(int i=0;i<64;i++)
        if(board[i] == target)
            return i;
    return -1;
}

bitset<64> get_valid_moves(char board[65], int pos, string player){
    bitset<64> area (0);
    if(tolower(board[pos]) == 'p'){
        int dirn;
        bool initial_move;
        if(player == WHITE){
            dirn = -1;
            initial_move = pos <= 55 && pos >= 48;
        }
        else{
            dirn = 1;
            initial_move = pos <= 15 && pos >= 8;
        }
        area[63 - (pos + 8*dirn)] = 1;
        if(board[pos + 8*dirn] == 'f' && initial_move)
            // Initial Double Step
            area[63 - (pos + 16*dirn)] = 1;
        if(can_go_left(pos) && get_player(board[pos + 8*dirn - 1]) == reverse_player(player))
            // Left Side Capture
            area[63 - (pos + 8*dirn - 1)] = 1;
        if(can_go_right(pos) && get_player(board[pos + 8*dirn + 1]) == reverse_player(player))
            // Right Side Capture
            area[63 - (pos + 8*dirn + 1)] = 1;
        // ENPASSANT GOES HERE

    }
    else if(tolower(board[pos]) == 'n'){
        int posns[8],k=0;
        if(can_go_left(pos)){
            posns[k++] = -17; // up
            posns[k++] = 15; // down
            if(can_go_left(pos - 1)){
                posns[k++] = 6; // down
                posns[k++] = -10; // up
            }
        }
        if(can_go_right(pos)){
            posns[k++] = -15; // up
            posns[k++] = 17; // down
            if(can_go_right(pos + 1)){
                posns[k++] = -6; // up
                posns[k++] = 10; // down
            }
        }
        for(int i=0; i < k; i++)
            if( pos + posns[i] >= 0 && pos + posns[i] < 64)
                area[63 - (pos + posns[i])] = 1;
    }
    else if(tolower(board[pos]) == 'k'){
        int posns[10] = {-8, 8, 0, 0, 0, 0, 0, 0, 0, 0}, k = 2;
        if(can_go_left(pos)){
            posns[k++] = -1;
            posns[k++] = -9;
            posns[k++] = 7;
        }
        if(can_go_right(pos)){
            posns[k++] = 1;
            posns[k++] = -7;
            posns[k++] = 9;
        }
        if(this->castle[player][0])
            // Queen Side Castling
            if(board[pos - 1] == 'f' && board[pos - 2] == 'f' && board[pos - 3] == 'f')
                posns[k++] = 2;
        if(this->castle[player][1])
            // King side castling
            if(board[pos + 1] == 'f' && board[pos + 2] == 'f')
                posns[k++] = -2;
        for(int i=0; i < k; i++)
            if( pos + posns[i] >= 0 && pos + posns[i] < 64 )
                area[63 - (pos + posns[i])] = 1;
    }
    else{
        // Q, B, R
        Updation rook_updation[4];
        Updation bishop_updation[4];
        rook_updation[0] = {-8, 7};
        rook_updation[1] = {8, 7};
        int r = 2, b = 0;
        // how_many_times_can_go_left and how_many_times_can_go_right returns the maximum
        // number of times the piece can go in that particular dirn if the board is empty
        // Since it isn't constrained to vertical bounds it may not always be true
        if(can_go_left(pos)){
            rook_updation[r++] = {-1, how_many_times_can_go_left(pos)};
            bishop_updation[b++] = {-9, how_many_times_can_go_left(pos)};
            bishop_updation[b++] = {7, how_many_times_can_go_left(pos)};
        }
        if(can_go_right(pos)){
           rook_updation[r++] = {1, how_many_times_can_go_right(pos)};
            bishop_updation[b++] = {-7, how_many_times_can_go_right(pos)};
            bishop_updation[b++] = {9, how_many_times_can_go_right(pos)};
        }
        if(tolower(board[pos]) == 'b')
            return move_generator(board, pos, bishop_updation, b);
        else if(tolower(board[pos]) == 'r')
            return move_generator(board, pos, rook_updation, r);
        else{
            // Queen = Rook + Bishop
            area = move_generator(board, pos, bishop_updation, b);
            area |= move_generator(board, pos, rook_updation, r);
        }
    }
    return area;
}


game::game(){
    //constructor
    char board[65] = "rnbqkbnrppppppppffffffffffffffffffffffffffffffffPPPPPPPPRNBQKBNR";
    strcpy(this->game_board, board);
    this->castle[WHITE][0] = true;
    this->castle[WHITE][1] = true;
    this->castle[BLACK][0] = true;
    this->castle[BLACK][1] = true;
    for(int i=0; i < 64; i++){
        if(this->game_board[i] != 'f' && get_player(this->game_board[i]) == WHITE)
            white_occupied[63-i] = 1;
        else if(this->game_board[i] != 'f' && get_player(this->game_board[i]) == BLACK)
            black_occupied[63-i] = 1;
    }
    int *white_pieces = get_true_pos(white_occupied);
    int *black_pieces = get_true_pos(black_occupied);
    int *ptr;
    for(ptr = white_pieces; *ptr != -1; ptr++){
        this->target_area[*ptr] = get_valid_moves(this->game_board, *ptr, WHITE);
    }
    for(ptr = black_pieces; *ptr != -1; ptr++)
        this->target_area[*ptr] = get_valid_moves(this->game_board, *ptr, BLACK);
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
    return this->get_true_target_area(reverse_player(player))[63-king_pos];
}

bool game::make_move(int current, int target, string player, bool _reverse=false){
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
        this -> target_area.clear();
        this -> target_area.insert(old_target_area.begin(), old_target_area.end());
        this -> game_board[current] = this->game_board[target];
        this -> game_board[target] = 'f';
        if(player == WHITE){
            this->white_occupied[63 - current] = 1;
            this->white_occupied[63 - target] = 0;
        }
        else{
            this->black_occupied[63 - current] = 1;
            this->black_occupied[63 - target] = 0;
        }
        return !checked;
    }
    if(tolower(board[target]) == 'r'){
        int left, right;
        if(player == WHITE){
            left = 56;
            right = 63;
        }
        else{
            left = 0;
            right = 7;
        }
        if(current == left)
            this->castle[0] = false;
        else if(current == right)
            this->castle[1] = false;
    }
    else if(tolower(board[target]) == 'k'){
        char rook;
        if(player == WHITE)
            rook = 'R';
        else
            rook = 'r';
        if(abs(target - current) == 2){
            if(target == current - 2 && this->castle[player][0]){
                // Queen Side
                this->game_board[target + 1] = rook;
                this->game_board[target - 2] = 'f';
            }
            else{
                // King Side
                this->game_board[target - 1] = rook;
                this->game_board[target + 1] = 'f';

            }
        }

        this->castle[player][0] = false;
        this->castle[player][1] = false;

    }
    else if(tolower(board[i]) == 'p'){
        int last_rank;
        if(player == WHITE)
            last_rank = target < 8 && target >= 0;
        else
            last_rank = target > 55 && target < 64;
        if(last_rank)
            this -> pawn_promotion = target;
    }
    return true;
}

game::~game()
{
    //destructor
}

