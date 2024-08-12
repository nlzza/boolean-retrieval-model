#pragma once
#ifndef HASH_TABLE_HPP
#define HASH_TABLE_HPP

#include "HashEntry.hpp"
using BYTE = unsigned char;

class HashTable
{
    friend class Trie;

public:
    const static BYTE size = 36; // positions 0-25 are for a-z; 26-35 are for 0-9

    // Constructors
    HashTable() = default;

    HashTable(const HashTable &other)
    {
        for (BYTE i = 0; i < size; i++)
            this->entries[i].data = other.entries[i].data;
    }

    HashTable &operator=(const HashTable &other)
    {
        if (this == &other)
            return *this;

        for (BYTE i = 0; i < size; i++)
            this->entries[i].data = other.entries[i].data;
        return *this;
    }

    // Insert a character into the hash table
    HashEntry *insert(const char &symbol)
    {
        HashEntry* entry; 
        if (symbol >= 'a' && symbol <= 'z')
            entry = &entries[symbol - 97];
        else
            entry = &entries[symbol - 22];
        entry->data = symbol;
        entry->empty = false;
        return entry;
    }

    // Search for a character in the hash table
    HashEntry *search(const char &symbol)
    {
        HashEntry* entry; 
        if (symbol >= 'a' && symbol <= 'z')
            entry = &entries[symbol - 97];
        else
            entry = &entries[symbol - 22];
        if (entry->empty)
            return nullptr;
        return entry;
    }

private:
    HashEntry entries[size];
};

#endif