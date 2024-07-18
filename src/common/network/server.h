#pragma once

#include "network_architecture.h"
#include "server_reconciliation.h"
#include <vector>
#include <unordered_map>

namespace GZDoom
{
    namespace Network
    {

        class Server
        {
        public:
            Server();
            ~Server();

            void Update(double currentTime);

        private:
            void Tick(double currentTime);
            void ProcessIncomingPackets();
            void UpdateClients();
            void UpdateGameState(double currentTime);
            void SendStateUpdates();
            void HandleClientPacket(int clientId, const Packet &packet);
            void HandleMovementPacket(int clientId, const MovementPacket &movementPacket);
            void HandleCommandPacket(int clientId, const CommandPacket &commandPacket);
            void ApplyMovementToGameState(int clientId, const MovementPacket &movementPacket);
            void ExecutePlayerCommand(int clientId, const CommandPacket &commandPacket);
            void SendPacketToClient(int clientId, PacketType type, const std::vector<uint8_t> &data);
            uint16_t GetNextSequenceNumber(int clientId);
            void SendToClient(int clientId, const Packet &packet);
            std::vector<Packet> ReceivePacketsFromClient(int clientId);
            void SendUpdatesToClient(const ClientInfo &client);

            std::vector<ClientInfo> m_clients;
            ServerReconciliation m_serverReconciliation;
            std::unordered_map<int, uint16_t> m_clientSequenceNumbers;
            double m_tickRate;
            double m_lastTickTime;
        };

    } // namespace Network
} // namespace GZDoom
