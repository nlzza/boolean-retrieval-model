#pragma once
#ifndef TRIE_HPP
#define TRIE_HPP
#define TOTAL_DOCS (30)

#include "HashTable.hpp"
#include <queue>
#include <vector>
#include <ostream>
#include <numeric>

class Trie
{
public:
    using Results = std::vector<std::pair<std::string, Posting *>>;

    // Constructor and deconstructor
    Trie()
    {
        root = new HashTable;
    }
    ~Trie()
    {
        deleteTrie();
        delete root;
    }
    void deleteTrie();
    HashEntry *search(const std::string& prefix);

    std::vector<unsigned> AND(const std::string& s1, const std::string& s2);
    std::vector<unsigned> OR(const std::string& s1, const std::string& s2);
    std::vector<unsigned> NOT(const std::string& s);

    // Overloaded functions in case a list of docs is provided
    std::vector<unsigned> AND(const std::string& s, const std::vector<unsigned> v);
    std::vector<unsigned> OR(const std::string& s, const std::vector<unsigned> v);
    std::vector<unsigned> NOT(const std::vector<unsigned> v);

    // Overloaded functions in case 2 lists of docs are provided
    std::vector<unsigned> AND(std::vector<unsigned> v1, std::vector<unsigned> v2);
    std::vector<unsigned> OR(std::vector<unsigned> v1, std::vector<unsigned> v2);

    HashEntry *insert(std::string& prefix)
    {
        HashTable *ptr = root;
        HashEntry *target;
        const auto &length = prefix.length();

        for (unsigned i = 0; i < length; i++)
        {
            // insert in current table
            target = ptr->search(prefix[i]);
            if (target == nullptr) // character not found
                target = ptr->insert(prefix[i]);

            if (i == length - 1)
            {
                target->endOfWord = true;
                break;
            }
            // move on to next table
            if (target->next_table == nullptr)
                target->next_table = new HashTable;
            ptr = target->next_table;
        }
        return target;
    }

    void write(std::ostream &buffer)
    {
        std::string prefix;
        writeUtil(root, prefix, buffer);
    }

private:
    HashTable *root{0};
    void writeUtil(HashTable *ptr, std::string &prefix, std::ostream &buffer);
};

// finds the given std::string
// returns nullptr if not found
HashEntry *Trie::search(const std::string &prefix)
{
    HashTable *ptr = root;
    HashEntry *target;
    const auto &length = prefix.length();

    for (unsigned i = 0; i < length; i++)
    {
        // search in current table
        target = ptr->search(prefix[i]);
        if (target == nullptr)
            break;

        if (i == length - 1)
        {
            if (target->endOfWord == true)
                return target;
            break;
        }
        // move on to next table
        ptr = target->next_table;
        if (ptr == nullptr) // if next table does not exist
            break;
    }
    return nullptr;
}

// Writes trie to file
void Trie::writeUtil(HashTable *ptr, std::string &prefix, std::ostream &buffer)
{
    for (HashEntry &beg : ptr->entries)
    {
        if (beg.empty == true)
            continue;

        prefix.push_back(beg.data);
        if (beg.endOfWord)
        {
            buffer << prefix
                   << " "
                   << beg.posting->doc_count
                   << " ";

            for (auto doc = beg.posting->documents.begin(); doc != nullptr; doc = doc->next)
            {
                buffer << doc->data.ID
                       << " "
                       << doc->data.term_freq;

                for (auto pos = doc->data.positions.begin(); pos != nullptr; pos = pos->next)
                    buffer << " "
                           << pos->data;
                buffer << " ";
            }
            buffer << "\n";

            if (!beg.next_table)
            {
                prefix.pop_back();
                continue;
            }
        }
        if (beg.next_table)
        {
            writeUtil(beg.next_table, prefix, buffer);
            prefix.pop_back();
        }
    }
}

std::vector<unsigned> Trie::AND(const std::string& s1, const std::string& s2)
{
    std::vector<unsigned> results;

    HashEntry* h1 = search(s1);
    if (h1 == nullptr)
        return results;
    HashEntry* h2 = search(s2);
    if (h2 == nullptr)
        return results;

    auto p1 = h1->posting->documents.begin();
    auto p2 = h2->posting->documents.begin();

    while (p1 && p2)
    {
        if (p1->data.ID == p2->data.ID)
        {
            results.push_back(p1->data.ID);
            p1 = p1->next;
            p2 = p2->next;
        }
        else if (p1->data.ID < p2->data.ID)
            p1 = p1->next;
        else
            p2 = p2->next;
    }
    return results;
}

std::vector<unsigned> Trie::AND(const std::string& s, const std::vector<unsigned> v)
{
    std::vector<unsigned> results;

    if (v.empty())
        return results;
    HashEntry* h = search(s);
    if (h == nullptr)
        return results;

    auto p1 = h->posting->documents.begin();
    auto p2 = v.begin();

    while (p1 && p2 != v.end())
    {
        if (p1->data.ID == *p2)
        {
            results.push_back(p1->data.ID);
            p1 = p1->next;
            p2++;
        }
        else if (p1->data.ID < *p2)
            p1 = p1->next;
        else
            p2++;
    }
    return results;
}

std::vector<unsigned> Trie::AND(const std::vector<unsigned> v1, const std::vector<unsigned> v2)
{
    std::vector<unsigned> results;

    if (v1.empty() || v2.empty())
        return results;

    auto p1 = v1.begin();
    auto p2 = v2.begin();

    while (p1 != v1.end() && p2 != v2.end())
    {
        if (*p1 == *p2)
        {
            results.push_back(*p1);
            p1++;
            p2++;
        }
        else if (*p1 < *p2)
            p1++;
        else
            p2++;
    }
    return results;
}

std::vector<unsigned> Trie::OR(const std::string& s1, const std::string& s2)
{
    std::vector<unsigned> results;

    HashEntry* h1 = search(s1);
    HashEntry* h2 = search(s2);
    
    if (h1 == nullptr && h2 == nullptr)
        return results;
    if (h1 == nullptr)
    {
        auto it = h2->posting->documents.begin();
        while (it != h2->posting->documents.last())
        {
            results.push_back((*it).data.ID);
            it = it->next;
        }
        return results;
    }
    if (h2 == nullptr)
    {
        auto it = h1->posting->documents.begin();
        while (it != h1->posting->documents.last())
        {
            results.push_back((*it).data.ID);
            it = it->next;
        }
        return results;
    }

    auto p1 = h1->posting->documents.begin();
    auto p2 = h2->posting->documents.begin();

    while (p1 || p2)
    {
        if (!p1)
        {
            auto it = p2;
            while (it)
            {
                results.push_back(it->data.ID);
                it = it->next;
            }
            break;
        }
        if (!p2)
        {
            auto it = p1;
            while (it)
            {
                results.push_back(it->data.ID);
                it = it->next;
            }
            break;
        }
        if (p1->data.ID == p2->data.ID)
        {
            results.push_back(p1->data.ID);
            p1 = p1->next;
            p2 = p2->next;
        }
        else if (p1->data.ID < p2->data.ID)
        {
            results.push_back(p1->data.ID);
            p1 = p1->next;
        }
        else
        {
            results.push_back(p2->data.ID);
            p2 = p2->next;
        }
    }
    return results;
}

std::vector<unsigned> Trie::OR(const std::string& s, const std::vector<unsigned> v)
{
    std::vector<unsigned> results;
    HashEntry* h = search(s);
    
    if (h == nullptr && v.empty())
        return results;
    if (h == nullptr)
        return v;
    if (v.empty())
    {
        auto it = h->posting->documents.begin();
        while (it != h->posting->documents.last())
        {
            results.push_back(it->data.ID);
            it = it->next;
        }
        return results;
    }

    auto p1 = h->posting->documents.begin();
    auto p2 = v.begin();

    while (p1 || p2 != v.end())
    {
        if (!p1)
        {
            results.insert(results.end(), p2, v.end());
            break;
        }
        if (p2 == v.end())
        {
            auto it = p1;
            while (it)
            {
                results.push_back(it->data.ID);
                it = it->next;
            }
            break;
        }
        if (p1->data.ID == *p2)
        {
            results.push_back(p1->data.ID);
            p1 = p1->next;
            p2++;
        }
        else if (p1->data.ID < *p2)
        {
            results.push_back(p1->data.ID);
            p1 = p1->next;
        }
        else
        {
            results.push_back(*p2);
            p2++;
        }
    }
    return results;
}

std::vector<unsigned> Trie::OR(const std::vector<unsigned> v1, const std::vector<unsigned> v2)
{
    std::vector<unsigned> results;
    
    if (v1.empty() && v2.empty())
        return results;
    if (v1.empty())
        return v2;
    if (v2.empty())
        return v1;

    auto p1 = v1.begin();
    auto p2 = v2.begin();

    while (p1 != v1.end() || p2 != v2.end())
    {
        if (p1 == v1.end())
        {
            results.insert(results.end(), p2, v2.end());
            break;
        }
        if (p2 == v2.end())
        {
            results.insert(results.end(), p1, v1.end());
            break;
        }
        if (*p1 == *p2)
        {
            results.push_back(*p1);
            p1++;
            p2++;
        }
        else if (*p1 < *p2)
        {
            results.push_back(*p1);
            p1++;
        }
        else
        {
            results.push_back(*p2);
            p2++;
        }
    }
    return results;
}

std::vector<unsigned> Trie::NOT(const std::string& s)
{
    std::vector<unsigned> results(TOTAL_DOCS);
    iota(results.begin(), results.end(), 1); // vector contains all doc IDs

    HashEntry* h = search(s);
    if (h == nullptr)
        return results;
    
    auto p = h->posting->documents.begin();
    unsigned short j = 0;

    while (p)
    {
        results.erase(results.begin() + p->data.ID - 1 - j);
        j++;
        p = p->next;
    }
    return results;
}

std::vector<unsigned> Trie::NOT(const std::vector<unsigned> v)
{
    std::vector<unsigned> results(TOTAL_DOCS);
    iota(results.begin(), results.end(), 1); // vector contains all doc IDs

    if (v.empty())
        return results;
    
    auto p = v.begin();
    unsigned short j = 0;

    while (p != v.end())
    {
        results.erase(results.begin() + *p - 1 - j);
        j++;
        p++;
    }
    return results;
}

// Deletes the trie
// The method is similar to BFS
void Trie::deleteTrie()
{
    std::queue<HashTable *> q;
    q.push(root);

    while (!q.empty())
    {
        HashTable *f = q.front();
        q.pop();

        for (HashEntry &beg : f->entries)
        {
            if (beg.next_table)
                q.push(beg.next_table);
        }
        delete f;
    }
    root = new HashTable;
}

#endif