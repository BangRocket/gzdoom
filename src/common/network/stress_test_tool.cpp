#include "stress_test_tool.h"
#include "network_manager.h"
#include <thread>
#include <chrono>
#include <iostream>

namespace GZDoom
{
namespace Network
{

StressTestTool::StressTestTool(NetworkManager* networkManager)
    : m_networkManager(networkManager), m_running(false)
{
}

void StressTestTool::RunTest(int numPlayers, int duration, int actionsPerSecond)
{
    m_running = true;
    std::vector<std::thread> playerThreads;

    for (int i = 0; i < numPlayers; ++i)
    {
        playerThreads.emplace_back(&StressTestTool::SimulatePlayer, this, i, duration, actionsPerSecond);
    }

    for (auto& thread : playerThreads)
    {
        thread.join();
    }

    m_running = false;
    std::cout << "Stress test completed." << std::endl;
}

void StressTestTool::SimulatePlayer(int playerId, int duration, int actionsPerSecond)
{
    auto startTime = std::chrono::steady_clock::now();
    auto endTime = startTime + std::chrono::seconds(duration);

    while (m_running && std::chrono::steady_clock::now() < endTime)
    {
        SimulatePlayerAction(playerId);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / actionsPerSecond));
    }
}

void StressTestTool::SimulatePlayerAction(int playerId)
{
    // Simulate various player actions
    switch (rand() % 5)
    {
    case 0:
        m_networkManager->SimulatePlayerMove(playerId);
        break;
    case 1:
        m_networkManager->SimulatePlayerShoot(playerId);
        break;
    case 2:
        m_networkManager->SimulatePlayerJump(playerId);
        break;
    case 3:
        m_networkManager->SimulatePlayerUseItem(playerId);
        break;
    case 4:
        m_networkManager->SimulatePlayerChat(playerId);
        break;
    }
}

} // namespace Network
} // namespace GZDoom
