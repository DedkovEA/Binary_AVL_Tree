#include <memory>
#include <iostream>
#include <deque>


template<class T, class Compare=std::less<T>>
class Tree {
    private:
        struct Node;
        class iterator;

        using Node_Ptr = std::shared_ptr<Node>;
        using Weak_Node_Ptr = std::weak_ptr<Node>;

    public:
        using const_iterator = iterator;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        Tree() {};
        
        std::pair<iterator, bool> insert(const T&& insert_value, Compare compare = Compare());
        std::pair<iterator, bool> insert(const T& insert_value, Compare compare = Compare());
        void print();

        // Different iterator getters
        iterator begin() const;
        const_iterator cbegin() const { return begin(); };
        iterator end() const { return iterator(mc_end); };
        const_iterator cend() const { return mc_end; };
        reverse_iterator rbegin() const { return reverse_iterator(mc_end); };
        const_reverse_iterator rcbegin() const { return const_reverse_iterator(mc_end); };
        reverse_iterator rend() const { return reverse_iterator(begin()); };
        const_reverse_iterator rcend() const { return const_reverse_iterator(begin()); };

        iterator before_end() const;

    private:
        Node_Ptr m_root;
        std::shared_ptr<Tree> self = std::shared_ptr<Tree>(this);

        // pass-the-end and befor-begin iterators
        const iterator mc_end = iterator(self);
        const iterator mc_before_begin = iterator(self);

        struct Node {
            Node_Ptr left;
            Node_Ptr right;
            Weak_Node_Ptr parent;

            int8_t diff; // H(L) - H(R)

            T value;

            Node(const Node_Ptr& parent, const T& value);
            Node(const Node_Ptr& parent, const T&& value);
        };

        class iterator : public std::iterator<std::bidirectional_iterator_tag, T> {
            private:
                Weak_Node_Ptr self;
                std::shared_ptr<Tree> owner;

            public:
                using reference       = const T&;

                iterator() = delete;
                iterator(iterator&& init) : self(std::move(init.self)), owner(std::move(init.owner)) {};
                iterator(const iterator& init) : self(init.self), owner(init.owner) {};
                ~iterator() {};

                iterator(const Node_Ptr& init, const std::shared_ptr<Tree>& owner) : self(init), owner(owner) {};
                iterator(const Weak_Node_Ptr& init, const std::shared_ptr<Tree>& owner) : self(init), owner(owner) {};
                iterator(Weak_Node_Ptr&& init, const std::shared_ptr<Tree>& owner) : self(init), owner(owner) {};
                iterator(const std::shared_ptr<Tree>& owner) : owner(owner) {};

                iterator& operator=(const iterator& to_copy); 
                iterator& operator=(iterator&& to_move);

                operator Node_Ptr() const { return self.lock(); };
                operator Weak_Node_Ptr() const { return self; };

                // for LegacyIterator
                const T& operator*() const;
                iterator& operator++();    

                // for LegacyInputIterator
                bool operator==(const iterator&) const; // EqualityComparable
                bool operator!=(const iterator&) const;
                const T* operator->() const;
                iterator operator++(int);

                // for BidirectionalIterator
                iterator& operator--();
                iterator operator--(int);
        };


        // emplaces subtree with top node instead of right or left parent's subtree
        void m_emplace_right(const Node_Ptr& node, const Node_Ptr& parent);
        void m_emplace_left(const Node_Ptr& node, const Node_Ptr& parent);

        // performing rotations with top node given
        void m_rotate_right(const Node_Ptr&);
        void m_rotate_left(const Node_Ptr&);
        void m_big_rotate_right(const Node_Ptr&);
        void m_big_rotate_left(const Node_Ptr&);

        // performing balancing dependent on node b from which we reach top node a to perform rotation with
        void m_left_balance(const Node_Ptr&);
        void m_right_balance(const Node_Ptr&);
};
        

//template<class T, class Compare>
//Tree<T>::Node::Tree_Node();
//Method realization

template<class T, class Compare>
void Tree<T, Compare>::print() {
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

template<class T, class Compare>
std::pair<typename Tree<T, Compare>::iterator, bool> Tree<T, Compare>::insert(const T&& i_value, Compare compare) {
    if(!m_root) {
        m_root = std::make_shared<Node>(Node(mc_before_begin, i_value));
        return std::make_pair<iterator, bool>(iterator(m_root, self), true);
    } else {
        std::shared_ptr<Node> temp = m_root;
        bool not_constructed = true;
        bool contained = false;
        iterator ret_it(self);
        while(not_constructed) {
            if(compare(i_value, temp->value)) {
                if(!(temp->left)) {
                    temp->left = std::make_shared<Node>(Node(temp, i_value));
                    temp->diff++;
                    ret_it = iterator(temp->left, self);
                    not_constructed = false;
                } else {
                    temp = temp->left;
                };
            } else if (compare(temp->value, i_value)) {
                if(!(temp->right)) {
                    temp->right = std::make_shared<Node>(temp, i_value);
                    temp->diff--;
                    ret_it = iterator(temp->right, self);
                    not_constructed = false;
                } else {
                    temp = temp->right;
                };
            } else {
                contained = true;
                ret_it = iterator(temp, self);
                not_constructed = false;
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
        return std::make_pair<iterator, bool>(std::move(ret_it), std::move(!contained));
    };
};
 
template<class T, class Compare>
std::pair<typename Tree<T, Compare>::iterator, bool> Tree<T, Compare>::insert(const T& i_value, Compare compare) {
    return insert(std::move(T(i_value)), compare);
};

// Begin and rbegin iterator getters
template<class T, class Compare>
typename Tree<T, Compare>::iterator Tree<T, Compare>::begin() const {
    Node_Ptr temp = m_root;
    while(temp->left) {
        temp = temp->left;
    };
    return iterator(temp, self);
};

template<class T, class Compare>
typename Tree<T, Compare>::iterator Tree<T, Compare>::before_end() const {
    Node_Ptr temp = m_root;
    while(temp->right) {
        temp = temp->right;
    };
    return iterator(temp, self);
};

//Different rotations and balances

// performing rotations with top node given
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

// performing balancing dependent on node b from which we reach top node a to perform rotation with
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

// emplaces subtree with top node instead of right or left parent's subtree
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


// class Tree<T>::iterator methods

template<class T, class Compare>
typename Tree<T, Compare>::iterator& Tree<T, Compare>::iterator::operator=(typename Tree<T, Compare>::iterator&& to_move) {
    if(&to_move == this) {
        return *this;
    };
    self = std::move(to_move.self); 
    owner = std::move(to_move.owner);
    return *this;
};

template<class T, class Compare>
typename Tree<T, Compare>::iterator& Tree<T, Compare>::iterator::operator=(const typename Tree<T, Compare>::iterator& to_copy) { 
    if(&to_copy == this) {
        return *this;
    };
    self = to_copy.self; 
    owner = to_copy.owner;
    return *this;
};

// for LegacyIterator
template<class T, class Compare>
const T& Tree<T, Compare>::iterator::operator*() const {
    return self.lock()->value;
};

template<class T, class Compare>
typename Tree<T, Compare>::iterator& Tree<T, Compare>::iterator::operator++() {
    if(*this == owner->mc_before_begin) {
        *this = owner->begin();
        return *this;
    };
    Node_Ptr temp = self.lock();
    if(temp->right) {
        temp = temp->right;
        while(temp->left) {
            temp = temp->left;
        };
        self = temp;
        return *this;
    } else {
        while(temp->parent.lock()->left != temp) {
            temp = temp->parent.lock();
            if(temp == owner->m_root) { 
                self = owner->mc_end; 
                return *this; 
            };
        };
        self = temp->parent;
        return *this;
    };
};    

// for LegacyInputIterator
template<class T, class Compare>
bool Tree<T, Compare>::iterator::operator==(const iterator& to_compare) const {        // EqualityComparable
    return (self.lock() == to_compare.self.lock());
};

template<class T, class Compare>
bool Tree<T, Compare>::iterator::operator!=(const iterator& to_compare) const {
    return !(*this == to_compare);
};

template<class T, class Compare>
const T* Tree<T, Compare>::iterator::operator->() const {
    return &(self.lock()->value);
};

template<class T, class Compare>
typename Tree<T, Compare>::iterator Tree<T, Compare>::iterator::operator++(int) {
    iterator temp = *this;
    ++(*this);
    return temp; 
};

// for BidirectionalIterator
template<class T, class Compare>
typename Tree<T, Compare>::iterator& Tree<T, Compare>::iterator::operator--() {
    if(*this == owner->mc_end) {
        *this = owner->before_end();
        return *this;
    };
    Node_Ptr temp = self.lock();
    if(temp->left) {
        temp = temp->left;
        while(temp->right) {
            temp = temp->right;
        };
        self = temp;
        return *this;
    } else {
        while(temp->parent.lock()->right != temp) {
            temp = temp->parent.lock();
            if(temp == owner->m_root) { 
                self = owner->mc_before_begin;
                return *this; 
            };
        };
        self = temp->parent;
        return *this;
    };
};

template<class T, class Compare>
typename Tree<T, Compare>::iterator Tree<T, Compare>::iterator::operator--(int) {
    iterator temp = *this;
    --(*this);
    return temp;
};



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
    std::cout << '\n';
    for(auto it = tr.rbegin(); it != tr.rend(); it++) {
        std::cout << *it << ' ';
    };

    auto res1 = tr.insert(2);
    auto res2 = tr.insert(2);
    std::cout << "\n" << (res1.first == res2.first) << " " << res1.second << " " << res2.second;

    return 0;
};