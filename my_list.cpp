#include <iostream>
#include <memory>

template <typename T, typename Allocator = std::allocator<T>>
class ListContainer 
{
public:
	ListContainer() {
		m_size = 0;
		m_first = nullptr;
	}

	~ListContainer() {
		Node* current = m_first;
		while (current) {
			Node* next = current->next;
			my_alloc.destroy(current);
			my_alloc.deallocate(current, 1);
			current = next;
		}
	}

    void push_back(T value) {
        Node* new_node = my_alloc.allocate(1); // создание нового узла
        new_node->data = value;
        new_node->next = nullptr; // следующего элемента пока нет

        if (m_first == nullptr) {
            m_first = new_node;
        } else {
            Node* current = m_first;
		    while (current->next != nullptr) {
			    current = current->next;
		    }
            current->next = new_node;
        }
        new_node->data = value; // сохраняем пользовательские данные

        m_size += 1; // обновляем размер
    }

	T operator[] (const size_t pos) const
    {
        if (pos >= m_size)
		{
			// invalid position
			return (T)NULL;
		}
		Node *tmp_node = m_first;

		for (size_t i = 0; i < m_size; ++i) {
			if (i == pos) {
				return tmp_node->data;
			}
			tmp_node = tmp_node->next;
		}

		return (T)NULL;
    }

	size_t size() const { return m_size; }

private:
	struct Node {
		Node* next; // указатель на следующий элемент Node
		T data; // пользовательские данные (хранимый объект)
	};
	Node *m_first;
	size_t m_size;
	using NodeAllocator = typename Allocator::template rebind<Node>::other;
    NodeAllocator my_alloc;
};