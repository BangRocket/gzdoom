#include "network_state_manager.h"
#include <algorithm>

namespace GZDoom
{
    namespace Network
    {

        NetworkStateManager::NetworkStateManager() {}

        NetworkStateManager::~NetworkStateManager() {}

        void NetworkStateManager::UpdateState(const GameState &newState)
        {
            m_stateHistory.push_back(m_currentState);
            m_currentState = newState;

            // Limit the size of state history to prevent excessive memory usage
            if (m_stateHistory.size() > 100)
            {
                m_stateHistory.erase(m_stateHistory.begin());
            }
        }

        std::vector<uint8_t> NetworkStateManager::GenerateDeltaUpdate(int clientId)
        {
            auto it = m_clientStates.find(clientId);
            if (it == m_clientStates.end())
            {
                // If we don't have a previous state for this client, send full state
                m_clientStates[clientId] = m_currentState;
                return SerializeDelta(GameState(), m_currentState);
            }

            std::vector<uint8_t> delta = SerializeDelta(it->second, m_currentState);
            m_clientStates[clientId] = m_currentState;
            return delta;
        }

        void NetworkStateManager::ApplyDeltaUpdate(const std::vector<uint8_t> &deltaUpdate)
        {
            m_currentState = DeserializeDelta(m_currentState, deltaUpdate);
        }

        void NetworkStateManager::RollbackState(int ticks)
        {
            if (ticks <= 0 || ticks > m_stateHistory.size())
            {
                return;
            }

            m_currentState = m_stateHistory[m_stateHistory.size() - ticks];
            m_stateHistory.erase(m_stateHistory.end() - ticks, m_stateHistory.end());
        }

        void NetworkStateManager::ReplayInputs(const std::vector<InputState> &inputs)
        {
            for (const auto &input : inputs)
            {
                // Apply input to current state
                // This is a simplified example; actual implementation would depend on your game logic
                m_currentState.ApplyInput(input);
            }
        }

        std::vector<uint8_t> NetworkStateManager::SerializeDelta(const GameState &oldState, const GameState &newState)
        {
            std::vector<uint8_t> delta;

            // Serialize the differences between oldState and newState
            // This is a simplified example; actual implementation would depend on your GameState structure
            for (size_t i = 0; i < newState.entities.size(); ++i)
            {
                if (i >= oldState.entities.size() || newState.entities[i] != oldState.entities[i])
                {
                    delta.push_back(static_cast<uint8_t>(i));
                    // Serialize entity data
                    // ...
                }
            }

            return delta;
        }

        GameState NetworkStateManager::DeserializeDelta(const GameState &baseState, const std::vector<uint8_t> &delta)
        {
            GameState newState = baseState;

            // Apply the delta to the base state
            // This is a simplified example; actual implementation would depend on your GameState structure
            for (size_t i = 0; i < delta.size();)
            {
                uint8_t entityIndex = delta[i++];
                // Deserialize entity data and update newState
                // ...
            }

            return newState;
        }

    } // namespace Network
} // namespace GZDoom
