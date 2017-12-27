#include <iostream>
#include <string>

#include "huffman.hpp"

using namespace std;

const auto usage = "./huffman [encode|decode] [filename]"s;


int main(int argc, char *argv[]) {
    ios::sync_with_stdio(false);

    if (argc != 3) {
        cout << usage << endl;
        return EXIT_FAILURE;
    }

    if (argv[1] == "encode"s) {
        encode(argv[2]);
    }
    else if (argv[1] == "decode"s) {
        decode(argv[2]);
    }
    else {
        cout << usage << endl;
        return EXIT_FAILURE;
    }


    return EXIT_SUCCESS;
}