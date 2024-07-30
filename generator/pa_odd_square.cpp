#include <bits/stdc++.h>

using namespace std;

using Unif = std::uniform_int_distribution<>;
constexpr int MN_MN[] = {-1, 1, 1, 1};
constexpr int MX_MN[] = {-1, 1000, 100000, 1000000000};
constexpr int MN_T[] = {-1, 0, 0, 0};
constexpr int MX_T[] = {-1, 1000, 100000, 100000};
std::mt19937_64 rng;

void init_rng(const string &str) {
	size_t str_hash = hash<string>{}(str);
	rng = std::mt19937_64(str_hash);
}

void gen_case_random(char fn[], int c) {
    FILE *f = fopen(fn, "w");
    int m, n, t, k, r;
    m = Unif(MN_MN[c], MX_MN[c])(rng);
    n = Unif(MN_MN[c], MX_MN[c])(rng);
    t = Unif(MN_T[c], min(n, MX_T[c]))(rng);
    k = Unif(1, min(m, n))(rng);
    r = Unif(1, m)(rng);
    set<int> ones;
    while(ones.size() != t) {
        ones.insert(Unif(1, n)(rng));
    }
    fprintf(f, "%d %d\n%d %d %d\n", m, n, t, k, r);
    for(int one : ones)
        fprintf(f, "%d%c", one, (--t) == 0 ? '\n' : ' ');
    fclose(f);
}

void gen_case_max_mn(char fn[], int c) {
    FILE *f = fopen(fn, "w");
    int m, n, t, k, r;
    vector<int> line;
    m = Unif(MX_MN[c], MX_MN[c])(rng);
    n = Unif(MX_MN[c], MX_MN[c])(rng);
    t = Unif(MN_T[c], min(n, MX_T[c]))(rng);
    k = Unif(1, min(m, n))(rng);
    r = Unif(1, m)(rng);
    set<int> ones;
    while(ones.size() != t) {
        ones.insert(Unif(1, n)(rng));
    }
    fprintf(f, "%d %d\n%d %d %d\n", m, n, t, k, r);
    for(int one : ones)
        fprintf(f, "%d%c", one, (--t) == 0 ? '\n' : ' ');
    fclose(f);
}

void gen_case_max_mn_small_k(char fn[], int c) {
    FILE *f = fopen(fn, "w");
    int m, n, t, k, r;
    vector<int> line;
    m = Unif(MX_MN[c], MX_MN[c])(rng);
    n = Unif(MX_MN[c], MX_MN[c])(rng);
    t = Unif(MN_T[c], min(n, MX_T[c]))(rng);
    k = Unif(1, min(min(m, n), 20))(rng);
    r = Unif(1, m)(rng);
    set<int> ones;
    while(ones.size() != t) {
        ones.insert(Unif(1, n)(rng));
    }
    fprintf(f, "%d %d\n%d %d %d\n", m, n, t, k, r);
    for(int one : ones)
        fprintf(f, "%d%c", one, (--t) == 0 ? '\n' : ' ');
    fclose(f);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("./test2 test");
        assert(0);
    }

    int c = atoi(argv[1]);

    char f[40];
    // sample: hard coded
    
    int test_cnt=0;
    // random
    for(int i=0;i<10;i++) {
        sprintf(f, "%d_%02d.in", c, test_cnt++);
        init_rng(string(f));
        gen_case_random(f, c);
    }
    // max mn
    for(int i=0;i<10;i++) {
        sprintf(f, "%d_%02d.in", c, test_cnt++);
        init_rng(string(f));
        gen_case_max_mn(f, c);
    }
    
    // small k
    for(int i=0;i<10;i++) {
        sprintf(f, "%d_%02d.in", c, test_cnt++);
        init_rng(string(f));
        gen_case_max_mn_small_k(f, c);
    }
}

