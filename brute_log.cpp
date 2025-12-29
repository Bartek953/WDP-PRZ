#include <map>
#include <set>
#include <vector>
#include <utility>
#include <assert.h>
#include <iostream>

using namespace std;

int n, U;

struct Point {
    int x, y;
    Point(int xx = 0, int yy = 0) : x(xx), y(yy) {}
};

inline bool in_range(int x, pair<int, int> I){
    return I.first <= x && x <= I.second;
}
inline int max(int x, int y){
    if(x < y)
        return y;
    return x;
}
inline int abs(int x){
    return max(x, -x);
}
inline int maxabs(int x, int y){
    return max(abs(x), abs(y));
}

vector<Point> points;

void read_data(){
    cin >> n >> U;
    assert(in_range(n, {1, 3e6}));
    assert(in_range(U, {0, 1e9}));

    points.resize(n + 1);
    int x, y;
    points[0] = Point(0, 0);
    for(int i = 1; i <= n; i++){
        cin >> x >> y;
        assert(in_range(x, {0, 1e9}));
        assert(in_range(y, {0, 1e9}));
        points[i] = Point(x, y);
    }
}

struct Interval {
    int l, r;
    Interval(int ll = 0, int rr = 0) : l(ll), r(rr) { assert(l <= r); }

    __int128_t dx2(){
        assert(l <= r);
        if(l == 0 && r == 0){
            return 0;
        }
        assert(1 <= l && r <= points.size());

        __int128_t dx = points[r].x - points[l].x;
        return dx * dx;
    }
    __int128_t dy2(){
        assert(l <= r);

        return r - l + 1;
    }

    bool operator<(Interval& other){
        if(dx2() * other.dy2() == other.dx2() * dy2())
            return other.l != 0 && l > other.l;
        else
            return dx2() * other.dy2() < other.dx2() * dy2();
    }
};
ostream& operator<<(ostream& os, const Interval& I){
    return os << I.l << ' ' << I.r;
}

struct MinMaxQueue {
    int push_index = 0;
    int pop_index = 0;

    map<int, int> val; //map[index] = value
    set<pair<int, int>> elements; //<value, index>

    int Size(){
        assert(push_index >= pop_index);
        return push_index - pop_index;
    }
    bool Empty(){
        return Size() == 0;
    }
    void Push(int x){
        elements.insert({x, push_index});
        val[push_index] = x;
        push_index++;
    }
    void Pop(){
        if(!Empty()){
            const int index = pop_index;
            const int value = val[index];
            elements.erase({value, index});
            pop_index++;
        }
    }
    int GetMax(){
        assert(!Empty());
        auto last = elements.end();
        last--;
        return (*last).first;
    }
    int GetMin(){
        assert(!Empty());
        return (*elements.begin()).first;
    }
    bool CanAdd(int x){
        if(Empty())
            return true;
        else
            return maxabs(x - GetMax(), x - GetMin()) <= U;
    }
};


vector<Interval> max_intervals;

void calc_max_intervals(){
    int left = 1;
    int right = 1;

    MinMaxQueue mmq;

    while(left <= n){
        bool change = false;
        while(right <= n && mmq.CanAdd(points[right].y)){
            mmq.Push(points[right].y);
            right++;
            change = true;
        }
        assert(left < right);
        assert(!mmq.Empty());
        if(change){
            max_intervals.push_back({left, right - 1});
        }
        left++;
        mmq.Pop();
    }
}

const int base = (1 << 22);
Interval tree[2 * base + 1];

Interval max(Interval& I, Interval& J){
    if(I < J)
        return J;
    else
        return I;
}

void update(int l, int r, Interval I){
    l += base - 1;
    r += base + 1;
    while(l/2 != r/2){
        if(l % 2 == 0)
            tree[l + 1] = max(tree[l + 1], I);
        if(r % 2 == 1)
            tree[r - 1] = max(tree[r - 1], I);

        l /= 2;
        r /= 2;
    }
}
Interval query(int pos){
    const int x = pos;
    Interval res = Interval(pos, pos);
    pos += base;
    // if(x == 9){
    //     cout << "\n9:\n";
    // }
    while(pos > 0){
        // if(x == 9){
        //     cout << tree[pos] << '\n';
        // }
        res = max(res, tree[pos]);
        pos /= 2;
    }
    // if(x == 9)cout << '\n';
    return res;
}

int main(){
    ios::sync_with_stdio(0);
    cin.tie(0);
    cerr << "BRUTE LOG\n";
    read_data();
    assert(n <= 4e6);
    calc_max_intervals();
    for(Interval I : max_intervals){
        update(I.l, I.r, I);
    }
    for(int i = 1; i <= n; i++){
        cout << query(i) << '\n';
    }
}