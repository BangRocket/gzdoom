#pragma once

#include <vector>
#include <string>
#include "network_architecture.h"
#include "packet_handler.h"
#include "client_prediction.h"

namespace GZDoom
{
    namespace Network
    {

        class Client
        {
        public:
            Client();
            ~Client();

            void Update();
            void Connect(const std::string &serverAddress);
            void Disconnect();
            void SendCommand(const std::string &cmd);
            void SendPacket(const Packet &packet);
            std::vector<Packet> ReceivePackets();
            void ProcessPacket(const Packet &packet);
            void UpdateGameState();

        private:
            enum class State
            {
                Disconnected,
                Connecting,
                Connected,
                Active
            };

            State m_state;
            int m_serverCount;
            double m_lastServerMessage;
            std::vector<int> m_stats;
            PacketHandler m_packetHandler;
            std::vector<Packet> m_incomingPackets;
            ClientPrediction m_clientPrediction;

            void HandleMovementPacket(const MovementPacket &movementPacket);
            void HandleStateUpdatePacket(const StateUpdatePacket &stateUpdatePacket);
            void HandleModSyncPacket(const ModSyncPacket &modSyncPacket);
            void PredictAndCorrectMovement();
        };

    } // namespace Network
} // namespace GZDoom
