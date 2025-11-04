#include <cstddef>
#include <memory>
#include <vector>
#include <iostream>
#include <map>

// SIMPLEST pool allocator - fixed size blocks with free list
template <typename T, std::size_t chunk_size = 10>
class MyFirstAllocator {
private:
    // Our memory pool
    static char* pool;
    static void** free_list;  // Linked list of free blocks
    static size_t block_size;
    static size_t total_blocks;
    static bool initialized;
    
    static const size_t POOL_BLOCKS = chunk_size; // Number of blocks in pool
    
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
    
    MyFirstAllocator() = default;
    
    template<typename U>
    MyFirstAllocator(const MyFirstAllocator<U, chunk_size>&) {}
    
    template <typename U>
    struct rebind {
        using other = MyFirstAllocator<U, chunk_size>;
    };
    
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

    void destroy(T* p) {
        p->~T();
    }
};

// Initialize static members
template<typename T, std::size_t chunk_size>
char* MyFirstAllocator<T, chunk_size>::pool = nullptr;

template<typename T, std::size_t chunk_size>
void** MyFirstAllocator<T, chunk_size>::free_list = nullptr;

template<typename T, std::size_t chunk_size>
size_t MyFirstAllocator<T, chunk_size>::block_size = 0;

template<typename T, std::size_t chunk_size>
size_t MyFirstAllocator<T, chunk_size>::total_blocks = 0;

template<typename T, std::size_t chunk_size>
bool MyFirstAllocator<T, chunk_size>::initialized = false;

// Required comparison operators
template<typename T, typename U>
bool operator==(const MyFirstAllocator<T>&, const MyFirstAllocator<U>&) {
    return true;
}

template<typename T, typename U>
bool operator!=(const MyFirstAllocator<T>&, const MyFirstAllocator<U>&) {
    return false;
}

size_t factorial(size_t fac) {
    size_t res = 1;
    if (fac == 0) {
        return 1;
    }
    else {
        for (size_t i = 1; i <= fac; i++) {
            res *= i;
        }
    }

    return res;
}

int main(int argc, char const *argv[])
{
    std::vector<int, MyFirstAllocator<int, 20>> v(0);
    for (size_t i = 0; i < 25; i++)
    {
        v.push_back(i);
        std::cout << v.size() << std::endl;
    }

        // create map with my custom allocator
    std::map<int, int, std::less<int>, MyFirstAllocator<std::pair<int, int>, 10>> my_map;
    std::cout << "create custom std::map<int, int>" << std::endl;
    for (size_t i = 0; i < 10; i++)
    {
        my_map.insert({i, factorial(i)});
    }
    for (std::map<int, int>::iterator it = my_map.begin(); it!= my_map.end(); ++it)
    {
        std::cout << it->first << " " << it->second << std::endl;
    }
    
    return 0;
}
