#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <vector>
#include <iomanip>

using namespace std;

const int CACHE_SIZE = 32768; // 32KB
const int BLOCK_SIZE = 64;    // 64 Bytes
const int ASSOCIATIVITY = 32; // 32-Way Set Associative

struct CacheLine {
    bool valid;
    unsigned long long tag;
    long long last_used; 
};

int main() {
    int num_sets = CACHE_SIZE / (BLOCK_SIZE * ASSOCIATIVITY);
    int offset_bits = log2(BLOCK_SIZE);
    int index_bits = log2(num_sets);
    unsigned long long index_mask = (1ULL << index_bits) - 1;

    vector<vector<CacheLine>> cache(num_sets, vector<CacheLine>(ASSOCIATIVITY));
    for (int i = 0; i < num_sets; i++) {
        for (int j = 0; j < ASSOCIATIVITY; j++) cache[i][j].valid = false;
    }

    long long total_accesses = 0, hits = 0, misses = 0, global_time = 0;

    ifstream trace_file("trace_2024CSB1118.out");
    if (!trace_file.is_open()) return 1;

    string line;
    while (getline(trace_file, line)) {
        if (line.empty() || line[0] == '#') continue;

        string ip_str, rw, addr_str, size_str;
        stringstream ss(line);
        ss >> ip_str >> rw >> addr_str >> size_str;

        unsigned long long address = stoull(addr_str, nullptr, 16);
        unsigned long long index = (address >> offset_bits) & index_mask;
        unsigned long long tag = address >> (offset_bits + index_bits);

        total_accesses++;
        global_time++;
        bool hit = false;

        for (int i = 0; i < ASSOCIATIVITY; i++) {
            if (cache[index][i].valid && cache[index][i].tag == tag) {
                hit = true;
                hits++;
                cache[index][i].last_used = global_time; // LRU Update
                break;
            }
        }

        if (!hit) {
            misses++;
            int lru_index = 0;
            long long oldest_time = global_time + 1; 

            for (int i = 0; i < ASSOCIATIVITY; i++) {
                if (!cache[index][i].valid) {
                    lru_index = i;
                    break; 
                }
                if (cache[index][i].last_used < oldest_time) {
                    oldest_time = cache[index][i].last_used;
                    lru_index = i;
                }
            }

            cache[index][lru_index].valid = true;
            cache[index][lru_index].tag = tag;
            cache[index][lru_index].last_used = global_time;
        }
    }

    cout << "--- LRU Cache Results ---" << endl;
    cout << "Hits: " << hits << " | Misses: " << misses << endl;
    cout << fixed << setprecision(2) << "Hit Rate: " << ((double)hits/total_accesses)*100 << "%" << endl;
    return 0;
}