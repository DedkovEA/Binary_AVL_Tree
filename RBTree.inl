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
	(*ptr) = std::move(Chain(key));
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
void RBTree<T, Compare>::rotate_LR(Chain* subTree)
{
	rotate_left(subTree);
	rotate_right(subTree->parent->parent);
}

template <class T, class Compare>
void RBTree<T, Compare>::rotate_RL(Chain* subTree)
{
	rotate_right(subTree);
	rotate_left(subTree->parent->parent);
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
void RBTree<T, Compare>::erase(const T& key)
{
	Chain* chainToDel = find_chain(key);
	if (chainToDel != nullptr) {
		erase_chain(chainToDel);
	}
}

template <class T, class Compare>
void RBTree<T, Compare>::erase_chain(Chain* chain)
{
	/*-------------------Case 1-----------------------
	-----Chain has 2 childs. Finding min value--------
	--------in right subtree, swap, and delete--------*/
	if (!chain->left->isLeaf && !chain->right->isLeaf) {
		Chain* min = min_value(chain->right);
		std::swap(chain->value, min->value);
		erase_chain(min);
		return;
	}
	/*-------------------Case 2----------------------
	----- Black chain with 1 child. Child must be----
	---red chain without childs. Moving red child ---
	--------------data to this chain----------------*/
	if (chain->color==BLACK && (chain->left->isLeaf != chain->right->isLeaf)) {
		if (!chain->left->isLeaf) {
			chain->value = chain->left->value;
			*(chain->left) = std::move(Chain(Flags::LEAF));
		}
		else {
			chain->value = chain->right->value;
			*(chain->right) = std::move(Chain(Flags::LEAF));
		}
		return;
	}
	/*-------------------Case 3----------------------
	--------- Red chain without childs. -------------
	------------Just removing this chain-------------*/
	if (chain->color == RED && (chain->left->isLeaf && chain->right->isLeaf)) {
		*(chain) = std::move(Chain(Flags::LEAF));
		return;
	}
	/*--------------------Case 4-----------------------
	--Black chain without child. Remove and BALANCE!!!--*/
	if (chain->color == BLACK && (chain->left->isLeaf && chain->right->isLeaf)) {
		*(chain) = std::move(Chain(Flags::LEAF));
		erase_balance(chain->parent, chain);
		return;
	}
}

template <class T, class Compare>
typename RBTree<T, Compare>::Chain* RBTree<T, Compare>::find_chain(const T& key)
{
	Chain* ptr = root_;
	while (!ptr->isLeaf) {
		if (!comp_(key, ptr->value) && !comp_(ptr->value, key)) {
			return ptr;
		}
		if (comp_(key, ptr->value)) {
			ptr = ptr->left;
		}
		else {
			ptr = ptr->right;
		}
	}
	return nullptr;
}

template <class T, class Compare>
bool RBTree<T, Compare>::find(const T& key)
{
	Chain* ptr = find_chain(key);
	if (ptr == nullptr) {
		return false;
	}
	else {
		return true;
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
void RBTree<T, Compare>::erase_balance(Chain* subTree, Chain* caller)
{
	if (subTree == nullptr) {
		return;
	}
	Chain* son = nullptr;
	bool isLeft = false;
	if (subTree->right == caller) {
		son = subTree->left;
		isLeft = true;
	}
	else {
		son = subTree->right;
		isLeft = false;
	}
	bool isRight = !isLeft;
	/*---------------------------Case 1-----------------------------
	Chain is red with black son and black grandsons.(They must exist)
	-----------Painting son to red, chain to black----------------*/
	if (subTree->color == RED && son->color == BLACK &&
		son->left->color == BLACK && son->right->color == BLACK) {
		son->color = RED;
		subTree->color = BLACK;
		return;
	}
	/*---------------------------Case 2-----------------------------
	-----Chain is red with black son and red grandson with same-----
	-----orientation as son. Rotation and painting son to red,------
	--------------chain and grandson to black----------------------*/
	if (subTree->color == RED && son->color == BLACK) {
		Chain* grandSon = nullptr;
		if (isLeft) {
			grandSon = son->left;
		}
		else {
			grandSon = son->right;
		}
		if (grandSon->color == RED) {
			if (isLeft) {
				rotate_right(subTree);
			}
			else {
				rotate_left(subTree);
			}
			son->color = RED;
			grandSon->color = BLACK;
			subTree->color = BLACK;
			return;
		}

	}

	/*---------------------------Case 2'------------------------------
	-----Chain is red with black son, black grandson with same--------
	---orientation as son, and red grandson with other orientation.---
	----------Big rotation and painting chain to black.-------------*/
	if (subTree->color == RED && son->color == BLACK) {
		Chain* mainGrandSon = nullptr;
		Chain* otherGrandSon = nullptr;
		if (isLeft) {
			mainGrandSon = son->left;
			otherGrandSon = son->right;
		}
		else {
			mainGrandSon = son->right;
			otherGrandSon = son->left;
		}
		if (mainGrandSon->color == BLACK && otherGrandSon->color == RED) {
			if (isLeft) {
				rotate_LR(son);
			}
			else {
				rotate_RL(son);
			}
			subTree->color = BLACK;
			return;
		}

	}

	/*---------------------------Case 3------------------------------
	---Chain is black with red son, black right grandson with black--
	------great childs. Rotation and painting son to black ----------
	---------------------and grandson to red------------------------*/
	if (subTree->color == BLACK && son->color == RED) {
		Chain* grandSon = nullptr;
		if (isLeft) {
			grandSon = son->right;
		}
		else {
			grandSon = son->left;
		}
		if (grandSon->color == BLACK && grandSon->left->color == BLACK && grandSon->right->color == BLACK) {
			if (isLeft) {
				rotate_right(subTree);
			}
			else {
				rotate_left(subTree);
			}
			son->color = BLACK;
			grandSon->color = RED;
			return;
		}

	}

	/*---------------------------Case 4------------------------------
	---Chain is black with red son, black right grandson with left---
	-red great child. Big rotation and painting great child to black*/

	if (subTree->color == BLACK && son->color == RED) {
		Chain* grandSon = nullptr;
		if (isLeft) {
			grandSon = son->right;
		}
		else {
			grandSon = son->left;
		}
		if (grandSon->color == BLACK) {
			if ((grandSon->left->color == RED && isLeft) || (grandSon->right->color == RED && isRight)) {
				if (isLeft) {
					grandSon->left->color = BLACK;
					rotate_LR(son);
				}
				else {
					grandSon->right->color = BLACK;
					rotate_RL(son);
				}
				return;
			}
		}
	}
	/*---------------------------Case 4'------------------------------
	---Chain is black with red son, black right grandson with left---
	-----------black and right red great child.----------------------
	-------Rotation of grandson, big rotation of child. Painting ----
	------------------red greatchild to black-----------------------*/
	if (subTree->color == BLACK && son->color == RED) {
		Chain* grandSon = nullptr;
		if (isLeft) {
			grandSon = son->right;
		}
		else {
			grandSon = son->left;
		}
		if (grandSon->color == BLACK) {
			Chain* greatChild1 = nullptr;
			Chain* greatChild2 = nullptr;
			if (isLeft) {
				greatChild1 = grandSon->left;
				greatChild2 = grandSon->right;
			}
			else {
				greatChild1 = grandSon->right;
				greatChild2 = grandSon->left;
			}
			if (greatChild1->color == BLACK && greatChild2->color == RED) {
				greatChild2->color = BLACK;
				if (isLeft) {
					rotate_left(grandSon);
					rotate_LR(son);
				}
				else {
					rotate_right(grandSon);
					rotate_RL(son);
				}
				return;
			}
		}
	}

	/*---------------------------Case 5-------------------------------
	-Chain is black with black son, red other way oriented great son.-
	----------Repainting great son to black, big rotation.----------*/
	if (subTree->color == BLACK && son->color == BLACK) {
		Chain* grandSon = nullptr;
		if (isLeft) {
			grandSon = son->right;
		}
		else {
			grandSon = son->left;
		}
		if (grandSon->color == RED) {
			grandSon->color = BLACK;
			if (isLeft) {
				rotate_LR(son);
			}
			else {
				rotate_RL(son);
			}
			return;
		}
	}

	/*---------------------------Case 5'-------------------------------
	-Chain is black with black son, black other way oriented great son.-
	---------------------and red same way oriented great son.-----------
	----------Repainting great son to black, rotation.----------*/
	if (subTree->color == BLACK && son->color == BLACK) {
		Chain* grandSon1 = nullptr;
		Chain* grandSon2 = nullptr;
		if (isLeft) {
			grandSon1 = son->left;
			grandSon2 = son->right;
		}
		else {
			grandSon1 = son->right;
			grandSon2 = son->left;
		}
		if (grandSon1->color == RED && grandSon2->color == BLACK) {
			grandSon1->color = BLACK;
			if (isLeft) {
				rotate_right(subTree);
			}
			else {
				rotate_left(subTree);
			}
			return;
		}
	}

	/*---------------------------Case 6-------------------------------
	--------Chain is black with black son and black grandsons.---------
	--------------Repainting son and recurisve call to parent---------*/
	if (subTree->color == BLACK && son->color == BLACK &&
		son->left->color == BLACK && son->right->color == BLACK) {
		son->color = RED;
		erase_balance(subTree->parent, subTree);
		return;
	}
}

template <class T, class Compare>
typename RBTree<T, Compare>::Chain* RBTree<T, Compare>::min_value(Chain* subTree)
{
	if (subTree->left->isLeaf) {
		return subTree;
	}
	else {
		return min_value(subTree->left);
	}
}

template <class T, class Compare>
void RBTree<T, Compare>::test() {
	*(root_) = std::move(Chain(7));
	root_->color = BLACK;
	*(root_->right) = std::move(Chain(8));
	root_->right->color = BLACK;
	*(root_->left) = std::move(Chain(3));
	root_->left->color = BLACK;

	/**(root_->left->left) = std::move(Chain(2));
	//root_->left->left->color = BLACK;
	*(root_->left->right) = std::move(Flags::LEAF);
	//root_->left->right->color = RED;

	/**(root_->left->right->left) = std::move(Chain(Flags::LEAF));
	*(root_->left->right->right) = std::move(Chain(6));*/
	erase(8);
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