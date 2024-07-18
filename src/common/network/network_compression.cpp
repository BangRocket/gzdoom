#include "network_compression.h"
#include <algorithm>
#include <cstring>
#include <queue>

namespace GZDoom
{
    namespace Network
    {

        Compressor::Compressor()
        {
            // Initialize any necessary compression resources
        }

        Compressor::~Compressor()
        {
            // Clean up any compression resources
        }

        std::vector<uint8_t> Compressor::Compress(const std::vector<uint8_t> &data)
        {
            // Use Huffman coding for general data compression
            return HuffmanCompress(data);
        }

        std::vector<uint8_t> Compressor::Decompress(const std::vector<uint8_t> &compressedData)
        {
            // Use Huffman coding for general data decompression
            return HuffmanDecompress(compressedData);
        }

        std::vector<uint8_t> Compressor::CompressDelta(const GameState &oldState, const GameState &newState)
        {
            std::vector<uint8_t> delta;

            // Compress player positions
            for (size_t i = 0; i < newState.playerPositions.size(); ++i)
            {
                if (i >= oldState.playerPositions.size() || newState.playerPositions[i] != oldState.playerPositions[i])
                {
                    delta.push_back(static_cast<uint8_t>(i));
                    const auto &pos = newState.playerPositions[i];
                    delta.insert(delta.end(), reinterpret_cast<const uint8_t *>(&pos), reinterpret_cast<const uint8_t *>(&pos) + sizeof(pos));
                }
            }

            // Compress player velocities
            for (size_t i = 0; i < newState.playerVelocities.size(); ++i)
            {
                if (i >= oldState.playerVelocities.size() || newState.playerVelocities[i] != oldState.playerVelocities[i])
                {
                    delta.push_back(static_cast<uint8_t>(i + 128)); // Use high bit to distinguish from positions
                    const auto &vel = newState.playerVelocities[i];
                    delta.insert(delta.end(), reinterpret_cast<const uint8_t *>(&vel), reinterpret_cast<const uint8_t *>(&vel) + sizeof(vel));
                }
            }

            return delta;
        }

        GameState Compressor::DecompressDelta(const GameState &baseState, const std::vector<uint8_t> &delta)
        {
            GameState newState = baseState;

            for (size_t i = 0; i < delta.size();)
            {
                uint8_t index = delta[i++];
                if (index < 128)
                {
                    // Position update
                    Vector3 pos;
                    memcpy(&pos, &delta[i], sizeof(Vector3));
                    i += sizeof(Vector3);
                    newState.playerPositions[index] = pos;
                }
                else
                {
                    // Velocity update
                    Vector3 vel;
                    memcpy(&vel, &delta[i], sizeof(Vector3));
                    i += sizeof(Vector3);
                    newState.playerVelocities[index - 128] = vel;
                }
            }

            return newState;
        }

        std::vector<uint8_t> Compressor::HuffmanCompress(const std::vector<uint8_t> &data)
        {
            // Count frequency of each byte
            std::vector<unsigned> frequencies(256, 0);
            for (uint8_t byte : data)
            {
                frequencies[byte]++;
            }

            // Build Huffman tree
            HuffmanNode *root = BuildHuffmanTree(frequencies);

            // Generate Huffman codes
            m_huffmanCodes.clear();
            m_reverseHuffmanCodes.clear();
            GenerateHuffmanCodes(root, "", m_huffmanCodes);
            for (const auto &pair : m_huffmanCodes)
            {
                m_reverseHuffmanCodes[pair.second] = pair.first;
            }

            // Compress data
            std::string compressedBits;
            for (uint8_t byte : data)
            {
                compressedBits += m_huffmanCodes[byte];
            }

            // Convert compressed bits to bytes
            std::vector<uint8_t> compressedData;
            for (size_t i = 0; i < compressedBits.length(); i += 8)
            {
                uint8_t byte = 0;
                for (int j = 0; j < 8 && i + j < compressedBits.length(); ++j)
                {
                    byte |= (compressedBits[i + j] == '1') << (7 - j);
                }
                compressedData.push_back(byte);
            }

            // Clean up
            DeleteHuffmanTree(root);

            return compressedData;
        }

        std::vector<uint8_t> Compressor::HuffmanDecompress(const std::vector<uint8_t> &compressedData)
        {
            // Convert compressed bytes to bits
            std::string compressedBits;
            for (uint8_t byte : compressedData)
            {
                for (int i = 7; i >= 0; --i)
                {
                    compressedBits += ((byte >> i) & 1) ? '1' : '0';
                }
            }

            // Decompress data
            std::vector<uint8_t> decompressedData;
            std::string currentCode;
            for (char bit : compressedBits)
            {
                currentCode += bit;
                if (m_reverseHuffmanCodes.find(currentCode) != m_reverseHuffmanCodes.end())
                {
                    decompressedData.push_back(m_reverseHuffmanCodes[currentCode]);
                    currentCode.clear();
                }
            }

            return decompressedData;
        }

        Compressor::HuffmanNode *Compressor::BuildHuffmanTree(const std::vector<unsigned> &frequencies)
        {
            auto compare = [](HuffmanNode *a, HuffmanNode *b)
            { return a->frequency > b->frequency; };
            std::priority_queue<HuffmanNode *, std::vector<HuffmanNode *>, decltype(compare)> pq(compare);

            for (int i = 0; i < 256; ++i)
            {
                if (frequencies[i] > 0)
                {
                    pq.push(new HuffmanNode(static_cast<uint8_t>(i), frequencies[i]));
                }
            }

            while (pq.size() > 1)
            {
                HuffmanNode *left = pq.top();
                pq.pop();
                HuffmanNode *right = pq.top();
                pq.pop();
                HuffmanNode *parent = new HuffmanNode(0, left->frequency + right->frequency);
                parent->left = left;
                parent->right = right;
                pq.push(parent);
            }

            return pq.top();
        }

        void Compressor::GenerateHuffmanCodes(HuffmanNode *root, std::string code, std::unordered_map<uint8_t, std::string> &huffmanCodes)
        {
            if (root == nullptr)
                return;
            if (root->left == nullptr && root->right == nullptr)
            {
                huffmanCodes[root->symbol] = code;
            }
            GenerateHuffmanCodes(root->left, code + "0", huffmanCodes);
            GenerateHuffmanCodes(root->right, code + "1", huffmanCodes);
        }

        void Compressor::DeleteHuffmanTree(HuffmanNode *root)
        {
            if (root == nullptr)
                return;
            DeleteHuffmanTree(root->left);
            DeleteHuffmanTree(root->right);
            delete root;
        }

    } // namespace Network
} // namespace GZDoom
