cmake_minimum_required(VERSION 3.16)
project(Huffman_Improved)

set(CMAKE_CXX_STANDARD 14)

add_executable(Huffman_Improved BitIoStream.cpp BitIoStream.hpp CanonicalCode.cpp CanonicalCode.hpp
        CodeTree.cpp CodeTree.hpp FrequencyTable.cpp FrequencyTable.hpp HuffmanCoder.cpp HuffmanCoder.hpp HuffmanCompress.cpp)