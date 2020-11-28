#include <iostream>
#include "RBTree.h"
int main()
{
	RBTree<int32_t> tree1;
	tree1.insert(5);
	tree1.insert(2);
	tree1.insert(3);
	tree1.insert(10);
	tree1.insert(15);
	tree1.test();
	std::cout << tree1;
}
