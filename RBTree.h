#pragma once
#include <iostream>
#define BLACK 0
#define RED 1
template <class T, class Compare = std::less<T>>
class RBTree {
public:
	RBTree();
	RBTree(const RBTree&);
	RBTree& operator=(const RBTree& tree);
	RBTree(RBTree&&) noexcept;
	RBTree& operator=(RBTree&&) noexcept;
	~RBTree();
	void insert(const T& key);
	void test();
private:
	enum class Flags {
		LEAF
	};
	class Chain
	{
	public:
		Chain();
		Chain(const T&);
		Chain(const Chain&);
		Chain(const Flags&);
		Chain& operator=(const Chain&);
		Chain(Chain&&) noexcept;
		Chain& operator=(Chain&&) noexcept;
		~Chain();
		Chain* right, *left, *parent;
		T value;
		/*true - red, false - black*/
		bool color, isLeaf;
	};
	/*Insert without rotation and s.o., returns true, if tree has changed*/
	bool native_insert(const T&);
	void rotate_left(Chain*);
	void rotate_right(Chain*);

	Chain* root_;
	static Compare comp_;

	template <class U, class Comp>
	friend std::ostream& operator<< (std::ostream& out, const RBTree<U, Comp>& tree);
/*

	int32_t height()
	{
		if (!root_) {
			return 0;
		}
		else {
			return subtree_height(root_);
		}
	}
	bool is_search_tree()
	{
		if (root_ == nullptr) {
			return true;
		}
		else {
			return is_search_subtree(root_);
		}
	}
	int32_t subtree_height(Chain* subtree) {
		if (subtree == nullptr) {
			return 0;
		}
		else {
			return std::max(subtree_height(subtree->left), subtree_height(subtree->right)) + 1;
		}
	}
	bool is_search_subtree(Chain* subtree)
	{
		if (subtree == nullptr) {
			return true;
		}
		if (subtree->left != nullptr && max_value(subtree->left) > subtree->value) {
			return false;
		}
		if (subtree->right != nullptr && min_value(subtree->right) < subtree->value) {
			return false;
		}
		return (is_search_subtree(subtree->left) && is_search_subtree(subtree->right));
	}
	int32_t min_value(Chain* subtree) {
		std::queue<Chain*> q;
		q.push(subtree);
		int32_t min = subtree->value;
		typename RBTree<T>::Chain* ptr = nullptr;
		while (!q.empty())
		{
			ptr = q.front();
			q.pop();
			if (ptr != nullptr) {
				min = std::min(min, ptr->value);
				q.push(ptr->left);
				q.push(ptr->right);
			}
		}
		return min;
	}
	int32_t max_value(Chain* subtree) {
		std::queue<Chain*> q;
		q.push(subtree);
		int32_t max = subtree->value;
		typename RBTree<T>::Chain* ptr = nullptr;
		while (!q.empty())
		{
			ptr = q.front();
			q.pop();
			if (ptr != nullptr) {
				max = std::max(max, ptr->value);
				q.push(ptr->left);
				q.push(ptr->right);
			}
		}
		return max;
	}*/
};
#include "RBTree.inl"
#include "Chain.inl"