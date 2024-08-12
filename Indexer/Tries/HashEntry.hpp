#pragma once
#ifndef HASH_ENTRY_HPP
#define HASH_ENTRY_HPP

#include "../Extensions/Posting.hpp"

class HashTable;

struct HashEntry
{
    char data{'$'};
    bool endOfWord{0};
    bool empty{true};
    Posting *posting{0};
    HashTable *next_table{0};

    // Constructors
    HashEntry() = default;

    HashEntry(const char &data)
        : data(data) {}

    HashEntry(const HashEntry &other)
        : data(other.data) {}

    HashEntry &operator=(const HashEntry &other)
    {
        if (this != &other)
            this->data = other.data;
        return *this;
    }
};

#endif