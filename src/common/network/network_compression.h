#pragma once

#include <vector>
#include <cstdint>

namespace GZDoom
{
    namespace Network
    {

        class Compressor
        {
        public:
            Compressor();
            ~Compressor();

            std::vector<uint8_t> Compress(const std::vector<uint8_t> &data);
            std::vector<uint8_t> Decompress(const std::vector<uint8_t> &compressedData);
            std::vector<uint8_t> CompressDelta(const GameState &oldState, const GameState &newState);
            GameState DecompressDelta(const GameState &baseState, const std::vector<uint8_t> &delta);

            // New Huffman coding methods
            std::vector<uint8_t> HuffmanCompress(const std::vector<uint8_t> &data);
            std::vector<uint8_t> HuffmanDecompress(const std::vector<uint8_t> &compressedData);

        private:
            struct HuffmanNode
            {
                uint8_t symbol;
                unsigned frequency;
                HuffmanNode *left;
                HuffmanNode *right;

                HuffmanNode(uint8_t s, unsigned f) : symbol(s), frequency(f), left(nullptr), right(nullptr) {}
            };

            HuffmanNode *BuildHuffmanTree(const std::vector<unsigned> &frequencies);
            void GenerateHuffmanCodes(HuffmanNode *root, std::string code, std::unordered_map<uint8_t, std::string> &huffmanCodes);
            void DeleteHuffmanTree(HuffmanNode *root);

            std::unordered_map<uint8_t, std::string> m_huffmanCodes;
            std::unordered_map<std::string, uint8_t> m_reverseHuffmanCodes;
        };

    } // namespace Network
} // namespace GZDoom
