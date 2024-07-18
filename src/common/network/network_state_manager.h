#pragma once

#include "network_architecture.h"
#include <vector>
#include <unordered_map>

namespace GZDoom
{
    namespace Network
    {

        class NetworkStateManager
        {
        public:
            NetworkStateManager();
            ~NetworkStateManager();

            void UpdateState(const GameState &newState);
            std::vector<uint8_t> GenerateDeltaUpdate(int clientId);
            void ApplyDeltaUpdate(const std::vector<uint8_t> &deltaUpdate);
            void RollbackState(int ticks);
            void ReplayInputs(const std::vector<InputState> &inputs);

        private:
            GameState m_currentState;
            std::unordered_map<int, GameState> m_clientStates;
            std::vector<GameState> m_stateHistory;

            std::vector<uint8_t> SerializeDelta(const GameState &oldState, const GameState &newState);
            GameState DeserializeDelta(const GameState &baseState, const std::vector<uint8_t> &delta);
        };

    } // namespace Network
} // namespace GZDoom
