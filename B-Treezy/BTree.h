/*****************************************************************//**
 * \file   BTree.h
 * \brief  BTree header file
 * 
 * \author Kkobari
 * \date   November 2022
 *********************************************************************/

#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <cmath>

using namespace std;

/**
 * \brief A B-Tree containing the order of the tree and a pointer to the root.
 */
template <class T>
class BTree
{
private:
	/**
	 * \brief A node in the B-Tree containing a list of values, a list of children nodes and a pointer to its parent.
	 */
	class Node
	{
	public:
		/** The list of values of this node */
		vector<T> values;
		/** The list of children of this node */
		vector<Node*> children;
		/** The parent of this node */
		Node* parent;

		/**
		 * Constructs the node.
		 */
		Node()
		{
			this->parent = nullptr;
		}
		
		/**
		 * Constructs the node.
		 * 
		 * \param value The first value to add to the node
		 * \param parent The parent of the node
		 */
		Node(T value, Node* parent)
		{
			values.push_back(value);

			this->parent = parent;
		}
		
		/**
		 * Destructs the node along with its children.
		 * 
		 */
		~Node()
		{
			for (auto child : children)
				delete child;
		}
		
		/**
		 * Adds a value to the node and sorts the values afterwards.
		 * 
		 * \param value The value to add to the node
		 */
		void PushValue(T value)
		{
			values.push_back(value);
			sort(values.begin(), values.end());
		}
		
		/**
		 * Removes a value from the node.
		 * 
		 * \param value The value to remove from the node
		 */
		void RemoveValue(T value)
		{
			values.erase(find(values.begin(), values.end(), value));
		}
		
		/**
		 * Makes sure this node is set as the parent for all its children.
		 * 
		 */
		void Adopt()
		{
			for (auto child : children)
				child->parent = this;
		}
		
		/**
		 * Checks whether the node contains a value.
		 * 
		 * \param value The value to check
		 * \return True if the node contains the value, false otherwise
		 */
		bool CheckValuePresent(T value)
		{
			for (auto val : values)
			{
				if (val == value)
					return true;
			}
			return false;
		}

		/**
		 * Checks whether the node is a leaf.
		 * 
		 * \return True if the node is a leaf, false otherwise
		 */
		bool IsLeaf()
		{
			return this->children.size() == 0;
		}

		/**
		 * Gets the index of a value in the node.
		 * 
		 * \param value The value to get the index of
		 * \return The index of the value
		 */
		int GetValueIndex(T value)
		{
			for (int i = 0; i < values.size(); i++)
			{
				if (values[i] == value)
					return i;
			}
			return values.size();
		}

		/**
		 * Gets the index of the node in its parent.
		 * 
		 * \return The index of the node
		 */
		int GetIndex()
		{
			if (parent == nullptr)
				return -1;

			for (int i = 0; i < parent->children.size(); i++)
			{
				if (parent->children[i] == this)
					return i;
			}
			return parent->children.size();
		}

		/**
		 * Gets the left sibling of the node.
		 *
		 * \return The left sibling
		 */
		Node* GetLeftSibling()
		{
			if (parent == nullptr)
				return nullptr;

			auto ptr = find(parent->children.begin(), parent->children.end() - 1, this);
			if (ptr == parent->children.begin())
				return nullptr;

			return *(prev(ptr));
		}

		/**
		 * Gets the right sibling of the node.
		 *
		 * \return The right sibling
		 */
		Node* GetRightSibling()
		{
			if (parent == nullptr)
				return nullptr;

			auto ptr = find(parent->children.begin(), parent->children.end() - 1, this);
			if (ptr == parent->children.end() - 1)
				return nullptr;

			return *(next(ptr));
		}

		/**
		 * Gets the child furthest to the left of the node.
		 *
		 * \return The most left child
		 */
		Node* GetMostLeftChild()
		{
			if (IsLeaf())
				return this;

			return children[0]->GetMostLeftChild();
		}

		/**
		 * Gets the child furthest to the right of the node.
		 *
		 * \return The most right child
		 */
		Node* GetMostRightChild()
		{
			if (IsLeaf())
				return this;

			return children[children.size() - 1]->GetMostRightChild();
		}

		/**
		 * Prints the values of the node.
		 */
		void PrintValues()
		{
			if (this == nullptr)
			{
				cout << "node is nullptr";
				return;
			}

			cout << "( ";
			for (auto val : this->values)
			{
				cout << val << " ";
			}
			cout << ")" << endl;
		}
	};

	/** The root of the tree */
	Node* root = nullptr;
	/** The order of the tree */
	int order;

	void InternalInsert(Node* node, T value);
	void InternalSplit(Node* node);
	bool InternalFind(Node* node, T value);
	void InternalRemove(Node* node, T value);
	void InternalRebalance(Node* node);

	void InternalPrint(Node* node, string indent, bool last, int siblings, int position);

	int GetNodeCount();
	int GetValueCount();
	int GetHeight();
	double GetFullness();

public:
	BTree(int order);
	~BTree();
	
	void Insert(T value);
	bool Find(T value);
	void Remove(T value);

	void PrintInfo();
	void PrintStats();
	void Print();

	void InsertPrint(T value);
	void FindPrint(T value);
	void RemovePrint(T value);
};
