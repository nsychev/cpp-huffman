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
    void read_raw(std::istream &input, T& value) {
        auto read = input.readsome(reinterpret_cast<char*>(&value), sizeof(value));
        if (read != sizeof(value))
            throw std::runtime_error(std::string("invalid file format: expected ") +
                                     std::to_string(sizeof(value)) + " data bytes at position " +
                                     std::to_string(input.tellg() - read));
    }

    void compress(std::istream &input, std::ostream &output) {
        std::array<size_t, 256> freq;
        freq.fill(0);

        std::array<unsigned char, BUFFER_SIZE> buf;

        size_t file_len = 0, len;

        do {
            len = static_cast<size_t>(input.readsome(reinterpret_cast<char*>(buf.begin()), BUFFER_SIZE));
            file_len += len;

            for (size_t i = 0; i < len; i++)
                ++freq[buf[i]];
        } while (len > 0);

        huffman_tree tree(freq);
        std::array<size_t, 256> code_table, len_table;
        tree.extract_to(code_table, len_table);

        size_t symbols = 0;

        for (size_t i = 0; i < 256; i++)
            if (freq[i] > 0)
                ++symbols;

        output.write(reinterpret_cast<const char *>(&symbols), sizeof(size_t));

        for (size_t i = 0; i < 256; i++) {
            if (freq[i] > 0) {
                output.put(static_cast<char>(i));
                output.write(reinterpret_cast<const char *>(&freq[i]), sizeof(size_t));
            }
        }

        output.write(reinterpret_cast<const char *>(&file_len), sizeof(size_t));

        input.seekg(0);

        size_t current_buffer = 0;
        size_t bits = 0;

        do {
            len = static_cast<size_t>(input.readsome(reinterpret_cast<char *>(buf.begin()), BUFFER_SIZE));

            for (size_t i = 0; i < len; i++) {
                size_t code = code_table[buf[i]];
                current_buffer |= code << bits;
                bits += len_table[buf[i]];
                while (bits >= 8) {
                    auto out = static_cast<char>(current_buffer & 255u);
                    output.put(out);
                    current_buffer >>= 8;
                    bits -= 8;
                }
            }
        } while (len > 0);

        if (bits > 0)
            output.put(static_cast<char>(current_buffer));
    }

    void decompress(std::istream &input, std::ostream &output) {
        size_t table_len;
        read_raw(input, table_len);

        std::array<size_t, 256> freq;
        freq.fill(0);

        for (size_t i = 0; i < table_len; i++) {
            unsigned char symbol;
            size_t count;
            read_raw(input, symbol);
            read_raw(input, count);

            if (freq[symbol] > 0)
                throw std::runtime_error("invalid file format: duplicated table records");

            if (count == 0)
                throw std::runtime_error("invalid file format: zero table record");

            freq[symbol] = count;
        }

        huffman_tree tree(freq);

        size_t text_len;
        read_raw(input, text_len);

        if (table_len == 0 && text_len > 0) {
            std::runtime_error("invalid file format: frequency table is empty");
        }

        size_t bits = 0;
        unsigned char current_char = 0;

        huffman_tree::node* current_node = tree.get_root();

        for (size_t i = 0; i < text_len; i++) {
            while (!current_node->is_leaf()) {
                if (bits == 0) {
                    read_raw(input, current_char);
                    bits = 8;
                }
                bool bit = static_cast<bool>(current_char & 1);

                current_char >>= 1;
                --bits;

                if (bit)
                    current_node = dynamic_cast<huffman_tree::inner_node*>(current_node)->get_right();
                else
                    current_node = dynamic_cast<huffman_tree::inner_node*>(current_node)->get_left();
            }

            output.put(dynamic_cast<huffman_tree::leaf*>(current_node)->get_symbol());

            current_node = tree.get_root();
        }

        if (current_char != 0) {
            throw std::runtime_error("invalid file format: last byte is corrupted with extra data");
        }

        if (input.peek() != std::char_traits<char>::eof()) {
            throw std::runtime_error("invalid file format: expected EOF, found excess bytes");
        }
    }
}

#endif // HUFFMAN_LIB