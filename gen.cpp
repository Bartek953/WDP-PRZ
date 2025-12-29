#include <iostream>
#include <random>
#include <set>
#include <algorithm>
#include <utility>
#include <assert.h>
#include <cmath>

using namespace std;

ostream& operator<<(ostream& os, const pair<int, int>& x){
    return os << "(" << x.first << " - " << x.second << ")";
}

int normal_dist(int a, int b) {
    assert(a <= b);
    // 1. Urządzenie losujące (non-deterministic seed)
    static std::random_device rd;
    
    // 2. Silnik Mersenne Twister (bardzo dobry rozkład i szybkość)
    static std::mt19937 gen(rd());
    
    // 3. Rozkład jednostajny [a, b] - włącznie z b!
    std::uniform_int_distribution<int> distr(a, b);
    
    return distr(gen);
}
int normal_dist(pair<int, int> p){
    return normal_dist(p.first, p.second);
}

int log_dist(int a, int b){
    assert(a <= b);
    // 1. Urządzenie losujące (non-deterministic seed)
    static std::random_device rd;
    // 2. Silnik Mersenne Twister (bardzo dobry rozkład i szybkość)
    static std::mt19937 gen(rd());

    const int maxExp = log2(b - a);

    std::uniform_int_distribution<int> exp_dist(0, maxExp);

    const int Exp = exp_dist(gen);

    std::uniform_int_distribution<int> modifier_dist(0, (b - a) / (1 << Exp));

    const int modifier = modifier_dist(gen);

    return a + (1 << Exp) * modifier;
}

int log_dist(pair<int, int> p){
    return log_dist(p.first, p.second);
}

struct Config {
    const pair<int, int> n_range = {1, 3e6};
    const pair<int, int> U_range = {0, 1e9};
    const pair<int, int> x_range = {0, 1e9};
    const pair<int, int> y_range = {0, 1e9};

    int (*n_dist)(pair<int, int>) = normal_dist;
    int (*other_dist)(pair<int, int>) = normal_dist;

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
    int losuj_n(){
        return n_dist(n_range);
    }
    int losuj_U(){
        return other_dist(U_range);
    }
    int losuj_x(){
        return other_dist(x_range);
    }
    int losuj_y(){
        return other_dist(y_range);
    }
};

int main(){
    Config cfg;

    cerr << "GEN CONFIG: " << cfg.n_range << ' ' << cfg.U_range << ' ' << cfg.x_range << ' ' << cfg.y_range << '\n';
    
    int n = cfg.losuj_n();
    int U = cfg.losuj_U();

    cout << n << ' ' << U << '\n';

    set<int> x;
    while(x.size() < n){
        x.insert(cfg.losuj_x());
    }
    vector<int>y;
    for(int i = 0; i < n; i++){
        y.push_back(cfg.losuj_y());
    }
    int ind = 0;
    for(int xx : x){
        cout << xx << ' ' << y[ind++] << '\n';
    }
}