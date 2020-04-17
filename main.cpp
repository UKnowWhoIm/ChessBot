#include <bitset>
#include<stdio.h>
#include<iostream>
#include<map>
#include<ctime>
#include<iterator>
using namespace std;

struct Comparer {
    bool operator() (const bitset<64> &b1, const bitset<64> &b2) const {
        return b1.to_ulong() < b2.to_ulong();
    }
};


void BubbleSort(int arr[100], int n){
    int temp;
    for(int i=0,j=0;i<n;i++)
        for(j=i;j<n;j++)
            if(arr[j] < arr[i]){
                temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
}


int main()
{
    const long N = 10000/2;
    int arr[N];
    for(long i=N; i > 0; i--)
        arr[N-i] = i;

    //cout<<arr[0]<<' '<<arr[N-1];
    time_t a = time(NULL);
    BubbleSort(arr, N);
    cout << time(NULL) - a;
    return 0;
}
