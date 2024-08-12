#pragma once
#ifndef LIST_HPP
#define LIST_HPP

// a general template structure for node of a singly linked list
template <typename type>
struct Node
{
    type data;           // the contents of node
    Node<type> *next{0}; // points to next node in list

    // constructors:
    Node() = default;

    Node(const type &data, Node<type> *next)
        : data(data), next(next) {}

    Node(const Node<type> &other)
        : data(other.data) {}

    Node<type> &operator=(const Node<type> &other)
    {
        if (this != &other)
            this->data = other.data;
        return *this;
    }
};

// a general template class of a singly linked list
template <typename type>
class List
{
private:
    Node<type> *head{0}; // points to first node
    Node<type> *tail{0}; // points to last node

public:
    // constructors & deconstructors:
    List() = default;

    List(const List<type> &other)
    {
        Node<type> *it = other.head;
        while (it != nullptr)
        {
            push_back(it->data);
            it = it->next;
        }
    }

    List<type> &operator=(const List<type> &other)
    {
        if (this == &other)
            return *this;
        clear();

        Node<type> *it = other.head;
        while (it != nullptr)
        {
            push_back(it->data);
            it = it->next;
        }
        return *this;
    }

    ~List()
    {
        clear();
    }

    void clear()
    {
        Node<type> *remove;
        while (head)
        {
            remove = head;
            head = head->next;
            delete remove;
        }
        head = tail = nullptr;
    }

    bool empty() const { return head == nullptr; }

    // inserts a new element at the end of list
    type *push_back(const type &data)
    {
        Node<type> *newnode = new Node<type>(data, nullptr);
        if (head == nullptr)
        { // insert directly:
            head = tail = newnode;
        }
        else
        { // insert at tail:
            tail->next = newnode;
            tail = tail->next;
        }
        return &(newnode->data);
    }

    // deletes a node at from the end of list
    type pop_back()
    {
        if (head == nullptr) // error if empty:
            throw "In pop_back() : list is already empty";

        type ret;         // will hold the contents of deleted node
        if (head == tail) // there is only one node?
        {                 // yes: delete directly:
            ret = head->data;
            delete head;
            head = tail = nullptr; // no node in list
        }
        else
        { // get 2nd-last node:
            Node<type> *secondLast = head;
            while (secondLast->next != tail)
                secondLast = secondLast->next;
            // delete last:
            Node<type> *to_remove = tail;
            ret = tail->data;
            tail = secondLast;
            tail->next = nullptr;
            delete to_remove;
        }
        return ret;
    }

    type *search(const type &data)
    {
        Node<type> *it = head;
        while (it)
        {
            if (it->data == data)
                return &(it->data);
            it = it->next;
        }
        return nullptr;
    }

    // returns an iterator to the start of list
    Node<type> *begin() const
    {
        return head;
    }

    // returns an iterator to the start of list
    Node<type> *last() const
    {
        return tail;
    }
};

#endif