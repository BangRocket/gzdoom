#include "server_reconciliation.h"
#include <algorithm>
#include <cstring>

namespace GZDoom
{
    namespace Network
    {

        ServerReconciliation::ServerReconciliation() {}

        void ServerReconciliation::UpdateAuthoritativeState(const GameState &newState)
        {
            m_authoritativeState = newState;
        }

        void ServerReconciliation::ProcessClientUpdate(int clientId, const ClientUpdate &update)
        {
            m_clientStates[clientId] = update.clientState;
        }

        std::vector<StateCorrection> ServerReconciliation::GenerateCorrections()
        {
            std::vector<StateCorrection> corrections;

            for (const auto &clientPair : m_clientStates)
            {
                int clientId = clientPair.first;
                const ClientState &clientState = clientPair.second;

                if (!CompareStates(m_authoritativeState, clientState.predictedState))
                {
                    StateCorrection correction = CalculateCorrection(clientId, clientState);
                    corrections.push_back(correction);
                }
            }

            return corrections;
        }

        std::vector<uint8_t> ServerReconciliation::GenerateDeltaUpdate(int clientId)
        {
            auto it = m_lastSentStates.find(clientId);
            if (it == m_lastSentStates.end())
            {
                // If we haven't sent a state to this client before, send the full state
                m_lastSentStates[clientId] = m_authoritativeState;
                return CompressDelta(GameState(), m_authoritativeState);
            }

            std::vector<uint8_t> delta = CompressDelta(it->second, m_authoritativeState);
            m_lastSentStates[clientId] = m_authoritativeState;
            return delta;
        }

        bool ServerReconciliation::CompareStates(const GameState &state1, const GameState &state2)
        {
            // Implement state comparison logic here
            // This is a simplified example, you may need to expand this based on your specific GameState structure
            return state1.playerPositions == state2.playerPositions &&
                   state1.playerVelocities == state2.playerVelocities;
        }

        StateCorrection ServerReconciliation::CalculateCorrection(int clientId, const ClientState &clientState)
        {
            StateCorrection correction;
            correction.clientId = clientId;

            // Calculate the differences between authoritative and client states
            correction.positionCorrection = m_authoritativeState.playerPositions[clientId] - clientState.predictedState.playerPositions[clientId];
            correction.velocityCorrection = m_authoritativeState.playerVelocities[clientId] - clientState.predictedState.playerVelocities[clientId];

            return correction;
        }

        std::vector<uint8_t> ServerReconciliation::GenerateDeltaUpdate(int clientId)
        {
            auto it = m_lastSentStates.find(clientId);
            if (it == m_lastSentStates.end())
            {
                // If we haven't sent a state to this client before, send the full state
                m_lastSentStates[clientId] = m_authoritativeState;
                return m_compressor.CompressDelta(GameState(), m_authoritativeState);
            }

            std::vector<uint8_t> delta = m_compressor.CompressDelta(it->second, m_authoritativeState);
            m_lastSentStates[clientId] = m_authoritativeState;
            return delta;
        }

    } // namespace Network
} // namespace GZDoom
