#include <vector>
#include <stdio.h>
#include <assert.h>
#include <utility>
#include <iostream>
using namespace std;

/*
Abstract: time complexity O(n)
The basic idea will be to divide program into two steps:
1. Calculation of maximum intervals
    It's easy to see, that only one max-interval can start at certain index,
    because they can't contain each other. Symmetrically only one max-interval
    can end at certain index.
    This allows us to calculate it using two pointers technique and max-queue.
    Both of these algorithms works in O(n), so combined complexity will be O(n).
    Of course number of intervals will be <= n and maximum intervals are sorted
2. For each index choose best interval
    Im doing it in two steps:
    First, I'll be removing intervals that are never the answer
    (These will be O(n) because I can remove interval at most once)
    Then I can just loop over indexes looking at stack of sorted intervals
    (Answers maintain given order)

Notice that I changed indexing (1-n originally, 0-(n-1) here)

Most of the code is library containing structures like Point, Interval and MaxQueue (Monotonic Queue),
which normally could be put into separate file
*/

const int MAX_N = 3e6;
const int MAX_X = 1e9;


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
//!!!!!!!!!!! LIBRARY !!!!!!!!!!!//
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
struct Point {
    int x, y;
    Point(int xx, int yy) : x(xx), y(yy) {}
};
struct Partition {
    long long up, down;
    Partition(long long x, long long y) : up(x), down(y) {}
    bool operator<(const Partition& other){
        return (__int128_t)up * (__int128_t)other.down < (__int128_t)other.up * (__int128_t)down;
    }
};
struct Interval {
    int l, r;
    Partition score;
    Interval(int ll, int rr) : l(ll), r(rr), score(0, 0) {
        assert(0 <= l && l <= r);
    }
    Interval(int ll, int rr, const vector<Point>& points) : l(ll), r(rr), score(0, 0){
        assert(0 <= l && l <= r && r < (int)points.size());
        long long up = (points[(size_t)r].x - points[(size_t)l].x);
        score = Partition(up * up, r - l + 1);
    }
};
bool operator <(const Interval& x, const Interval& y){
    __int128_t res = (__int128_t)x.score.up * (__int128_t)y.score.down - (__int128_t)y.score.up * (__int128_t)x.score.down;
    if(res == 0){
        return x.l > y.l;
    }
    else return res < 0;
}

//just classical two monotonic fifo queues - one with max(), second with min()
struct MinMaxQueue {
    //<value, index>
    vector<pair<int, int>> max_queue, min_queue;
    int L = 0, R = 0;
    int max_end = 0, min_end = 0;

    void reserve(int n){
        max_queue.reserve((size_t)n);
        min_queue.reserve((size_t)n);
    }
    int size(){
        assert(L <= R);
        return R - L;
    }
    bool empty(){
        return size() == 0;
    }
    void push(int x){
        while(max_end < (int)max_queue.size() && max_queue.back().first <= x)
            max_queue.pop_back();
        max_queue.push_back({x, R});

        while(min_end < (int)min_queue.size() && min_queue.back().first >= x)
            min_queue.pop_back();
        min_queue.push_back({x, R});

        R++;
    }
    void pop(){
        assert(!empty());
        if(max_queue[(size_t)max_end].second == L)max_end++;
        if(min_queue[(size_t)min_end].second == L)min_end++;
        L++;
    }
    int get_max(){
        return max_queue[(size_t)max_end].first;
    }
    int get_min(){
        return min_queue[(size_t)min_end].first;
    }
    bool can_add(int x, int U){
        if(empty())
            return true;
        return max(get_max(), x) - min(get_min(), x) <= U;
    }
};


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
//!!!!!!!!!!! CORE LOGIC !!!!!!!!!!!//
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//

//points max memory: 3mln * 2 * 4B = 24MB;
//function max memory: 24MB
void read_data(int& n, int& U, vector<Point>& points){
    cin >> n;
    cin >> U;
    assert(1 <= n && n <= MAX_N && 0 <= U && U <= MAX_X);

    points.reserve((size_t)n);
    int x, y;
    int prev_x = -1;
    for(int i = 0; i < n; i++){
        cin >> x;
        cin >> y;

        assert(0 <= x && x <= MAX_X && prev_x < x);
        prev_x = x;
        assert(0 <= y && y <= MAX_X);

        points.emplace_back(x, y);
    }
}

//Calculation of maximum intervals
//Technique: Two pointers + MinMaxQueue -> O(n)
//points max memory: 24 MB
//mmq max memory: 3mln * 2 * 2 * 4B = 48 MB
//intervals max memory: 3mln * (2 + 4) * 4B = 72 MB
//sum: 144 MB
void calc_max_intervals(const int& n, const int& U, const vector<Point>& points, vector<Interval>& intervals){
    int right = 0;
    MinMaxQueue mmq;
    mmq.reserve(n);
    intervals.reserve((size_t)n);
    for(int left = 0; left < n; left++){
        bool right_changed = false;
        while(right < n && mmq.can_add(points[(size_t)right].y, U)){
            mmq.push(points[(size_t)(right++)].y);
            right_changed = true;
        }
        assert(left < right);
        if(right_changed){
            intervals.emplace_back(left, right - 1, points);
        }
        mmq.pop();
    }
}

//now i would like to get rid of useless intervals
//useless interval is one that isn't result for any point
//Im using property that maximum intervals do not contain each other, they are also sorted
//And that each element is in at least one maximum interval
/*
Observation:
A:..----------.........
B:....-----------......
C.......--------------.
...........^...........
if Score(B) < min(Score(A), Score(C)), then C is useless
in other case C will be result for some point
*/
//this function is O(n), because every element is only once removed from intervals
//and in nested while, only one element can be checked without removing it forever
//max memory: 2 * 72MB = 144MB (+points it gives 168MB in total)


//check whether there's free space between two intervals
//in other words: are there any points between the intervals
inline bool distinct(const Interval& left, const Interval& right){
    return left.r + 1 < right.l;
}
inline bool useful(const Interval& I, const vector<Interval>& left_stack, const vector<Interval>& right_stack){
    if(left_stack.empty() || right_stack.empty() || distinct(left_stack.back(), right_stack.back())
    || left_stack.back() < I || right_stack.back() < I){
        return true;
    }
    else {return false;}
}
void remove_useless(vector<Interval>& intervals){
    vector<Interval> good_intervals;
    good_intervals.reserve(intervals.size());
    //I treat intervals and good_intervals as two stacks

    while(!intervals.empty()){
        //INTERVALS....curr....GOOD_INTERVALS
        Interval curr = intervals.back();
        intervals.pop_back();
        if(useful(curr, intervals, good_intervals)){
            good_intervals.push_back(curr);
        }
        else {
            while(!good_intervals.empty()){
                curr = good_intervals.back();
                good_intervals.pop_back();
                if(useful(curr, intervals, good_intervals)){
                    good_intervals.push_back(curr);
                    break;
                }
            }
        }
    }
    while(!good_intervals.empty()){
        intervals.push_back(good_intervals.back());
        good_intervals.pop_back();
    }
}

//checks if ind is in range [interval.first, interval.second]
inline bool in_range(const int index, const Interval& I){
    return I.l <= index && index <= I.r;
}

//now lets make an observation - given sorted not-useless intervals, result will maintain the order
//proof: we are given intervals A < B, and points x < y, and we're going from right to left
//by contradiction suppose result(y) = A and result(x) = B
//because B does not contain A, so A covers x
//if x is covered by A and B, and res(x) = B, then score(B) > score(A).
//because A does not contain B, therefore y belongs to B, so res(y) = B. Contradiction.
//Image:
//A: ..---------y--..........
//B: ......--x-----------....
//This is O(n) because we're moving pointer thru intervals

//sum: 72 MB (+ 24 MB from existing points)
//intervals are sorted
//I just need to pick best result for each point
//I also know that intervals will be answers in the same order they are sorted
void calc_result(const int n, const vector<Interval>& intervals){
    int interval_ind = 0;
    for(int i = 0; i < n; i++){
        while(interval_ind < (int)intervals.size() && !in_range(i, intervals[(size_t)interval_ind])){
            interval_ind++;
        }
        assert(in_range(i, intervals[(size_t)interval_ind]));

        while(interval_ind + 1 < (int)intervals.size() && in_range(i, intervals[(size_t)interval_ind + 1])
        && intervals[(size_t)interval_ind] < intervals[(size_t)interval_ind + 1]){
            interval_ind++;
        }
        cout << intervals[(size_t)interval_ind].l + 1 << ' ' << intervals[(size_t)interval_ind].r + 1 << '\n';
    }
}


int main(){
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    int n, U;
    vector<Point> points;
    read_data(n, U, points);
    vector<Interval> intervals;
    calc_max_intervals(n, U, points, intervals);
    remove_useless(intervals);
    calc_result(n, intervals);
}
