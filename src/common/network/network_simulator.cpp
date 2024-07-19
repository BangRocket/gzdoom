#include "network_simulator.h"
#include <random>
#include <chrono>
#include <thread>

namespace GZDoom
{
namespace Network
{

NetworkSimulator::NetworkSimulator()
    : m_enabled(false), m_latency(0), m_jitter(0), m_packetLoss(0), m_bandwidthLimit(0)
{
}

void NetworkSimulator::SetConditions(int latency, int jitter, float packetLoss, int bandwidthLimit)
{
    m_latency = latency;
    m_jitter = jitter;
    m_packetLoss = packetLoss;
    m_bandwidthLimit = bandwidthLimit;
}

void NetworkSimulator::Enable(bool enable)
{
    m_enabled = enable;
}

std::vector<uint8_t> NetworkSimulator::SimulatePacket(const std::vector<uint8_t>& packet)
{
    if (!m_enabled)
    {
        return packet;
    }

    // Simulate packet loss
    if (ShouldDropPacket())
    {
        return std::vector<uint8_t>();
    }

    // Simulate latency and jitter
    SimulateLatency();

    // Simulate bandwidth limit
    return SimulateBandwidthLimit(packet);
}

bool NetworkSimulator::ShouldDropPacket()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1);
    return dis(gen) < m_packetLoss;
}

void NetworkSimulator::SimulateLatency()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(-m_jitter, m_jitter);
    int delay = m_latency + dis(gen);
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
}

std::vector<uint8_t> NetworkSimulator::SimulateBandwidthLimit(const std::vector<uint8_t>& packet)
{
    if (m_bandwidthLimit == 0)
    {
        return packet;
    }

    int delayMs = (packet.size() * 8 * 1000) / m_bandwidthLimit;
    std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
    return packet;
}

} // namespace Network
} // namespace GZDoom
