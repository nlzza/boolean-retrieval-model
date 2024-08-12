#include <iostream>
#include <vector>
#include "Indexer/Tries/Trie.hpp"
using namespace std;

int main(void)
{
    Trie t;
    auto res = t.AND("cricket", "captain");
    for (auto& r: res)
        cout << r << " ";
    return 0;
}