#include "Indexer/Indexer.hpp"
#include <iostream>
#define TOTAL (30)
using namespace std;

int main()
{
    unsigned id;
    Indexer indexer;

    for (int id = 1; id <= TOTAL; id++) // Index files one by one
    {
        string filename = to_string(id) + ".txt";
        indexer.index(("../Dataset/" + filename).c_str(), id);
    }
    indexer.write_on("index.txt");
    fflush(stdin);
    system("pause");
    return 0;
}
