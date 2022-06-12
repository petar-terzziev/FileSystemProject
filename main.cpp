#include <iostream>
#include "include/FileSystem.h"
#include "include/FileAPI.h"
#include "include/FileCLI.h"
using namespace std;

int main()
{

    FileSystem MyFirstFS;
    FileReader::read(0, MyFirstFS);
    FileCLI MyFirstFCLI(MyFirstFS);
    MyFirstFCLI.run();

    return 0;
}
