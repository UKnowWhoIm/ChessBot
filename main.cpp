#include "game.h"

using namespace std;

void disp_board(bitset<64> board){
    for(int i=0;i<64;i++){
        if(i % 8 == 0)
            cout<<'\n';
        cout<<board[63-i];

    }
}

int convert_to_pos(char notation[2]){
    // Receives chess notation(eg A2) and converts to 'pos'
    int pos=0;
    pos += toupper(notation[0]) - 65;
    // 48 is the corresponding ASCII value of '0'
    pos += 8 * (8 - (notation[1] - 48));
    return pos;
}

int main()
{

    return 0;
}
