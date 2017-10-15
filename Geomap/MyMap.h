#ifndef MYMAP_INCLUDED
#define MYMAP_INCLUDED
#include "support.h"
// MyMap.h

// Skeleton for the MyMap class template.  You must implement the first six
// member functions.

template<typename KeyType, typename ValueType>
class MyMap
{
public:
	MyMap() 
		:m_size(0), head(nullptr)
	{}
	~MyMap() {
		clear();
	}
	void clear();
	
	int size() const { return m_size; }

	void associate(const KeyType& key, const ValueType& value);

	  // for a map that can't be modified, return a pointer to const ValueType
	const ValueType* find(const KeyType& key) const;

	  // for a modifiable map, return a pointer to modifiable ValueType
	ValueType* find(const KeyType& key)
	{
		return const_cast<ValueType*>(const_cast<const MyMap*>(this)->find(key));
	}

	  // C++11 syntax for preventing copying and assignment
	MyMap(const MyMap&) = delete;
	MyMap& operator=(const MyMap&) = delete;

private:
	struct BinaryTree {
		BinaryTree( KeyType key,ValueType value) {
			m_key = key;
			m_value = value;
			right = nullptr;
			left = nullptr;
		}
		KeyType m_key;
		ValueType m_value;
		BinaryTree* right;
		BinaryTree* left;
	};
	int m_size;
	BinaryTree* head;
	void clear(BinaryTree* curr);
};

template<typename KeyType, typename ValueType>
void MyMap<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value) {
	//create our first tree if there isn't one
	if (m_size == 0){
		head = new BinaryTree(key, value);
		m_size++;
		return;
	}

	BinaryTree* temp = head;
	while (temp != nullptr) {
		//if we've seen this before adjust value and end loop.
		if (temp->m_key == key) {
			temp->m_value = value;
			temp = nullptr;
			continue;
		}
		//smaller keys go one the left
		if (key < temp->m_key) {
			if (temp->left == nullptr) {
				temp->left = new BinaryTree(key, value);
				m_size++;
				return;
			}
			else
				temp = temp->left;
		}
		//larger keys go on the right
		else {
			if (temp->right == nullptr) {
				temp->right = new BinaryTree(key, value);
				m_size++;
				return;
			}
			else
				temp = temp->right;
		}
	}
}

template<typename KeyType, typename ValueType>
const ValueType* MyMap<KeyType, ValueType>::find(const KeyType& key) const {
	
	if (m_size == 0)
		return nullptr; //tree is empty

	BinaryTree* temp = head;
	//search for the values. larger on right smaller on left.
	while (temp != nullptr)
	{
		if (key == temp->m_key)
			return &temp->m_value;
		else if (key < temp->m_key)
			temp = temp->left;
		else
			temp = temp->right;
	}
	return nullptr;
}
template<typename KeyType, typename ValueType>
void MyMap<KeyType, ValueType>::clear(BinaryTree* curr) {
	//base case
	if (curr == nullptr)
		return;

	clear(curr->right);
	clear(curr->left);
	
	delete curr;
}

template<typename KeyType, typename ValueType>
void MyMap<KeyType, ValueType>::clear() {
	clear(head); //easiest to solve by recursion.
	m_size = 0;
}
#endif // MYMAP_INCLUDED