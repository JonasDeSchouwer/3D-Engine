#ifndef TESTS_H_INCLUDED
#define TESTS_H_INCLUDED

#include <fstream>

namespace Tests
{
    void runObjectTests();
    void testLoadSave();
}

bool compareFiles(std::ifstream &f1, std::ifstream &f2);

#endif // TESTS_H_INCLUDED
