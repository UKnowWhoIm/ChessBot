#include "game.h"

using namespace std;

string file_name = "moves.txt";


short convert_to_pos(char notation[3]){
    // Receives chess notation(eg A2) and converts to 'pos'
    short pos=0;
    pos += toupper(notation[0]) - 65;
    // 48 is the corresponding ASCII value of '0'
    pos += 8 * (8 - ((short)notation[1] - 48));
    return pos;
}

void process_move_file(game &GameObj){
    ifstream move_file(file_name);
    short current, target;
    string player;
    while(!move_file.eof()){
        move_file >> player;
        move_file >> current;
        move_file >> target;
        GameObj.make_move(Move(current, target), player, false, true);
    }
    move_file.close();
}

void dump_move(string player, Move m){
    ofstream move_file(file_name, ios::app);
    move_file << player << ' ' << m.current << ' ' << m.target << '\n';
}

void get_status(game GameObj, string player){
    cout<<"Player: "<<player<<'\n';
    cout<<"Target Area(White):"<<'\n';
    disp_board(GameObj.get_true_target_area(WHITE));
    cout<<"\nTarget Area(Black):"<<'\n';
    disp_board(GameObj.get_true_target_area(BLACK));
    cout<<"\nCheck: "<<GameObj.checked;
    cout<<"\nGameOver: "<<GameObj.game_over;
    cout<<"\nWinner: "<<GameObj.winner<<endl;
}

int main()
{
    /// Initialize PRN
    initialize_prn(true);
    game GameObj;
    string player = WHITE;
    const string human = WHITE;
    const string ai_player = BLACK;
    char notation_target[3], notation_current[3];
    Move a;
    int target, current;
    bool status;
    bool ai_only = true;
    bool play_game = true;
    bool multiplayer = true;
    GameObj.initial_zobrist_hash(player);

    //process_move_file(GameObj);

    cout<<endl<<GameObj.white_castle[0]<<' '<<GameObj.white_castle[1];
    while(!GameObj.game_over && play_game){
        disp_board(GameObj.game_board);
        cout<<endl<<GameObj.white_castle[0]<<" "<<GameObj.white_castle[1]<<" : White"<<endl;
        cout<<GameObj.black_castle[0]<<" "<<GameObj.black_castle[1]<<" : Black"<<endl;
        cout<<"\n"<<player<<"\'s Turn";
        if((player == human || multiplayer )&& !ai_only){
            cout<<"\nEnter Current & Target: ";
            cin>>notation_current;
            cin>>notation_target;
            current = convert_to_pos(notation_current);
            target = convert_to_pos(notation_target);
            cout<<'\n'<<current<<' '<<target<<'\n';
            status = GameObj.make_move(Move(current, target), player, false, false);
            //status = GameObj.make_move(Move(current, target), player, false, true);
        }
        else{
            auto start = std::chrono::high_resolution_clock::now();
            a = call_ai(GameObj, player, 5);
            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
            cout<<'\n'<<a.current<<' '<<a.target<<' '<<duration.count()<<endl;

            //dump_move(player, a);
            if(a == Move(-1, -1)){
                // no moves
            }
            status = GameObj.make_move(a, player, false, true);
        }
        if(status){
            if(GameObj.pawn_promotion != -1){
                if(player != human){
                    GameObj.promote_pawn(a.target, 'q', player, true);
                }
                else{
                    char piece;
                    bool result = false;
                    while(!result){
                        cout<<"\nEnter Piece For Promotion: ";
                        cin>>piece;
                        result = GameObj.promote_pawn(target, piece, player, false);
                    }
                }
            }
            player = reverse_player(player);
        }
        else
            cout<<"\n\nERROR\n\n";
    }
/*
*/
    return 0;
}
