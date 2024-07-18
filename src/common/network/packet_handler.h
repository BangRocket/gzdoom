#pragma once

#include "network_architecture.h"
#include <vector>
#include <cstdint>

namespace GZDoom
{
    namespace Network
    {

        enum class PacketType : uint8_t
        {
            Handshake,
            GameState,
            PlayerInput,
            ServerCommand,
            ClientCommand,
            Disconnect,
            Ping,
            Pong,
            MapChange,
            PlayerJoin,
            PlayerLeave,
            ChatMessage,
            GameSettings,
            ClockSync,
            AckPacket,
            ResendRequest
        };

        class Packet
        {
        public:
            Packet(PacketType type = PacketType::GameState);

            void Serialize(std::vector<uint8_t> &buffer) const;
            void Deserialize(const std::vector<uint8_t> &buffer);

            void Write(const void *data, size_t size);
            void Read(void *data, size_t size);

            PacketType GetType() const { return m_type; }
            uint16_t GetSequence() const { return m_sequence; }
            void SetSequence(uint16_t seq) { m_sequence = seq; }

        private:
            PacketType m_type;
            uint16_t m_sequence;
            uint16_t m_ack;
            uint32_t m_ack_bits;
            std::vector<uint8_t> m_data;
            size_t m_readPos;
        };

        class PacketQueue
        {
        public:
            PacketQueue();

            void Push(const Packet &packet);
            Packet Pop();
            bool IsEmpty() const;

        private:
            std::queue<Packet> m_queue;
        };

        class PacketHandler
        {
        public:
            PacketHandler();

            Packet CreatePacket(PacketType type, const std::vector<uint8_t> &data);
            void ProcessPacket(const Packet &packet);

            void SetAckData(Packet &packet);
            void ProcessAckData(const Packet &packet);

        private:
            uint16_t m_nextSequence;
            uint16_t m_lastReceivedSequence;
            std::bitset<32> m_receivedPackets;
        };

    } // namespace Network
} // namespace GZDoom
