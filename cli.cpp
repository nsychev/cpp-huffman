#include "lib/huffman.hpp"
#include <iostream>
#include <fstream>
#include <ctime>
#include <iomanip>

enum huffman_mode_t {
    COMPRESS, DECOMPRESS
};

void print_usage() {
    std::cerr << "\033[1;32mhuffman\033[0m - something about data compression\n\n"
              << "Usage: huffman [mode <input file> [<output file>]]\n\n"
              << "Available modes: \n"
              << "  -c, --compress\n"
              << "  -d, --decompress\n"
              << "  -h, --help (default)\n\n"
              << "If only output file is not specified, output is written to <input file>.out." << std::endl;
}

int main(int argc, char *argv[]) {
    if (argc <= 1 || argc >= 5) {
        print_usage();
        return 1;
    }

    std::string mode_arg = argv[1];
    huffman_mode_t mode;

    if (mode_arg == "-h" || mode_arg == "--help") {
        print_usage();
        return 1;
    } else if (mode_arg == "-c" || mode_arg == "--compress") {
        mode = COMPRESS;
    } else if (mode_arg == "-d" || mode_arg == "--decompress") {
        mode = DECOMPRESS;
    } else {
        std::cerr << "Unknown option: " << mode_arg << std::endl;
        return 1;
    }

    if (argc == 2) {
        std::cerr << "Input file is not specified" << std::endl;
        return 1;
    }

    std::string input_path = argv[2];
    std::string output_path;

    if (argc == 4) {
        output_path = argv[3];

        if (input_path == output_path) {
            std::cerr << "Please specify differerent input and output files" << std::endl;
            return 2;
        }
    } else {
        output_path = input_path + ".out";
    }

    try {
        std::ifstream input_file(input_path, std::ios::in | std::ios::binary);
        if (input_file.fail())
            throw std::runtime_error("cannot open " + input_path);

        std::ofstream output_file(output_path, std::ios::out | std::ios::binary);
        if (output_file.fail())
            throw std::runtime_error("cannot open " + output_path);

        clock_t begin = clock();

        if (mode == COMPRESS) {
            std::cerr << "Starting compressing.." << std::endl;
            huffman::compress(in, out);
        } else {
            std::cerr << "Starting decompressing.." << std::endl;
            huffman::decompress(in, out);
        }

        clock_t end = clock();

        std::cerr << "\n\033[1;32mFinished\033[0m in "
                  << std::fixed << std::setprecision(3) << double(end - begin) / CLOCKS_PER_SEC
                  << " s" << std::endl;

        return 0;
    } catch (std::exception &e) {
        std::cerr << "\033[1;31m[ERROR]\033[0m " << e.what() << std::endl;
        return 2;
    }
}