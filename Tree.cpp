#include <memory>
#include <iostream>
#include <deque>


template<class T, class Compare=std::less<T>>
class Tree {
    private:
        struct Node;

        using Node_Ptr = std::shared_ptr<Node>;
        using Weak_Node_Ptr = std::weak_ptr<Node>;

        struct Node {
            //using Node_Ptr = std::shared_ptr<Node>;
            //using Weak_Node_Ptr  = std::weak_ptr<Node>;

            Node_Ptr left;
            Node_Ptr right;
            Weak_Node_Ptr parent;

            int8_t diff; // H(L) - H(R)

            T value;

            Node(const Node_Ptr& parent, const T& value);
            Node(const Node_Ptr& parent, const T&& value);

        };

        Node_Ptr m_root;

        // service variables
        const Node_Ptr mc_ptr_end;
        const Node_Ptr mc_ptr_before_begin;


    public:
        Tree() {};
        
        void insert(const T&& insert_value, Compare compare = Compare());
        void insert(const T& insert_value, Compare compare = Compare());

        void print() {
            std::deque<Node_Ptr> queue;
            queue.push_back(m_root);
            Node_Ptr temp_p;
            while(!queue.empty()) {
                temp_p = queue.front();
                queue.pop_front();
                if(temp_p != nullptr) {
                    std::cout << (temp_p->value) << " ";
                    if(temp_p->left != nullptr) {
                        queue.push_back(temp_p->left);
                    };
                    if(temp_p->right != nullptr) {
                        queue.push_back(temp_p->right);
                    };
                };
            };       
        };

    private:
        void m_emplace_right(const Node_Ptr& node, const Node_Ptr& parent);
        void m_emplace_left(const Node_Ptr& node, const Node_Ptr& parent);

        void m_rotate_right(const Node_Ptr&);
        void m_rotate_left(const Node_Ptr&);
        void m_big_rotate_right(const Node_Ptr&);
        void m_big_rotate_left(const Node_Ptr&);

        void m_left_balance(const Node_Ptr&);
        void m_right_balance(const Node_Ptr&);
};
        

//template<class T, class Compare>
//Tree<T>::Node::Tree_Node();
//Method realization

template<class T, class Compare>
void Tree<T, Compare>::insert(const T&& i_value, Compare compare) {
    if(!m_root) {
        m_root = std::make_shared<Node>(Node(mc_ptr_before_begin, i_value));
    } else {
        std::shared_ptr<Node> temp = m_root;
        bool not_constructed = true;
        while(not_constructed) {
            if(compare(i_value, temp->value)) {
                if(!(temp->left)) {
                    temp->left = std::make_shared<Node>(Node(temp, i_value));
                    temp->diff++;
                    not_constructed = false;
                } else {
                    temp = temp->left;
                };
            } else if (compare(temp->value, i_value)) {
                if(!(temp->right)) {
                    temp->right = std::make_shared<Node>(temp, i_value);
                    temp->diff--;
                    not_constructed = false;
                } else {
                    temp = temp->right;
                };
            };
        };
        while(temp != m_root) {
            if(temp->diff == 0) {
                break;
            };
            Node_Ptr parent_cache = temp->parent.lock();
            if((parent_cache->diff == 1) && (parent_cache->left == temp)) {
                m_left_balance(temp);
            } else if((parent_cache->diff == -1) && (parent_cache->right == temp)) {
                m_right_balance(temp);
            } else {
                if(parent_cache->left == temp) {
                    parent_cache->diff++;
                } else {
                    parent_cache->diff--;
                };
            };
            if(temp == m_root) {break;};
            temp = temp->parent.lock();
        };
    };
};
 
template<class T, class Compare>
void Tree<T, Compare>::insert(const T& i_value, Compare compare) {
    insert(std::move(T(i_value)), compare);
};

//Different rotations and balances

template<class T, class Compare>
void Tree<T, Compare>::m_rotate_right(const Node_Ptr& a) {
    Node_Ptr b = a->left;
    if(a == m_root) {
        m_root = b;
    } else {
        Node_Ptr parent_cache = a->parent.lock();
        if(parent_cache->right == a) {
            parent_cache->right = b;
        } else {
            parent_cache->left = b;
        };
    };
    b->parent = a->parent;

    m_emplace_left(b->right, a);
    m_emplace_right(a, b);
    if(b->diff == 1) {a->diff = 0; b->diff = 0;}
    else if (b->diff == 0) {a->diff = 1; b->diff = -1;};
};

template<class T, class Compare>
void Tree<T, Compare>::m_rotate_left(const Node_Ptr& a) {
    Node_Ptr b = a->right;
    if(a == m_root) {
        m_root = b;
    } else {
        Node_Ptr parent_cache = a->parent.lock();
        if(parent_cache->right == a) {
            parent_cache->right = b;
        } else {
            parent_cache->left = b;
        };
    };
    b->parent = a->parent;
    
    m_emplace_right(b->left, a);
    m_emplace_left(a, b);
    if(b->diff == -1) {a->diff = 0; b->diff = 0;}
    else if (b->diff == 0) {a->diff = -1; b->diff = 1;};
};

template<class T, class Compare>
void Tree<T, Compare>::m_big_rotate_right(const Node_Ptr& a) {
    Node_Ptr b = a->left;
    Node_Ptr c = b->right;
    if(a == m_root) {
        m_root = c;
    } else {
        Node_Ptr parent_cache = a->parent.lock();
        if(parent_cache->right == a) {
            parent_cache->right = c;
        } else {
            parent_cache->left = c;
        };
    };
    c->parent = a->parent;
    uint8_t temp_diff_c = c->diff;

    m_emplace_left(c->right, a);
    m_emplace_right(c->left, b);
    m_emplace_left(b, c);
    m_emplace_right(a, c);

    if (temp_diff_c == -1) {a->diff = -1; b->diff = 0; c->diff = 0;}
    else if (temp_diff_c == 0) {a->diff = 0; b->diff = 0; c->diff = 0;}
    else if (temp_diff_c == 1) {a->diff = 0; b->diff = 1; c->diff = 0;};
};

template<class T, class Compare>
void Tree<T, Compare>::m_big_rotate_left(const Node_Ptr& a) {
    Node_Ptr b = a->right;
    Node_Ptr c = b->left;
    if(a == m_root) {
        m_root = c;
    } else {
        Node_Ptr parent_cache = a->parent.lock();
        if(parent_cache->right == a) {
            parent_cache->right = c;
        } else {
            parent_cache->left = c;
        };
    };
    c->parent = a->parent;
    uint8_t temp_diff_c = c->diff;

    m_emplace_right(c->left, a);
    m_emplace_left(c->right, b);
    m_emplace_right(b, c);
    m_emplace_left(a, c);

    if (temp_diff_c == -1) {a->diff = 1; b->diff = 0; c->diff = 0;}
    else if (temp_diff_c == 0) {a->diff = 0; b->diff = 0; c->diff = 0;}
    else if (temp_diff_c == 1) {a->diff = 0; b->diff = -1; c->diff = 0;};
};

template<class T, class Compare>
void Tree<T, Compare>::m_left_balance(const Node_Ptr& node) {
    Node_Ptr parent = node->parent.lock();
    if(node->diff == -1) {
        m_big_rotate_right(parent);
    } else {
        m_rotate_right(parent);
    };
};

template<class T, class Compare>
void Tree<T, Compare>::m_right_balance(const Node_Ptr& node) {
    Node_Ptr parent = node->parent.lock();
    if(node->diff == 1) {
        m_big_rotate_left(parent);
    } else {
        m_rotate_left(parent);
    };
};

template<class T, class Compare>
void Tree<T, Compare>::m_emplace_right(const Node_Ptr& node, const Node_Ptr& parent) {
    if(node) {
        node->parent = parent;
    };
    parent->right = node;
};

template<class T, class Compare>
void Tree<T, Compare>::m_emplace_left(const Node_Ptr& node, const Node_Ptr& parent) {
    if(node) {
        node->parent = parent;
    };
    parent->left = node;
};


// structure Tree<T>::Node methods

template<class T, class Compare>
Tree<T, Compare>::Node::Node(const Node_Ptr& parent, const T& value) : parent(parent), value(value), diff(0) {};

template<class T, class Compare>
Tree<T, Compare>::Node::Node(const Node_Ptr& parent, const T&& value) : parent(parent), value(value), diff(0) {};

int main() {
    Tree<int> tr;
    int N;
    std::cin >> N;
    int tmp = 0;
    for(int i = 0; i < N; i++) {
        std::cin >> tmp;
        tr.insert(tmp);
    };
    tr.print();

    return 0;
};