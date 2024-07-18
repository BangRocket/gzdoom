#pragma once

#include <unordered_map>
#include <vector>
#include <cstdint>
#include <chrono>
#include <string>

namespace GZDoom
{
    namespace Network
    {

        class NetworkDiagnostics
        {
        public:
            struct TimestampedEvent
            {
                int64_t timestamp;
                std::string event;
            };

            NetworkDiagnostics();
            ~NetworkDiagnostics();

            void UpdatePing(int clientId, float ping);
            void UpdatePacketLoss(int clientId, float packetLoss);
            void UpdateBandwidthUsage(int bytesReceived, int bytesSent);
            void LogNetworkEvent(const std::string &event);

            float GetAveragePing() const;
            float GetAveragePacketLoss() const;
            float GetBandwidthUsage() const;
            std::vector<TimestampedEvent> GetRecentNetworkEvents(int count) const;

            void ResetStats();

        private:
            std::unordered_map<int, float> m_clientPings;
            std::unordered_map<int, float> m_clientPacketLoss;
            int m_totalBytesReceived;
            int m_totalBytesSent;
            std::vector<TimestampedEvent> m_networkEvents;
            std::chrono::steady_clock::time_point m_startTime;
        };

    } // namespace Network
} // namespace GZDoom
