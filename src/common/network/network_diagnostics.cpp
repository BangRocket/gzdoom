#include "network_diagnostics.h"
#include <numeric>
#include <algorithm>
#include <chrono>

namespace GZDoom
{
    namespace Network
    {

        NetworkDiagnostics::NetworkDiagnostics()
            : m_totalBytesReceived(0), m_totalBytesSent(0), m_startTime(std::chrono::steady_clock::now()) {}

        NetworkDiagnostics::~NetworkDiagnostics() {}

        void NetworkDiagnostics::UpdatePing(int clientId, float ping)
        {
            m_clientPings[clientId] = ping;
        }

        void NetworkDiagnostics::UpdatePacketLoss(int clientId, float packetLoss)
        {
            m_clientPacketLoss[clientId] = packetLoss;
        }

        void NetworkDiagnostics::UpdateBandwidthUsage(int bytesReceived, int bytesSent)
        {
            m_totalBytesReceived += bytesReceived;
            m_totalBytesSent += bytesSent;
        }

        void NetworkDiagnostics::LogNetworkEvent(const std::string &event)
        {
            auto now = std::chrono::steady_clock::now();
            auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_startTime).count();
            m_networkEvents.push_back({timestamp, event});
            if (m_networkEvents.size() > 100)
            {
                m_networkEvents.erase(m_networkEvents.begin());
            }
        }

        float NetworkDiagnostics::GetAveragePing() const
        {
            if (m_clientPings.empty())
                return 0.0f;
            float sum = std::accumulate(m_clientPings.begin(), m_clientPings.end(), 0.0f,
                                        [](float acc, const auto &pair)
                                        { return acc + pair.second; });
            return sum / m_clientPings.size();
        }

        float NetworkDiagnostics::GetAveragePacketLoss() const
        {
            if (m_clientPacketLoss.empty())
                return 0.0f;
            float sum = std::accumulate(m_clientPacketLoss.begin(), m_clientPacketLoss.end(), 0.0f,
                                        [](float acc, const auto &pair)
                                        { return acc + pair.second; });
            return sum / m_clientPacketLoss.size();
        }

        float NetworkDiagnostics::GetBandwidthUsage() const
        {
            auto now = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - m_startTime).count();
            return static_cast<float>(m_totalBytesReceived + m_totalBytesSent) / duration;
        }

        std::vector<NetworkDiagnostics::TimestampedEvent> NetworkDiagnostics::GetRecentNetworkEvents(int count) const
        {
            count = std::min(count, static_cast<int>(m_networkEvents.size()));
            return std::vector<TimestampedEvent>(m_networkEvents.end() - count, m_networkEvents.end());
        }

        void NetworkDiagnostics::ResetStats()
        {
            m_clientPings.clear();
            m_clientPacketLoss.clear();
            m_totalBytesReceived = 0;
            m_totalBytesSent = 0;
            m_networkEvents.clear();
            m_startTime = std::chrono::steady_clock::now();
        }

    } // namespace Network
} // namespace GZDoom
