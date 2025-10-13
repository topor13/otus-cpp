#include <iostream>
#include <vector>
#include <memory>

// SIMPLEST pool allocator - fixed size blocks with free list
template<typename T>
class PoolAllocator {
private:
    // Our memory pool
    static char* pool;
    static void** free_list;  // Linked list of free blocks
    static size_t block_size;
    static size_t total_blocks;
    static bool initialized;
    
    static const size_t POOL_BLOCKS = 10; // Number of blocks in pool
    
    // Initialize pool with free list
    static void init_pool() {
        if (initialized) return;
        
        block_size = sizeof(T) > sizeof(void*) ? sizeof(T) : sizeof(void*);
        total_blocks = POOL_BLOCKS;
        
        // Allocate big chunk
        pool = static_cast<char*>(std::malloc(block_size * total_blocks));
        
        // Build free list - each block points to next free block
        free_list = nullptr;
        for (int i = total_blocks - 1; i >= 0; --i) {
            void** block = reinterpret_cast<void**>(pool + i * block_size);
            *block = free_list;  // Point to previous free block
            free_list = block;   // This block becomes head of free list
        }
        
        initialized = true;
        std::cout << "Pool initialized: " << total_blocks << " blocks of " << block_size << " bytes\n";
    }
    
public:
    using value_type = T;
    
    PoolAllocator() = default;
    
    template<typename U>
    PoolAllocator(const PoolAllocator<U>&) {}
    
    // Take first block from free list
    T* allocate(size_t n) {
        if (n != 1) {
            std::cout << "Pool only supports single object allocation, falling back to malloc\n";
            return static_cast<T*>(std::malloc(n * sizeof(T)));
        }
        
        init_pool();
        
        // Check if we have free blocks
        if (!free_list) {
            std::cout << "Pool exhausted! Falling back to malloc\n";
            return static_cast<T*>(std::malloc(sizeof(T)));
        }
        
        // Take first free block
        void* result = free_list;
        free_list = *free_list;  // Move to next free block
        
        std::cout << "Allocated block from pool\n";
        return static_cast<T*>(result);
    }
    
    // Return block to free list
    void deallocate(T* ptr, size_t n) {
        if (n != 1) {
            std::cout << "Freeing non-pool memory\n";
            std::free(ptr);
            return;
        }
        
        // Check if pointer is from our pool
        char* char_ptr = reinterpret_cast<char*>(ptr);
        if (char_ptr < pool || char_ptr >= pool + (block_size * total_blocks)) {
            std::cout << "Freeing non-pool memory\n";
            std::free(ptr);
            return;
        }
        
        // Return to free list
        void** block = reinterpret_cast<void**>(ptr);
        *block = free_list;  // Point to current free list head
        free_list = block;   // This block becomes new head
        
        std::cout << "Returned block to pool\n";
    }
    
    static void print_free_blocks() {
        int count = 0;
        void** current = free_list;
        while (current) {
            count++;
            current = static_cast<void**>(*current);
        }
        std::cout << "Free blocks available: " << count << "\n";
    }
    
    static void cleanup() {
        if (pool) {
            std::free(pool);
            pool = nullptr;
            free_list = nullptr;
            initialized = false;
            std::cout << "Pool destroyed\n";
        }
    }
};

// Initialize static members
template<typename T>
char* PoolAllocator<T>::pool = nullptr;

template<typename T>
void** PoolAllocator<T>::free_list = nullptr;

template<typename T>
size_t PoolAllocator<T>::block_size = 0;

template<typename T>
size_t PoolAllocator<T>::total_blocks = 0;

template<typename T>
bool PoolAllocator<T>::initialized = false;

// Required comparison operators
template<typename T, typename U>
bool operator==(const PoolAllocator<T>&, const PoolAllocator<U>&) {
    return true;
}

template<typename T, typename U>
bool operator!=(const PoolAllocator<T>&, const PoolAllocator<U>&) {
    return false;
}

int main() {
    std::cout << "=== SIMPLEST Pool Allocator Demo ===\n\n";
    
    // Manual allocation/deallocation to show pool behavior
    std::cout << "Manual allocation test:\n";
    PoolAllocator<int> allocator;
    
    PoolAllocator<int>::print_free_blocks();
    
    // Allocate some objects
    int* obj1 = allocator.allocate(1);
    int* obj2 = allocator.allocate(1);
    int* obj3 = allocator.allocate(1);
    
    *obj1 = 42;
    *obj2 = 24;
    *obj3 = 99;
    
    std::cout << "Values: " << *obj1 << ", " << *obj2 << ", " << *obj3 << "\n";
    PoolAllocator<int>::print_free_blocks();
    
    // Free middle object
    std::cout << "\nFreeing middle object...\n";
    allocator.deallocate(obj2, 1);
    PoolAllocator<int>::print_free_blocks();
    
    // Allocate again - should reuse freed block
    std::cout << "\nAllocating again (should reuse freed block)...\n";
    int* obj4 = allocator.allocate(1);
    *obj4 = 77;
    
    std::cout << "New value: " << *obj4 << "\n";
    PoolAllocator<int>::print_free_blocks();
    
    // Clean up
    allocator.deallocate(obj1, 1);
    allocator.deallocate(obj3, 1);
    allocator.deallocate(obj4, 1);
    
    std::cout << "\nAfter cleanup:\n";
    PoolAllocator<int>::print_free_blocks();
    
    PoolAllocator<int>::cleanup();
    
    return 0;
}

/*
HOW POOL ALLOCATOR WORKS:

1. INITIALIZATION:
   - Allocate big chunk (e.g., 10 blocks of 4 bytes each)
   - Build free list: each block points to next free block
   - Free list: block0 → block1 → block2 → ... → nullptr

2. ALLOCATION:
   - Take first block from free list
   - Update free list to point to next block
   - Return the block

3. DEALLOCATION:
   - Add returned block to front of free list
   - Block now points to previous free list head
   - Free list head becomes this block

KEY DIFFERENCE FROM ARENA:
- Arena: Can't free individual objects
- Pool: Can free any object, gets added back to free list

MEMORY LAYOUT:
Initial: [free0→1][free1→2][free2→3][free3→null]
After allocating 2: [used][used][free2→3][free3→null]
After freeing first: [free0→2][used][free2→3][free3→null]

HOW TO COMPILE:
g++ -fpermissive pool_allocator.cpp 
*/
