//Author: Gracjan Barski

#include <bits/stdc++.h>
#include "wys.h"
#define TABLE_SIZE 13

using namespace std;

typedef unsigned long long ull;

int n, k, g; // parameters given by the external library
unordered_map<ull, int> M; // keys of the map is hash of current state of lies_table, the value is the worst case depth of search needed to get an answer
int lies_table[TABLE_SIZE]; // table that counts, how many times Ala had to lie, for i-th number to be the result // make it an array?
int counter; // keeps how many entries in lies_table are less or equal than k

/**
 * @brief Returns an unique hash for a current table state 
*/
ull key() {
    ull res = 0ULL;
    for(int i = 1; i <= n; i++)
        res =  res * (k + 2) + min(lies_table[i], k + 1);
    return res;
}

/**
 * @brief Modifies the lies_table based on the current query and answer
*/
void step(int y, bool answer) {
    if(answer) {
        for(int i = n; i >= y; i--)
            if((++lies_table[i]) == k + 1) counter--;
    } else {
        for(int i = 1; i < y; i++)
            if((++lies_table[i]) == k + 1) counter--;
    }
}

/**
 * @brief Reverts the modification made byc step()
*/
void revert_step(int y, bool answer) {
    if(answer) {
        for(int i = y; i <= n; i++)
            if((lies_table[i]--) == k + 1) counter++;
    } else {
        for(int i = y - 1; i >= 1; i--)
            if((lies_table[i]--) == k + 1) counter++;
    }
}

/**
 * @brief Calculates the worst case depth of search for every considered state of game
*/
int count_map(ull current) {
    if(counter == 1) return 0;
    if(M.find(current) != M.end()) return M[current];

    M[current] = 2e9;
    for(int y = 2; y <= n; y++) {
        if(lies_table[y] > k)
            continue;
        step(y, true);
        int temp = count_map(key());
        revert_step(y, true);
        
        step(y, false);
        temp = max(temp, count_map(key()));
        revert_step(y, false);

        M[current] = min(M[current] , temp + 1);
    }

    return M[current];
}

/**
 * @brief Answer and reset all the global values to the initial state
*/
void answer_and_reset() {
    int ans = -1;
    for(int i = 1; i <= n; i++) {
        if(lies_table[i] <= k) 
            ans = i;
        lies_table[i] = 0;
    }
    assert(ans != -1);
    odpowiedz(ans);
    counter = n;
}

/**
 * @brief Simulates the game
*/
void play() {
    ull current_key = 0ULL;
    while(counter > 1) {
        int query = 0;
        for(int y = 2; y <= n; y++) {
            if(lies_table[y] > k)
                continue;
            step(y, true);
            int temp = count_map(key()); // M[key()] is already there or counter == 1 so this is O(1)
            revert_step(y, true);

            step(y, false);
            temp = max(temp, count_map(key()));
            revert_step(y, false);

            if(M[current_key] - 1 == temp) {
                query = y;
                break;
            }
        }

        assert(query != 0);

        step(query, mniejszaNiz(query));
        current_key = key(); // update the key
    }
    
    answer_and_reset();
}

int main() {
    dajParametry(n, k, g);
    
    assert(n < TABLE_SIZE);
    counter = n;
    for(int i = 0; i < TABLE_SIZE; i++)
        lies_table[i] = 0;
    

    auto start = std::chrono::high_resolution_clock::now();
    count_map(0ULL); // 0 is the hash for the empty table
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cerr << "preprocessing time: " << duration.count() << " ms\n";
    
    assert(counter == n);
    for(int i = 1; i <= n; i++)
        assert(lies_table[i] == 0);

    while(g--) 
        play();
    auto finish_play = std::chrono::high_resolution_clock::now();
    auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(finish_play - stop);
    std::cerr << "solution time: " << duration2.count() << " ms\n";
    
    M.clear();
    return 0;
}