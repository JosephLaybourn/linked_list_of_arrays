#include <iostream>
#include <iomanip>
#include "lariat.h"

#if 1
/*******************************************************************************
========================= Constructors and Destructor ==========================
*******************************************************************************/

// Constructor
template<typename T, int Size>
Lariat<T, Size>::Lariat() : head_(), tail_(), size_(0), nodecount_(0), asize_(Size)
{
  // This constructor is really simple. You don't need to do any logic, just
  // use a member initializer list to initialize
}

// Copy Constructor (own-type)
template<typename T, int Size>
Lariat<T, Size>::Lariat(Lariat const & rhs)
{
  // This is the standard copy constructor. The function should loop through the
  // instance passed in, pushing each element of the other onto the back of
  // the one being constructed.
  // It should be done using the algorithm for (or directly calling) push_back
  // so that all of the nodes are split correctly.
  asize_ = rhs.asize_;
  size_ = 0;
  for (int i = 0; i < rhs.size_; i++)
  {
    push_back(rhs[i]);
  }
}

template<typename T, int Size>
template<typename L, int NewSize>
Lariat<T, Size>::Lariat(const Lariat<L, NewSize> &rhs)
{
  asize_ = Size;
  size_ = 0;
  for (int i = 0; i < rhs.size_; i++)
  {
    push_back(static_cast<T>(rhs[i]));
  }
}

// Destructor
template<typename T, int Size>
Lariat<T, Size>::~Lariat()
{
  // The destructor is a simple, generic destructor. It's sole purpose is to
  // free all the nodes in the linked list so there are no memory leaks.
  while (nodecount_)
  {
    removeNode(head_);
  }
}

// operator= (own-type)
template<typename T, int Size>
Lariat<T, Size> & Lariat<T, Size>::operator=(const Lariat &rhs)
{
  clear();
  // This assignment operator generally works exactly the same as you might
    // expect. Set the non-pointer members as necessary, clear this instance's
    // data, then walk through the right-hand argument's list adding each
    // element to this instance
  asize_ = rhs.asize_;
  for (int i = 0; i < rhs.size_; i++)
  {
    push_back(rhs[i]);
  }

  return *this;
}

template<typename T, int Size>
template<typename L, int NewSize>
Lariat<T, Size> &Lariat<T, Size>::operator=(const Lariat<L, NewSize> &rhs)
{
  clear();
  // This assignment operator generally works exactly the same as you might
    // expect. Set the non-pointer members as necessary, clear this instance's
    // data, then walk through the right-hand argument's list adding each
    // element to this instance
  asize_ = Size;
  for (int i = 0; i < rhs.size_; i++)
  {
    push_back(static_cast<T>(rhs[i]));
  }

  return *this;
}

/*******************************************************************************
============================ Assignment Operators ==============================
*******************************************************************************/


/*******************************************************************************
============================== Element Addition ================================
*******************************************************************************/

// insert
// Insert an element into the data structure at the index, between the element
// at [index - 1] and the element at [index]
template<typename T, int Size>
void Lariat<T, Size>::insert(int index, const T & value)
{
  //std::cout << "Inserting " << value << " at index " << index << std::endl;
  
  // The first thing to this function is to check for an Out of Bounds error. If
    // the index is invalid, throw a LariatException with E_BAD_INDEX and the
    // description "Subscript is out of range"
  if (index < 0 || index > size_)
  {
    throw LariatException(LariatException::E_BAD_INDEX, "Subscript is out of range");
  }
  // Make sure to handle the "edge" cases, which allow for insertion at the end
    // of the deque as well as the beginning. I personally suggest calling
    // push_back and push_front in these cases, as it helps minimize the amount
    //  of code written and the algorithm is identical anyways.

  IndexedNode indexedNode = findElement(index);
  LNode *currentNode = indexedNode.node;
  int newIndex = indexedNode.index;

  if (index == size_)
  {
    push_back(value);
    return;
  }
  else if (index == 0)
  {
    push_front(value);
    return;
  }

  if (!head_)
  {
    head_ = makeNode();
    tail_ = head_;
  }
  // The next thing to do is to set up the actual insertion algorithm.
  // First, find the node and local index of the element being inserted. This
  // can be done with the findElement helper function detailed in the
  // Recommended Helper Functions section of this guide.

  // Next, shift all elements past that local index one element to the right.
  // This can be done with the shiftUp function detailed in the Recommended
  // Helper Functions section of the guide.
  //...Diagram.  See handout

  // This should work for the most common use-case, but it will cause a
  // problem if the node is full.
  // If the node is full, you will need to keep track of the last element in
  // the node before you shift all the elements to the right. Again, this is
  // easy to do with the recommended helper function.
  // I call this "popped off" element the overflow.
  //...Diagram.  See handout

  if (currentNode->count == asize_)
  {
    if (newIndex < currentNode->count)
    {
      split(currentNode, SplitType::BOTTOMHEAVY);
    }
    else
    {
      split(currentNode, SplitType::TOPHEAVY);
    }

    if (currentNode->count < newIndex)
    {
      newIndex -= currentNode->count + 1;
      currentNode->count++;
      currentNode = currentNode->next;
      currentNode->count--;
      shiftDown(currentNode);
    }
  }

  if (newIndex > currentNode->count)
  {
    newIndex -= currentNode->count;
    currentNode = currentNode->next;
  }
  shiftUp(currentNode, newIndex);
  currentNode->count++;

  // Next, you will need to split the node.
  // I would recommend writing a helper function for this algorithm as it
  // will be used elsewhere. I have detailed the split algorithm in the
  // Recommended Helper Functions section below
  // You will need to transfer the overflow to the last element of the new
  // node created by the split. It is possible to put this part of the
  // algorithm directly in the split function. I will not detail that in
  // this guide, but rather leave it for you to discover on your own.
  // Working out that algorithm will probably make your code much cleaner,
  // so I would definitely recommend figuring it out.
  // As the split algorithm accurately updates the node counts for the split
  // nodes, the only thing left to do is increment the node count.

  if (!currentNode->next)
  {
    //std::cout << "Tail Updated" << std::endl;
    tail_ = currentNode;
  }
  else
  {
    findTail(currentNode);
  }

  currentNode->values[newIndex] = value;
  size_++;
}

// push_back
template<typename T, int Size>
void Lariat<T, Size>::push_back(const T & value)
{
  
  if (!head_)
  {
    head_ = makeNode();
    tail_ = head_;
  }
  // This is an easy algorithm using the split function.
  // If the tail node is full, split the node and update the tail_ pointer.
  if (tail_->count == asize_)
  {
    split(tail_, SplitType::TOPHEAVY);
    tail_ = tail_->next;
  }
  // Set the last element in the tail's array to the value.
  tail_->values[tail_->count] = value;
  // Increment the tail node's count.
  tail_->count++;

  size_++;
}

// push_front
template<typename T, int Size>
void Lariat<T, Size>::push_front(const T & value)
{
  if (!head_)
  {
    head_ = makeNode();
    tail_ = head_;
  }
  // This algorithm is more similar to the insert function than the push_back
    // algorithm, but is still relatively simple.
  // If the head node is empty, increment the node's count.
  // If the head node is full, you will need to shift the elements up, in the
    // same way they were shifted in the insert function, making sure to track
    // the overflow.
    // Next you will need to split the node.
  if (head_->count == asize_)
  {
    split(head_);
  }
  // In order to account for splitting the only node in the linked list, you
    // will have to update the tail_ pointer as necessary.
  if (nodecount_ == 2)
  {
    tail_ = head_->next;
    tail_->prev = head_;
  }
  // If the head node isn't full yet, just shift the head node up an element
  // from element 0 and increase the count.

  shiftUp(head_, 0);

  // Set the 0'th element of the head to the value.
  head_->values[0] = value;
  head_->count++;

  size_++;
}

/*******************************************************************************
=============================== Element Removal ================================
*******************************************************************************/

// erase
template<typename T, int Size>
void Lariat<T, Size>::erase(int index)
{
  // This function uses the findElement helper function I have detailed in the
    // Recommended Helper Functions section of this guide. Having implemented
    // that, the function itself is relatively simple.
  // You can use the pop_back and pop_front functions if the index requested is
    // the first or last element.
  //std::cout << "Deleting at index " << index << std::endl;

  if (index == 0)
  {
    pop_front();
    return;
  }
  else if (index == (size_ - 1))
  {
    pop_back();
    return;
  }
  // First, find the containing node and local index of the requested global
    // index.
  IndexedNode iNode = findElement(index);
  // Shift all the elements in the node beyond the local index left one element,
    // covering the element being erased. This is done with the shiftDown
    // function detailed in the Recommended Helper Functions section. Make sure
    // to account for the node being only one element large.
  shiftDown(iNode.node, iNode.index);
  // Decrement the node's count.
  iNode.node->count--;
  size_--;
}

// pop_back
template<typename T, int Size>
void Lariat<T, Size>::pop_back()
{
  // Decrement the count of the tail node.
  tail_->count--;
  size_--;
  if (tail_->count == 0)
  {
    removeNode(tail_);
  }
}

// pop_front
template<typename T, int Size>
void Lariat<T, Size>::pop_front()
{
  // Shift all elements in the head node down one element.
  shiftDown(head_);
  // Decrement the head's count.
  head_->count--;
  size_--;
  // If the head node is now empty, free the associated memory.
  if (head_->count == 0)
  {
    removeNode(head_);
  }
}

/*******************************************************************************
================================ Element Access ================================
*******************************************************************************/

// operator[]
template<typename T, int Size>
T & Lariat<T, Size>::operator[](int index)
{
  // Find the containing node and local index of the index passed in. Like
    // insert and erase, this is easily done with the findElement helper
    // function I detailed in the Recommended Helper Functions section of this
    // guide.
  IndexedNode iNode = findElement(index);

  // Return the element at the local index of the containing node.
  return iNode.node->values[iNode.index];
}

template<typename T, int Size>
const T & Lariat<T, Size>::operator[](int index) const
{
  // Find the containing node and local index of the index passed in. Like
    // insert and erase, this is easily done with the findElement helper
    // function I detailed in the Recommended Helper Functions section of this
    // guide.
  IndexedNode iNode = findElement(index);

  // Return the element at the local index of the containing node.
  return iNode.node->values[iNode.index];
}

// first
template<typename T, int Size>
T & Lariat<T, Size>::first()
{
  // This is one of the easiest functions in this assignment.
  // Return the first element of the head node.

  // TODO: insert return statement here
}

template<typename T, int Size>
T const & Lariat<T, Size>::first() const
{
  // TODO: insert return statement here
}

// last
template<typename T, int Size>
T & Lariat<T, Size>::last()
{
  // This is also an easy function.
  // Return the last element in the tail node.

  // TODO: insert return statement here
}

template<typename T, int Size>
T const & Lariat<T, Size>::last() const
{
  // TODO: insert return statement here
}

/*******************************************************************************
========================== Data Structure Information ==========================
*******************************************************************************/

// find
template<typename T, int Size>
unsigned Lariat<T, Size>::find(const T & value) const
{
  // Walk the list in a similar fashion to that detailed in the findElement
    // helper function, but check equivalence for each element in each node,
    // returning the index when the desired element is found.
  LNode *node = head_;
  for (unsigned globalIndex = 0; globalIndex < unsigned(size_); )
  {
    for (int i = 0; i < node->count; i++)
    {
      if (node->values[i] == value)
      {
        return globalIndex;
      }
      globalIndex++;
    }
    node = node->next;
  }
  // If the desired element is not found, return the total number of elements
    // contained in the data structure.
  return size_;
}

// size
template<typename T, int Size>
size_t Lariat<T, Size>::size(void) const
{
  // You should be tracking the size_ member variable throughout the element
    // addition and removal processes. Return that variable now.
  return size_;
}

/*******************************************************************************
============================ Data Structure Control ============================
*******************************************************************************/

// clear
template<typename T, int Size>
void Lariat<T, Size>::clear(void)
{
  // This is a relatively simple function with a similar algorithm to the
    // class destructor.
  // First, loop through the list, freeing each node in turn.
  // Once the list is empty, update the necessary member variables.
  while (head_)
  {
    removeNode(head_);
  }
  size_ = 0;
}

// compact
template<typename T, int Size>
void Lariat<T, Size>::compact()
{
  //std::cout << "Node Count: " << nodecount_ << std::endl;

  // Compact takes all the data stored in the linked list and moves it into the
    // smallest number of nodes possible. Then it frees all empty nodes at the
    // end of the list.
  if (nodecount_ == 1 || head_ == nullptr)
  {
    //std::cout << "Nothing happened" << std::endl;
    return;
  }
  // The algorithm for this walks the list at two points in parallel. I will
    // refer to the walker having elements shifted into it as the left foot and
    // the walker reading through the elements to shift as the right foot.
  LNode *leftFoot = head_;
  LNode *rightFoot = head_->next;
  // First, loop through the list with both feet until the left node is not
    // already full and the right foot hasn't walked off the list.
  while (rightFoot)
  {
    if (leftFoot->count != asize_)
    {
      break;
    }
    leftFoot = leftFoot->next;
    rightFoot = rightFoot->next;
  }
  // Next, walk through the list while the right foot hasn't lost the list.
    // This loop should first store the count of elements in the right node,
      // then set the count of the right node to zero. This allows the left foot
      // to update it as it is given elements to store.
  //std::cout << *this << std::endl;
  while (rightFoot)
  {
    int savedCount = leftFoot->count;
    for (int i = savedCount; i < asize_; i++)
    {
      if (rightFoot->count != 0)
      { 
        leftFoot->values[i] = rightFoot->values[0];
        leftFoot->count++;
        rightFoot->count--;
        shiftDown(rightFoot, 0);
      }
      if(rightFoot->count == 0)
      {
        LNode *temp = rightFoot->prev;
        removeNode(rightFoot);
        if (!temp->next)
        {
          tail_ = temp;
          return;
        }
        rightFoot = temp->next;
        break;
      }
    }
    if (leftFoot->count == asize_)
    {
      leftFoot = leftFoot->next;
      rightFoot = rightFoot->next;
    }
  }

  if (!leftFoot->next)
  {
    tail_ = leftFoot;
  }
  else
  {
    findTail(leftFoot);
  }

    // This loop should have a nested loop that stores each value from the right
      // foot in the left foot's node, and steps the left foot to the next node
      // when it is filled.
  //...Diagram.  See handout

      // Once the deque has been compacted, remove all the extra nodes from the
      // end of the list.
}

// split
template<typename T, int Size>
void Lariat<T, Size>::split(LNode * node, SplitType type)
{
  // This helper function takes a full node and splits into two nodes of an
  // aproximately equivalent number of elements.
  // First, create a new node to split the original node into.
  // Calculate the index from which to move elements over from. This should
  // account for an element being appended to the second node after the
  // function has been called. If you're implementing this with overflow,
  // you should have already worked out how the overflow gets assigned its
  // position.
  // Loop through the elements from the index in the first node, transferring
  // them each to the newly added node. Update the added node's count during
  // the loop.
      //...See handout for diagram

  // Set the count of the original node to index, and insert the added node
  // immediately after the original node.
  LNode *newNode = makeNode();
  LNode *oldNext = nullptr;
  if (node->next)
  {
    oldNext = node->next;
    oldNext->prev = newNode;
  }
  newNode->next = node->next;
  newNode->prev = node;
  node->next = newNode;

  int newNodeCount;
  if (asize_ % 2)
  {
    if (type == SplitType::TOPHEAVY)
    {
      newNodeCount = (asize_ / 2) + 1;
      newNode->count = newNodeCount - 1;
    }
    else
    {
      newNodeCount = (asize_ / 2);
      newNode->count = newNodeCount + 1;
    }
  }
  else
  {
    if (type == SplitType::TOPHEAVY)
    {
      newNodeCount = (asize_ / 2) + 1;
      newNode->count = newNodeCount - 2;
    }
    else
    {
      newNodeCount = asize_ / 2;
      newNode->count = newNodeCount;
    }
  }

  for (int i = 0; i < asize_ - newNodeCount; i++)
  {
    newNode->values[i] = node->values[newNodeCount + i];
  }

  node->count = newNodeCount;
}

// findElement
template<typename T, int Size>
typename Lariat<T, Size>::IndexedNode Lariat<T, Size>::findElement(int index) const
{
  // This function takes a global index to find in the deque and must return
  // both a pointer to the node in the list and the local index of the element
  // in the returned node.
  // Clearly returning multiple variables is a *small* challenge in C and C++,
  // so you will have to find a way to work around that.
  // The algorithm for this function is focused around looping through the list
  // and tracking the total number of elements stored before the current
  // node. I used a counter and added each node's count until I passed the
  // desired global index.
  // Because each node in the list only stores the number of elements it is
  // using actively, the count of each has to be added individually to keep an
  // accurate count of the number of elements that have been stepped over.
  if (!head_)
  {
    IndexedNode nullNode; 
    nullNode.node = nullptr;
    nullNode.index = 0;
    return nullNode;
  }

  if (index < head_->count)
  {
    IndexedNode onlyNode;
    onlyNode.node = head_;
    onlyNode.index = index;
    return onlyNode;
  }

  int newIndex = index;
  LNode *node = head_;
  while (node->next)
  {
    newIndex -= node->count;
    node = node->next;
    if (newIndex < node->count)
    {
      break;
    }
  }
  IndexedNode iNode;
  iNode.node = node;
  iNode.index = newIndex;

  return iNode;
}

// shiftUp
template<typename T, int Size>
void Lariat<T, Size>::shiftUp(LNode *node, int index)
{
  // This helper function is almost identical to one that was particularly
  // useful for the CS170 Vector lab. It uses a simple swap algorithm to swap
  // every element between the index and the last element in the node's array
  // in place.
  // Make sure your bound-checking is good and you have accurately defined the
  // limits of your memory. If you find a wrong value somewhere unexpected,
  // you probably have the index limit wrong somehow.
    //...See handout for diagram

  for (int i = index; i < node->count; i++)
  {
    node->values[node->count - (i - index)] = node->values[(node->count - 1) - (i - index)];
  }

  // I would highly recommend writing a templatized swap function. It has uses
  // in most basic algorithms related to data structures.
}

// shiftDown 
template<typename T, int Size>
void Lariat<T, Size>::shiftDown(LNode *node, int index)
{
  // The shift down helper function works very similarly to the shift up, by
  // swapping values in the node's array.
  // The key difference between the two is that the shift down function does
  // not have a fixed index. It should move down the array, swapping each
  // element with the element immediately before it. In this way, it still
  // preserves the data it is writing over, but does so by shifting it from
  // the start of the range to the end rather than the opposited direction.
  for (int i = index; i < node->count; i++)
  {
    node->values[i] = node->values[i + 1];
  }
}

template<typename T, int Size>
typename Lariat<T, Size>::LNode * Lariat<T, Size>::makeNode()
{
  LNode *newNode = new LNode();
  nodecount_++;

  return newNode;
}

template<typename T, int Size>
void Lariat<T, Size>::removeNode(typename Lariat<T, Size>::LNode *node)
{
  // Removing the only node in the list
  if (!node->next && !node->prev)
  {
    delete node;
    head_ = nullptr;
    tail_ = nullptr;
    nodecount_ = 0;
    return;
  }
  // Removing the tail
  if (!node->next)
  {
    LNode *newTail = node->prev;
    newTail->next = nullptr;
    tail_ = newTail;
    delete node;
    nodecount_--;
    return;
  }
  // Removing the head
  if (!node->prev)
  {
    LNode *newHead = node->next;
    newHead->prev = nullptr;
    head_ = newHead;
    delete node;
    nodecount_--;
    return;
  }

  // Removing a middle node
  LNode *nextNode = node->next;
  LNode *prevNode = node->prev;

  prevNode->next = nextNode;
  nextNode->prev = prevNode;
  delete node;
  nodecount_--;
}

template<typename T, int Size>
void Lariat<T, Size>::findTail(LNode * node)
{
  LNode *temp = node;
  while (temp->next)
  {
    temp = temp->next;
  }
  tail_ = temp;
}

template <typename T, int Size>
std::ostream& operator<<(std::ostream &os, Lariat<T, Size> const & list)
{
  typename Lariat<T, Size>::LNode * current = list.head_;
  int index = 0;
  while (current)
  {
    os << "Node starting (count " << current->count << ")\n";
    for (int local_index = 0; local_index < current->count; ++local_index)
    {
      os << index << " -> " << current->values[local_index] << std::endl;
      ++index;
    }
    os << "-----------\n";
    current = current->next;
  }
  return os;
}
#else // fancier 
#endif