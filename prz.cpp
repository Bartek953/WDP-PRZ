#include "prz.h"
#include <stdio.h>

//This solution is O(n)

void read_data(int& n, int& U, vector<pair<int, int>>& points){
    cin >> n >> U;
    assert(1 <= n && 0 <= U);

    points.reserve((size_t)n);

    int x, y;
    int prev_x = -1;
    for(int i = 0; i < n; i++){
        //cin >> x >> y;
        scanf("%d %d", &x, &y);
        assert(0 <= x && 0 <= y);
        assert(prev_x < x);

        points.push_back({x, y});
        prev_x = x;
    }
}

//This function calculates maximum intervals and stores them in stack
//Im using fact that any subinterval of U-strict interval is U-strict
//That allows me to use two-pointers technique, which results in complexity O(n)
void calculate_intervals(int n, int U, const vector<pair<int, int>>& points, IntervalStack& intervals){
    MinMaxQueue mq;
    mq.Reserve(n);
    int right = 0;
    for(int left = 0; left < n; left++){
        //[left, right)
        bool right_changed = false;
        while(right < n && mq.CanAdd(points[(size_t)right].second, U)){
            mq.Push(points[(size_t)right].second);
            right++;
            right_changed = true;
        }
        if(right_changed){
            assert(left <= right - 1);
            intervals.Push({left, right - 1});
            //cerr << "Przedzial: " << (left + 1) << " " << right << ' ' << intervals.GetTopScore().GetValue() << '\n';
        }

        mq.Pop();
    }
}

//checks if ind is in range [interval.first, interval.second]
inline bool in_range(int ind, pair<int, int> interval){
    return interval.first <= ind && ind <= interval.second;
}
//check whetever there's free space between two intervals
//in other words: are there any points between the intervals
bool distinct(pair<int, int> left, pair<int, int> right){
    return left.second + 1 < right.first;
}
//now i would like to get rid off useless intervals
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
void remove_useless(IntervalStack& intervals){
    IntervalStack good_intervals(intervals.points);

    good_intervals.Push(intervals.Top());
    intervals.Pop();

    while(!intervals.Empty()){
        pair<int, int> curr_interval = intervals.Top();
        Partition curr_score = intervals.GetTopScore();
        intervals.Pop();

        if(intervals.Empty() || distinct(intervals.Top(), good_intervals.Top()) || 
        curr_score > intervals.GetTopScore() || curr_score >= good_intervals.GetTopScore()){
            good_intervals.Push(curr_interval);
        }
        //this function checks whetever nothing broke in good elements stack
        while(!good_intervals.Empty() && !intervals.Empty()){
            curr_interval = good_intervals.Top();
            curr_score = good_intervals.GetTopScore();
            good_intervals.Pop();

            if(good_intervals.Empty() || distinct(intervals.Top(), good_intervals.Top()) || 
            curr_score > intervals.GetTopScore() || curr_score >= good_intervals.GetTopScore()){
                good_intervals.Push(curr_interval);
                break;
            }
        }
    }
    while(!good_intervals.Empty()){
        intervals.Push(good_intervals.Top());
        good_intervals.Pop();
    }
}

//now lets make an observation - given sorted not-useless intervals, result will sustain the order
//proof: we are given intervals A < B, and points x < y, and we're going from right to left
//by contradiction suppose result(y) = A and result(x) = B
//because B do not contain A, so A covers x
//if x is covered by A and B, and res(x) = B, then score(B) > score(A).
//because A do not contain B, therefore y belongs to B, so res(y) = B. Contradiction.
//Image:
//A: ..---------y--..........
//B: ......--x-----------....
//This is O(n) because we're moving pointer thru intervals
void calculate_result(int n, IntervalStack& intervals, vector<pair<int, int>>& result){
    pair<int, int> curr_interval = {-1, -1};
    Partition curr_score(0, 1);

    result.reserve((size_t)n);

    for(int i = n - 1; i >= 0; i--){

        bool debug = in_range(i, {510, 512});
        debug = false;
        if(debug)cerr << "I = " << i << '\n';

        while(!intervals.Empty() && !in_range(i, curr_interval)){
            if(debug)cerr << "Rozwazamy: " << intervals.Top() << '\n';
            curr_interval = intervals.Top();
            curr_score = intervals.GetTopScore();
            intervals.Pop();
        }
        assert(in_range(i, curr_interval));

        if(debug)cerr<< "Start: " << curr_interval << '\n';

        while(!intervals.Empty() && in_range(i, intervals.Top()) && curr_score <= intervals.GetTopScore()){
            if(debug)cerr<<"Obliczamy " << intervals.Top() << '\n';
            curr_interval = intervals.Top();
            curr_score = intervals.GetTopScore();
            intervals.Pop();
        }
        if(!intervals.Empty() && debug)cerr<< "Nastepny" << intervals.Top() << '\n';
        result[(size_t)i] = curr_interval;
    }
}



int main(){
    //ios::sync_with_stdio(0);
    //cin.tie(0);
    
    cerr << "WZO\n";

    int n, U;
    vector< pair<int, int> > points;
    read_data(n, U, points);

    IntervalStack intervals(points);
    calculate_intervals(n, U, points, intervals);

    remove_useless(intervals);

    vector<pair<int, int>> result((size_t)n);
    calculate_result(n, intervals, result);

    for(auto [l, r] : result){
        //cout << (l + 1) << ' ' << (r + 1) << '\n';
        printf("%d %d\n", l + 1, r + 1);
    }
    
}