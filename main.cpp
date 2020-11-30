#include <iostream>
#include "RBTree.h"
int main()
{
	RBTree<int32_t> tree1;

	tree1.insert(5);
	tree1.erase(5);
	std::cout << tree1.find(5)<<tree1.find(4);
}
