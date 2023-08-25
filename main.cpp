#include <iostream>
#include "search.h"

int main(int argc, char *argv[])
{
    if (argc == 3)
    {
        search(argv[1], argv[2]);
    }
    else
    {
        std::cout << "Invalid use. Please use like this: search path filename\n";
    }
    return 0;
}

