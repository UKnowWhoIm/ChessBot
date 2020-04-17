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

    return 0;
}
