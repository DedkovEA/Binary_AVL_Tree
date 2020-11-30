#pragma once
#include "RBTree.h"
#include <utility>
/*--------------------------Constructors and distructor ----------------*/
template <class T, class Compare>
RBTree<T, Compare>::Chain::Chain(): left(new Chain(Flags::LEAF)), right(new Chain(Flags::LEAF)), color(RED), value(std::move(T())), isLeaf(false), parent(nullptr)
{
	left->parent = right -> parent = this;
}

template <class T, class Compare>
RBTree<T, Compare>::Chain::Chain(const Flags& flag)
{
	if (flag == Flags::LEAF) {
		parent = nullptr;
		color = BLACK;
		left = nullptr;
		right = nullptr;
		value = std::move(T());
		isLeaf = true;
	}
	else {
		throw "Unxpected flag!";
	}
}

template <class T, class Compare>
RBTree<T, Compare>::Chain::Chain(const T& initValue) : left(new Chain(Flags::LEAF)), right(new Chain(Flags::LEAF)), color(RED), isLeaf(false), parent(nullptr)
{
	left->parent = right->parent = this;
	value = initValue;
}

template <class T, class Compare>
RBTree<T, Compare>::Chain::Chain(const Chain& other)
{
	if (left != nullptr) {
		delete left;
	}
	if (right != nullptr) {
		delete right;
	}
	if (other.left != nullptr) {
		left = new Chain(*other.left);
		left->parent = this;
	}
	else {
		left = nullptr;
	}
	if (other.right != nullptr) {
		right = new Chain(*other.right);
		right->parent = this;
	}
	else {
		right = nullptr;
	}
	color = other.color;
	value = other.value;
	isLeaf = other.isLeaf;
	parent = nullptr;
}

template <class T, class Compare>
typename RBTree<T, Compare>::Chain& RBTree<T, Compare>::Chain::operator=(const Chain& other)
{
	if (&other == this) {
		return *this;
	}

	if (left != nullptr) {
		delete left;
	}
	if (right != nullptr) {
		delete right;
	}
	if (other.left != nullptr) {
		left = new Chain(*other.left);
		left->parent = this;
	}
	else {
		left = nullptr;
	}
	if (other.right != nullptr) {
		right = new Chain(*other.right);
		right->parent = this;
	}
	else {
		right = nullptr;
	}
	color = other.color;
	value = other.value;
	isLeaf = other.isLeaf;
	return (*this);
}

template <class T, class Compare>
RBTree<T, Compare>::Chain::Chain(typename RBTree<T, Compare>::Chain&& other) noexcept
{

	if (left != nullptr) {
		delete left;
	}
	if (right != nullptr) {
		delete right;
	}
	left = other.left;
	right = other.right;
	if (left != nullptr) {
		left->parent = this;
	}
	if (right != nullptr) {
		right->parent = this;
	}
	color = std::move(other.color);
	value = std::move(other.value);
	isLeaf = std::move(other.isLeaf);
	parent = nullptr;
	other.left = nullptr;
	other.right = nullptr;
	other.color = 0;
	other.value = std::move(T());
	other.isLeaf = false;
	other.parent = nullptr;
}

template <class T, class Compare>
typename RBTree<T, Compare>::Chain& RBTree<T, Compare>::Chain::operator=(Chain&& other) noexcept
{
	if (&other == this) {
		return (*this);
	}
	if (left != nullptr) {
		delete left;
	}
	if (right != nullptr) {
		delete right;
	}
	left = other.left;
	right = other.right;
	if (left != nullptr) {
		left->parent = this;
	}
	if (right != nullptr) {
		right->parent = this;
	}
	color = std::move(other.color);
	value = std::move(other.value);
	isLeaf = std::move(other.isLeaf);

	other.left = nullptr;
	other.right = nullptr;
	other.color = 0;
	other.value = std::move(T());
	other.isLeaf = false;
	other.parent = nullptr;
	return *(this);
}


template <class T, class Compare>
RBTree<T, Compare>::Chain::~Chain() {
	if (left != nullptr) {
		delete left;
	}
	if (right != nullptr) {
		delete right;
	}
}
/*------------------------------------------------------------------------*/

/*------------------------Other methods-----------------------------------*/
