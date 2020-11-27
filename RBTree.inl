#pragma once
#include "RBTree.h"
#include <queue>
#include <utility>
/*--------------------------Constructors and distructor ----------------*/
template <class T, class Compare>
RBTree<T, Compare>::RBTree()
{
	root_ = nullptr;
}

template <class T, class Compare>
RBTree<T, Compare>::RBTree(const RBTree& other) 
{
	if(root_!=nullptr) {
		delete root_;
	}
	root_ = new typename RBTree<T, Compare>::Chain(*other.root_);
}

template <class T, class Compare>
RBTree<T, Compare>& RBTree<T, Compare>::operator=(const RBTree& other)
{
	if (&other == this) {
		return *this;
	}
	if (root_ != nullptr) {
		delete root_;
	}
	root_ = new typename RBTree<T, Compare>::Chain(*other.root_);
	return *this;
}

template <class T, class Compare>
RBTree<T, Compare>::RBTree(RBTree&& other) noexcept
{
	if (root_ != nullptr) {
		delete root_;
	}
	root_ = other.root_;
	other.root_ = nullptr;
}

template <class T, class Compare>
RBTree<T, Compare>& RBTree<T, Compare>::operator=(RBTree&& other) noexcept
{
	if (&other == this) {
		return *this;
	}

	if (root_ != nullptr) {
		delete root_;
	}
	root_ = other.root_;
	other.root_ = nullptr;
	return *this;
}

template <class T, class Compare>
RBTree<T, Compare>::~RBTree() {
	if (root_ != nullptr) {
		delete root_;
	}
}
/*------------------------------------------------------------------------*/
template <class T, class Compare>
std::ostream& operator<<(std::ostream& out, const RBTree<T, Compare>& tree)
{
	std::queue<typename RBTree<T, Compare>::Chain*> q;
	q.push(tree.root_);
	typename RBTree<T, Compare>::Chain* ptr = nullptr;
	while (!q.empty())
	{
		ptr = q.front();
		q.pop();
		if (ptr != nullptr) {
			std::cout << ptr->value << " ";
			q.push(ptr->left);
			q.push(ptr->right);
		}
	}
	return out;
}