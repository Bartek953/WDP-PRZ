#include <vector>
#include <stdio.h>
#include <assert.h>
#include <utility>
#include <cstdio>
#include <cstdlib>
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

struct FastIO {
    static const int BUF_SIZE = 1 << 20; // 1MB buffer
    char in_buf[BUF_SIZE], out_buf[BUF_SIZE];
    int in_pos = 0, in_len = 0, out_pos = 0;

    inline char getChar() {
        if (in_pos == in_len) {
            in_pos = 0;
            in_len = (int)fread(in_buf, (size_t)1, (size_t)BUF_SIZE, stdin);
            if (in_len <= 0) return EOF;
        }
        return in_buf[in_pos++];
    }

    inline int readInt() {
        int x = 0;
        char c = getChar();
        
        while (c != EOF && (c < '0' || c > '9')) {
            c = getChar();
        }
        
        if (c == EOF) {
            fprintf(stderr, "[FastIO Error]\n");
            exit(1); 
        }
        
        while (c >= '0' && c <= '9') {
            x = x * 10 + (c - '0');
            c = getChar();
        }
        return x;
    }

    inline void putChar(char c) {
        if (out_pos == BUF_SIZE) {
            fwrite(out_buf, (size_t)1, (size_t)BUF_SIZE, stdout);
            out_pos = 0;
        }
        out_buf[out_pos++] = c;
    }

    void writeInt(int x) {
        if (x == 0) { putChar('0'); return; }
        char s[12];
        int i = 0;
        while (x > 0) {
            s[i++] = (x % 10) + '0';
            x /= 10;
        }
        while (i--) putChar(s[i]);
    }

    void flush() {
        if (out_pos > 0) {
            fwrite(out_buf, (size_t)1, (size_t)out_pos, stdout);
            out_pos = 0;
        }
    }

    ~FastIO() {
        flush();
    }
};
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
    /*bool operator<(const Partition& other){
        //down <= 10^6
        long long x1 = up / down;
        long long r1 = up % down;
        //(up/down) = x1 + (r1/down)

        long long x2 = other.up / other.down;
        long long r2 = other.up % other.down;
        //(other.up/other.down) = x2 + (r2/other.down)

        if(x1 == x2){
            return r1 * other.down < r2 * down;
        }
        else return x1 < x2;
    }*/
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

//deque implementation on vector - it's faster
//push_front() is not available
template<typename T>
struct QuickDeque {
    vector<T> queue;
    int push_index = 0;
    int pop_index = 0;

    void reserve(int n){
        queue.reserve((size_t)n);
    }
    int size(){
        assert(pop_index <= push_index);
        return push_index - pop_index;
    }
    bool empty(){
        return size() == 0;
    }
    void pop_front(){
        if(!empty()){
            pop_index++;
        }
        assert(pop_index <= push_index);
    }
    void push_back(T x){
        assert(pop_index <= push_index);
        assert(push_index <= (int)queue.size());

        if(push_index == (int)queue.size()){
            queue.push_back(x);
            push_index++;
        }
        else {
            queue[(size_t)(push_index++)] = x;
        }
    }
    void pop_back(){
        if(!empty()){
            push_index--;
        }
        assert(pop_index <= push_index);
    }
    T front(){
        assert(!empty());
        return queue[(size_t)pop_index];
    }
    T back(){
        assert(!empty());
        return queue[(size_t)(push_index - 1)];
    }
};
template<typename T>
struct MaxQueue {
    QuickDeque<pair<T, int> > dq;
    int push_index = 0;
    int pop_index = 0;
    bool (*less_in_order)(T, T);

    MaxQueue(bool (*cmp)(T, T)) : less_in_order(cmp) {}

    void reserve(int n){
        dq.reserve(n);
    }
    int size(){
        assert(pop_index <= push_index);
        return push_index - pop_index;
    }
    bool empty(){
        return size() == 0;
    }
    void push(T x){
        while(!dq.empty() && less_in_order(dq.back().first, x)){
            dq.pop_back();
        }
        dq.push_back({x, push_index++});
    }
    void pop(){
        if(!dq.empty() && dq.front().second == pop_index){
            dq.pop_front();
        }
        pop_index++;
        assert(pop_index <= push_index);
    }
    T get_max(){
        assert(!empty());
        return dq.front().first;
    }
};

inline int max(int x, int y){
    if(x >= y)
        return x;
    else
        return y;
}
inline int abs(int x){
    return max(x, -x);
}

struct MinMaxQueue {
    MaxQueue<int> maxQ{ [](int x, int y){ return x <= y; } };
    MaxQueue<int> minQ{ [](int x, int y){ return x >= y; } };

    void reserve(int n){
        maxQ.reserve(n);
        minQ.reserve(n);
    }
    int size(){
        assert(maxQ.size() == minQ.size());
        return maxQ.size(); 
    }
    bool empty(){
        return size() == 0;
    }
    void push(int x){
        maxQ.push(x);
        minQ.push(x);
    }
    void pop(){
        assert(!empty());
        maxQ.pop();
        minQ.pop();
    }
    int get_max(){
        return maxQ.get_max();
    }
    int get_min(){
        return minQ.get_max();
    }
    bool can_add(int x, int U){
        if(empty())
            return true;
        if(max(abs(get_min() - x), abs(get_max() - x)) <= U)
            return true;
        else
            return false;
    }
};



//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
//!!!!!!!!!!! CORE LOGIC !!!!!!!!!!!//
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//

//Im using c-style io for improved speed
//points max memory: 3mln * 2 * 4B = 24MB;
//function max memory: 24MB
void read_data(FastIO& IO, int& n, int& U, vector<Point>& points){
    //int res = scanf("%d %d", &n, &U);
    //assert(res == 2);
    n = IO.readInt();
    U = IO.readInt();
    assert(1 <= n && n <= MAX_N && 0 <= U && U <= MAX_X);

    points.reserve((size_t)n);
    int x, y;
    int prev_x = -1;
    for(int i = 0; i < n; i++){
        //res = scanf("%d %d", &x, &y);
        //assert(res == 2);
        x = IO.readInt();
        y = IO.readInt();

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
    //printf("Comaprision: (%d, %d) (%d, %d), (%d, %d)", left_stack.back().l + 1, left_stack.back().r + 1, I.l + 1, I.r + 1, right_stack.back().l + 1, right_stack.back().r + 1);
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
void calc_result(FastIO& IO, const int n, const vector<Interval>& intervals){
    int interval_ind = 0;
    for(int i = 0; i < n; i++){
        while(interval_ind < (int)intervals.size() && !in_range(i, intervals[(size_t)interval_ind])){
            interval_ind++;
        }
        //printf("%d %d %d\n", i, intervals[interval_ind].l, intervals[interval_ind].r);
        assert(in_range(i, intervals[(size_t)interval_ind]));

        while(interval_ind + 1 < (int)intervals.size() && in_range(i, intervals[(size_t)interval_ind + 1])
        && intervals[(size_t)interval_ind] < intervals[(size_t)interval_ind + 1]){
            interval_ind++;
        }
        //printf("%d %d\n", intervals[(size_t)interval_ind].l + 1, intervals[(size_t)interval_ind].r + 1);
        IO.writeInt(intervals[(size_t)interval_ind].l + 1);
        IO.putChar(' ');
        IO.writeInt(intervals[(size_t)interval_ind].r + 1);
        IO.putChar('\n');
    }
}


int main(){
    FastIO IO;
    int n, U;
    vector<Point> points;
    read_data(IO, n, U, points);
    vector<Interval> intervals;
    calc_max_intervals(n, U, points, intervals);
    remove_useless(intervals);
    calc_result(IO, n, intervals);
}
