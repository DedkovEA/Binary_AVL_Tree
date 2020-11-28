#include <iostream>
#include "RBTree.h"
int main()
{
	RBTree<int32_t> tree1;


	for (int32_t i = 0; i < 1000; ++i) {
		tree1.insert(i);
	}
	std::cout << tree1.height();
}
