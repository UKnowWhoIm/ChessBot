#include "game.h"

using namespace std;



void disp_board(char board[65]){
    for(int i=0;i<64;i++){
        if(i % 8 == 0)
            cout<<'\n';
        cout<<board[i];

    }
}

int convert_to_pos(char notation[2]){
    // Receives chess notation(eg A2) and converts to 'pos'
    int pos=0;
    pos += toupper(notation[0]) - 65;
    // 48 is the corresponding ASCII value of '0'
    pos += 8 * (8 - ((int)notation[1] - 48));
    return pos;
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
    game GameObj;
    int current, target;
    char c[2],b[2];
    bool status;
    string player = WHITE;
    GameObj.make_move(62, 47, WHITE, false, false);
    GameObj.make_move(54, 38, WHITE, false, false);
    GameObj.make_move(61, 54, WHITE, false, false);
    GameObj.make_move(60, 62, WHITE, false, false);
    GameObj.make_move(1, 16, BLACK, false, false);
    GameObj.make_move(11, 27, BLACK, false, false);
    GameObj.make_move(3, 19, BLACK, false, false);
    GameObj.make_move(2, 11, BLACK, false, false);
    GameObj.make_move(4, 2, BLACK, false, false);
    disp_board(GameObj.black_occupied);

    cout<<endl<<GameObj.castle[WHITE][0]<<' '<<GameObj.castle[WHITE][1];
    cout<<endl<<GameObj.castle[BLACK][0]<<' '<<GameObj.castle[BLACK][1];
    disp_board(GameObj.game_board);
    /*
    while(!GameObj.game_over){
        get_status(GameObj, player);
        disp_board(GameObj.game_board);
        cout<<"\nEnter Current & Target: ";
        cin>>b;
        //cout<<'\n';
        cin>>c;
        current = convert_to_pos(b);
        target = convert_to_pos(c);
        cout<<current<<' '<<target<<'\n';
        status = GameObj.make_move(current, target, player, false, false);
        if(status){
            if(GameObj.pawn_promotion != -1){
                char piece;
                bool result = false;
                while(!result){
                    cout<<"\nEnter Piece For Promotion: ";
                    cin>>piece;
                    result = GameObj.promote_pawn(target, piece, player, false);
                }
            }
            player = reverse_player(player);
        }
        else
            cout<<"\n\nERROR\n\n";
    }
    */
    return 0;
}
