
template <typename T, std::size_t chunk_size = 10>
class MyFirstAllocator{
    public:
    using value_type = T;

    char* pool;
    size_t block_size = sizeof(T) > sizeof(void*) ? sizeof(T) : sizeof(void*);
    bool initialized = false;

    //struct for allocated nodes
    struct Node {
        alignas(T) char data[sizeof(T) * chunk_size];
        Node* next = nullptr;
    };

    // free nodes list
    void** m_free_list = nullptr;

    // Initialize pool with free list
    void init_pool() {
        if (initialized) return;
        
        // Allocate big chunk
        pool = static_cast<char*>(std::malloc(block_size * chunk_size));
        
        // Build free list - each block points to next free block
        for (int i = chunk_size - 1; i >= 0; --i) {
            void** block = reinterpret_cast<void**>(pool + i * block_size);
            *block = m_free_list;  // Point to previous free block
            m_free_list = block;   // This block becomes head of free list
        }
        
        initialized = true;
        std::cout << "Pool initialized: " << chunk_size << " blocks of " << block_size << " bytes\n";
    }

    T* allocate(size_t n) {
        if (n == 0) {
            return nullptr;
        }

        if (n != 1) {
            std::cout << "Pool only supports single object allocation, falling back to malloc\n";
            return static_cast<T*>(std::malloc(n * sizeof(T)));
        }

        if (m_free_list) {
            // Take first free block
            void* result = m_free_list;
            m_free_list = *m_free_list;  // Move to next free block
            std::cout << "Allocated block from allocated pool\n";
            return static_cast<T*>(result);
        }
        
        // if not free blocks allocate new one
        std::cout << "Pool exhausted! Falling back to malloc\n";
        return static_cast<T*>(std::malloc(sizeof(T)));
    }

    void deallocate(T* p, size_t n) { 
        if (p == nullptr) {
            return;
        }

        if (n != 1) {
            std::cout << "Freeing non-pool memory\n";
            std::free(p);
            return;
        }

        // Check if pointer is from our pool
        char* char_ptr = reinterpret_cast<char*>(p);
        if (char_ptr < pool || char_ptr >= pool + (block_size * chunk_size)) {
            std::cout << "Freeing non-pool memory\n";
            std::free(p);
            return;
        }

        //return block to free_list
        void** block = reinterpret_cast<void**>(p);
        *block = m_free_list;  // Point to current free list head
        m_free_list = block;   // This block becomes new head
        
        std::cout << "Returned block to pool\n";
    }

    template <typename U>
    struct rebind {
        using other = MyFirstAllocator<U, chunk_size>;
    };

    void destroy(T* p) {
        p->~T();
    }
};
