#pragma once

#include <iostream>
#include <deque>
#include <utility>
#include <assert.h>
#include <vector>

using namespace std;

inline int max(int x, int y){
    if(x >= y)
        return x;
    else
        return y;
}
inline int abs(int x){
    return max(x, -x);
}
ostream& operator<<(ostream& os, pair<int, int> interval){
    return os << "(" << interval.first + 1 << " " << interval.second + 1 << ")";
}

template<typename T>
struct QuickDeque {
    int back_ind = 0;
    int front_ind = 0;
    vector<T> vec;

    bool empty(){
        return front_ind - back_ind == 0;
    }
    void push_front(T x){
        if((int)vec.size() == front_ind){
            vec.push_back(x);
            front_ind++;
        }
        else {
            vec[(size_t)(front_ind++)] = x;
        }
    }
    void pop_front(){
        if(!empty()){
            front_ind--;
        }
    }
    //void push_back(T x);
    void pop_back(){
        if(!empty()){
            back_ind++;
        }
    }
    T front(){
        assert(!empty());
        return vec[(size_t)(front_ind - 1)];
    }
    T back(){
        assert(!empty());
        return vec[(size_t)back_ind];
    }
    void reserve(int n){
        if(n > 0)
            vec.reserve((size_t)n);
    }
};

struct MaxQueue {
    int push_index = 0;
    int pop_index = 0;
    bool (*less)(int, int); //specify the order
    //deque<pair<int, int> > dq; //<value, index>
    QuickDeque<pair<int, int>> dq;

    MaxQueue(bool (*order)(int, int)) : less(order) {}

    void Push(int x){
        while(!dq.empty() && less(dq.front().first, x)){
            dq.pop_front();
        }
        dq.push_front({x, push_index++});
    }
    void Pop(){
        if(!dq.empty()){
            if(dq.back().second == pop_index){
                dq.pop_back();
            }
            pop_index++;
        }
    }
    int GetMax(){
        assert(!dq.empty());
        return dq.back().first;
    }
    int Size(){
        return push_index - pop_index;
    }
    void Reserve(int n){
        dq.reserve(n);
    }
};

struct MinMaxQueue {
    MaxQueue maxQ{ [](int x, int y){ return x <= y; } };
    MaxQueue minQ{ [](int x, int y){ return x >= y; } };

    void Push(int x){
        maxQ.Push(x);
        minQ.Push(x);
    }
    void Pop(){
        maxQ.Pop();
        minQ.Pop();
    }
    int GetMax(){
        return maxQ.GetMax();
    }
    int GetMin(){
        return minQ.GetMax();
    }
    int Size(){
        assert(maxQ.Size() == minQ.Size());
        return maxQ.Size(); 
    }
    bool CanAdd(int x, int U){
        if(Size() == 0)
            return true;
        if(max(abs(GetMin() - x), abs(GetMax() - x)) <= U)
            return true;
        else
            return false;
    }
    void Reserve(int n){
        maxQ.Reserve(n);
        minQ.Reserve(n);
    }
};

//paprtitions which we'll be dealing in this program cannot be reduced
struct Partition {
    long long up, down;
    Partition(long long x, long long y) : up(x), down(y) {}
    /*float GetValue(){
        return static_cast<double>(up) / static_cast<double>(down);
    }*/
    bool operator<=(const Partition& other){
        return (__int128_t)up * (__int128_t)other.down <= (__int128_t)other.up * (__int128_t)down;
    }
    bool operator>(const Partition& other){
        return (__int128_t)up * (__int128_t)other.down > (__int128_t)other.up * (__int128_t)down;
    }
    bool operator>=(const Partition& other){
        
        return (__int128_t)up * (__int128_t)other.down >= (__int128_t)other.up * (__int128_t)down;
    }
};

struct IntervalStack {
    //deque<pair<int, int> > dq;
    vector<pair<pair<int, int>, Partition> > dq;
    const vector<pair<int, int>>& points;

    IntervalStack(const vector<pair<int, int>>& points_info) : points(points_info) {}

    Partition GetIntervalScore2(pair<int, int> i){
        assert((int)points.size() > i.second);
        assert(i.first <= i.second);
        long long xr = points[(size_t)i.second].first;
        long long xl = points[(size_t)i.first].first;
        return Partition((xr - xl) * (xr - xl), i.second - i.first + 1);
    }

    void Push(pair<int, int> interval){
        dq.push_back({interval, GetIntervalScore2(interval)});
    }
    void Pop(){
        if(!dq.empty()){
            dq.pop_back();
        }
    }
    pair<int, int> Top(){
        assert(!dq.empty());
        return dq.back().first;
    }
    bool Empty(){
        return dq.empty();
    }
    Partition GetTopScore(){
        assert(!dq.empty());
        return dq.back().second;
    }
    
};