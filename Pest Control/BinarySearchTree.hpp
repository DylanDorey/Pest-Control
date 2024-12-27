
#ifndef BINARY_SEARCH_TREE_HPP
#define BINARY_SEARCH_TREE_HPP

#include "LinkedList.hpp"

template <typename T>
class BinarySearchTree
{
    private:

    struct Node
    {
        Node* leftChild;
        Node* rightChild;
        T data;

        Node(T d) : data(d), leftChild(nullptr), rightChild(nullptr){}
    }* root_;


    // Helper function to copy a node and all of its descendants
    Node* copyNode(Node const * const &nodeToCopy)
    {
        if (nodeToCopy != nullptr)
        {
            Node* newNode = new Node(nodeToCopy->data);
            newNode->leftChild = copyNode(nodeToCopy->leftChild);
            newNode->rightChild = copyNode(nodeToCopy->rightChild);
            return newNode;
        }

        return nullptr;
    }


    // Recursively determine proper location in tree and insert new data.
    // Return false if data already exists in tree, otherwise true
    bool insert(T &data, Node* &subRoot)
    {
        if (subRoot == nullptr)
        {
            subRoot = new Node(data);
            return true;
        }
        else if (data < subRoot->data)
        {
            return insert(data, subRoot->leftChild);
        }
        else if (data > subRoot->data)
        {
            return insert(data, subRoot->rightChild);
        }

        return false;
    }


    // Recursively find and return pointer to minimum data in tree, or null if emptry tree
    Node* findMinimumNode(Node* subRoot)
    {
        if (subRoot == nullptr)
        {
            return subRoot;
        }
        else if (subRoot->leftChild != nullptr)
        {
            return findMinimumNode(subRoot->leftChild);
        }

        return subRoot;
    }


    // Recursively find and return pointer to maximum data in tree, or null if emptry tree
    Node* findMaximumNode(Node* subRoot)
    {
        if (subRoot == nullptr)
        {
            return subRoot;
        }
        else if (subRoot->rightChild != nullptr)
        {
            return findMaximumNode(subRoot->rightChild);
        }

        return subRoot;
    }



    // Recursively find and remove data from tree.
    // Return false if data doesn't exist in tree, otherwise true
    bool remove(T &data, Node* &subRoot)
    {
        if (subRoot == nullptr)
        {
            return false;
        }
        else if (data < subRoot->data)
        {
            return remove(data, subRoot->leftChild);
        }
        else if (data > subRoot->data)
        {
            return remove(data, subRoot->rightChild);
        }
        else
        {
            // Case 1: No child
            if (subRoot->leftChild == nullptr && subRoot->rightChild == nullptr)
            {
                delete subRoot;
                subRoot = nullptr;
            }
            // Case 2: One child
            else if (subRoot->leftChild == nullptr)
            {
                Node* temp = subRoot;
                subRoot = subRoot->rightChild;
                delete temp;
            }
            else if (subRoot->rightChild == nullptr)
            {
                Node* temp = subRoot;
                subRoot = subRoot->leftChild;
                delete temp;
            }
            // Case 3: Two children
            else
            {
                //Option 1: Find minimum value in right sub-tree
                Node* temp = findMinimumNode(subRoot->rightChild);
                subRoot->data = temp->data;
                remove(temp->data, subRoot->rightChild);


                // //Option 2: Find maximum value in left sub-tree
                // Node* temp = findMaximumNode(subRoot->leftChild);
                // subRoot->data = temp->data;
                // remove(temp->data, subRoot->leftChild);
            }
        }

        return true;
    }



    // Recursively determine and return the number of nodes in the tree
    size_t size(Node* subRoot)
    {
        if (subRoot != nullptr)
        {
            return 1 + size(subRoot->leftChild) + size(subRoot->rightChild);
        }

        return 0;
    }


    // Recursively remove all nodes in the tree
    void clear(Node* &subRoot)
    {
        if (subRoot != nullptr)
        {
            clear(subRoot->leftChild);
            clear(subRoot->rightChild);
            delete subRoot;
            subRoot = nullptr;
        }
    }



    // Recursively append all tree data to "returnList" parameter in ascending order
    void getSortedList(Node* subRoot, LinkedList<T> &returnList)
    {
        if (subRoot != nullptr)
        {
            // more efficient because using pushFront
            getSortedList(subRoot->rightChild, returnList);
            returnList.pushFront(subRoot->data);
            getSortedList(subRoot->leftChild, returnList);
        }
    }


    
    public:

    // Constructor
    BinarySearchTree() : root_(nullptr){}

    // Copy constructor
    BinarySearchTree(BinarySearchTree const &rhs) : root_(nullptr)
    {
        root_ = copyNode(rhs.root_);
    }

    // Swap the contents of this tree with another
    void swap(BinarySearchTree &other)
    {
        Node* temp = root_;
        root_ = other.root_;
        other.root_ = temp;
    }

    // Assignment operator
    BinarySearchTree &operator =(BinarySearchTree rhs)
    {
        swap(rhs);
        return *this;
    }

    // Ruturn true if the tree is empty, otherwise false
    bool empty()
    {
        return root_ == nullptr;
    }

    // Insert new data into tree.
    // Return false if data already exists in tree, otherwise true
    bool insert(T data)
    {
        return insert(data, root_);
    }

    // Find and return pointer to minimum data in tree, or null if emptry tree
    T const * findMinimum()
    {
        Node* min = findMinimumNode(root_);
        if (min != nullptr)
        {
            return &min->data;
        }

        return nullptr;
    }

    // Find and return pointer to maximum data in tree, or null if emptry tree
    T const * findMaximum()
    {
        Node* max = findMaximumNode(root_);
        if (max != nullptr)
        {
            return &max->data;
        }

        return nullptr;
    }

    // Remove data from tree.
    // Return false if data doesn't exist in tree, otherwise true
    bool remove(T data)
    {
        return remove(data, root_);
    }

    // Return the number of nodes in the tree
    size_t size()
    {
        return size(root_);
    }

    // Remove all nodes in the tree
    void clear()
    {
        clear(root_);
    }

    // Return linked list of all data in tree in ascending order
    LinkedList<T> getSortedList()
    {
        LinkedList<T> returnList;
        getSortedList(root_, returnList);
        return returnList;
    }

    // Destructor
    ~BinarySearchTree()
    {
        clear(root_);
    }



    // Binary Search Tree (in-order) iterator
    class Iterator
    {
        private:
        LinkedList<Node*> nodeStack_;

        public:
        
        // Constructor
        Iterator(Node* root)
        {
            // Push all left children onto stack
            while (root != nullptr)
            {
                nodeStack_.pushFront(root);
                root = root->leftChild;
            }
        }

        // Get immutable pointer to current node's data 
        T const * current()
        {
            if (!nodeStack_.empty())
            {
                return &nodeStack_.at(0)->data;
            }

            return nullptr;
        }


        // Move to the next node
        void next()
        {
            if (!nodeStack_.empty())
            {
                Node* temp = nodeStack_.popFront()->rightChild;
                while (temp != nullptr)
                {
                    nodeStack_.pushFront(temp);
                    temp = temp->leftChild;
                }
            }
        }

        // Return true if there is a next node, otherwise false
        bool hasNext()
        {
            return !nodeStack_.empty();
        }

    };


    // Return a new in-order iterator object
    Iterator generateIterator()
    {
        return Iterator(root_);
    }

};

#endif