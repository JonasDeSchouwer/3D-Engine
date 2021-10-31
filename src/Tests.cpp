#include "Tests.h"
#include "Object.h"

#include <fstream>
#include <iterator>
#include <string>
#include <algorithm>
#include <assert.h>

namespace Tests
{

void runObjectTests()
{
    testLoadSave();
}
void testLoadSave()
{
    Object O;
    O.load("test_cube");
    O.save("test_cube2");

    std::string filename1 = "C:/Users/jonas/OneDrive/informatica/c++/3D engine - reboot 2/objects/test_cube/test_cube.txt";
    std::string filename2 = "C:/Users/jonas/OneDrive/informatica/c++/3D engine - reboot 2/objects/test_cube2/test_cube2.txt";

    std::ifstream f1 (filename1);
    std::ifstream f2 (filename2);

    bool eq = compareFiles(f1, f2);
    if (eq)
        {std::cout << "testLoadSave succeeded: files are equal" << std::endl;}
    else
        {std::cout << "testLoadSave failed: files are different" << std::endl;}
    assert (eq);
}

}



bool compareFiles(std::ifstream &file1, std::ifstream &file2) {

    std::ifstream &f1 = file1;
    std::ifstream &f2 = file2;

    if (f1.fail() || f2.fail()) {
    std::cout << "file problem\n";
    return false; //file problem
    }

    if (f1.tellg() != f2.tellg()) {
    std::cout << "size mismatch\n";
    return false; //size mismatch
    }

    //seek back to beginning and use std::equal to compare contents
    f1.seekg(0, std::ifstream::beg);
    f2.seekg(0, std::ifstream::beg);
    return std::equal(std::istreambuf_iterator<char>(f1.rdbuf()),
                    std::istreambuf_iterator<char>(),
                    std::istreambuf_iterator<char>(f2.rdbuf()));
}
