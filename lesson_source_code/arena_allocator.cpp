#include <iostream>
#include <vector>
#include <memory>

// SIMPLEST arena allocator - just a pointer that moves forward
template<typename T>
class ArenaAllocator {
private:
    // Our memory arena
    static char* arena;
    static char* current;     // Current position in arena
    static size_t arena_size;
    static size_t used;
    static bool initialized;
    
    static const size_t ARENA_SIZE = 1024; // 1KB arena
    
    // Initialize arena
    static void init_arena() {
        if (initialized) return;
        
        arena = static_cast<char*>(std::malloc(ARENA_SIZE));
        current = arena;
        arena_size = ARENA_SIZE;
        used = 0;
        initialized = true;
        
        std::cout << "Arena initialized: " << arena_size << " bytes\n";
    }
    
public:
    using value_type = T;
    
    ArenaAllocator() = default;
    
    template<typename U>
    ArenaAllocator(const ArenaAllocator<U>&) {}
    
    // Just move the pointer forward - THAT'S IT!
    T* allocate(size_t n) {
        init_arena();
        
        size_t bytes_needed = n * sizeof(T);
        
        // Check if we have enough space
        if (used + bytes_needed > arena_size) {
            std::cout << "Arena exhausted! Falling back to malloc\n";
            return static_cast<T*>(std::malloc(bytes_needed));
        }
        
        // Give memory from arena and move pointer
        T* result = reinterpret_cast<T*>(current);
        current += bytes_needed;
        used += bytes_needed;
        
        std::cout << "Allocated " << bytes_needed << " bytes from arena (used: " 
                  << used << "/" << arena_size << ")\n";
        
        return result;
    }
    
    // Arena can't free individual objects - IGNORE IT!
    void deallocate(T* ptr, size_t n) {
        // Check if pointer is from our arena
        char* char_ptr = reinterpret_cast<char*>(ptr);
        if (char_ptr >= arena && char_ptr < arena + arena_size) {
            std::cout << "Ignoring deallocate request for arena memory\n";
            // DO NOTHING - arena can't free individual objects!
        } else {
            std::cout << "Freeing non-arena memory\n";
            std::free(ptr);
        }
    }
    
    // Reset entire arena - THIS IS THE POWER!
    static void reset() {
        if (arena) {
            current = arena;
            used = 0;
            std::cout << "Arena reset - all " << arena_size << " bytes available again\n";
        }
    }
    
    static void print_status() {
        std::cout << "Arena status: " << used << "/" << arena_size << " bytes used\n";
    }
    
    static void cleanup() {
        if (arena) {
            std::free(arena);
            arena = nullptr;
            current = nullptr;
            used = 0;
            initialized = false;
            std::cout << "Arena destroyed\n";
        }
    }
};

// Initialize static members
template<typename T>
char* ArenaAllocator<T>::arena = nullptr;

template<typename T>
char* ArenaAllocator<T>::current = nullptr;

template<typename T>
size_t ArenaAllocator<T>::arena_size = 0;

template<typename T>
size_t ArenaAllocator<T>::used = 0;

template<typename T>
bool ArenaAllocator<T>::initialized = false;

// Required comparison operators
template<typename T, typename U>
bool operator==(const ArenaAllocator<T>&, const ArenaAllocator<U>&) {
    return true;
}

template<typename T, typename U>
bool operator!=(const ArenaAllocator<T>&, const ArenaAllocator<U>&) {
    return false;
}

int main() {
    std::cout << "=== SIMPLEST Arena Allocator Demo ===\n\n";
    
    // Manual allocation test
    std::cout << "Manual allocation test:\n";
    ArenaAllocator<int> allocator;
    
    ArenaAllocator<int>::print_status();
    
    // Allocate some objects
    int* obj1 = allocator.allocate(1);
    int* obj2 = allocator.allocate(5);  // Array of 5 ints
    int* obj3 = allocator.allocate(1);
    
    *obj1 = 42;
    obj2[0] = 10; obj2[1] = 20; obj2[2] = 30; obj2[3] = 40; obj2[4] = 50;
    *obj3 = 99;
    
    std::cout << "obj1: " << *obj1 << "\n";
    std::cout << "obj2: ";
    for (int i = 0; i < 5; i++) std::cout << obj2[i] << " ";
    std::cout << "\nobj3: " << *obj3 << "\n";
    
    ArenaAllocator<int>::print_status();
    
    // Try to free individual object - will be ignored
    std::cout << "\nTrying to free obj2 (will be ignored):\n";
    allocator.deallocate(obj2, 5);
    ArenaAllocator<int>::print_status();
    
    // Allocate more - pointer just keeps moving forward
    std::cout << "\nAllocating more objects:\n";
    int* obj4 = allocator.allocate(3);
    obj4[0] = 100; obj4[1] = 200; obj4[2] = 300;
    
    ArenaAllocator<int>::print_status();
    
    // Reset entire arena - THIS IS THE MAGIC!
    std::cout << "\nResetting entire arena:\n";
    ArenaAllocator<int>::reset();
    ArenaAllocator<int>::print_status();
    
    // Now we can allocate again from the beginning
    std::cout << "\nAllocating after reset:\n";
    int* new_obj = allocator.allocate(1);
    *new_obj = 777;
    std::cout << "new_obj: " << *new_obj << "\n";
    
    ArenaAllocator<int>::print_status();
    
    // Test with std::vector
    std::cout << "\nTesting with std::vector:\n";
    ArenaAllocator<int>::reset();
    
    {
        std::vector<int, ArenaAllocator<int>> vec;
        for (int i = 0; i < 10; i++) {
            vec.push_back(i * 10);
        }
        
        std::cout << "Vector contents: ";
        for (int val : vec) {
            std::cout << val << " ";
        }
        std::cout << "\n";
        
        ArenaAllocator<int>::print_status();
    }
    
    std::cout << "\nVector destroyed, but arena memory still 'used':\n";
    ArenaAllocator<int>::print_status();
    
    ArenaAllocator<int>::cleanup();
    
    return 0;
}

/*
HOW ARENA ALLOCATOR WORKS:

1. INITIALIZATION:
   - Allocate one big chunk (1KB)
   - Set current pointer to start of chunk

2. ALLOCATION:
   - Give memory at current position
   - Move current pointer forward by requested size
   - THAT'S IT! No free lists, no bookkeeping!

3. DEALLOCATION:
   - Individual deallocations are IGNORED
   - Only reset() reclaims all memory at once

MEMORY LAYOUT:
Initial:  [-------- 1KB free space --------]
          ^current

After allocating 3 objects:
[obj1][obj2---array---][obj3][---free---]
                              ^current

After reset():
[-------- 1KB free space --------]
^current (back to start)

KEY ADVANTAGES:
- Allocation is just pointer arithmetic (FASTEST possible)
- Zero fragmentation (all memory is contiguous)
- Very simple (just move a pointer)

KEY LIMITATION:
- Can't free individual objects
- Memory only reclaimed on reset()
- Perfect for "batch processing" patterns
*/
