#include <iostream>
#include "RBTree.h"
int main()
{
	RBTree<int32_t> tree1;
	tree1.insert(5);
	tree1.insert(10);
	tree1.insert(15);
	RBTree<int32_t> tree2 = std::move(tree1);
}
