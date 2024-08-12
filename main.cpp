#include <iostream>
#include <vector>
#include "Indexer/Indexer.hpp"
using namespace std;

// Proximity queries not implemented. SORRY!

// Returns a number denoting operator precedence
inline int precedence(string& op)
{
    if (op == "not")
        return 2;
    else if (op == "and")
        return 1;
    else if (op == "or")
        return 0;
    else
        return -1;
}

// Ensures that for every opening bracket, there is a closing bracket too
bool bracket_check(const string& s)
{
    vector<char> stack;
    for (const char& i: s)
    {
        if (i == '(')
            stack.push_back(i);
        else if (i == ')')
        {
            if (!stack.size())
                return false;
            stack.pop_back();
        }
    }
    return stack.empty();
}

// Removes excess NOTs since two consecutive NOTs cancel one another
void remove_excess_nots(vector<string>& v)
{
    const int len = v.size();
    unsigned j = 0;
    for (unsigned i = 0; i < len - 1 - j;)
    {
        if (v[i] == v[i + 1] && v[i] == "not")
        {
            v.erase(v.begin() + i, v.begin() + i + 2); // Remvove both NOTs
            j += 2;
            continue;
        }
        i++;
    }
}

// Insert word
bool insert_word(string& word, vector<string>& stack, vector<string>& postfix, short& a)
{
    if (word == "not") // operator
    {
        stack.push_back(word);
        word.clear();
        a = 2;
    }
    else if (word == "and" || word == "or")
    {
        if (a == 2) // 2 operators cannot appear consecutively unless second one is NOT
            return false;
        while (!stack.empty() && precedence(word) <= precedence(stack.back()))
        {
            postfix.push_back(stack.back());
            stack.pop_back();
        }
        stack.push_back(word);
        word.clear();
        a = 2;
    }
    else if (word.length()) // term
    {
        if (a == 1) // 2 terms cannot appear consecutively
            return false;
        postfix.push_back(word);
        word.clear();
        a = 1;
    }
    return true;
}

// Insert last word. Last word can never be an operator
bool insert_last_word(string& word, vector<string>& stack, vector<string>& postfix, short& a)
{
    if (word == "not" || word == "and" || word == "or") // operator
        return false;
    else if (word.length()) // term
    {
        if (a == 1) // 2 terms cannot appear consecutively
            return false;
        postfix.push_back(word);
        word.clear();
        a = 1;
    }
    return true;
}

int main()
{
    cout << "Reading index...\n" << endl;
    Indexer indexer;
    indexer.read("index.txt");

    cout << "The query operators (AND, OR, NOT) must be fully capitalised." << endl;
    cout << "Enter a query: ";
    string query;
    getline(cin, query);
    
    vector<string> postfix, stack;
    const int length = query.length();

    if (!bracket_check(query))
    {
        cout << "\nIncorrect query!\n";
        return 0;
    }

    string word;
    short a = 0; // 1 if prev term was term, 2 if prev term was operator, 3 in case of /k

    for (unsigned i = 0; i < length; i++)
    {
        if ((query[i] >= 'a' && query[i] <= 'z') || (query[i] >= '0' && query[i] <= '9'))
            word.push_back(query[i]);
        else if (query[i] >= 'A' && query[i] <= 'Z')
            word.push_back(query[i] | 32); // case folding
        else if (query[i] == '(')
            stack.push_back("(");
        else if (query[i] == ')')
        {
            if (!insert_last_word(word, stack, postfix, a))
            {
                cout << "\nIncorrect query!\n";
                return 0;
            }
            while (!stack.empty() && stack.back() != "(")
            {
                postfix.push_back(stack.back());
                stack.pop_back();
            }
            stack.pop_back();
        }
        else if (query[i] == ' ')
        {
            if (!insert_word(word, stack, postfix, a))
            {
                cout << "\nIncorrect query!\n";
                return 0;
            }
        }
        else // Any other character
        {
            // That character is not part of a word so error thrown
            if (i == 0 || query[i - 1] == ' ')
            {
                cout << "\nIncorrect query!\n";
                return 0;
            }
            // Otherwise, character is part of word; however, it is still
            // not pushed in word as these characters are ignored anyway
        }
    }
    if (!insert_last_word(word, stack, postfix, a))
    {
        cout << "\nIncorrect query!\n";
        return 0;
    }
    while (!stack.empty())
    {
        postfix.push_back(stack.back());
        stack.pop_back();
    }
    remove_excess_nots(postfix);
    if (postfix.empty())
    {
        cout << "\nIncorrect query!\n";
        return 0;
    }
    auto result = indexer.query_eval(postfix);
    if (!result.second)
    {
        cout << "\nIncorrect query!\n";
        return 0;
    }
    if (result.first.empty())
        cout << "\nSorry! No results were found!\n";
    else
    {
        cout << "\nResult(s): ";
        for (const auto& i: result.first)
            cout << i << " ";
        cout << endl;
    }
    return 0;
}
