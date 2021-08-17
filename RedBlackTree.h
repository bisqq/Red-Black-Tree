#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>

using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::ifstream;
using std::ios;
using std::lower_bound;
using std::upper_bound;

// --MACROS
// To print easier instead of writing cout over again
#define LOG(x) cout << x << endl;

// I realized that in a lot of functions I am doing the same thing so I decided to simplify it
// has a x param to test for nullptr and has an optional param for returning
// --PARAM: x is the value that will be tested if null, ... is an optional return
#define isNullptr(x, ...) if(x == nullptr) return __VA_ARGS__;


// prototype
void statistics(string filename);


template<class T>
class NodeT
{
public:

	// variables to keep track of the tree
	T data;
	NodeT<T>* left;
	NodeT<T>* right;
	NodeT<T>* parent;
	bool isBlack;

	// init the vars
	NodeT(T val)
		:data(val), left(nullptr), right(nullptr), parent(nullptr), isBlack(false)
	{};

};

template<class T>
class RedBlackTree
{
public:

	// constructor
	RedBlackTree();

	// copy constructor
	// creates a deep copy
	RedBlackTree(const RedBlackTree<T>& copyRBT);

	// operator=
	// deeps copys and deallocates dynamic memory
	RedBlackTree<T>& operator=(const RedBlackTree<T>& copyRBT);

	// destructor
	// deallocates dynamic memory allocated by the tree
	~RedBlackTree();

	// inserts its template type parameter into the tree
	bool insert(T value);

	// remove's its template type parameter from the tree
	bool remove(T value);

	// search if value is in the tree and return true if found otherwise false
	bool search(T value) const;

	// search the tree for values in a specific range and returm a vector of T types
	vector<T> search(T begin, T end) const;

	// returns the largest value stored in the tree that is less than the method's single template parameter
	T closestLess(T value) const;

	// returns the smallest value stored in the tree that is greater than the method's single template parameter
	T closestGreater(T value) const;

	// returns a vector with all the values in the tree
	vector<T> values() const;

	// return the tree size
	int size() const; 

private:

	// variables
	// tree root
	NodeT<T>* root;

	// tree size
	int currentSize;

	// --HELPERS =================================================================================================

	// recursive function to copy all the values in the tree
	NodeT<T>* copyHelper(NodeT<T>* copy);
	
	// clear the whole tree
	void clearTreeHelper(NodeT<T>* nd) const;

	// rotate RBT
	void rotateRight(NodeT<T>* nd);
	void rotateLeft(NodeT<T>* nd);

	// fix RBT after remove
	// --PARAM: it takes in the child node (nd), and nd's parent node
	void fixRemovalRBT(NodeT<T>* ndChild, NodeT<T>* ndParent);

	// BST insert
	// --PARAM: takes in the current node, which happens to be the root, and a newNode created in dynamic memory
	NodeT<T>* BinaryTreeInsert(NodeT<T>* ndCurrent, NodeT<T>* newNode);

	// find value and return the node
	NodeT<T>* find(T value) const;

	// predecessor recurive helper
	NodeT<T>* predecessor(NodeT<T>* nd) const;

	// traverse the entire tree recursively and update's the vector ref from the value vector method
	void valueTraversalHelper(NodeT<T>* nd, vector<T>& vec) const;

	// traverse the tree recursively in the given range and update's the vector ref from the search vector method
	void searchTraversalHelper(NodeT<T>* nd, vector<T>& vec, T begin, T end) const;
	
};


//======================================================================================================
// --PART 1
//======================================================================================================

template<class T>
RedBlackTree<T>::RedBlackTree()
{
	// init the root and set the size
	root = nullptr;
	currentSize = 0;
}

template<class T>
RedBlackTree<T>::RedBlackTree(const RedBlackTree<T>& copyRBT)
{
	// copy the size from the param
	currentSize = copyRBT.currentSize;

	// call the recurisve method and assign it to the root
	root = copyHelper(copyRBT.root);
}

template<class T>
RedBlackTree<T>& RedBlackTree<T>::operator=(const RedBlackTree<T>& copyRBT)
{
	// check if the param is self
	if (this != &copyRBT)
	{
		// clear the tree
		clearTreeHelper(root);
		
		// copy the size from the param
		currentSize = copyRBT.currentSize;

		// call the recurisve method and assign it to the root
		root = copyHelper(copyRBT.root);
	}

	return *this;
}

template<class T>
RedBlackTree<T>::~RedBlackTree()
{
	// call the method to clear the tree recursively
	clearTreeHelper(root);

	// set size to 0
	currentSize = 0;
}

template<class T>
bool RedBlackTree<T>::insert(T value)
{
	// returns true if the insertParam does not exist in the tree
	if (!search(value))
	{
		// create a new node with the value given
		NodeT<T>* newNode = new NodeT<T>(value);

		// increase the size
		currentSize++;

		// call the BST insert and assign it to the root
		root = BinaryTreeInsert(root, newNode);

		// set the newNode to RED
		newNode->isBlack = false;

		// if the newNode is not the root and it's parent colour is RED
		// this will iterate until the root or a black parent is reached
		while (newNode != root && newNode->parent->isBlack == false)
		{
			// Set the Grandparent of the NewNode
			NodeT<T>* grandParent = newNode->parent->parent;

			// checks if the newNode parent is a left child
			if (newNode->parent == grandParent->left)
			{
				// "uncle" of newNode
				NodeT<T>* uncle = grandParent->right;

				if (uncle != nullptr && uncle->isBlack == false)
				{
					/* 
					The uncle and newNodes�s parent are both red so they can be
					made black, and newNodes�s grandparent can be made
					red, then make the newNode the grandparentand repeat
					*/
					newNode->parent->isBlack = true;
					uncle->isBlack = true;
					grandParent->isBlack = false;
					newNode = grandParent;
				}
				else
				{
					/*
					The newNodes�s grandparent must be black,
					arrange newNode and parent in a line,
					rotate newNode�s grandparent to
					balance the tree, and fix the
					colours
					*/
					if (newNode == newNode->parent->right)
					{
						newNode = newNode->parent;
						rotateLeft(newNode);
					}
					newNode->parent->isBlack = true;
					grandParent->isBlack = false;
					rotateRight(grandParent);
				}
			}
			else // symmetric to the if
			{
				NodeT<T>* uncle = grandParent->left;

				if (uncle != nullptr && uncle->isBlack == false)
				{
					newNode->parent->isBlack = true;
					uncle->isBlack = true;
					grandParent->isBlack = false;
					newNode = grandParent;
				}
				else
				{
					if (newNode == newNode->parent->left)
					{
						newNode = newNode->parent;
						rotateRight(newNode);
					}
					newNode->parent->isBlack = true;
					grandParent->isBlack = false;
					rotateLeft(grandParent);
				}
			}
		}

		// if the root set it to black
		root->isBlack = true;

		// return true
		return true;
	}

	// otherwise if the value is in the tree return false. This is to prevent duplication
	return false;
}

template<class T>
bool RedBlackTree<T>::remove(T value)
{
	// find the value you want to remove
	NodeT<T>* removeNode = find(value);

	// assign other pointers to nullptr for predecessor and the predecessor's child
	NodeT<T>* temp = nullptr;
	NodeT<T>* tempChild = nullptr;

	// returns true if the insertParam does exist in the tree
	if (search(value))
	{
		// checks if the removeNode has no childern
		if (removeNode->left == nullptr || removeNode->right == nullptr)
		{
			temp = removeNode;
		}
		else 
		{ 
			// otherwise it has two children so we grab the predecessor
			temp = predecessor(removeNode);
		}

		// identify if temp�s only child is right or left
		if (temp->left != nullptr) 
		{ 
			tempChild = temp->left; 
		}
		else 
		{ 
			tempChild = temp->right; 
		}

		// check if temp child is not null
		if (tempChild != nullptr)
		{
			// detach the tempChild from temp
			tempChild->parent = temp->parent;
		}

		// check if temp is the root
		if (temp->parent == nullptr) 
		{ 
			// make the tempChild the root
			root = tempChild;
		}
		else
		{
			// attach tempChild to temp's parent
			if (temp == temp->parent->left)//is left
			{ 
				temp->parent->left = tempChild; 
			}
			else // is right
			{ 
				temp->parent->right = tempChild;
			}
		}

		// if temp and removeNode aren't the same
		if (temp != removeNode)
		{
			// replace removeNode data with the temp data
			removeNode->data = temp->data;
		}

		// checks if the temp is black, if so call the fix for removal method
		if (temp->isBlack == true) 
		{ 
			fixRemovalRBT(tempChild, temp->parent); 
		}

		// delete the temp
		delete temp;

		// decrease the tree size
		currentSize--;

		// return true
		return true;
	}

	// otherwise the value is not in the tree
	return false;
}

template<class T>
bool RedBlackTree<T>::search(T value) const
{
	// a traverse pointer
	NodeT<T>* ptr = root;

	// as long as if the traverse pointer is not nullptr
	while (ptr != nullptr) 
	{
		// if the value is found return true
		if (value == ptr->data) 
		{ 
			return true; 
		}
		else if (value < ptr->data) // checks if value is less than
		{
			ptr = ptr->left; 
		}
		else // value is greater than
		{
			ptr = ptr->right; 
		}
	}

	// otherwise not in the tree
	return false;
}

template<class T>
vector<T> RedBlackTree<T>::search(T begin, T end) const
{
	// create a vector with T types
	vector<T> results;

	// if there is no tree return an empty vector
	isNullptr(root, results);

	// check if begin is less then the end 
	if (begin < end)
	{
		// call the recursive method and pass the vector as a ref
		// also pass the being and end param's
		searchTraversalHelper(root, results, begin, end);
	}
	else// second ie bigger so we flip
	{
		searchTraversalHelper(root, results, end, begin);
	}

	// return the vector when finished
	return results;
}

template<class T>
T RedBlackTree<T>::closestLess(T value) const // returns the largest value that is smaller then value
{
	/// checks if root is a nullptr if so return an empty value
	isNullptr(root, value);

	// get the values and store it into a vector
	vector<T> vec = values();

	// find's the largest which is less than the value param
	auto const it = lower_bound(vec.begin(), vec.end(), value);

	// value is not found
	if (it == vec.begin()) { return value; }

	// value is found
	return *(it - 1);
}

template<class T>
T RedBlackTree<T>::closestGreater(T value) const // returns the smallest value that is greater then value
{
	// checks if root is a nullptr if so return an empty value
	isNullptr(root, value);

	// get the values and store it into a vector
	vector<T> vec = values();

	// find's the smallest which is greater than the value param
	auto const it = upper_bound(vec.begin(), vec.end(), value);

	// value is not found
	if (it == vec.end()) { return value; }

	// value is found
	return *it;
}

template<class T>
vector<T> RedBlackTree<T>::values() const
{
	// create a vector with T type's
	vector<T> res;

	// check if the root is nullptr
	// if so return an empty vector
	isNullptr(root, res);

	// call the recurisve method and pass the vector as a refrence
	valueTraversalHelper(root, res);

	// return the vector
	return res;
}

template<class T>
int RedBlackTree<T>::size() const
{
	// return the tree size
	return currentSize;
}

// --Helpers =======================================================================================

template<class T>
NodeT<T>* RedBlackTree<T>::copyHelper(NodeT<T>* copy)
{
	// check if the param is a nullptr
	// if so return the param
	isNullptr(copy, copy);

	// create a newNode 
	NodeT<T>* newNode = new NodeT<T>(copy->data);

	// go through the tree assigning the left and right to the NewNode
	newNode->left = copyHelper(copy->left);
	newNode->right = copyHelper(copy->right);

	// copy the colour
	newNode->isBlack = copy->isBlack;

	// connect parents
	if (copy->left != nullptr) 
	{
		newNode->left->parent = newNode;
	}

	if (copy->right != nullptr) 
	{
		newNode->right->parent = newNode;
	}

	// return the newNode
	return newNode;
}

template<class T>
void RedBlackTree<T>::clearTreeHelper(NodeT<T>* nd) const
{
	// check if the param is null if so return
	isNullptr(nd);

	// go through the tree
	clearTreeHelper(nd->left);
	clearTreeHelper(nd->right);

	// and delete each node
	delete nd;
}

template <class T>
NodeT<T>* RedBlackTree<T>::find(T value) const
{
	// create a traverse pointer
	NodeT<T>* ptr = root;

	// as long as the pointer is not nullptr
	while (ptr != nullptr) 
	{
		// if the value is found return the pointer
		if (value == ptr->data) 
		{ 
			return ptr; 
		}
		else if (value < ptr->data)// check if the value is less than
		{ 
			ptr = ptr->left; 
		}
		else // greater than
		{ 
			ptr = ptr->right; 
		}

	}

	// value does not exist
	return nullptr;
}

template <class T>
void RedBlackTree<T>::valueTraversalHelper(NodeT<T>* nd, vector<T>& vec) const
{
	// check if the param is null if so return
	isNullptr(nd);

	// recurse the tree and push_back the values into the vector refrence param
	valueTraversalHelper(nd->left, vec);
	vec.push_back(nd->data);
	valueTraversalHelper(nd->right, vec);
}

template <class T>
void  RedBlackTree<T>::searchTraversalHelper(NodeT<T>* nd, vector<T>& vec, T begin, T end) const
{
	// check if the param is null if so return
	isNullptr(nd);

	// recurse over the tree in the given range and puch_back the values into the vector param
	searchTraversalHelper(nd->left, vec, begin, end);
	if (nd->data >= begin && nd->data <= end)
	{
		vec.push_back(nd->data);
	}
	searchTraversalHelper(nd->right, vec, begin, end);
}

template <class T>
NodeT<T>* RedBlackTree<T>::predecessor(NodeT<T>* nd) const
{
	// make a pointer to the nd left
	NodeT<T>* current = nd->left;

	// check if it's left child is a nullptr
	isNullptr(current, nd);

	// make the predecessor point to nd
	NodeT<T>* predParent = nd;

	// iterate through and return the largest value
	while (current->right != nullptr) {
		predParent = current;
		current = current->right;
	}
	return current;
}

template<class T>
NodeT<T>* RedBlackTree<T>::BinaryTreeInsert(NodeT<T>* ndCurrent, NodeT<T>* newNode)
{
	// check if the param is null if so return
	isNullptr(ndCurrent, newNode);

	// recursively add the node into the tree
	if (newNode->data < ndCurrent->data)
	{
		ndCurrent->left = BinaryTreeInsert(ndCurrent->left, newNode);
		ndCurrent->left->parent = ndCurrent;
	}
	else if (newNode->data > ndCurrent->data)
	{
		ndCurrent->right = BinaryTreeInsert(ndCurrent->right, newNode);
		ndCurrent->right->parent = ndCurrent;
	}

	return ndCurrent;
}

template<class T>
void RedBlackTree<T>::fixRemovalRBT(NodeT<T>* ndChild, NodeT<T>* ndParent)
{
	// loop's if ndChild is a leaf or is a black ndChild and isn't the root
	while ((ndChild == nullptr || ndChild->isBlack == true) && ndChild != root)
	{
		NodeT<T>* sibling;

		// check if the ndChild is left child
		if (ndChild == ndParent->left)
		{
			// nd's sibling
			sibling = ndParent->right;

			// check if the sibling is not a leaf and not a black node
			if (sibling != nullptr && sibling->isBlack == false)
			{
				// we change the colours and rotate left, then assign the sibling to th nd's parent right
				sibling->isBlack = true;
				ndParent->isBlack = false;
				rotateLeft(ndParent);
				sibling = ndParent->right;
			}

			// checks if the the sibling's children is either leaf or a black node
			if ((sibling->left == nullptr || sibling->left->isBlack == true) && (sibling->right == nullptr|| sibling->right->isBlack == true))
			{
				// we change the colours and assign nd to the parent, then we go into the while loop again
				sibling->isBlack = false;
				ndChild = ndParent;

				// I notices my ndChild == ndParent which wouldn't work when looping again
				// so we need to set the new ndParent by assigning the new ndChild's parent
				ndParent = ndChild->parent;
			}
			else
			{
				// otherwise checks if the sibling's right child is a leaf or a black node
				if (sibling->right == nullptr || sibling->right->isBlack == true)
				{
					// we change the colours and rotate it right
					// and we make the new sibling
					sibling->left->isBlack = true;
					sibling->isBlack = false;
					rotateRight(sibling);
					sibling = ndParent->right;
				}
				
				// otherwise it's sibling's left child
				// we change the colours and rotate it left
				// we make nd the root and leave the while loop
				sibling->isBlack = ndParent->isBlack;
				ndParent->isBlack = true;
				sibling->right->isBlack = true;
				rotateLeft(ndParent);
				ndChild = root;
			}
		}
		else // symmetric to the if
		{
			sibling = ndParent->left;

			if (sibling != nullptr && sibling->isBlack == false)
			{
				sibling->isBlack = true;
				ndParent->isBlack = false;
				rotateRight(ndParent);
				sibling = ndParent->left;
			}
			
			if ((sibling->left == nullptr || sibling->left->isBlack == true) && (sibling->right == nullptr || sibling->right->isBlack == true))
			{
				sibling->isBlack = false;
				ndChild = ndParent;
				ndParent = ndChild->parent;
			}
			else
			{
				if (sibling->left == nullptr || sibling->left->isBlack == true)
				{
					sibling->right->isBlack = true;
					sibling->isBlack = false;
					rotateLeft(sibling);
					sibling = ndParent->left;
				}

				sibling->isBlack = ndParent->isBlack;
				ndParent->isBlack = true;
				sibling->left->isBlack = true;
				rotateRight(ndParent);
				ndChild = root;
			}
		}
	}

	// if not a leaf or a black node and it is the root we change ndChild's colour
	if (ndChild != nullptr) { ndChild->isBlack = true; }
}

template<class T>
void RedBlackTree<T>::rotateRight(NodeT<T>* nd)
{
	// assign a ptr to the nd's left and then nd's right node
	NodeT<T>* parentNode = nd->left;
	nd->left = parentNode->right;

	// we check if the nd->left->right is not a nullptr
	if (parentNode->right != nullptr) 
	{ 
		// we assign the nd->left->right parent to the node
		parentNode->right->parent = nd; 
	}

	// discconect the two nodes
	parentNode->parent = nd->parent;

	// we check if nd is the roots
	// if not we check if nd is the right child or left
	if (nd->parent == nullptr) 
	{ 
		root = parentNode; 
	}
	else if (nd == nd->parent->right)
	{ 
		nd->parent->right = parentNode; 
	}
	else 
	{ 
		nd->parent->left = parentNode; 
	}

	// connect the nodes
	parentNode->right = nd;
	nd->parent = parentNode;
}


template<class T>
void RedBlackTree<T>::rotateLeft(NodeT<T>* nd)
{
	// symmetric to the rotateRight method

	NodeT<T>* parentNode = nd->right;
	nd->right = parentNode->left;

	if (parentNode->left != nullptr) 
	{ 
		parentNode->left->parent = nd; 
	}

	parentNode->parent = nd->parent;

	if (nd->parent == nullptr) 
	{ 
		root = parentNode; 
	}
	else if (nd == nd->parent->left) 
	{ 
		nd->parent->left = parentNode; 
	}
	else 
	{ 
		nd->parent->right = parentNode; 
	}

	parentNode->left = nd;
	nd->parent = parentNode;
}

//======================================================================================================
// --PART 2
//======================================================================================================

void statistics(string filename)
{
	// declare variables
	RedBlackTree<double> rbtObj;
	ifstream file;
	
	double vals, sum = 0.0;

	// we open the file
	file.open(filename, ios::in);

	// we check if the file is open
	if (file.is_open())
	{
		//check if the file is empty
		if (file.peek() == ifstream::traits_type::eof())
		{
			LOG("File is empty, it could not be read");
			return;
		}

		// we read it's contents and if something bad happens it stops the loop
		while (file >> vals)
		{
			sum += vals;

			// inserts the data into the tree
			rbtObj.insert(vals);
		}
	}
	else// other wise if it's not open we close the file and return
	{
		LOG("File could not be opened");
		file.close();
		return;
	}
	
	// close the file after success
	file.close();

	// print the according values instructed to print
	LOG("# of values:  " << rbtObj.size());
	LOG("Average:      " << sum / rbtObj.size());

	vector<double> vec = rbtObj.values();

	if (rbtObj.size() % 2 == 0)// is even
	{
		int valueOne = (vec.size() + 1) / 2;
		int valueTwo = (vec.size() - 1) / 2;

		double medianEven = (vec.at(valueOne) + vec.at(valueTwo)) / 2.0;

		LOG("Median:       " << medianEven);
	}
	else // is odd
	{
		double medianOdd = vec.at(vec.size() / 2);
		LOG("Median:       " << medianOdd);
	}

	LOG("Closest < 42: " << rbtObj.closestLess(42));
	LOG("Closest > 42: " << rbtObj.closestGreater(42));
}
