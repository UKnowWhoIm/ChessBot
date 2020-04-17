#ifndef GAME_H
#define GAME_H
#include <bitset>
#include <string>
#include <cstring>
#include <map>

using namespace std;

const string WHITE = "white";
const string BLACK = "black";

struct Comparer {
    bool operator() (const bitset<64> &b1, const bitset<64> &b2) const {
        return b1.to_ulong() < b2.to_ulong();
    }
};

struct Updation{
    int updation;
    bool (&check)(int);
};

int* get_true_pos(bitset<64> board){
    // Find the true values in a bitboard
    int posns[17], k=0;
    for(int i=0;i<64;i++){
        if(board[i])
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

string reverse_player(string player){
    if(player == WHITE)
        return BLACK;
    return WHITE;
}

bitset<64> move_generator(char board[65], int pos, Updation updation[4], int valid_updations){
    int pos_temp;
    bitset<64> area (0);
    for(int i=0; i < valid_updations; i++){
        pos_temp = pos;
        for(;pos_temp >= 0 && pos_temp <= 63; pos_temp += updation[i].updation){
            area[63-pos_temp] = 1;
            if(updation[i].check != NULL)
                if(!updation[i].check(pos_temp))
                    // Will go out of bounds
                    break;
            if(board[pos_temp] != 'f')
                // Current col is occupied
                break;
        }
    }
    return area;
}

bool return_true(int i){
    // Since NULL is just 0,  this is a hotfix for Updation arr initialization
    return true;
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
            if(can_go_left(pos + 1)){
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
        for(int i=0; i < k; i++)
            if( pos + posns[i] >= 0 && pos + posns[i] < 64 )
                area[63 - (pos + posns[i])] = 1;
    }
    else{
        // Q, B, R
        Updation rook_updation[4] = {{-8, return_true}, {8, return_true}, {-1, can_go_left}, {1, can_go_right}};
        Updation bishop_updation[4] = {{7, can_go_left}, {-9, can_go_left}, {9, can_go_right}, {-7, can_go_right}};
        int r = 2, b = 0;
        if(can_go_left(pos)){
            r++;
            b += 2;
        }
        if(can_go_right(pos)){
            r++;
            b += 2;
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

class game
{
    public:
        game();
        //game(char[65])
        virtual ~game();
        bitset<64> get_occupied(string);
        bitset<64> get_occupied();
        bitset<64> get_true_target_area(int, string);
        bitset<64> get_true_target_area(string);
        bitset<64> get_pseudo_target_area(int);
        bitset<64> get_pseudo_target_area(string);
        int* get_player_posns(string);
        bool make_move(int, int, string, bool);
        bool is_check(string);
        // size = 65 because of \0
        char game_board[65];
        bitset<64> white_occupied;
        bitset<64> black_occupied;
        map<int, bitset<64>, Comparer> target_area;
};

#endif // GAME_H
