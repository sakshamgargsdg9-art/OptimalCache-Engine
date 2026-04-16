#include <iostream>

int main() {
    // Cache is configured as 32-way associative. 
    // We intentionally cycle through 33 blocks to force continuous evictions.
    const int WAYS = 32;
    const int NUM_BLOCKS = WAYS + 1; 
    const int BLOCK_SIZE = 64; 

    char* memory = new char[NUM_BLOCKS * BLOCK_SIZE];

    for (int i = 0; i < NUM_BLOCKS * BLOCK_SIZE; i++) {
        memory[i] = 1;
    }

    volatile int sum = 0;

    // The Cyclic Thrashing Loop
    for (int loop = 0; loop < 500; loop++) {
        for (int i = 0; i < NUM_BLOCKS; i++) {
            sum += memory[i * BLOCK_SIZE];
        }
    }

    std::cout << "Test completed. Dummy output: " << sum << std::endl;
    delete[] memory;
    return 0;
}