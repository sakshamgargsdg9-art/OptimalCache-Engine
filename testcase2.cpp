#include <iostream>

struct Node {
    Node* next;
};

int main() {
    // The Math for a Perfect Conflict 
    // Cache: 32KB, 64-Byte Blocks, 32-Way Associative.
    // 32768 / (64 * 32) = 16 Sets.
    // To guarantee addresses map to the EXACT same set (Set 0), 
    // they must be separated by (16 Sets * 64 Bytes) = 1024 Bytes.
    const int STRIDE = 1024; 
    
    // We want 33 blocks to overflow the 32 available slots in the set.
    const int NUM_BLOCKS = 33; 

    // Allocate the memory chunk
    char* memory = new char[NUM_BLOCKS * STRIDE + 100];

    // Map 33 nodes exactly 1024 bytes apart
    Node* nodes[NUM_BLOCKS];
    for (int i = 0; i < NUM_BLOCKS; i++) {
        nodes[i] = (Node*)(memory + i * STRIDE);
    }

    // Link them in a perfect cycle: 0 -> 1 -> 2 ... -> 32 -> 0
    for (int i = 0; i < NUM_BLOCKS - 1; i++) {
        nodes[i]->next = nodes[i + 1];
    }
    nodes[NUM_BLOCKS - 1]->next = nodes[0]; 

    Node* current = nodes[0];

    for (int loop = 0; loop < 500; loop++) {
        current = current->next; current = current->next; current = current->next;
        current = current->next; current = current->next; current = current->next;
        current = current->next; current = current->next; current = current->next;
        current = current->next; current = current->next; current = current->next;
        current = current->next; current = current->next; current = current->next;
        current = current->next; current = current->next; current = current->next;
        current = current->next; current = current->next; current = current->next;
        current = current->next; current = current->next; current = current->next;
        current = current->next; current = current->next; current = current->next;
        current = current->next; current = current->next; current = current->next;
        current = current->next; current = current->next; current = current->next;
    }

    // Print to prevent compiler from deleting the code
    std::cout << "Test completed. Final node: " << current << std::endl;
    
    delete[] memory;
    return 0;
}