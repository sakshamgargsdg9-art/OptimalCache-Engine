#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <vector>
#include <iomanip>
#include <unordered_map>
#include <queue>

using namespace std;

const int CACHE_SIZE = 32768; 
const int BLOCK_SIZE = 64;    
const int ASSOCIATIVITY = 32;  

struct CacheLine {
    bool valid;
    unsigned long long tag;
    unsigned long long block_addr;
};

struct Access {
    unsigned long long block_addr, index, tag;
};

int main() {
    int num_sets = CACHE_SIZE / (BLOCK_SIZE * ASSOCIATIVITY);
    int offset_bits = log2(BLOCK_SIZE);
    int index_bits = log2(num_sets);
    unsigned long long index_mask = (1ULL << index_bits) - 1;

    ifstream trace_file("trace_2024CSB1118.out");
    if (!trace_file.is_open()) return 1;

    string line;
    vector<Access> trace_list; 
    unordered_map<unsigned long long, queue<int>> future_uses; 

    // PASS 1: Build the Oracle
    int current_time = 0;
    while (getline(trace_file, line)) {
        if (line.empty() || line[0] == '#') continue;
        string ip_str, rw, addr_str, size_str;
        stringstream ss(line);
        ss >> ip_str >> rw >> addr_str >> size_str;

        unsigned long long address = stoull(addr_str, nullptr, 16);
        unsigned long long block_addr = address >> offset_bits; 
        unsigned long long index = block_addr & index_mask;
        unsigned long long tag = block_addr >> index_bits;

        trace_list.push_back({block_addr, index, tag});
        future_uses[block_addr].push(current_time); 
        current_time++;
    }

    // PASS 2: Simulate Cache
    vector<vector<CacheLine>> cache(num_sets, vector<CacheLine>(ASSOCIATIVITY));
    for (int i = 0; i < num_sets; i++) {
        for (int j = 0; j < ASSOCIATIVITY; j++) cache[i][j].valid = false;
    }

    long long hits = 0, misses = 0;

    for (int t = 0; t < trace_list.size(); t++) {
        Access acc = trace_list[t];
        future_uses[acc.block_addr].pop(); 

        bool hit = false;
        for (int i = 0; i < ASSOCIATIVITY; i++) {
            if (cache[acc.index][i].valid && cache[acc.index][i].tag == acc.tag) {
                hit = true; hits++; break;
            }
        }

        if (!hit) {
            misses++;
            bool placed = false;
            
            for (int i = 0; i < ASSOCIATIVITY; i++) {
                if (!cache[acc.index][i].valid) {
                    cache[acc.index][i].valid = true;
                    cache[acc.index][i].tag = acc.tag;
                    cache[acc.index][i].block_addr = acc.block_addr;
                    placed = true; break;
                }
            }

            if (!placed) {
                int block_to_evict = -1, furthest_time = -1;

                for (int i = 0; i < ASSOCIATIVITY; i++) {
                    unsigned long long cached_block = cache[acc.index][i].block_addr;

                    if (future_uses[cached_block].empty()) {
                        block_to_evict = i;
                        break; 
                    } 
                    else {
                        int next_use = future_uses[cached_block].front();
                        if (next_use > furthest_time) {
                            furthest_time = next_use;
                            block_to_evict = i;
                        }
                    }
                }
                cache[acc.index][block_to_evict].tag = acc.tag;
                cache[acc.index][block_to_evict].block_addr = acc.block_addr;
            }
        }
    }

    cout << "--- Belady's Optimal Results ---" << endl;
    cout << "Hits: " << hits << " | Misses: " << misses << endl;
    cout << fixed << setprecision(2) << "Hit Rate: " << ((double)hits/trace_list.size())*100 << "%" << endl;
    return 0;
}