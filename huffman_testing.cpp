#include <sstream>
#include "gtest/gtest.h"
#include "huffman/huffman.hpp"
#include <random>
#include <iostream>
#include <iomanip>

void test_compress_decompress(const std::string &str) {
    std::istringstream source(str, std::ios::in | std::ios::binary);
    std::stringstream compressed("", std::ios::in | std::ios::out | std::ios::binary);
    std::ostringstream decompressed("", std::ios::out | std::ios::binary);

    huffman::compress(source, compressed);
    compressed.seekg(0);
    huffman::decompress(compressed, decompressed);

    ASSERT_EQ(str, decompressed.str());
}

std::string gen_random(size_t len) {
    std::mt19937_64 gen(0x19337);
    std::uniform_int_distribution<char> dist(std::numeric_limits<char>::min(), std::numeric_limits<char>::max());

    std::string res = "";
    for (size_t i = 0; i < len; i++)
        res.push_back(dist(gen));

    return res;
}

TEST(compress_decompress, empty) {
    test_compress_decompress("");
}

TEST(compress_decompress, single_character) {
    test_compress_decompress("A");
}

TEST(compress_decompress, zerobyte) {
    test_compress_decompress({'\0'});
}

TEST(compress_decompress, simple_string) {
    test_compress_decompress("Hello World");
}

TEST(compress_decompress, one_symbol_type) {
    test_compress_decompress("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
}

TEST(compress_decompress, random_5) {
    test_compress_decompress(gen_random(5));
}

TEST(compress_decompress, random_10k) {
    test_compress_decompress(gen_random(1e4));
}

TEST(compress_decompress, random_10m) {
    test_compress_decompress(gen_random(1e7));
}

void test_throw(const std::string& str) {
    std::istringstream compressed(str, std::ios::in | std::ios::binary);
    std::ostringstream decompressed("", std::ios::out | std::ios::binary);

    EXPECT_ANY_THROW(huffman::decompress(compressed, decompressed));
}

TEST(decoder_exceptions, empty) {
    test_throw("");
}

TEST(decoder_exceptions, bad_table_size) {
    test_throw("\x01\x00\x00\x00\x00\x00\x00\x00");
}

TEST(decoder_exceptions, no_table) {
    test_throw("\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00");
}

TEST(decoder_exceptions, eof_trash) {
    test_throw("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xCA\xFE\xBA\xBE");
}

TEST(decoder_exceptions, table_repeat) {
    test_throw("\x02\x00\x00\x00\x00\x00\x00\x00A\x02\x00\x00\x00\x00\x00\x00\x00A\x01\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00");
}

TEST(decoder_exceptions, not_enough_symbols) {
    test_throw("\x02\x00\x00\x00\x00\x00\x00\x00A\x02\x00\x00\x00\x00\x00\x00\x00B\x01\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00");
}
