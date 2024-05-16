#include "testlib.h"
#include <algorithm>

constexpr int MN_MN[] = {1, 1, 1, 1};
constexpr int MX_MN[] = {1000000000, 1000, 100000, 1000000000};
constexpr int MN_T[] = {0, 0, 0, 0};
constexpr int MX_T[] = {100000, 1000, 100000, 100000};

int main(int argc,char* argv[]) {
    registerValidation(argc, argv);
    
    int c = atoi(argv[2]);

    int m = inf.readInt(MN_MN[c],MX_MN[c]);
    inf.readSpace();
    int n = inf.readInt(MN_MN[c],MX_MN[c]);
    inf.readChar('\n');
    int t = inf.readInt(0, std::min(n, MX_T[c]));
    inf.readSpace();
    int k = inf.readInt(1, std::min(m, n));
    inf.readSpace();
    int r = inf.readInt(1, m);
    inf.readChar('\n');
    int prev=-1;
    for(int i=0;i<t;i++) {
        int x = inf.readInt(1, n);
        ensure(x>prev);
        prev = x;
        if(i==t-1)
            inf.readChar('\n');
        else
            inf.readSpace();
    }
    inf.readEof();
}

