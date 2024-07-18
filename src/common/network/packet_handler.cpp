#include "packet_handler.h"
#include <cstring>

namespace GZDoom
{
    namespace Network
    {

        Packet::Packet()
            : sequence(0), ack(0), ack_bits(0) {}

        void Packet::Serialize(std::vector<uint8_t> &buffer) const
        {
            buffer.resize(sizeof(uint16_t) * 3 + data.size());
            uint8_t *ptr = buffer.data();

            memcpy(ptr, &sequence, sizeof(uint16_t));
            ptr += sizeof(uint16_t);

            memcpy(ptr, &ack, sizeof(uint16_t));
            ptr += sizeof(uint16_t);

            memcpy(ptr, &ack_bits, sizeof(uint16_t));
            ptr += sizeof(uint16_t);

            memcpy(ptr, data.data(), data.size());
        }

        void Packet::Deserialize(const std::vector<uint8_t> &buffer)
        {
            const uint8_t *ptr = buffer.data();

            memcpy(&sequence, ptr, sizeof(uint16_t));
            ptr += sizeof(uint16_t);

            memcpy(&ack, ptr, sizeof(uint16_t));
            ptr += sizeof(uint16_t);

            memcpy(&ack_bits, ptr, sizeof(uint16_t));
            ptr += sizeof(uint16_t);

            data.assign(ptr, ptr + buffer.size() - 3 * sizeof(uint16_t));
        }

        PacketHandler::PacketHandler() : m_nextSequence(0) {}

        Packet PacketHandler::CreatePacket(const std::vector<uint8_t> &data)
        {
            Packet packet;
            packet.sequence = m_nextSequence++;
            packet.data = data;
            return packet;
        }

        void PacketHandler::ProcessPacket(const Packet &packet)
        {
            // Process the received packet
            // This is where you would handle different packet types
            // and update game state accordingly
        }

    } // namespace Network
} // namespace GZDoom
#include "packet_handler.h"
#include "network_architecture.h"
#include <cstring>
#include <numeric>

Packet::Packet(PacketType type) : m_type(type), m_size(0) {}

void Packet::Write(const void *data, size_t size)
{
    if (m_size + size > MAX_PACKET_SIZE)
    {
        // Handle error: packet size exceeded
        return;
    }
    memcpy(m_data + m_size, data, size);
    m_size += size;
}

void Packet::Read(void *data, size_t size)
{
    if (m_readPos + size > m_size)
    {
        // Handle error: trying to read beyond packet size
        return;
    }
    memcpy(data, m_data + m_readPos, size);
    m_readPos += size;
}

PacketQueue::PacketQueue() {}

void PacketQueue::Push(const Packet &packet)
{
    m_queue.push(packet);
}

Packet PacketQueue::Pop()
{
    Packet packet = m_queue.front();
    m_queue.pop();
    return packet;
}

bool PacketQueue::IsEmpty() const
{
    return m_queue.empty();
}

std::vector<uint8_t> MessageCompressor::Compress(const std::vector<uint8_t> &data)
{
    // Implement a simple run-length encoding compression
    std::vector<uint8_t> compressed;
    for (size_t i = 0; i < data.size(); ++i)
    {
        uint8_t count = 1;
        while (i + 1 < data.size() && data[i] == data[i + 1] && count < 255)
        {
            ++count;
            ++i;
        }
        compressed.push_back(count);
        compressed.push_back(data[i]);
    }
    return compressed;
}

std::vector<uint8_t> MessageCompressor::Decompress(const std::vector<uint8_t> &compressedData)
{
    std::vector<uint8_t> decompressed;
    for (size_t i = 0; i < compressedData.size(); i += 2)
    {
        uint8_t count = compressedData[i];
        uint8_t value = compressedData[i + 1];
        decompressed.insert(decompressed.end(), count, value);
    }
    return decompressed;
}

BandwidthEstimator::BandwidthEstimator() : m_currentEstimate(0.0f) {}

void BandwidthEstimator::AddSample(size_t packetSize, double timeDelta)
{
    float bps = static_cast<float>(packetSize) / static_cast<float>(timeDelta);
    m_samples.push_back(bps);
    if (m_samples.size() > 10)
    {
        m_samples.erase(m_samples.begin());
    }
    m_currentEstimate = std::accumulate(m_samples.begin(), m_samples.end(), 0.0f) / m_samples.size();
}

float BandwidthEstimator::GetEstimate() const
{
    return m_currentEstimate;
}
