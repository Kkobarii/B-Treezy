/*****************************************************************//**
 * \file   BTree.cpp
 * \brief  BTree cpp file
 * 
 * \author Kkobari
 * \date   November 2022
 *********************************************************************/

#include "BTree.h"
#include "color.h"

/**
 * Constructs the tree and sets its order.
 * 
 * \param order The order of the tree
 */
template <class T>
BTree<T>::BTree(int order)
{
	if (order < 3)
	{
		cout << "tree of order " << order << " is not possible to make" << endl;
		delete this;
		exit(1);
	}
	else
	{
		this->order = order;
	}
}

/**
 * Destructs the tree.
 */
template <class T>
BTree<T>::~BTree()
{
	delete root;
}


/**
 * Inserts a value into a node.
 * 
 * \param node The node to insert into
 * \param value	The value to insert
 */
template <class T>
void BTree<T>::InternalInsert(Node* node, T value)
{
	// if tree is empty
	if (this->root == nullptr)
	{
		// creates a new root
		Node* newNode = new Node(value, nullptr);
		this->root = newNode;

		return;
	}

	// checks if this value is already present - no duplicates allowed
	if (node->CheckValuePresent(value))
	{
		cout << "This value is already present" << endl;
		return;
	}

	// if node is leaf
	if (node->IsLeaf())
	{
		// inserts value into this leaf
		node->PushValue(value);

		// then we have to check if this leaf has overflown
		InternalSplit(node);

		return;
	}

	// if node has children
	if (!node->IsLeaf())
	{
		// choose correct branch to go down
		for (int i = 0; i < node->values.size(); i++)
		{
			if (value < node->values[i])
			{
				InternalInsert(node->children[i], value);
				return;
			}
		}
		InternalInsert(node->children[node->values.size()], value);
		return;
	}
}

/**
 * Splits the node after it was inserted into.
 * 
 * \param node The node to split
 */
template <class T>
void BTree<T>::InternalSplit(Node* node)
{
	// if node has allowed number of values, we don't have to split
	if (node->values.size() < order)
	{
		return;
	}

	// the split is happening. this means node has order values and order + 1 children (or order values and 0 children if it is a leaf)

	// if node is tree->root, we have to create a new root - the tree is now 1 level higher
	if (node == this->root)
	{
		Node* newRoot = new Node();
		this->root = newRoot;
		newRoot->children.push_back(node);
		this->root->Adopt();
	}

	int middleIndex = floor((order - 1) / 2);

	// push the middle value into parent
	node->parent->PushValue(node->values[middleIndex]);

	// create a new right node and push right values into it
	Node* newRight = new Node();
	newRight->parent = node->parent;
	for (int i = middleIndex + 1; i < node->values.size(); i++)
	{
		newRight->PushValue(node->values[i]);
	}

	// delete the inserted values from old (now left) node
	node->values.erase(node->values.begin() + middleIndex, node->values.end());

	// if node has children we have to sort them out too
	if (!node->IsLeaf())
	{
		// push right children into right node
		for (int i = middleIndex + 1; i < node->children.size(); i++)
		{
			newRight->children.push_back(node->children[i]);
		}
		newRight->Adopt();

		// delete the inserted children from left node
		node->children.erase(node->children.begin() + middleIndex + 1, node->children.end());
	}

	// push new right node into parent.children after original node (to keep their order)
	node->parent->children.insert(next(find(node->parent->children.begin(), node->parent->children.end(), node)), newRight);

	// now call InternalSplit on parent to make sure it didn't overflow too
	InternalSplit(node->parent);
}

/**
 * Checks whether a node or its children contain a value.
 * 
 * \param node The node to check
 * \param value The value to check for
 * \return True if the value is present, false otherwise
 */
template <class T>
bool BTree<T>::InternalFind(Node* node, T value)
{
	// check if current node has the value
	if (node->CheckValuePresent(value))
		return true;

	// if node is leaf there are no more children to search
	if (node->IsLeaf())
		return false;

	// if node has children we find the correct one to search
	for (int i = 0; i < node->values.size(); i++)
	{
		if (value < node->values[i])
		{
			return InternalFind(node->children[i], value);
		}
	}
	return InternalFind(node->children[node->values.size()], value);
}

/**
 * Removes a value from a node.
 * 
 * \param node The node to remove from
 * \param value The value to remove
 */
template <class T>
void BTree<T>::InternalRemove(Node* node, T value)
{
	// if value is present in this node
	if (node->CheckValuePresent(value))
	{
		if (node->IsLeaf())
		{
			// if node is leaf, just remove the value and then rebalance from leaf
			node->RemoveValue(value);
			InternalRebalance(node);
			return;
		}
		else
		{
			// if node is internal, replace deleted value with the closest value
			int minAllowed = floor((order - 1) / 2);

			Node* closestLeftLeaf = node->children[node->GetValueIndex(value)]->GetMostRightChild();
			Node* closestRightLeaf = node->children[node->GetValueIndex(value) + 1]->GetMostLeftChild();

			if (closestRightLeaf->values.size() > minAllowed)
			{
				// steal the closest right value and put it instead of the separator (value)
				int stolenValue = *(closestRightLeaf->values.begin());
				replace(node->values.begin(), node->values.end(), value, stolenValue);
				closestRightLeaf->values.erase(closestRightLeaf->values.begin());

				// we stole one value so we have to rebalance the leaf
				InternalRebalance(closestRightLeaf);
			}
			else
			{
				// steal the closest left value and put it instead of the separator (value)
				int stolenValue = *(closestLeftLeaf->values.end() - 1);
				replace(node->values.begin(), node->values.end(), value, stolenValue);
				closestLeftLeaf->values.erase(closestLeftLeaf->values.end() - 1);

				// we stole one value so we have to rebalance the leaf
				InternalRebalance(closestLeftLeaf);
			}
			return;
		}
	}
	// if value is not present
	else
	{
		// if we reached the bottom of the tree
		if (node->IsLeaf())
		{
			cout << "This value is not present" << endl;
			return;
		}
		// if node is internal
		else
		{
			// call Remove on the correct child
			for (int i = 0; i < node->values.size(); i++)
			{
				if (value < node->values[i])
				{
					InternalRemove(node->children[i], value);
					return;
				}
			}
			InternalRemove(node->children[node->values.size()], value);
			return;
		}
	}
}

/**
 * Rebalances a node after a value was removed.
 *
 * \param node The node to rebalance
 */
template <class T>
void BTree<T>::InternalRebalance(Node* node)
{
	int minAllowed = floor((order - 1) / 2);

	// if node has enough values, we don't have to rebalance
	if (node->values.size() >= minAllowed)
		return;

	// if node is root we have to check if it wasn't robbed by its children merging in a previous rebalance
	if (node == this->root)
	{		
		// root can have any minimal number of values except for 0
		if (node->values.size() == 0)
		{
			// if the tree has been deleted completely
			if (node->children.size() == 0)
			{
				delete node;
				this->root = nullptr;
			}
			// if the root is empty, its only child will become the new root - the tree is now 1 level shorter
			else
			{
				this->root = node->children[0];
				delete node;
			}
		}
		return;
	}

	Node* leftSibling = node->GetLeftSibling();
	Node* rightSibling = node->GetRightSibling();

	// if node has a left sibling it can borrow a value from
	if (leftSibling != nullptr && leftSibling->values.size() > minAllowed)
	{
		int separator = node->parent->values[node->GetIndex() - 1];

		// put separator from parent into node instead of deleted value
		node->values.insert(node->values.begin(), separator);

		// put most right value from left sibling into parent instead of separator
		replace(node->parent->values.begin(), node->parent->values.end(), separator, *(leftSibling->values.end() - 1));
		leftSibling->values.erase(leftSibling->values.end() - 1);

		// if node has children, push the right child from sibling to the front of node children
		if (!node->IsLeaf())
		{
			node->children.insert(node->children.begin(), *(leftSibling->children.end() - 1));
			leftSibling->children.erase(leftSibling->children.end() - 1);
			node->Adopt();
		}

		// rebalance parent in case of underflow - should't happen but just in case
		InternalRebalance(node->parent);
		return;
	}
	// if node has a right sibling it can borrow a value from
	else if (rightSibling != nullptr && rightSibling->values.size() > minAllowed)
	{
		int separator = node->parent->values[node->GetIndex()];

		// put separator from parent into node instead of deleted value
		node->values.insert(node->values.end(), separator);

		// put most left value from right sibling into parent instead of separator
		replace(node->parent->values.begin(), node->parent->values.end(), separator, *(rightSibling->values.begin()));
		rightSibling->values.erase(rightSibling->values.begin());

		// if node has children, push the left child from sibling to the back of node children
		if (!node->IsLeaf())
		{
			node->children.insert(node->children.end() - 1, *(rightSibling->children.begin()));
			rightSibling->children.erase(rightSibling->children.begin());
			node->Adopt();
		}

		// rebalance parent in case of underflow - should't happen but just in case
		InternalRebalance(node->parent);
		return;
	}
	// if node doesn't have any sibling who can spare values - we have to merge
	else
	{
		Node* left = nullptr;
		Node* right = nullptr;

		// choose which two nodes we will be merging
		if (leftSibling != nullptr)
		{
			left = leftSibling;
			right = node;
		}
		else
		{
			left = node;
			right = rightSibling;
		}

		// separator is the value in parent which separates the two nodes
		int separator = left->parent->values[left->GetIndex()];

		// put separator into left
		left->PushValue(separator);
		left->parent->values.erase(find(left->parent->values.begin(), left->parent->values.end(), separator));

		// put all values from right into left
		for (auto val : right->values)
			left->PushValue(val);

		// if nodes have children, put all children from right into left
		if (!left->IsLeaf())
		{
			for (auto chi : right->children)
				left->children.push_back(chi);
			left->Adopt();
		}

		// delete right from parent and memory
		left->parent->children.erase(find(left->parent->children.begin(), left->parent->children.end(), right));
		right->children.clear();
		delete right;

		// call rebalance on parent - we just stole one value from it
		InternalRebalance(left->parent);
	}
}

/**
 * Prints a node and all its children.
 * 
 * \param node The node to print
 * \param indent The number of spaces and characters to indent the node
 * \param last Whether the node is the last child of its parent
 * \param siblings The number of siblings the node has
 * \param position The position of the node in its parent
 */
template <class T>
void BTree<T>::InternalPrint(Node* node, string indent, bool last, int siblings, int position)
{
	// \xB3 = |
	// \xC3 = T
	// \xC0 = L
	// \xC4 = -

	// if tree is empty
	if (this->root == nullptr)
	{
		cout << "Tree is empty" << endl;
		return;
	}

	// last child needs a different symbol
	cout << indent;
	if (last)
	{
		if (siblings != 0)
			cout << "\xC0\xC4";
		indent.append("   ");
	}
	else
	{
		cout << "\xC3\xC4";
		indent.append("\xB3  ");
	}

	// for the node
	if (siblings == 0)
	{
		cout << CYAN << "Root: " << RESET;
		node->PrintValues();
	}
	else
	{
		cout << CYAN << " " << position + 1 << ": " << RESET;
		node->PrintValues();
	}

	// print all node children
	for (int i = 0; i < node->children.size(); i++)
	{
		InternalPrint(node->children[i], indent, i == node->children.size() - 1, node->children.size(), i);
	}
}


/**
 * Gets the number of nodes in the tree.
 * 
 * \return The number of nodes
 */
template <class T>
int BTree<T>::GetNodeCount()
{
	if (root == nullptr)
		return 0;

	int count = 0;

	queue<Node*> q;
	q.push(root);

	while (!q.empty())
	{
		Node* current = q.front();
		q.pop();

		count++;

		for (auto child : current->children)
			q.push(child);
	}

	return count;
}

/**
 * Gets the number of values in the tree.
 * 
 * \return The number of values
 */
template <class T>
int BTree<T>::GetValueCount()
{
	if (root == nullptr)
		return 0;

	int count = 0;

	queue<Node*> q;
	q.push(root);

	while (!q.empty())
	{
		Node* current = q.front();
		q.pop();
		count += current->values.size();

		for (auto child : current->children)
			q.push(child);
	}

	return count;
}

/**
 * Gets the procentual fullness of the tree.
 * 
 * \return The procentual fullness
 */
template <class T>
double BTree<T>::GetFullness()
{
	if (root == nullptr)
		return 0;

	int potentialCount = GetNodeCount() * (order - 1);
	int actualCount = GetValueCount();

	return ((double)actualCount / (double)potentialCount) * 100;
}

/**
 * Gets the height of the tree.
 * 
 * \return The height of the tree
 */
template <class T>
int BTree<T>::GetHeight()
{
	if (root == nullptr)
		return 0;

	int height = 1;
	Node* current = root;

	while (!current->IsLeaf())
	{
		height++;
		current = current->children[0];
	}

	return height;
}

/**
 * Prints some basic information about the tree.
 * 
 */
template <class T>
void BTree<T>::PrintInfo()
{
	cout << endl;
	cout << "This tree is of order " << order << endl;
	cout << "  max number of values in a node: " << order - 1 << endl;
	cout << "  max number of children: " << order << endl;
	cout << "  min number of values (except root): " << floor((order- 1)/2) << endl;
	cout << "  min number of children (except root and leaves): " << floor((order - 1) / 2) + 1 << endl;

	cout << endl;
}

/**
 * Prints some statistics of the tree regarding its contents.
 * 
 */
template <class T>
void BTree<T>::PrintStats()
{
	cout << "Tree stats:" << endl;

	cout << "  Order: " << order << endl;
	cout << "  Height: " << GetHeight() << endl;
	cout << "  Number of nodes: " << GetNodeCount() << endl;
	cout << "  Number of values: " << GetValueCount() << endl;
	cout << "  Fullness: " << GetFullness() << "%" << endl;
}

/**
 * Prints the tree.
 * 
 */
template <class T>
void BTree<T>::Print()
{
	this->InternalPrint(this->root, " ", true, 0, 0);
}

/**
 * Inserts a value into the tree.
 * 
 * \param value The value to insert
 */
template <class T>
void BTree<T>::Insert(T value)
{
	this->InternalInsert(this->root, value);
}

/**
 * Checks whether a value is present within the tree.
 * 
 * \param value The value to check for
 * \return True if the value is present, false otherwise
 */
template <class T>
bool BTree<T>::Find(T value)
{
	return this->InternalFind(this->root, value);
}

/**
 * Removes a value from the tree.
 *
 * \param value The value to remove
 */
template <class T>
void BTree<T>::Remove(T value)
{
	this->InternalRemove(this->root, value);
}

/**
 * Inserts a value into the tree and prints the result.
 * 
 * \param value The value to insert
 */
template <class T>
void BTree<T>::InsertPrint(T value)
{
	cout << endl << BLACK << WHITE_B << "  Inserting " << value << "  " << RESET << endl;

	Insert(value);
	Print();
}

/**
 * Checks whether a value is present within the tree and prints the result.
 *
 * \param value The value to check for
 */
template <class T>
void BTree<T>::FindPrint(T value)
{
	cout << endl << BLACK << WHITE_B << "  Finding " << value << "  " << RESET << endl;

	bool found = Find(value);

	if (found)
		cout << "value found" << endl;
	else
		cout << "value not found" << endl;

	Print();
}

/**
 * Removes a value from the tree and prints the result.
 *
 * \param value The value to remove
 */
template <class T>
void BTree<T>::RemovePrint(T value)
{
	cout << endl << BLACK << WHITE_B << "  Removing " << value << "  " << RESET << endl;

	Remove(value);
	Print();
}
