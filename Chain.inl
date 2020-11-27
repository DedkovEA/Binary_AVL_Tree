#pragma once
#include "RBTree.h"
#include <utility>
/*--------------------------Constructors and distructor ----------------*/
template <class T, class Compare>
RBTree<T, Compare>::Chain::Chain(): left(new Chain(Flags::LEAF)), right(new Chain(Flags::LEAF)), color(RED), value(std::move(T())), isLeaf(false)
{
}

template <class T, class Compare>
RBTree<T, Compare>::Chain::Chain(const Flags& flag)
{
	if (flag == Flags::LEAF) {
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
RBTree<T, Compare>::Chain::Chain(const T& initValue) : Chain(), value(initValue)
{}

template <class T, class Compare>
RBTree<T, Compare>::Chain::Chain(const typename RBTree<T, Compare>::Chain& other)
{
	if (left != nullptr) {
		delete left;
	}
	if (right != nullptr) {
		delete right;
	}
	if (other.left != nullptr) {
		left = new Chain(*other.left);
	}
	else {
		left = nullptr;
	}
	if (other.right != nullptr) {
		right = new Chain(*other.right);
	}
	else {
		right = nullptr;
	}
	color = other.color;
	value = other.value;
	isLeaf = other.isLeaf;
}

template <class T, class Compare>
typename RBTree<T, Compare>::Chain& RBTree<T, Compare>::Chain::operator=(const typename RBTree<T, Compare>::Chain& other)
{
	if (left != nullptr) {
		delete left;
	}
	if (right != nullptr) {
		delete right;
	}
	if (other.left != nullptr) {
		left = new Chain(*other.left);
	}
	else {
		left = nullptr;
	}
	if (other.right != nullptr) {
		right = new Chain(*other.right);
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
	left =other.left;
	right = other.right;
	color = std::move(other.color);
	value = std::move(other.value);
	isLeaf = std::move(other.isLeaf);
	other.left = nullptr;
	other.right = nullptr;
	other.color = 0;
	other.value = std::move(T());
	other.isLeaf = false;
}

template <class T, class Compare>
typename RBTree<T, Compare>::Chain& RBTree<T, Compare>::Chain::operator=(typename RBTree<T, Compare>::Chain&& other) noexcept
{
	if (left != nullptr) {
		delete left;
	}
	if (right != nullptr) {
		delete right;
	}
	left = other.left;
	right = other.right;
	color = std::move(other.color);
	value = std::move(other.value);
	isLeaf = std::move(other.isLeaf);

	other.left = nullptr;
	other.right = nullptr;
	other.color = 0;
	other.value = std::move(T());
	other.isLeaf = false;
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
