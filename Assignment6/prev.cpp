//Author: Gracjan Barski

#include <cstdlib>
#include <iostream>
#include <memory>
#include <climits>
#include "prev.h"
#define MAX_VAL (INT_MAX + (-(long long)INT_MIN)) //everything is shifted by -INT_MIN

using namespace std;

namespace {
typedef struct n node;
typedef shared_ptr<node> Node;

struct n {
    Node left, right;
    int val;
    
    n() : left(nullptr), right(nullptr), val(-1) {}
    n(int i) : left(nullptr), right(nullptr), val(i) {}
    n(Node l, Node r) : left(l), right(r), val(-1) {
        val = max(l ? l->val : -1, r ? r->val : -1);
    }
};

// Builds the part of tree from scratch
Node build(long long pos, int i, long long tl, long long tr) {
    if (tl == tr)
        return make_shared<node>(i);
    long long tm = (tl + tr) / 2;
    if(pos <= tm)
        return make_shared<node>(build(pos, i, tl, tm), nullptr);
    else
        return make_shared<node>(nullptr, build(pos, i, tm + 1, tr));
}

int get_max(Node v, long long tl, long long tr, long long low, long long high) {
    // Case 1 - v is null or sets are disjoint
    if(!v || high < tl || tr < low)
        return -1;
    // Case 2 - tree interval is included in searched interval
    if (low <= tl && tr <= high)
        return v->val;
    // Case 3 - sets overlap but do not include themselves
    long long tm = (tl + tr) / 2;
    return max(get_max(v->left, tl, tm, low, high), get_max(v->right, tm+1, tr, low, high));
}

// Returns the new tree which is the updated version of v
Node update(Node v, long long tl, long long tr, long long pos, int i) {
    if (tl == tr)
        return make_shared<node>(i);
    long long tm = (tl + tr) / 2;
    if (pos <= tm) {
        if(v->left)
            return make_shared<node>(update(v->left, tl, tm, pos, i), v->right);
        else
            return make_shared<node>(build(pos, i, tl, tm), v->right);
    } else {
        if(v->right)
            return make_shared<node>(v->left, update(v->right, tm+1, tr, pos, i));
        else
            return make_shared<node>(v->left, build(pos, i, tm + 1, tr)); 
    }
}

vector<Node> roots; // vector which includes all the versions of trees (their roots)

}

//Everything is shifted by -INT_MIN so that the tree interval can be [0; INT_MAX + (-INT_MIN)]
void init(const std::vector<int> &seq) {
    if(seq.size() == 0)
        return;
    roots.push_back(build(seq[0] - (long long) INT_MIN, 0, 0, MAX_VAL)); //push the first tree
    
    for(int i = 1; i < (int) seq.size(); i++)
        pushBack(seq[i]);
}

int prevInRange(int i, int lo, int hi) {
    return get_max(roots[i], 0, MAX_VAL, lo - (long long) INT_MIN, hi - (long long) INT_MIN);
}

void pushBack(int value) {
    if(roots.size() == 0) {
        init(vector<int>{value});
        return;
    }
    roots.push_back(update(roots.back(), 0, MAX_VAL, ((long long) value) - INT_MIN, (int) roots.size()));
}

// No need to clear the contents of the tree, shared_ptrs to this on their own
void done() {
    roots.clear();
}