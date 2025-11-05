#include <cstddef>
#include <memory>
#include <vector>
#include <iostream>
#include <map>

// SIMPLEST pool allocator - fixed size blocks with free list
template <typename T, std::size_t total_blocks = 10>
class MyFirstAllocator {
private:
    // Our memory pool
    char* pool = nullptr;
    void** free_list = nullptr;  // Linked list of free blocks
    size_t block_size = sizeof(T) > sizeof(void*) ? sizeof(T) : sizeof(void*);
    
    // Initialize pool with free list
    void init_pool() {
        // Allocate big chunk
        pool = static_cast<char*>(std::malloc(block_size * total_blocks));
        
        // Build free list - each block points to next free block
        free_list = nullptr;
        for (int i = total_blocks - 1; i >= 0; --i) {
            void** block = reinterpret_cast<void**>(pool + i * block_size);
            *block = free_list;  // Point to previous free block
            free_list = block;   // This block becomes head of free list
        }

        std::cout << "Pool initialized: " << total_blocks << " blocks of " << block_size << " bytes\n";
    }
    
public:
    using value_type = T;
    
    MyFirstAllocator() {
        init_pool();
    }
    
    template<typename U>
    MyFirstAllocator(const MyFirstAllocator<U, total_blocks>&) {}
    
    template <typename U>
    struct rebind {
        using other = MyFirstAllocator<U, total_blocks>;
    };
    
    // Take first block from free list
    T* allocate(size_t n) {
        if (n != 1) {
            std::cout << "Pool only supports single object allocation, falling back to malloc\n";
            return static_cast<T*>(std::malloc(n * sizeof(T)));
        }

        // Check if we have free blocks
        if (!free_list) {
            std::cout << "Pool exhausted! Falling back to malloc\n";
            return static_cast<T*>(std::malloc(sizeof(T)));
        }
        
        // Take first free block
        void* result = free_list;
        free_list = reinterpret_cast<void**>(*free_list);  // Move to next free block
        
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
    
    void print_free_blocks() {
        int count = 0;
        void** current = free_list;
        while (current) {
            count++;
            current = static_cast<void**>(*current);
        }
        std::cout << "Free blocks available: " << count << "\n";
    }
    
    void cleanup() {
        if (pool) {
            std::free(pool);
            pool = nullptr;
            free_list = nullptr;
            std::cout << "Pool destroyed\n";
        }
    }

    void destroy(T* p) {
        p->~T();
    }
};

// Required comparison operators
template<typename T, typename U>
bool operator==(const MyFirstAllocator<T>&, const MyFirstAllocator<U>&) {
    return true;
}

template<typename T, typename U>
bool operator!=(const MyFirstAllocator<T>&, const MyFirstAllocator<U>&) {
    return false;
}