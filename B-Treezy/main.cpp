/*****************************************************************//**
 * \file   main.cpp
 * \brief  Main
 * 
 * \author Kkobari
 * \date   November 2022
 *********************************************************************/

#include "BTree.h"
#include <ctime>

int main()
{
	BTree* tree = new BTree(5);

	tree->PrintInfo();

	vector<int> insert;
	vector<int> remove;

// set to one to test with a set array of values to insert and remove
// set to zero to test with randomized arrays
#if 0
	insert = { 5, 7, 8, 3, 2, 4, 6, 9, 10, 8, 7, -2, 0, 11, 13, -1 };
	remove = { 3, 7, -20, -1 };
#else
	srand(time(NULL));

	int count = 90;			// how many numbers to generate
	int maxValue = 100;

	for (int i = 0; i < count; i++)
		insert.push_back(rand() % maxValue);

	for (int i = 0; i < count/4; i++)
		remove.push_back(rand() % maxValue);
#endif

	tree->Print();
	tree->PrintStats();

	for (auto ins : insert)
	{
		tree->InsertPrint(ins);
		tree->PrintStats();
	}

	tree->FindPrint(10);
	tree->FindPrint(-20);

	for (auto rem : remove)
	{
		tree->RemovePrint(rem);
		tree->PrintStats();
	}

	delete tree;
}
