#include "game.h"

using namespace std;

void disp_board(bitset<64> board){
    for(int i=0;i<64;i++){
        if(i % 8 == 0)
            cout<<'\n';
        cout<<board[63-i];

    }
}

int main()
{
    game GameObj = game();
    char board[65] = "rnbqkbnrppppppppffffffffffffffffffffffffffffffffPPPPPPPPRNBQKBNR";
    GameObj.make_move(52, 36, WHITE, false);
    GameObj.make_move(59, 31, WHITE, false);
    cout<<GameObj.make_move(13, 21, BLACK, false);
    cout<<GameObj.is_check(BLACK);
    disp_board(GameObj.get_true_target_area(WHITE));
    return 0;
}
