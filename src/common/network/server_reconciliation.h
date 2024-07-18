#pragma once

#include "network_architecture.h"
#include <vector>
#include <unordered_map>

namespace GZDoom
{
    namespace Network
    {

        class ServerReconciliation
        {
        public:
            ServerReconciliation();

            void UpdateAuthoritativeState(const GameState &newState);
            void ProcessClientUpdate(int clientId, const ClientUpdate &update);
            std::vector<StateCorrection> GenerateCorrections();
            std::vector<uint8_t> GenerateDeltaUpdate(int clientId);

        private:
            GameState m_authoritativeState;
            std::unordered_map<int, ClientState> m_clientStates;
            std::unordered_map<int, GameState> m_lastSentStates;

            bool CompareStates(const GameState &state1, const GameState &state2);
            StateCorrection CalculateCorrection(int clientId, const ClientState &clientState);
            std::vector<uint8_t> CompressDelta(const GameState &oldState, const GameState &newState);
        };

    } // namespace Network
} // namespace GZDoom
