/*
 * Decompression application using static Huffman coding
 *
 * Usage: HuffmanDecompress InputFile OutputFile
 * This decompresses files generated by the "HuffmanCompress" application.
 *
 * Copyright (c) Project Nayuki
 *
 * https://www.nayuki.io/page/reference-huffman-coding
 * https://github.com/nayuki/Reference-Huffman-coding
 */

#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <vector>
#include "BitIoStream.hpp"
#include "CanonicalCode.hpp"
#include "FrequencyTable.hpp"
#include "HuffmanCoder.hpp"
#include "math.h"

using std::uint32_t;


int main(int argc, char *argv[]) {
    // Handle command line arguments
    /*if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " InputFile OutputFile" << std::endl;
        return EXIT_FAILURE;
    }
    const char *inputFile  = "/home/dvir/CLionProjects/huffman 3/binaryCoded";
    const char outputFile = "/home/dvir/CLionProjects/huffman 3/binaryDecoded";*/

    const char *inputFile  = "/home/moshse/CLionProjects/Huffmam/binarySerach";
    const char *outputFile = "/home/moshse/CLionProjects/Huffmam/binarySerachV2.out";

    // Perform file decompression
    std::ifstream in(inputFile, std::ios::binary);
    std::ofstream out(outputFile, std::ios::binary);
    BitInputStream bin(in);
    try {

        // Read code length table
        std::vector<uint32_t> codeLengths;
        for (int i = 0; i < 321; i++) {
            // For this file format, we read 8 bits in big endian
            uint32_t val = 0;
            for (int j = 0; j < 8; j++)
                val = (val << 1) | bin.readNoEof();
            codeLengths.push_back(val);
           // std::cout << std::endl;
        }
        const CanonicalCode canonCode(codeLengths);
        const CodeTree code = canonCode.toCodeTree();

        HuffmanDecoder dec(bin);
        dec.codeTree = &code;
        while (true) {
            uint32_t symbol = dec.read();
            if (symbol == 256)  // EOF symbol
                break;
            int b = static_cast<int>(symbol);
            std::cout << b << " ";
         /*   if (std::numeric_limits<char>::is_signed)
                b -= (b >> 7) << 8;
            std::cout << b << std::endl;*/
            if (b < 257){
                out.put(static_cast<char>(b));
            }
            else {
                int dif = b - 256;
                int sum = pow(2,dif);
                for (int i = 0 ; i < sum ; i ++)
                    out.put(0);
            }

        }
        return EXIT_SUCCESS;

    } catch (const char *msg) {
        std::cerr << msg << std::endl;
        return EXIT_FAILURE;
    }
}