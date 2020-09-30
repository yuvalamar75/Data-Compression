/* 
 * Compression application using static Huffman coding
 * 
 * Usage: HuffmanCompress InputFile OutputFile
 * Then use the corresponding "HuffmanDecompress" application to recreate the original input file.
 * Note that the application uses an alphabet of 257 symbols - 256 symbols for the byte values
 * and 1 symbol for the EOF marker. The compressed file format starts with a list of 257
 * code lengths, treated as a canonical code, and then followed by the Huffman-coded data.
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
#include <stdexcept>
#include <vector>
#include "BitIoStream.hpp"
#include "CanonicalCode.hpp"
#include "FrequencyTable.hpp"
#include "HuffmanCoder.hpp"
#include "math.h"

using std::uint32_t;

std :: vector<uint32_t> breakNum(int counter){
    std :: vector<uint32_t> nums;
    while(counter != 0){
        unsigned int num = log2(counter);
        counter = counter - pow(2, num);
        nums.push_back(num);
    }
    return nums;
}


int main(int argc, char *argv[]) {
    // Handle command line arguments

  /*  if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " InputFile OutputFile" << std::endl;
        return EXIT_FAILURE;
    }
    const char *inputFile  = argv[1];
    const char *outputFile = argv[2];*/

    const char *inputFile  = "/home/moshse/CLionProjects/Huffmam/BST.out";
    const char *outputFile = "/home/moshse/CLionProjects/Huffmam/BST";

    // Read input file once to compute symbol frequencies.
    // The resulting generated code is optimal for static Huffman coding and also canonical.
    std::ifstream in(inputFile, std::ios::binary);
    FrequencyTable freqs(std::vector<uint32_t>(322, 0)); // add 2^32 0 00 0000 00000000 ..
    uint32_t null_counter  = 0;
    std::string[] list_of_strings = {"_ZStlsISt11char_traitsIcEERSt13basic_ostreamIcT_ES5_PK"};
    while (true) {
        int b = in.get();
        if (b == EOF)
            break;
        if (b < 0 || b > 255)
            throw std::logic_error("Assertion error");
        if (b == 0) {
            null_counter++;
        }
        else {
            if (null_counter == 0)
                freqs.increment(static_cast<uint32_t>(b));
            else if(null_counter == 1) {
                freqs.increment(static_cast<uint32_t>(0));
                freqs.increment(static_cast<uint32_t>(b)); //add breaking char
                null_counter = 0;
            }
            else {
                freqs.increment(static_cast<uint32_t>(b)); //add breaking char
                std::vector<uint32_t> log_of_num = breakNum(null_counter);
                for(const auto &i : log_of_num){
                    if(i == 0)
                        freqs.increment(static_cast<uint32_t>(0));
                    else
                        freqs.increment(static_cast<uint32_t>(256 + i));   //add null
                }
                null_counter = 0;
            }
        }
    }
    freqs.increment(256);  // EOF symbol gets a frequency of 1
    CodeTree code = freqs.buildCodeTree();
    const CanonicalCode canonCode(code, freqs.getSymbolLimit());
    // Replace code tree with canonical one. For each symbol,
    // the code value may change but the code length stays the same.
    code = canonCode.toCodeTree();

    // Read input file again, compress with Huffman coding, and write output file
    in.clear();
    in.seekg(0);
    std::ofstream out(outputFile, std::ios::binary);
    BitOutputStream bout(out);
    try {
        // Write code length table
        for (uint32_t i = 0; i < canonCode.getSymbolLimit(); i++) {
            uint32_t val = canonCode.getCodeLength(i);
            // For this file format, we only support codes up to 255 bits long
            if (val >= 256)
                throw std::domain_error("The code for a symbol is too long");
            // Write value as 8 bits in big endian
            for (int j = 7; j >= 0; j--){
                bout.write((val >> j) & 1);
               // std::cout << ((val >> j) & 1);
            }
            // std::cout << std::endl;
        }
        HuffmanEncoder enc(bout);
        enc.codeTree = &code;
        int zero_counter = 0;
        while (true) {
            // Read and encode one byte
            int symbol = in.get();
            if (symbol == EOF)
                break;
            if (symbol < 0 || symbol > 255)
                throw std::logic_error("Assertion error");
            if (symbol == 0){
                while (symbol == 0){
                    zero_counter ++;
                    symbol = in.get();
                }
                // reached non-zero symbol
                if(zero_counter == 1)
                    enc.write(static_cast<uint32_t >(0));
                else {
                    std::vector<uint32_t> log_of_num = breakNum(zero_counter);
                    for (const auto &i : log_of_num) {
                        if(i == 0)
                            enc.write(static_cast<uint32_t >(0));
                        else
                            enc.write(static_cast<uint32_t>(256 + i));
                    }
                }
                zero_counter = 0;
                if (symbol != EOF)
                    enc.write(static_cast<uint32_t>(symbol));
                else if(symbol == EOF)
                    break;
            }
            else{
                enc.write(static_cast<uint32_t>(symbol));
            }
        }
        enc.write(256);  // EOF
        bout.finish();
        return EXIT_SUCCESS;

    } catch (const char *msg) {
        std::cerr << msg << std::endl;
        return EXIT_FAILURE;
    }


}