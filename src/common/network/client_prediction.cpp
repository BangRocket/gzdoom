#include "client_prediction.h"
#include <algorithm>

namespace GZDoom
{
    namespace Network
    {

        ClientPrediction::ClientPrediction() : m_lastPredictionTime(0.0) {}

        void ClientPrediction::PredictMovement(const InputState &input, double currentTime)
        {
            double deltaTime = currentTime - m_lastPredictionTime;
            m_lastPredictionTime = currentTime;

            // Apply input to predict new position and velocity
            m_predictedState.position += m_predictedState.velocity * deltaTime;
            m_predictedState.velocity += input.movement * PLAYER_ACCELERATION * deltaTime;

            // Apply simple friction
            m_predictedState.velocity *= (1.0f - FRICTION * deltaTime);

            // Store the input for later replay if needed
            BufferInput(input);
        }

        void ClientPrediction::CorrectPrediction(const ServerState &serverState)
        {
            // Calculate the difference between predicted and server state
            Vector3 positionError = serverState.position - m_predictedState.position;
            Vector3 velocityError = serverState.velocity - m_predictedState.velocity;

            // If the error is significant, apply correction
            if (positionError.LengthSquared() > ERROR_THRESHOLD_SQ)
            {
                m_predictedState.position = serverState.position;
                m_predictedState.velocity = serverState.velocity;

                // Replay inputs that occurred after the server state
                ReplayInput(serverState.lastProcessedInput);
            }
            else
            {
                // Apply smooth correction
                m_predictedState.position += positionError * POSITION_CORRECTION_FACTOR;
                m_predictedState.velocity += velocityError * VELOCITY_CORRECTION_FACTOR;
            }
        }

        void ClientPrediction::BufferInput(const InputState &input)
        {
            m_inputBuffer.push_back(input);
            // Limit buffer size to prevent memory issues
            if (m_inputBuffer.size() > MAX_INPUT_BUFFER_SIZE)
            {
                m_inputBuffer.erase(m_inputBuffer.begin());
            }
        }

        void ClientPrediction::ReplayInput(uint32_t lastProcessedInput)
        {
            auto startIt = std::find_if(m_inputBuffer.begin(), m_inputBuffer.end(),
                                        [lastProcessedInput](const InputState &input)
                                        {
                                            return input.sequence > lastProcessedInput;
                                        });

            for (auto it = startIt; it != m_inputBuffer.end(); ++it)
            {
                // Re-apply each input
                m_predictedState.position += m_predictedState.velocity * INPUT_DELTA_TIME;
                m_predictedState.velocity += it->movement * PLAYER_ACCELERATION * INPUT_DELTA_TIME;
                m_predictedState.velocity *= (1.0f - FRICTION * INPUT_DELTA_TIME);
            }
        }

        const PredictedState &ClientPrediction::GetPredictedState() const
        {
            return m_predictedState;
        }

    } // namespace Network
} // namespace GZDoom
