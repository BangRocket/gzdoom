#pragma once

#include <vector>
#include "network_architecture.h"

namespace GZDoom
{
    namespace Network
    {

        class ClientPrediction
        {
        public:
            ClientPrediction();

            void PredictMovement(const InputState &input, double currentTime);
            void CorrectPrediction(const ServerState &serverState);
            void BufferInput(const InputState &input);
            void ReplayInput();

        private:
            std::vector<InputState> m_inputBuffer;
            double m_lastPredictionTime;
        };

    } // namespace Network
} // namespace GZDoom
