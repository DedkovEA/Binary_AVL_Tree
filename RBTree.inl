#pragma once
#include "RBTree.h"
#include <queue>
#include <utility>
/*--------------------------Constructors and distructor ----------------*/
template <class T, class Compare>
RBTree<T, Compare>::RBTree()
{
	root_ = new Chain(Flags::LEAF);
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
/*---------------------Modifing functions-----------------------------*/

template <class T, class Compare>
typename RBTree<T, Compare>::Chain* RBTree<T, Compare>::BST_insert(const T& key)
{
	Chain* ptr = root_;
	while (!(ptr->isLeaf)) {
		if (!comp_(key, ptr->value) && !comp_(ptr->value, key)) {
			return nullptr;	//The object already exists
		}
		if (comp_(key,ptr->value)) {
			ptr = ptr->left;
		}
		else {
			ptr = ptr->right;
		}
	}
	Chain* tmpParent = ptr->parent;
	(*ptr) = std::move(Chain(key));
	ptr->parent = tmpParent;
	return ptr;
}

template <class T, class Compare>
void RBTree<T, Compare>::rotate_left(Chain* subTree)
{
	if (subTree->right->isLeaf) {
		throw "Invailed left rotation!";
	}
	Chain* newChain = subTree->right;
	newChain->parent = subTree->parent;
	if (subTree->parent == nullptr) {
		root_ = newChain;
	}
	else {
		if (subTree->parent->left == subTree) {
			subTree->parent->left = newChain;
		}
		else {
			subTree->parent->right = newChain;
		}
	}

	subTree->right = newChain->left;
	subTree->right->parent = subTree;

	newChain->left = subTree;
	subTree->parent = newChain;

}

template <class T, class Compare>
void RBTree<T, Compare>::rotate_right(Chain* subTree)
{
	if (subTree->left->isLeaf) {
		throw "Invailed right rotation!";
	}
	Chain* newChain = subTree->left;
	newChain->parent = subTree->parent;
	if (subTree->parent == nullptr) {
		root_ = newChain;
	}
	else {
		if (subTree->parent->left == subTree) {
			subTree->parent->left = newChain;
		}
		else {
			subTree->parent->right = newChain;
		}
	}

	subTree->left = newChain->right;
	subTree->left->parent = subTree;

	newChain->right = subTree;
	subTree->parent = newChain;

}

template <class T, class Compare>
void RBTree<T, Compare>::insert(const T& key)
{
	Chain* insertedChain = BST_insert(key);
	if (insertedChain==nullptr) {
		return;
	}
	else {
		ins_balance(insertedChain);
	}

}

template <class T, class Compare>
void RBTree<T, Compare>::ins_balance(Chain* subTree)
{
	Chain* parent = subTree->parent;
	Chain* grandad = nullptr;
	Chain* uncle = nullptr;
	if (parent != nullptr) {
		grandad = parent->parent;
		if (grandad != nullptr) {
			if (grandad->left == parent) {
				uncle = grandad->right;
			}
			else {
				uncle = grandad->left;
			}
		}
	}

	/*------------------------Case 1--------------------------------*/
	/*---------------It's root, painting it to black----------------*/
	if (subTree == root_) {
		subTree->color = BLACK;
		return;
	}
	/*------------------------Case2---------------------------------*/
	/*-------------Parent is black. All right-----------------------*/
	if (subTree->parent->color == BLACK) {
		return;
	}
	/*-------------------------Case3---------------------------------*/
	/*----Parent and uncle are red. Repainting and rerecursion-------*/
	/*----------for grandad(it always exists in this case)-----------*/
	if (parent->color == RED && uncle->color == RED) {
		parent->color = BLACK;
		uncle->color = BLACK;
		grandad->color = RED;
		ins_balance(grandad);
		return;
	}
	/*--------------------------Case4--------------------------------*/
	/*-------Parent is red, uncle is black, child "orientation"------*/
	/*---differs from parent. Rotation parent to reduce to case 5,---*/
	/*----------------------rerecursion for parent-------------------*/
	if (parent->color == RED && uncle->color == BLACK) {
		if (parent == grandad->left && subTree == parent->right) {
			rotate_left(parent);
			ins_balance(parent);
			return;
		}
		if (parent == grandad->right && subTree == parent->left) {
			rotate_right(parent);
			ins_balance(parent);
			return;
		}
	}

	/*--------------------------Case5--------------------------------*/
	/*-------Parent is red, uncle is black, child has same ----------*/
	/*------orientation as parent. Rotation grandad, repainting------*/
	if (parent->color == RED && uncle->color == BLACK) {
		if (parent == grandad->left && subTree == parent->left) {
			rotate_right(grandad);
			grandad->color = RED;
			parent->color = BLACK;
			return;
		}
		if (parent == grandad->right && subTree == parent->right) {
			rotate_left(grandad);
			grandad->color = RED;
			parent->color = BLACK;
			return;
		}
	}
}

template <class T, class Compare>
void RBTree<T, Compare>::test() {
	rotate_left(root_);
	rotate_right(root_);
}

/*-------------------Static members------------------------------------*/
template <class T, class Compare>
Compare RBTree<T, Compare>::comp_ = Compare();

/*--------------------------I/O-------------------------------*/
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
			std::cout << "----------"<<'\n';
			if (ptr->isLeaf) {
				std::cout << "Leaf" << '\n';
			}
			else {
				std::cout << "Chain" << '\n';
			}
			if (ptr->color == BLACK) {
				std::cout << "Color: BLACK" << '\n';
			}
			else {
				if (ptr->color == RED) {
					std::cout << "Color: RED" << '\n';
				}
				else {
					std::cout << "Color: ERROR" << '\n';
				}
			}
			std::cout << "Value: "<< ptr->value <<'\n';
			std::cout << "----------" << '\n'<<'\n';
			q.push(ptr->left);
			q.push(ptr->right);
		}
	}
	return out;
}