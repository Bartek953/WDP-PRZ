#include <iostream>
#include <vector>
#include <assert.h>
#include <utility>

using namespace std;

int max(int x, int y){
    return (x >= y) * x + (x < y) * y;
}
int min(int x, int y){
    return -max(-x, -y);
}
int abs(int x){
    return max(x, -x);
}

struct Point {
    int x, y;
    Point(int a, int b) : x(a), y(b) {}
};
int maxabs(int x, int y){
    return max(abs(x), abs(y));
}

int n, U;
vector<Point> points;

void read_data(){
    cin >> n >> U;
    assert(1 <= n && 0 <= U);

    int x, y;
    int prev_x = -1;
    points.push_back({0, 0});
    for(int i = 1; i <= n; i++){
        cin >> x >> y;
        assert(0 <= x && 0 <= y);
        assert(prev_x < x);
        prev_x = x;
        points.push_back({x, y});
    }
}

struct Interval {
    int l, r;
    Interval(int ll = 0, int rr = 0) : l(ll), r(rr) { assert(l <= r); }

    __int128_t dx2(){
        assert(l <= r);
        assert(r < points.size());

        long long dx = points[r].x - points[l].x;
        return dx * dx;
    }
    __int128_t dy2(){
        return r - l + 1;
    }
    bool operator <(Interval& other){
        return dx2() * other.dy2() < other.dx2() * dy2();
    }
};
ostream& operator<<(ostream& os, const Interval& i){
    return os << "(" << i.l << ' ' << i.r << ")";
}

vector<Interval> max_intervals;

void calc_max_intervals(){
    for(int left = 1; left <= n; left++){
        int right = left;

        int minY = points[right].y;
        int maxY = points[right].y;
        right++;

        while(right <= n){
            int currY = points[right].y;
            if(maxabs(currY - minY, currY - maxY) > U){
                break;
            }
            minY = min(currY, minY);
            maxY = max(currY, maxY);
            right++;
        }
        if(max_intervals.empty() || max_intervals.back().r < right - 1){
            max_intervals.push_back({left, right - 1});
        }
    }
}

vector<Interval> results;

void calc_res(){
    results.resize(n+1);
    for(int i = 1; i <= n; i++){
        results[i] = Interval(i, i);
        for(Interval I : max_intervals){
            if(results[i] < I && I.l <= i && i <= I.r){
                results[i] = I;
            }
        }
    }
}



int main(){
    read_data();
    calc_max_intervals();
    calc_res();

    for(auto [l, r] : results){
        if(l == 0 && r == 0)
            continue;
        cout << l << ' ' << r << '\n';
    }
}