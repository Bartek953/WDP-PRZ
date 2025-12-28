#include <iostream>
#include <random>
#include <set>
#include <algorithm>
#include <utility>
#include <assert.h>

using namespace std;

ostream& operator<<(ostream& os, const pair<int, int>& x){
    return os << "(" << x.first << " - " << x.second << ")";
}

struct Config {
    const pair<int, int> n_range = {1, 10000};
    const pair<int, int> U_range = {0, 1000000000};
    const pair<int, int> x_range = {0, 1000000000};
    const pair<int, int> y_range = {0, 1000000000};

    Config(){
        assert(1 <= n_range.first && n_range.second <= 3000000 && n_range.first <= n_range.second);
        assert(0 <= U_range.first && U_range.second <= 1000000000);
        assert(U_range.first <= U_range.second);
        assert(0 <= x_range.first && x_range.second <= 1000000000);
        assert(x_range.first <= x_range.second);
        assert(0 <= y_range.first && y_range.second <= 1000000000);
        assert(y_range.first <= y_range.second);

        assert(n_range.second - n_range.first < x_range.second - x_range.first);
    }
};

int losuj(int a, int b) {
    assert(a <= b);
    // 1. Urządzenie losujące (non-deterministic seed)
    static std::random_device rd;
    
    // 2. Silnik Mersenne Twister (bardzo dobry rozkład i szybkość)
    static std::mt19937 gen(rd());
    
    // 3. Rozkład jednostajny [a, b] - włącznie z b!
    std::uniform_int_distribution<int> distr(a, b);
    
    return distr(gen);
}
int losuj(pair<int, int> p){
    return losuj(p.first, p.second);
}

int main(){
    Config cfg;

    cerr << "GEN CONFIG: " << cfg.n_range << ' ' << cfg.U_range << ' ' << cfg.x_range << ' ' << cfg.y_range << '\n';
    
    int n = losuj(cfg.n_range);
    int U = losuj(cfg.U_range);

    cout << n << ' ' << U << '\n';

    set<int> x;
    while(x.size() < n){
        x.insert(losuj(cfg.x_range));
    }
    vector<int>y;
    for(int i = 0; i < n; i++){
        y.push_back(losuj(cfg.y_range));
    }
    int ind = 0;
    for(int xx : x){
        cout << xx << ' ' << y[ind++] << '\n';
    }
}