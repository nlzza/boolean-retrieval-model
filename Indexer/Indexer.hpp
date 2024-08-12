#pragma once
#ifndef INDEXER_HPP
#define INDEXER_HPP
#define TOTAL_DOCS (30)

#include "Tries/Trie.hpp"
#include <cmath>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <map>

class Indexer
{
    std::vector<std::string> stopwords;

private:
    char c1;
    unsigned pos{0};
    HashEntry *target{0};

    Trie dictionary;

    bool binary_search(std::vector<std::string>::iterator it, const int size, const std::string& s)
    {
        if (it[size/2] == s)
            return true;
        else if (size == 1)
            return false;
        else if (it[size/2] > s)
            return binary_search(it, size/2, s);
        else
            return binary_search(it + size/2, (size/2) + (size % 2), s);
    }

    // Returns true if the word is a stopword; else returns false
    bool is_stopword(const std::string& word)
    {
        return binary_search(stopwords.begin(), stopwords.size(), word);
    }

    // Reads stopwords from "Stopword-List.txt" into the vector
    void load_stopwords()
    {
        std::ifstream file;
        file.open("../Stopword List.txt", std::ios::in);
        std::string s;
        while (getline(file, s))
            stopwords.push_back(s);
        file.close();
        sort(stopwords.begin(), stopwords.end());
    }

    std::string& stem(std::string& word)
    {
        // Prefix removal (tested on "ultraaggressive")
        std::string prefixes[] = {"kilo", "micro", "milli", "intra", "ultra",
                                  "mega", "nano", "pico", "pseudo"}; // Only those given in slides are included
        int wordlen = word.length();
        for (short i = 0; i < 9; i++)
        {
            const int len = prefixes[i].length();
            if (wordlen <= len)
                continue;
            if (prefixes[i] == word.substr(0, len))
            {
                word.erase(0, len);
                break;
            }
        }
        // Suffix removal step 1
        // Remove "es" from words ending in "sses" (tested on "dismisses")
        wordlen = word.length();
        if (wordlen > 4 && word.substr(wordlen - 4, 4) == "sses")
            word.erase(wordlen - 2, 2);
        
        // Remove last "s" from words ending in s whose second last letter is not s (tested on "runs")
        wordlen = word.length();
        if (word[wordlen - 1] == 's' && word[wordlen - 2] != 's')
            word.erase(wordlen - 1, 1);
        
        // If word has a vowel and ends in "eed", remove "ed" (tested on "agreed")
        wordlen = word.length();
        if (wordlen > 3)
        {
            bool has_vowel = false;
            for (short i = 0; i < wordlen - 3; i++)
            {
                if (word[i] == 'a' || word[i] == 'e' || word[i] == 'i' || word[i] == 'o' || word[i] == 'u')
                {
                    has_vowel = true;
                    break;
                }
            }
            if (has_vowel && word.substr(wordlen - 3, 3) == "eed")
                word.erase(wordlen - 2, 2);
        }

        // If word has a vowel and ends in "ed", remove "d" (tested on "chased")
        wordlen = word.length();
        if (wordlen > 2)
        {
            bool has_vowel = false;
            for (short i = 0; i < wordlen - 2; i++)
            {
                if (word[i] == 'a' || word[i] == 'e' || word[i] == 'i' || word[i] == 'o' || word[i] == 'u')
                {
                    has_vowel = true;
                    break;
                }
            }
            if (has_vowel && word.substr(wordlen - 2, 2) == "ed")
                word.erase(wordlen - 1, 1);
        }

        // Replace trailing y with an i (tested on "briefly")
        wordlen = word.length();
        if (wordlen > 1 && word[wordlen - 1] == 'y')
            word[wordlen - 1] = 'i';

        // Suffix removal step 2
        // Ran out of time. Sorry!
        return word;
    }

    bool is_operator(const std::string& s)
    {
        return s == "not" || s == "and" || s == "or";
    }

public:

    // Constructor
    Indexer()
    {
        load_stopwords();
    }

    void index(const char *filename, const unsigned &doc_ID = 0)
    {
        std::string word;
        pos = 0;
        FILE *file = fopen(filename, "r");
        while ((c1 = fgetc(file)) != EOF)
        {
            if ((c1 >= 'a' && c1 <= 'z') || (c1 >= '0' && c1 <= '9'))
                word.push_back(c1);
            else if (c1 >= 'A' && c1 <= 'Z')
            {
                c1 |= 32; // case folding
                word.push_back(c1);
            }
            else if (c1 == ' ' || c1 == '\n')
            {
                if (word.length() && !is_stopword(word))
                {
                    target = dictionary.insert(stem(word));
                    if (target->posting)
                        target->posting->push_directly(doc_ID, pos);
                    else
                        target->posting = new Posting(doc_ID, pos);
                }
                word.clear();
                pos++;
            }
        }
        // Dont forget to index the last word!
        if (word.length() && !is_stopword(word))
        {
            target = dictionary.insert(stem(word));
            if (target->posting)
                target->posting->push_directly(doc_ID, pos);
            else
                target->posting = new Posting(doc_ID, pos);
        }
        fclose(file);
    }

    void write_on(const char *filename)
    {
        std::ofstream file;
        file.open(filename, std::ios::out);
        dictionary.write(file);
        file.close();
    }

    void read(const char *filename)
    {
        std::string token;
        unsigned doc_count{0};
        unsigned doc_ID{0};
        unsigned term_freq{0};
        unsigned pos{0};
        HashEntry *target;

        dictionary.deleteTrie();

        std::ifstream file;
        file.open(filename, std::ios::in);
        while (!file.eof())
        {
            file >> token;
            if (file.eof())
                break;

            file >> doc_count;
            if (file.eof())
                break;

            for (unsigned i = 0; i < doc_count; i++)
            {
                file >> doc_ID;
                if (file.eof())
                    break;
                file >> term_freq;
                if (file.eof())
                    break;

                for (unsigned j = 0; j < term_freq; j++)
                {
                    file >> pos;
                    if (file.eof())
                        break;

                    target = dictionary.insert(token);
                    if (target->posting)
                        target->posting->push_directly(doc_ID, pos);
                    else
                        target->posting = new Posting(doc_ID, pos);
                }
            }
        }
        file.close();
    }

    HashEntry *search(const std::string &token)
    {
        return dictionary.search(token);
    }

    std::pair<std::vector<unsigned>, bool> query_eval(std::vector<std::string> query)
    {
        // Vector is the result containg IDs of all docs that satisfy query
        // Bool will be false if query is incorrect
        // Remember query is in postfix form

        const int len = query.size();
        std::vector<bool> done_or_not(len, false); // keeps track of what's been done
        std::map<std::string, std::vector<unsigned>> m;
        std::vector<unsigned> result;

        if (len == 1)
        {
            auto h = dictionary.search(query[0]);
            if (h)
            {
                auto p = h->posting->documents.begin();
                while (p)
                {
                    result.push_back(p->data.ID);
                    p = p->next;
                }
            }
            return std::pair<std::vector<unsigned>, bool> (result, true);
        }

        for (int i = 0; i < len; i++)
        {
            if (query[i] == "not")
            {
                // Find the term to NOT
                int j = i - 1;
                for (;j >= 0; j--)
                {
                    if (!is_operator(query[j]))
                        break;
                }
                if (j == -1) // Incorrect query
                    return std::pair<std::vector<unsigned>, bool> (result, false);
                if (done_or_not[j] == false)
                {
                    done_or_not[j] = true;
                    m[query[j]] = dictionary.NOT(query[j]);
                    result = m[query[j]];
                }
                else
                {
                    m[query[j]] = dictionary.NOT(m[query[j]]);
                    result = m[query[j]];
                }
            }
            else if (query[i] == "and")
            {
                // Find the terms to AND
                int j = i - 1;
                for (;j >= 0; j--)
                {
                    if (!is_operator(query[j]))
                        break;
                }
                if (j == -1) // Incorrect query
                    return std::pair<std::vector<unsigned>, bool> (result, false);
                int k = j - 1;
                for (;k >= 0; k--)
                {
                    if (!is_operator(query[k]))
                        break;
                }
                if (k == -1) // Incorrect query
                    return std::pair<std::vector<unsigned>, bool> (result, false);
                
                if (done_or_not[j] == false && done_or_not[k] == false)
                {
                    done_or_not[j] = true;
                    done_or_not[k] = true;
                    m[query[j]] = dictionary.AND(query[j], query[k]);
                    m[query[k]] = m[query[j]];
                    result = m[query[j]];
                }
                else if (done_or_not[j] == true && done_or_not[k] == false)
                {
                    done_or_not[k] = true;
                    m[query[j]] = dictionary.AND(query[k], m[query[j]]);
                    m[query[k]] = m[query[j]];
                    result = m[query[j]];
                }
                else if (done_or_not[j] == false && done_or_not[k] == true)
                {
                    done_or_not[j] = true;
                    m[query[j]] = dictionary.AND(query[j], m[query[k]]);
                    m[query[k]] = m[query[j]];
                    result = m[query[j]];
                }
                else
                {
                    m[query[j]] = dictionary.AND(m[query[j]], m[query[k]]);
                    m[query[k]] = m[query[j]];
                    result = m[query[j]];
                }
            }
            else if (query[i] == "or")
            {
                // Find the terms to OR
                int j = i - 1;
                for (;j >= 0; j--)
                {
                    if (!is_operator(query[j]))
                        break;
                }
                if (j == -1) // Incorrect query
                    return std::pair<std::vector<unsigned>, bool> (result, false);
                int k = j - 1;
                for (;k >= 0; k--)
                {
                    if (!is_operator(query[k]))
                        break;
                }
                if (k == -1) // Incorrect query
                    return std::pair<std::vector<unsigned>, bool> (result, false);
                if (done_or_not[j] == false && done_or_not[k] == false)
                {
                    done_or_not[j] = true;
                    done_or_not[k] = true;
                    m[query[j]] = dictionary.OR(query[j], query[k]);
                    m[query[k]] = m[query[j]];
                    result = m[query[j]];
                }
                else if (done_or_not[j] == true && done_or_not[k] == false)
                {
                    done_or_not[k] = true;
                    m[query[j]] = dictionary.OR(query[k], m[query[j]]);
                    m[query[k]] = m[query[j]];
                    result = m[query[j]];
                }
                else if (done_or_not[j] == false && done_or_not[k] == true)
                {
                    done_or_not[j] = true;
                    m[query[j]] = dictionary.OR(query[j], m[query[k]]);
                    m[query[k]] = m[query[j]];
                    result = m[query[j]];
                }
                else
                {
                    m[query[j]] = dictionary.OR(m[query[j]], m[query[k]]);
                    m[query[k]] = m[query[j]];
                    result = m[query[j]];
                }
            }
        }
        return std::pair<std::vector<unsigned>, bool> (result, true);
    }
};
#endif