#include <iostream>
#include <vector>
#include <memory>

// TRUE custom allocator - manages its own memory pool
template<typename T>
class PoolAllocator {
private:
    // Our memory pool - pre-allocated chunk
    static char* pool;
    static size_t pool_size;
    static char* next_free;  // Points to next available memory
    
    static const size_t POOL_SIZE = 1024; // 1KB pool
    
    // Initialize pool on first use
    static void init_pool() {
        if (!pool) {
            pool = static_cast<char*>(std::malloc(POOL_SIZE));
            next_free = pool;
            pool_size = POOL_SIZE;
            std::cout << "Created memory pool of " << POOL_SIZE << " bytes\n";
        }
    }
    
public:
    using value_type = T;
    
    PoolAllocator() = default;
    
    template<typename U>
    PoolAllocator(const PoolAllocator<U>&) {}
    
    // THIS IS THE CUSTOM PART - allocate from our pool, not system
    T* allocate(size_t n) {
        init_pool();
        
        size_t bytes_needed = n * sizeof(T);
        
        // Check if we have enough space in our pool
        if (next_free + bytes_needed > pool + pool_size) {
            std::cout << "Pool exhausted! Falling back to malloc\n";
            return static_cast<T*>(std::malloc(bytes_needed));
        }
        
        // Give memory from our pool
        T* result = reinterpret_cast<T*>(next_free);
        next_free += bytes_needed;
        
        std::cout << "Allocated " << bytes_needed << " bytes from pool\n";
        std::cout << "Pool usage: " << (next_free - pool) << "/" << pool_size << " bytes\n";
        
        return result;
    }
    
    // IMPORTANT: Pool allocator can't free individual pieces!
    // This is a limitation of simple pool allocators
    void deallocate(T* ptr, size_t n) {
        // Check if pointer is from our pool
        char* char_ptr = reinterpret_cast<char*>(ptr);
        if (char_ptr >= pool && char_ptr < pool + pool_size) {
            std::cout << "Deallocation request for pool memory (ignored - pool doesn't support individual frees)\n";
        } else {
            std::cout << "Freeing non-pool memory\n";
            std::free(ptr);
        }
    }
    
    // Reset entire pool (custom function)
    static void reset_pool() {
        if (pool) {
            next_free = pool;
            std::cout << "Pool reset - all memory available again\n";
        }
    }
    
    static void cleanup() {
        if (pool) {
            std::free(pool);
            pool = nullptr;
            std::cout << "Pool destroyed\n";
        }
    }
};

// Initialize static members
template<typename T>
char* PoolAllocator<T>::pool = nullptr;

template<typename T>
size_t PoolAllocator<T>::pool_size = 0;

template<typename T>
char* PoolAllocator<T>::next_free = nullptr;

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
    std::cout << "=== Custom Pool Allocator Demo ===\n\n";
    
    {
        std::vector<int, PoolAllocator<int>> vec;
        
        std::cout << "Adding elements...\n";
        for (int i = 0; i < 20; ++i) {
            vec.push_back(i);
        }
        
        std::cout << "\nVector contents: ";
        for (int val : vec) {
            std::cout << val << " ";
        }
        std::cout << "\n\n";
        
        // Try to add more elements to see pool behavior
        std::cout << "Adding more elements to test pool limits...\n";
        std::cout << "DEBUG1" << std::endl;
        for (int i = 20; i < 100; ++i) {
            vec.push_back(i);
        }
        std::cout << "DEBUG2" << std::endl;

    }
    
    std::cout << "\nVector destroyed, cleaning up...\n";
    PoolAllocator<int>::cleanup();
    
    return 0;
}

/*
WHAT MAKES THIS TRULY CUSTOM:
1. Pre-allocates a memory pool (1KB chunk)
2. Hands out memory from this pool, not from system
3. Faster allocation (no system calls after pool creation)
4. Memory comes from contiguous block (better cache locality)
5. Can't free individual allocations (pool limitation)

TRADE-OFFS:
+ Much faster allocation
+ Better cache performance
+ Predictable memory usage
- Can't free individual pieces
- Fixed pool size
- Memory waste if pool not fully used
*/
