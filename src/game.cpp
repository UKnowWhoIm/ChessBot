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


/**
Random Num Array

1 number to indicate the side to move is black
1 number for each piece at each square, since pawns don't occupy the last rank(10 * 64 + 2 * 56)
8 numbers to indicate the castling rights(4 values, each can be 1 or 0, therefore 2*4)
8 numbers to indicate the file of a valid En passant square, if any
*/

array<unsigned long long , 1 + 10*64+2*56 + 8 + 8> PRN;


/// Define static members
map<char, short> game::TT_INDEXES = init_map();

short game::tt_castle_start_index = game::TT_INDEXES['k'] + 64;

short game::tt_en_passant_start = game::tt_castle_start_index + 8;

/**
TT CASTLE REF:

    PRN Has 8 values for storing castling rights
    "tt_castle_start_index" has been set to the 0th index of PRN's castling rights
    Index 0: White QueenSide Castle True
    Index 1: White QueenSide Castle False
    Index 2: White KingSide Castle True
    Index 3: White KingSide Castle False
    Index 4: Black QueenSide Castle True
    Index 5: Black QueenSide Castle False
    Index 6: Black KingSide Castle True
    Index 7: Black KingSide Castle False

    ie tt_castle_start_index + i -> Index i

*/

ostream & operator << (ostream & out, const Move & m){
    out << m.current << ' ' << m.target << endl;
    return out;
}

long piece_vals(char piece){
    switch(piece){
        case 'p':return 50;
        case 'n': return 200;
        case 'b': return 300;
        case 'r': return 500;
        case 'q': return 1000;
        case 'k': return pow(10, 7);
        case 'P': return -50;
        case 'N': return -200;
        case 'B': return -300;
        case 'R': return -500;
        case 'Q': return -1000;
        case 'K': return -pow(10, 7);
        default: return 0;
    }
}

const short KillerSize = 3;

const unsigned long HashSize = pow(10, 7);

const short depth_max = 8;

array<array<Move, KillerSize>, depth_max> KillerMoves;

struct Updation{
    int updation;
    int times;
};

void disp_board(bitset<64> board){
    for(int i=0;i<64;i++){
        if(i % 8 == 0)
            cout<<'\n';
        cout<<board[63-i];

    }
}

void disp_board(string board){
    for(int i=0;i<64;i++){
        if(i % 8 == 0)
            cout<<"\n\n";
        if(board[i] == 'f')
            cout<<". ";
        else
            cout<<board[i]<<' ';

    }
}

void set_bit(bitset<64> &bit_board, short pos, short val=1){
    bit_board.set(63 - pos, val);
}

bool test_bit(bitset<64> bit_board, short pos){
    return bit_board.test(63 - pos);
}


vector<int> get_true_pos(bitset<64> board){
    // Find the true values in a bitboard
    vector<int> posns;
    for(int i=0;i<64;i++){
        if(test_bit(board, i))
            posns.push_back(i);
    }

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

bitset<64> move_generator(string board, int pos, array<Updation, 4> updation, int valid_updations){
    int pos_temp;
    bitset<64> area (0);
    for(int i=0, k=0; i < valid_updations; i++){
        pos_temp = pos + updation[i].updation;
        for(k=0;pos_temp >= 0 && pos_temp <= 63; pos_temp += updation[i].updation, k++){
            if(updation[i].times == k)
                break;
            set_bit(area, pos_temp);
            if(board[pos_temp] != 'f')
                // Current col is occupied
                break;
        }
    }
    return area;
}



int find_king(string board, string player){
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

bitset<64> get_valid_moves(string board, int pos, string player, array<bool, 2> white_castle, array<bool, 2> black_castle){
    bitset<64> area (0);
    if(tolower(board[pos]) == 'n'){
        array<int, 8> posns;
        short k=0;
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
                set_bit(area, pos + posns[i]);
    }
    else if(tolower(board[pos]) == 'k'){
        array<int, 10> posns = {-8, 8, 0, 0, 0, 0, 0, 0, 0, 0};
        short k = 2;
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
        if((white_castle[0] && player == WHITE) || (black_castle[0] && player == BLACK))
            // Queen Side Castling
            if(board[pos - 1] == 'f' && board[pos - 2] == 'f' && board[pos - 3] == 'f')
                posns[k++] = -2;
        if((white_castle[1] && player == WHITE) || (black_castle[1] && player == BLACK))
            // King side castling
            if(board[pos + 1] == 'f' && board[pos + 2] == 'f')
                posns[k++] = 2;
        for(int i=0; i < k; i++)
            if( pos + posns[i] >= 0 && pos + posns[i] < 64 )
                set_bit(area, pos + posns[i]);
    }
    else{
        // Q, B, R
        array<Updation, 4> rook_updation;
        array<Updation, 4> bishop_updation;
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
    this->game_board = "rnbqkbnrppppppppffffffffffffffffffffffffffffffffPPPPPPPPRNBQKBNR";
    this->white_castle = {true, true};
    this->black_castle = {true, true};
    this->en_passant = -1;
    this->checked = false;
    this->pawn_promotion = -1;
    this->moves_since_last_capture = 0;
    this->game_over = false;
    this->winner = "";
    for(int i=0; i < 64; i++){
        if(this->game_board[i] != 'f')
            this->set_occupied(get_player(this->game_board[i]), i, 1);
    }
    vector <int> white_pieces = get_true_pos(white_occupied);
    vector <int> black_pieces = get_true_pos(black_occupied);

    for(auto ptr = white_pieces.begin(); ptr != white_pieces.end(); ptr++){
        if(this->game_board[*ptr] == 'P')
            this->target_areas[*ptr] = this->get_protected_area(*ptr);
        else
            this->target_areas[*ptr] = get_valid_moves(this->game_board, *ptr, WHITE, this->white_castle, this->black_castle);
    }
    for(auto ptr = black_pieces.begin(); ptr != black_pieces.end(); ptr++){
        if(this->game_board[*ptr] == 'p')
            this->target_areas[*ptr] = this->get_protected_area(*ptr);
        else
            this->target_areas[*ptr] = get_valid_moves(this->game_board, *ptr, BLACK, this->white_castle, this->black_castle);
    }
}

game::game(const game &old){
    // Copy Constructor
    this->game_board = old.game_board;
    this->white_castle = old.white_castle;
    this->black_castle = old.black_castle;
    this->en_passant = old.en_passant;
    this->game_over = old.game_over;
    this->checked = old.checked;
    this->winner = old.winner;
    this->black_occupied = old.black_occupied;
    this->white_occupied = old.white_occupied;
    this->target_areas = old.target_areas;
    this->pawn_promotion = old.pawn_promotion;
    this->zobrist_val = old.zobrist_val;
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

bitset<64> game::get_protected_area(int pos){
    /// Get pseudo target area of a particular piece
    if(tolower(this->game_board[pos]) == 'p'){
        string player = get_player(this->game_board[pos]);
        short dirn = 1;
        if(player == WHITE)
            dirn = -1;
        bitset<64> area (0);
        if(can_go_left(pos))
            /// Left Capture
            set_bit(area, pos + 8*dirn - 1);
        if(can_go_right(pos))
            /// Right Capture
            set_bit(area, pos + 8*dirn + 1);
        return area;
    }
    return this->target_areas[pos];
}

bitset<64> game::get_protected_area(string player){
    // Get pseudo target area of a player
    bitset<64> area (0);
    vector <int> posns = get_true_pos(this->get_occupied(player));
    for(auto posn=posns.begin();posn != posns.end(); posn++)
        area |= this->get_protected_area(*posn);
    return area;
}

bitset<64> game::get_true_target_area(int pos, string player){
    // Get true target area of piece
    if(tolower(this->game_board[pos]) == 'p'){
        // Fucking pawn with all the "special" moves
        short dirn;
        bitset<64> area (0);
        bool initial_move;
        if(player == WHITE){
            dirn = -1;
            initial_move = pos < 56 && pos > 47;
        }
        else{
            dirn = 1;
            initial_move = pos < 16 && pos > 7;
        }
        if(this->game_board[pos + 8*dirn] == 'f')
            // Normal Move
            set_bit(area, pos + 8*dirn);
        if(this->game_board[pos + 8*dirn] == 'f' && this->game_board[pos + 16*dirn] == 'f' && initial_move)
            // Initial Double Step
            set_bit(area, pos + 16*dirn);
        if(get_player(this->game_board[pos + 8*dirn - 1]) == reverse_player(player) && can_go_left(pos))
            // Left Capture
            set_bit(area, pos + 8*dirn - 1);
        if(get_player(this->game_board[pos + 8*dirn + 1]) == reverse_player(player) && can_go_right(pos))
            // Right Capture
            set_bit(area, pos + 8*dirn + 1);
        // EN PASSANT
        if(tolower(this->game_board[pos - 1]) == 'p' && get_player(this->game_board[pos - 1]) == reverse_player(player) && can_go_left(pos) && this->en_passant == pos - 1 + 8 * dirn && this->game_board[pos - 1 + 8 * dirn] == 'f')
            set_bit(area, pos + 8*dirn - 1);
        if(tolower(this->game_board[pos + 1]) == 'p' && get_player(this->game_board[pos + 1]) == reverse_player(player) && can_go_right(pos) && this->en_passant == pos + 1 + 8 * dirn && this->game_board[pos + 1 + 8 * dirn] == 'f')
            set_bit(area, pos + 8*dirn + 1);
        return area;
    }
    return this->target_areas[pos] & ~this->get_occupied(player);
}

bitset<64> game::get_true_target_area(string player){
    // Get True target area of player
    bitset<64> area (0);
    for(int i = 0; i < 64; i++)
        if(get_player(this->game_board[i]) == player)
            area |= this->get_true_target_area(i, player);
    return area;
}

bool game::is_check(string player){
    int king_pos = find_king(this->game_board, player);
    if (king_pos == -1)
        // King is 'captured' by AI
        return true;
    return test_bit(this->get_true_target_area(reverse_player(player)), king_pos);
}

void game::set_check(string player, unordered_map<short, bitset<64>> enemy_areas){
    short king_pos = find_king(this->game_board, player);
    this->checked = false;
    if(king_pos == -1)
        return;
    for(auto itr = enemy_areas.begin(); itr != enemy_areas.end(); itr++)
        if(test_bit(itr->second, king_pos)){
            this->checked = true;
            this->checked_pieces.push_back(itr->first);
        }
}


void game::update_status(string player){
    // Check For CheckMate Or Draw
    int status = this->check_game_over(reverse_player(player));
    if(status == 1){
        this->game_over = true;
        this->winner = player;
    }
    if(status == -1){
        this->game_over = true;
        this->winner = "";
    }
}

unordered_map<short, bitset<64>> to_map(game GameObj, string player){
    // Converts true target area of player into unordered map
    unordered_map<short, bitset<64>> _map;
    for(int i = 0; i < 64; i++)
        if(get_player(GameObj.game_board[i]) == player)
            _map[i] = GameObj.get_true_target_area(i, player);
    return _map;
}

bool game::promote_pawn(int current, char piece, string player, bool ai=false){
    if(ai){
        // AI CALL
        this->game_board[current] = piece;
        this->target_areas[current] = get_valid_moves(this->game_board, current, player, this->white_castle, this->black_castle);
        this->zobrist_val ^= PRN[this->TT_INDEXES[piece] + current];
        this->set_check(player, to_map(*this, reverse_player(player)));
        //cout<<"Added New Piece From Promotion @ "<<current<<endl;
        this->zobrist_val ^= PRN[0];
        //cout<<"Switch Player @ PP\n";
        return true;
    }
    if(this->pawn_promotion == current){
        // PLAYER CALL
        if(tolower(piece) == 'r' || tolower(piece) == 'q' || tolower(piece) == 'q'){
            if(player == WHITE)
                piece = toupper(piece);
            else
                piece = tolower(piece);
            this->game_board[current] = piece;
            this->target_areas[current] = get_valid_moves(this->game_board, current, player, this->white_castle, this->black_castle);
            this->update_status(player);
            this->set_check(player, to_map(*this, reverse_player(player)));
            this->zobrist_val ^= PRN[this->TT_INDEXES[piece] + current];
            //cout<<"Added New Piece From Promotion @ "<<current<<endl;
            this->zobrist_val ^= PRN[0];
            //cout<<"Switch Player @ PP\n";
            return true;
        }
    }
    return false;
}

void game::set_occupied(string player, int index, bool val){
    if(player == WHITE)
        set_bit(this->white_occupied, index, val);
    else if(player == BLACK)
        set_bit(this->black_occupied, index, val);
}

int game::check_game_over(string player){
    // Check if current player is checkmated or game is drawn(If player has no legal move and not in check)
    // 1 -> Game Over => player has lost
    // 0 -> Game Not Over
    // -1 -> Game is drawn

    /**
     'moves' contain only legal moves
     If it is empty, then the player has no legal moves
     Implying the game is either lost or drawn
    */

    vector<Move> moves = this->get_all_moves(player, false);

    if(moves.empty()){
        if(this->is_check(player))
            return 1;
        return -1;
    }
    if(this->moves_since_last_capture >= 100)
        return -1;
    /// 'player' has a legal move, implying not a draw or checkmate
    return 0;
}

bool game::is_capture(short current, short target, string player){

    if(this->game_board[target] != 'f')
        // Normal capture
        return true;
    char enemy_pawn = 'p';
    if(player == BLACK)
        enemy_pawn = 'P';

    if(this->en_passant == target && this->game_board[current] == enemy_pawn)
        // En Passant
        return true;
    return false;
}

bool comparer(Move a, Move b){
    ///* ! is for sorting in descending order of score
    if(a.score == b.score)
        return false;
    return !(a.score < b.score);
}

bool is_killer(short depth, Move m){
    for(short i=0; i < KillerSize; i ++){
        if(m == KillerMoves[depth][i])
            return true;
    }
    return false;
}

struct ChangedAreas{
    unordered_map <short, bitset<64>> enemy_changed_areas;
    unordered_map <short, bitset<64>> player_changed_areas;
};

ChangedAreas implement_move(game &GameObj, Move m, string player){
    bitset<64> move_board(0);
    set_bit(move_board, m.current);
    set_bit(move_board, m.target);
    char old_piece = GameObj.game_board[m.target];
    GameObj.game_board[m.target] = GameObj.game_board[m.current];
    GameObj.game_board[m.current] = 'f';
    char en_passant_piece;
    array<bool, 2> old_castle;
    if(tolower(GameObj.game_board[m.target]) == 'k' && abs(m.current - m.target) != 2){
        /// King has made a non-castle move, so suspend castling rights
        if(player == WHITE){
            old_castle = GameObj.white_castle;
            GameObj.white_castle = {false, false};
        }
        else{
            old_castle = GameObj.black_castle;
            GameObj.black_castle = {false, false};
        }
    }
    bool is_en_passant = false;
    short en_passant_pos;
    GameObj.set_occupied(player, m.target, 1);
    GameObj.set_occupied(player, m.current, 0);
    /// Essential For Captures
    GameObj.set_occupied(reverse_player(player), m.target, 0);
    bitset<64> new_moves;
    ChangedAreas changes;
    bool last_rank = (m.target < 64 && m.target > 55 && player == BLACK) || (m.target < 8 && m.target >= 0 && player == WHITE);

    if(tolower(GameObj.game_board[m.target]) == 'p')
        if(last_rank)
            GameObj.target_areas[m.target] = 0;
        else
            new_moves = GameObj.get_protected_area(m.target);
    else{
        new_moves = get_valid_moves(GameObj.game_board, m.target, player, GameObj.white_castle, GameObj.black_castle);
        GameObj.target_areas[m.target] = new_moves;
        changes.player_changed_areas[m.target] = new_moves;
    }

    GameObj.target_areas[m.current] = 0;

    /// Implementing En Passant
    if(GameObj.en_passant == m.target && tolower(GameObj.game_board[m.target]) == 'p' && old_piece == 'f'){
        is_en_passant = true;
        short dirn = 1;
        if(player == WHITE)
            dirn = -1;
        short enemy_pos = m.target - dirn*8;
        en_passant_piece = GameObj.game_board[enemy_pos];
        en_passant_pos = enemy_pos;
        GameObj.game_board[enemy_pos] = 'f';
        GameObj.set_occupied(reverse_player(player), enemy_pos, 0);
        GameObj.target_areas[enemy_pos] = 0;
        set_bit(move_board, en_passant_pos);
    }

    for(int i = 0; i < 64; i++)
        if((GameObj.target_areas[i] & move_board).any()){
            /// update target_area
            if(tolower(GameObj.game_board[i]) == 'p')
                new_moves = GameObj.get_protected_area(i);
            else
                new_moves = get_valid_moves(GameObj.game_board, i, get_player(GameObj.game_board[i]), GameObj.white_castle, GameObj.black_castle);
            GameObj.target_areas[i] = new_moves;
            if(get_player(GameObj.game_board[i]) == player)
                changes.player_changed_areas[i] = new_moves;
            else
                changes.enemy_changed_areas[i] = new_moves;
        }

    if(old_piece != 'f')
        GameObj.moves_since_last_capture += 1;
    else
        GameObj.moves_since_last_capture = 0;

    if(is_en_passant){
        /// Remove en_passant piece from zobrist value
        GameObj.zobrist_val ^= PRN[GameObj.TT_INDEXES[en_passant_piece] + en_passant_pos];
        //cout<<"Remove en passant piece as it is captured from "<<en_passant_pos<<endl;
    }
    if(GameObj.en_passant != -1){
        /// Remove en passant file from zobrist value if it was set
        GameObj.zobrist_val ^= PRN[GameObj.tt_en_passant_start + GameObj.en_passant % 8];
        //cout<<"Remove en passant file due to inaction "<<GameObj.en_passant % 8<<endl;
    }
    return changes;
}

bool check_legality(game GameObj, Move m, string player){
    /// Quickly check for legality
    short king_pos = find_king(GameObj.game_board, player);
    if(king_pos == -1)
        /// Player's king is "captured"
        return false;
    if(king_pos == m.current)
        return !test_bit(GameObj.get_protected_area(reverse_player(player)), m.target);
    game NewObj = game(GameObj);
    ChangedAreas areas = implement_move(NewObj, m, player);
    for(auto i = areas.enemy_changed_areas.begin(); i != areas.enemy_changed_areas.end(); i++){
        if(test_bit(i->second, king_pos))
            return false;
    }
    return true;
}

vector<Move> game::get_all_moves(string player, bool ai, bool capture_only, short depth){
    if(this->checked){
        return this->check_move_generator(player);
    }
    vector<Move> non_captures;
    vector<Move> captures;
    vector<Move> Killers;
    vector<int> temp;
    for(int i=0; i < 64; i++){
        if(get_player(this->game_board[i]) == player){
            temp = get_true_pos(this->get_true_target_area(i, player));
            for(auto j=temp.begin(); j != temp.end(); j++){
                if(!this->make_move(Move(i, *j), player, true, ai))
                    continue;
                if(this->is_capture(i, *j, player)){
                    /// To encourage Low value pieces to capture high value pieces
                    captures.push_back(Move(i, *j, abs(piece_vals(this->game_board[*j])) - abs(piece_vals(this->game_board[i]))));
                }
                else if(!capture_only){
                    if (depth != -1 && is_killer(depth, Move(i, *j)))
                        Killers.push_back(Move(i, *j, 0));
                    else
                        non_captures.push_back(Move(i, *j, 0));
                }
            }
        }
    }

    vector<Move> all_moves;
    sort(captures.begin(), captures.end(), comparer);
    all_moves.reserve(captures.size() + non_captures.size() + Killers.size());
    all_moves.insert(all_moves.end(), captures.begin(), captures.end());
    if(!capture_only){
        if(depth != -1)
            all_moves.insert(all_moves.end(), Killers.begin(), Killers.end());
        all_moves.insert(all_moves.end(), non_captures.begin(), non_captures.end());
    }
    return all_moves;
}

bool game::make_move(Move m, string player, bool _reverse, bool ai){
    if(!ai){
        if(get_player(this->game_board[m.current]) != player)
            return false;
        if(!test_bit(this->get_true_target_area(m.current, player), m.target))
            return false;
    }
    char old_piece = this->game_board[m.target];
    ChangedAreas changes;
    if(check_legality(*this, m, player)){
        if(_reverse)
            return true;
        changes = implement_move(*this, m, player);
    }
    else
        return false;

    if(tolower(this->game_board[m.target]) == 'r'){
        int left, right;
        if(player == WHITE){
            left = 56;
            right = 63;
        }
        else{
            left = 0;
            right = 7;
        }
        /// For TT Castle Ref see line 40
        if(m.current == left){
            if(player == WHITE && this->white_castle[0]){
                this->white_castle[0] = false;
                this->zobrist_val ^= PRN[this->tt_castle_start_index + 0 + 0];
                //cout<<"White Castle[0] @ "<<this->tt_castle_start_index + 0 + 0<<endl;
            }
            else if(player == BLACK && this->black_castle[0]){
                this->black_castle[0] = false;
                this->zobrist_val ^= PRN[this->tt_castle_start_index + 4 + 0];
                //cout<<"Black Castle[0] @ "<<this->tt_castle_start_index + 4 + 0<<endl;
            }
        }
        else if(m.current == right){
            if(player == WHITE && this->white_castle[1]){
                this->white_castle[1] = false;
                this->zobrist_val ^= PRN[this->tt_castle_start_index + 2 + 0];
                //cout<<"White Castle[1] @ "<<this->tt_castle_start_index + 2 + 0<<endl;
            }
            else if(player == BLACK && this->black_castle[1]){
                this->black_castle[1] = false;
                this->zobrist_val ^= PRN[this->tt_castle_start_index + 6 + 0];
                //cout<<"Black Castle[1] @ "<<this->tt_castle_start_index + 6 + 0<<endl;
            }
        }
    }
    else if(tolower(this->game_board[m.target]) == 'k'){
        char rook;
        if(player == WHITE)
            rook = 'R';
        else
            rook = 'r';
        if(abs(m.target - m.current) == 2){
            if(m.target == m.current - 2){
                // Queen Side
                this->game_board[m.target + 1] = rook;
                this->game_board[m.target - 2] = 'f';
                this->target_areas[m.target + 1] = get_valid_moves(this->game_board, m.target + 1,  player, {false, false}, {false, false});
                this->set_occupied(player, m.target - 2, 0);
                this->set_occupied(player, m.target + 1, 1);
                this->target_areas[m.target - 2] = 0;
                // Remove Rook from initial
                this->zobrist_val ^= PRN[TT_INDEXES[rook] + m.target - 2];
                //cout<<"Remove rook at "<<m.target - 2<<endl;
                // Fix rook at target
                this->zobrist_val ^= PRN[TT_INDEXES[rook] + m.target + 1];
                //cout<<"Fix rook at "<<m.target + 1<<endl;

            }
            else{
                // King Side
                this->game_board[m.target - 1] = rook;
                this->game_board[m.target + 1] = 'f';
                this->target_areas[m.target - 1] = get_valid_moves(this->game_board, m.target - 1,  player, {false, false}, {false, false});
                this->set_occupied(player, m.target - 1, 1);
                this->set_occupied(player, m.target + 1, 0);
                this->target_areas[m.target + 1] = 0;
                // Remove Rook from initial
                this->zobrist_val ^= PRN[TT_INDEXES[rook] + m.target + 1];
                //cout<<"Remove rook at "<<m.target + 1<<endl;
                // Fix rook at target
                this->zobrist_val ^= PRN[TT_INDEXES[rook] + m.target - 1];
                //cout<<"Fix rook at "<<m.target - 1<<endl;
            }
        }
        // For TT Castle Ref see line 40
        if(player == WHITE){
            if(this->white_castle[0]){
                this->zobrist_val ^= PRN[this->tt_castle_start_index + 0 + 0];
                //cout<<"White Castle[0] @ "<<this->tt_castle_start_index + 0 + 0<<endl;
            }
            if(this->white_castle[1]){
                this->zobrist_val ^= PRN[this->tt_castle_start_index + 2 + 0];
                //cout<<"White Castle[1] @ "<<this->tt_castle_start_index + 2 + 0<<endl;
            }
            this->white_castle = {false, false};
        }
        else if(player == BLACK){
            if(this->black_castle[0]){
                this->zobrist_val ^= PRN[this->tt_castle_start_index + 4 + 0];
                //cout<<"Black Castle[0] @ "<<this->tt_castle_start_index + 4 + 0<<endl;
            }
            if(this->black_castle[1]){
                this->zobrist_val ^= PRN[this->tt_castle_start_index + 6 + 0];
                //cout<<"Black Castle[1] @ "<<this->tt_castle_start_index + 6 + 0<<endl;
            }
            this->black_castle = {false, false};
        }
    }
    else if(tolower(this->game_board[m.target]) == 'p'){
        // Setting Up Pawn Promotion
        short dirn = 1;
        char ai_piece = 'q';

        if(player == WHITE){
            dirn = -1;
            ai_piece = 'Q';
        }
        bool last_rank = (m.target < 64 && m.target > 55 && player == BLACK) || (m.target < 8 && m.target >= 0 && player == WHITE);

        if(last_rank){
            /// Remove Pawn From Zobrist Hash
            this->zobrist_val ^= PRN[this->TT_INDEXES[this->game_board[m.target]] + m.current];
            //cout<<"Remove Pawn(PP) @ "<<m.current<<endl;
            if(!ai){
                this->pawn_promotion = m.target;
            }
            else{
                this->promote_pawn(m.target, ai_piece, player, true);
            }
            return true;
        }

        // Setting Up En Passant
        bool initial_move;
        if(player == BLACK)
            initial_move = m.current < 16 && m.current > 7 && m.target > 23 && m.target < 32;
        else
            initial_move = m.current < 56 && m.current > 47 && m.target < 40 && m.target > 31;
        if(initial_move){
            this->en_passant = m.target - dirn*8;
            this->zobrist_val ^= PRN[this->tt_en_passant_start + this->en_passant % 8];
            //cout<<"Trigger En Passant @ "<<this->en_passant % 8<<" File\n";
            ////cout<<this->tt_en_passant_start + this->en_passant % 8<<" HASHED EN PASSANT\n";
            //cout<<PRN[this->tt_en_passant_start + this->en_passant % 8]<<endl;
        }
        else
            this->en_passant = -1;
    }
    else
        this->en_passant = -1;

    this->checked_pieces.clear();

    this->set_check(reverse_player(player), changes.player_changed_areas);

    /// Update position in zobrist(placed here to avoid cases of pawn promotion)
    this->update_zobrist_val(m.current, m.target, this->game_board[m.target], old_piece);

    /// Switch Player in zobrist
    this->zobrist_val ^= PRN[0];
    //cout<<"Switched Players Zobrist\n";
    return true;
}

vector<short> generate_path(string board, short current, short target){
    vector<short> path;
    if(tolower(board[current]) == 'p' || tolower(board[current]) == 'n')
        /// Can't be blocked
        path.push_back(current);
    else{
        short updation;
        if(tolower(board[current]) == 'q' || tolower(board[current]) == 'b'){
            if((target - current) % 7 == 0)
                /// Minor Diagonal
                updation = 7 * ((target - current) / abs((target - current)));
            else if((target - current) % 9 == 0)
                /// Major Diagonal
                updation = 9 * ((target - current) / abs((target - current)));
        }
        if(tolower(board[current]) == 'q' || tolower(board[current]) == 'r'){
            if(target % 8 == current % 8)
                /// Horizontal
                updation = (target - current) / abs((target - current));
            else if((target - current) % 8 == 0)
                /// Vertical
                updation = 8 * ((target - current) / abs((target - current)));
        }
        for(short i=current;i != target;i += updation)
            path.push_back(i);
    }
    return path;
}


vector<Move> game::check_move_generator(string player){
    vector<Move> non_captures;
    vector<Move> captures;
    short king_pos = find_king(this->game_board, player);
    /// King Moves
    array<int, 8> posns = {-8, 8, 0, 0, 0, 0, 0, 0};
    short k = 2;
    if(can_go_left(king_pos)){
        posns[k++] = -1;
        posns[k++] = -9;
        posns[k++] = 7;
    }
    if(can_go_right(king_pos)){
        posns[k++] = 1;
        posns[k++] = -7;
        posns[k++] = 9;
    }
    for(short i=0; i<posns.size();i++){
        if(king_pos + posns[i] < 63 && king_pos + posns[i] >= 0){
            if(get_player(this->game_board[king_pos + posns[i]]) == player)
                continue;
            if(check_legality(*this, Move(king_pos, king_pos + posns[i]), player)){
                if(is_capture(king_pos, king_pos + posns[i], player))
                    captures.push_back(Move(king_pos, king_pos + posns[i], piece_vals(this->game_board[i])));
                else
                    non_captures.push_back(Move(king_pos, king_pos + posns[i]));
            }
        }
    }
    if(this->checked_pieces.size() == 1){
        /// Capturing the piece or blocking the path
        short dirn;
        char player_pawn;
        /// En passant captures must be treated seperately
        if(player == WHITE){
            dirn = -1;
            player_pawn = 'P';
        }
        else{
            dirn = 1;
            player_pawn = 'p';
        }
        if(this->en_passant - dirn*8 == this->checked_pieces[0]){
            if(this->game_board[this->en_passant - dirn*8 + 1] == player_pawn && can_go_right(this->en_passant))
                captures.push_back(Move(this->en_passant - dirn*8 + 1, this->en_passant, 0));

            if(this->game_board[this->en_passant - dirn*8 - 1] == player_pawn && can_go_left(this->en_passant))
                captures.push_back(Move(this->en_passant - dirn*8 - 1, this->en_passant, 0));
        }

        unordered_map<short, bitset<64>> player_target_area;
        for(short i = 0; i < 64; i++){
            if(get_player(this->game_board[i]) == player)
                player_target_area[i] = this->get_true_target_area(i, player);
        }
        vector<short> path = generate_path(this->game_board, this->checked_pieces[0], king_pos);

        for(auto i = path.begin(); i != path.end(); i++){
            for(auto itr = player_target_area.begin(); itr != player_target_area.end(); itr++){
                if(test_bit(itr->second, *i) && check_legality(*this, Move(itr->first, *i), player)){
                    if(is_capture(itr->first, *i, player))
                        captures.push_back(Move(itr->first, *i, piece_vals(this->game_board[itr->first]) - piece_vals(this->game_board[*i])));
                    else
                        non_captures.push_back(Move(itr->first, *i));
                }
            }
        }


    }
    sort(captures.begin(), captures.end(), comparer);
    vector<Move> all_moves;
    all_moves.reserve(captures.size() + non_captures.size());
    all_moves.insert(all_moves.end(), captures.begin(), captures.end());
    all_moves.insert(all_moves.end(), non_captures.begin(), non_captures.end());
    return all_moves;
}

game::~game()
{
    //destructor
}


// AI functions

class HashEntry{
    public:
    bool ancient;
    short depth;
    unsigned long long zobrist;
    long score;
    Move BestMove;
    short type; /// 1 -> beta cutoff(>=score) , 0 -> exact evaluation(=score)
    HashEntry(short _depth, unsigned long long _zobrist, short _type, long _score, Move _BestMove){
        this->depth = _depth;
        this->zobrist = _zobrist;
        this->type = _type;
        this->score = _score;
        this->ancient = false;
        this->BestMove = _BestMove;
    }
    HashEntry(){
        this->ancient = true;
        this->depth = -1;
    }
    bool replace_hash(HashEntry new_hash){
        if(!this->ancient)
            /// This hash is not old
            return false;
        if(this->depth > new_hash.depth)
            /// This hash searched deeper
            return false;
        if(this->depth < new_hash.depth)
            /// New hash searched deeper
            return true;
        if(new_hash.type == 0)
            /// New hash has exact value
            return true;
        if(this->type == 0)
            /// Old hash has exact value
            return false;

        if(this->type == new_hash.type && this->type == 1){
            /// Both have beta cutoffs, store the 1 with lower cutoff
            return (this->score < new_hash.score);
        }
        /// Store new one by default
        return true;
    }
    void looked(){
        this->ancient = false;
    }
};

array<HashEntry, HashSize> trans_tables;

void initialize_prn(bool debug){
    /// Run once in begining to initialize PRN

    random_device rd;

    mt19937_64 e2(rd());

    uniform_int_distribution<unsigned long long> dist(pow(2, 63), pow(2, 64)-1);

    ofstream file("debug.txt");
    if(debug)
        if(!file.is_open())
            cout<<"File is Fucked";
    for(short i=0; i < PRN.size(); i++){
        PRN[i] = dist(e2);
        if(debug)
            file << PRN[i] <<endl;
    }
}

void game::initial_zobrist_hash(string player){
    /// Initially calculate zobrist value(ONLY CALL ONCE AT BEGINING)

    this->zobrist_val = 0;

    /// PRN[0] for player == Black

    if(player == BLACK)
        this->zobrist_val ^= PRN[0];

    /// Next 10*64 + 2*56 indexes for position of pieces

    for(int i = 0; i < 64; i++)
        if(this->game_board[i] != 'f')
            this->zobrist_val^= PRN[this->TT_INDEXES.at(this->game_board[i]) + i];

    /// Next 8 indexes for Castling

    this->zobrist_val ^= PRN[this->tt_castle_start_index + (short)white_castle[0]];
    this->zobrist_val ^= PRN[this->tt_castle_start_index + 2 +(short)white_castle[1]];
    this->zobrist_val ^= PRN[this->tt_castle_start_index + 4 +(short)black_castle[0]];
    this->zobrist_val ^= PRN[this->tt_castle_start_index + 6 +(short)black_castle[1]];

    /// Next 8 indexes for enpassant file if any

    if(this->en_passant != -1){
        this->zobrist_val ^= PRN[this->tt_en_passant_start + this->en_passant % 8];
    }
}

void game::update_zobrist_val(short current, short target, char new_piece, char old_piece){
    this->zobrist_val ^= PRN[this->TT_INDEXES[new_piece] + current];
    //cout<<"PRN @ "<<TT_INDEXES[new_piece] + current<<" Removing @ "<<current<<endl;
    this->zobrist_val ^= PRN[this->TT_INDEXES[new_piece] + target];
    //cout<<"PRN @ "<<TT_INDEXES[new_piece] + target<<" Placing @ "<<target<<endl;;
    ////cout<<this->TT_INDEXES[new_piece] + current<<" Current "<<this->TT_INDEXES[new_piece] + target<<endl;
    if(old_piece != 'f'){
        this->zobrist_val ^= PRN[this->TT_INDEXES[old_piece] + current];
        //cout<<"PRN @ "<<TT_INDEXES[old_piece] + current<<" Removing(capture) "<<old_piece<<" @ "<<current<<endl;

    }
}

long nodes = 0;

long tt_saves = 0;

long advanced_heuristic(game GameObj, string player, string max_player){

    long score = 0, player_score = 0, enemy_score = 0;
    short pos_multiplier;
    map<string, bitset<64>> psuedo_target_areas;
    psuedo_target_areas[WHITE] = GameObj.get_protected_area(WHITE);
    psuedo_target_areas[BLACK] = GameObj.get_protected_area(BLACK);
    bool initial_rank;

    short piece_multiplier = 1;
    if(max_player == WHITE)
        piece_multiplier = -1;

    short basic_multiplier = 1;
    if(player != max_player)
        basic_multiplier = -1;

    const short const_protection = 5;

    const short const_offensive = 2;

    const short const_mobility = 0.5;

    const short const_strong_threat = 5;

    /// Slight increment for basic threat
    bitset<64> basic_threats = GameObj.get_true_target_area(player) & GameObj.get_occupied(reverse_player(player));
    score += basic_threats.count() * const_offensive * basic_multiplier;

    /// Higher Increment for strong threat
    score += (basic_threats & ~psuedo_target_areas[reverse_player(player)]).count() * const_offensive * const_strong_threat * basic_multiplier;

    /// Mobility Score
    score += (psuedo_target_areas[player] & ~GameObj.get_occupied(player)).count() * const_mobility * basic_multiplier;

    for(short i = 0; i < 64; i++){
        if(GameObj.game_board[i] != 'f'){
            if(get_player(GameObj.game_board[i]) != max_player)
                pos_multiplier = -1;
            else
                pos_multiplier = 1;

            /// Material Score
            score += piece_vals(GameObj.game_board[i]) * piece_multiplier;
            if(get_player(GameObj.game_board[i]) == player)
                player_score += abs(piece_vals(GameObj.game_board[i]));
            else
                enemy_score += abs(piece_vals(GameObj.game_board[i]));

            /// Protection Score
            initial_rank = (i > 7 && i < 16 && get_player(GameObj.game_board[i]) == BLACK) || (i > 47 && i < 56 && get_player(GameObj.game_board[i]) == WHITE);
            if(test_bit(psuedo_target_areas.at(get_player(GameObj.game_board[i])), i) && tolower(GameObj.game_board[i]) == 'p'){
                if(!initial_rank)
                    /// Pawns at initial rank are protected, so to move them forward, protection score wont be applied at initial rank
                    score += const_protection * pos_multiplier;
            }
            else if(test_bit(psuedo_target_areas.at(get_player(GameObj.game_board[i])), i) && tolower(GameObj.game_board[i]) != 'p' )
                score += const_protection * pos_multiplier;
        }
    }
    nodes++;
    return score;
}

long heuristic(game GameObj, string player, string max_player){
    /// Material Score + Protection Score
    long score = 0, player_score = 0, enemy_score = 0;
    short pos_multiplier;
    map<string, bitset<64>> psuedo_target_areas;
    psuedo_target_areas[WHITE] = GameObj.get_protected_area(WHITE);
    psuedo_target_areas[BLACK] = GameObj.get_protected_area(BLACK);
    bool initial_rank;
    short piece_multiplier = 1;
    if(max_player == WHITE)
        piece_multiplier = -1;

    const short const_protection = 5;

    for(int i = 0; i < 64; i++){
        if(GameObj.game_board[i] != 'f'){
            if(get_player(GameObj.game_board[i]) != max_player)
                pos_multiplier = -1;
            else
                pos_multiplier = 1;

            initial_rank = (i > 7 && i < 16 && get_player(GameObj.game_board[i]) == BLACK) || (i > 47 && i < 56 && get_player(GameObj.game_board[i]) == WHITE);

            score += piece_vals(GameObj.game_board[i]) * piece_multiplier;

            if(get_player(GameObj.game_board[i]) == player)
                player_score += abs(piece_vals(GameObj.game_board[i]));
            else
                enemy_score += abs(piece_vals(GameObj.game_board[i]));

            if(test_bit(psuedo_target_areas.at(get_player(GameObj.game_board[i])), i) && tolower(GameObj.game_board[i]) == 'p'){
                if(!initial_rank)
                    /// Pawns at initial rank are protected, so to move them forward, protection score wont be applied at initial rank
                    score += const_protection * pos_multiplier;
            }
            else if(test_bit(psuedo_target_areas.at(get_player(GameObj.game_board[i])), i) && tolower(GameObj.game_board[i]) != 'p' )
                score += const_protection * pos_multiplier;

        }
    }
    nodes++;
    return score;
}


long individual_score(game GameObj ,string player){
    long score = 0;
    for(int i=0;i<64;i++)
        if(get_player(GameObj.game_board[i]) == player){
            score += abs(piece_vals(GameObj.game_board[i]));
        }
    return score;
}

const int late_game_cutoff = 600;

bool check_null_move(game GameObj, string player){

    if(GameObj.is_check(player))
        return false;

    if(individual_score(GameObj, player)  - pow(10, 7) <= late_game_cutoff)
        return false;

    return true;
}

void insert_killer(Move m, short depth){
    for(short i = 0; i < KillerSize - 1; i++)
        // Shift the moves
        KillerMoves[depth][i] = KillerMoves[depth][i + 1];
    KillerMoves[depth][KillerSize - 1] = m;
}

const short R = 2;

const short max_q_depth = 4;

long quiescence_search(game GameObj, string player, long alpha, long beta, bool use_tt, short depth){
    HashEntry this_state = trans_tables[GameObj.zobrist_val % HashSize];
    if(this_state.zobrist == GameObj.zobrist_val && use_tt){
        // Found an entry
        tt_saves++;
        trans_tables[GameObj.zobrist_val % HashSize].looked();

        if(this_state.type == 0)
            // Exact
            return this_state.score;

        else if(this_state.type == 1){
            // Beta cutoff
            if(this_state.score >= beta)
                return this_state.score;
            beta = this_state.score;
        }

    }

    bool is_check = GameObj.is_check(player);
    //long stand_pat = heuristic(GameObj, player, player);
    long stand_pat = advanced_heuristic(GameObj, player, player);
    if(stand_pat >= beta && !is_check)
        return beta;

    if(depth == 0)
        return stand_pat;

    long val;
    const short delta = 500; // Rook Value
    game tempObj;

    // Delta pruning
    if(!is_check && stand_pat < alpha - delta && individual_score(GameObj, player) - pow(10, 7) >=  late_game_cutoff)
        // Cannot improve alpha
        return alpha;
    if(!is_check)
        alpha = stand_pat > alpha ? stand_pat : alpha;

    vector<Move> captures;

    captures = GameObj.get_all_moves(player, true, true);

    for(auto i = captures.begin();i != captures.end(); i++){
        tempObj = game(GameObj);
        tempObj.make_move(*i, player, false, true);
        val = -quiescence_search(tempObj, reverse_player(player), -beta, -alpha, use_tt, depth - 1);
        if(val >= beta){
            return beta;
        }
        alpha = val > alpha ? val : alpha;
    }

    return alpha;

}

map<short, bool> depth_reset;

long negamax(game GameObj, string player, short depth, long alpha, long beta, bool null_move, bool use_tt=true){
    /// Reset the tt age ONLY once every depth
    if(!depth_reset[depth] && use_tt){
        for(int i = 0; i < HashSize  ; i++)
            trans_tables[i].ancient = true;
        depth_reset[depth] = true;
    }

    bool found_pv = false;

    HashEntry this_state = trans_tables[GameObj.zobrist_val % HashSize];
    if(this_state.zobrist == GameObj.zobrist_val && use_tt){
        // Found an entry
        tt_saves++;
        trans_tables[GameObj.zobrist_val % HashSize].looked();
        if(this_state.depth >= depth){
            found_pv = true;
            if(this_state.type == 0)
                // Exact
                return this_state.score;

            else if(this_state.type == 1){
                // Beta cutoff
                if(this_state.score >= beta)
                    return this_state.score;
                beta = this_state.score;
            }
            else if(this_state.type == -1){
                // Alpha cutoff
                if(this_state.score <= alpha)
                    return this_state.score;
                alpha = this_state.score;
            }
        }
    }
    if(depth < 1)
        //return heuristic(GameObj, player, player);
        return quiescence_search(GameObj, player, alpha, beta, use_tt, max_q_depth);

    // Null move pruning
    if(!null_move && check_null_move(GameObj, player) && depth > R){
        int val = -negamax(GameObj, reverse_player(player), depth - R - 1, -beta, -beta + 10, true, use_tt);
        if(val >= beta)
            return beta;
    }
    game tempObj;
    Move BestMove;

    long val;
    vector<Move> moves = GameObj.get_all_moves(player, true, false, depth);
    vector<Move> NewMoves;

    bool use_pv = false;
    if(!null_move && found_pv){
        NewMoves.push_back(this_state.BestMove);
        for(auto itr = moves.begin(); itr != moves.end(); itr++)
            if(*itr == this_state.BestMove)
                use_pv = true;
            else
                NewMoves.push_back(*itr);
        if(!use_pv){
            disp_board(GameObj.game_board);
            cout<<endl<<this_state.BestMove;
            cout<<player<<endl;
            cout<<GameObj.checked<<endl;
            for(auto j = GameObj.checked_pieces.begin();j != GameObj.checked_pieces.end(); j++){
                cout<<*j;
            }
            for(auto j = moves.begin();j != moves.end(); j++){
                cout<<j->current<<' '<<j->target<<endl;
            }
            cout<<"STOPPPPPPPPPPPPPPPPPP\n";
            while(true){
                cout<<"`";
            }
        }
    }


    for(auto i = moves.begin();i != moves.end(); i++){
        tempObj = game(GameObj);
        tempObj.make_move(*i, player, false, true);
        val = -negamax(tempObj, reverse_player(player), depth - 1, -beta, -alpha, null_move, use_tt);
        if(val >= beta){
            // Non capture move that causes a beta cutoff = Killer Move
            if(!GameObj.is_capture(i->current, i->target, player))
                insert_killer(*i, depth);
            // Store beta cutoff
            this_state = HashEntry(depth, GameObj.zobrist_val, 1, beta, *i);
            if(trans_tables[this_state.zobrist % HashSize].replace_hash(this_state))
                trans_tables[this_state.zobrist % HashSize] = this_state;
            return beta;
        }
        if(alpha < val){
            alpha = val;
            BestMove = *i;
        }

    }
    if(alpha == -pow(10, 5)){
        // TODO(No Moves)
    }

    this_state = HashEntry(depth, GameObj.zobrist_val, 0, alpha, BestMove);
    if(trans_tables[this_state.zobrist % HashSize].replace_hash(this_state))
        trans_tables[this_state.zobrist % HashSize] = this_state;
    return alpha;
}

Move negamax_root(game GameObj, string player, short depth, vector<Move> &ordered_moves, bool use_tt, bool use_pv, short &researches, long best_val = -pow(10, 5), Move best_move=Move(-1,-1)){
    /// Negamax Root Call
    long temp_val;
    vector<Move> moves;
    bool pv_move = true;
    Move node;
    if(ordered_moves.size() == 0){
        pv_move = false;
        moves = GameObj.get_all_moves(player, true, false, depth);
    }
    else{
        moves.assign(ordered_moves.begin(), ordered_moves.end());
        ordered_moves.clear();
    }

    game tempObj;
    for(auto temp = moves.begin();temp != moves.end(); temp++){
        tempObj = game(GameObj);
        tempObj.make_move(*temp, player, false, true);
        if(pv_move || !use_pv){
            // FWS
            temp_val = -negamax(tempObj, reverse_player(player), depth - 1, -pow(10, 5), -best_val, false, use_tt);
        }
        else{
            // ZWS
            temp_val = -negamax(tempObj, reverse_player(player), depth - 1, -best_val - 1, -best_val, false, use_tt);
            if(best_val < temp_val){
                // FWS (Re Search)
                researches++;
                temp_val = -negamax(tempObj, reverse_player(player), depth - 1, -pow(10, 5), -best_val, false, use_tt);
            }
        }

        node = Move(temp->current, temp->target, temp_val);
        ordered_moves.push_back(node);
        if(temp_val > best_val){
            best_move = node;
            best_val = temp_val;
        }
        pv_move = false;
    }
    cout<<endl<<best_val<<" at depth "<<depth<<endl;
    return best_move;
}

Move call_ai(game GameObj, string player, short depth){
    nodes = 0;
    tt_saves = 0;
    vector<Move> ordered_moves;
    // Set all TT entries to ancient
    for(short i=depth;i>=0;i--)
        depth_reset[i]=false;
    for(int i = 0; i < HashSize  ; i++)
            trans_tables[i].ancient = true;

    // Reset Killers
    for(short i = 0; i < depth_max; i++)
        for(short j = 0; j < KillerSize; j++)
            KillerMoves[i][j] = Move(-1, -1);

    const short shallow_depth = 3;

    const bool use_tt = true;

    short researches = 0;

    long best_val;

    Move best_move;

    if(depth > shallow_depth){
        // Order the moves according to shallow search & get best_val
        best_move = negamax_root(GameObj, player, shallow_depth, ordered_moves, use_tt, false, researches);
    }
    else
        return negamax_root(GameObj, player, depth, ordered_moves, use_tt, false, researches);

    for(short current_depth = shallow_depth + 1; current_depth <= depth; current_depth++){
        sort(ordered_moves.begin(), ordered_moves.end(), comparer);
        best_val = ordered_moves[0].score;
        //cout<<best_val;
        best_move = negamax_root(GameObj, player, current_depth, ordered_moves, use_tt, true, researches, best_val, best_move);
    }
    cout<<"Nodes "<<nodes<<' '<<tt_saves<<' '<<researches;
    return best_move;
}

