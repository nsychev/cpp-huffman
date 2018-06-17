#ifndef HUFFMAN_LIB
#define HUFFMAN_LIB

#include <istream>
#include <ostream>
#include "tree.hpp"
#include <array>
#include <vector>
#include <cstddef>

namespace huffman {
    static const int BUFFER_SIZE = 8192;

    template<typename T>
    void read_raw(std::istream&, T&);

    void compress(std::istream&, std::ostream&);
    void decompress(std::istream&, std::ostream&);
}

#endif // HUFFMAN_LIB
