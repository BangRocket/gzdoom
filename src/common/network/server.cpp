#include "server.h"
#include "network_manager.h"
#include <iostream>
#include <algorithm>

namespace GZDoom
{
    namespace Network
    {

        Server::Server() : m_tickRate(60), m_lastTickTime(0.0) {}

        Server::~Server() = default;

        void Server::Update(double currentTime)
        {
            if (currentTime - m_lastTickTime >= 1.0 / m_tickRate)
            {
                Tick(currentTime);
                m_lastTickTime = currentTime;
            }

            ProcessIncomingPackets();
            UpdateClients();
            ProcessClockSyncRequests();
            ProcessGameSettingsRequests();
            ProcessClockSyncRequests();
        }

        void Server::ProcessClockSyncRequests()
        {
            for (const auto &request : m_clockSyncRequests)
            {
                Packet response;
                response.type = PacketType::ClockSyncResponse;

                double currentTime = GetCurrentTime();
                double roundTripTime = currentTime - request.clientTime;

                std::vector<uint8_t> data(sizeof(double) * 2);
                memcpy(data.data(), &currentTime, sizeof(double));
                memcpy(data.data() + sizeof(double), &roundTripTime, sizeof(double));

                response.data = data;
                SendPacketToClient(request.clientId, response);
            }
            m_clockSyncRequests.clear();
        }

        void Server::HandleClockSyncRequest(int clientId, const Packet &packet)
        {
            if (packet.data.size() != sizeof(double))
            {
                // Invalid packet size
                return;
            }

            double clientTime;
            memcpy(&clientTime, packet.data.data(), sizeof(double));

            m_clockSyncRequests.push_back({clientId, clientTime});
        }

        void Server::Tick(double currentTime)
        {
            UpdateGameState(currentTime);
            SendStateUpdates();
        }

        void Server::ProcessIncomingPackets()
        {
            // Process packets from all clients
            for (auto &client : m_clients)
            {
                std::vector<Packet> packets = ReceivePacketsFromClient(client.id);
                for (const auto &packet : packets)
                {
                    HandleClientPacket(client.id, packet);
                }
            }
        }

        void Server::UpdateClients()
        {
            for (auto &client : m_clients)
            {
                SendUpdatesToClient(client);
            }
        }

        void Server::UpdateGameState(double currentTime)
        {
            // Update game physics, AI, etc.
            // This is where you'd integrate with the main game loop
        }

        void Server::SendStateUpdates()
        {
            for (auto &client : m_clients)
            {
                std::vector<uint8_t> stateUpdate = m_serverReconciliation.GenerateDeltaUpdate(client.id);
                SendPacketToClient(client.id, PacketType::StateUpdate, stateUpdate);
            }
        }

        void Server::HandleClientPacket(int clientId, const Packet &packet)
        {
            switch (static_cast<PacketType>(packet.data[0]))
            {
            case PacketType::Movement:
                HandleMovementPacket(clientId, *reinterpret_cast<const MovementPacket *>(packet.data.data() + 1));
                break;
            case PacketType::Command:
                HandleCommandPacket(clientId, *reinterpret_cast<const CommandPacket *>(packet.data.data() + 1));
                break;
            default:
                std::cerr << "Unknown packet type received from client " << clientId << std::endl;
            }
        }

        void Server::HandleMovementPacket(int clientId, const MovementPacket &movementPacket)
        {
            // Update the client's predicted state
            m_serverReconciliation.ProcessClientUpdate(clientId, movementPacket.clientState);

            // Apply the movement to the authoritative game state
            ApplyMovementToGameState(clientId, movementPacket);
        }

        void Server::HandleCommandPacket(int clientId, const CommandPacket &commandPacket)
        {
            // Process the command (e.g., use item, change weapon)
            ExecutePlayerCommand(clientId, commandPacket);
        }

        void Server::ApplyMovementToGameState(int clientId, const MovementPacket &movementPacket)
        {
            // Apply the movement to the game state, considering server-side physics and game rules
            // This is where you'd integrate with your game's physics system
        }

        void Server::ExecutePlayerCommand(int clientId, const CommandPacket &commandPacket)
        {
            // Execute the player's command in the game world
            // This could involve using items, changing weapons, etc.
        }

        void Server::SendPacketToClient(int clientId, PacketType type, const std::vector<uint8_t> &data)
        {
            Packet packet;
            packet.sequence = GetNextSequenceNumber(clientId);
            packet.data = data;
            packet.data.insert(packet.data.begin(), static_cast<uint8_t>(type));

            // Send the packet to the client (implement this based on your network layer)
            SendToClient(clientId, packet);
        }

        uint16_t Server::GetNextSequenceNumber(int clientId)
        {
            // Implement sequence number generation for each client
            return ++m_clientSequenceNumbers[clientId];
        }

        void Server::SendToClient(int clientId, const Packet &packet)
        {
            // Implement the actual sending of the packet to the client
            // This will depend on your underlying network implementation
        }

        std::vector<Packet> Server::ReceivePacketsFromClient(int clientId)
        {
            // Implement receiving packets from a specific client
            // This will depend on your underlying network implementation
            return std::vector<Packet>();
        }

        void Server::SendUpdatesToClient(const ClientInfo &client)
        {
            // Send any necessary updates to the client (e.g., other players' positions)
            // This could involve sending multiple packets of different types
        }

    bool Server::ValidateClientAction(int clientId, const ClientAction &action)
    {
        // Implement server-side validation of client actions
        // This is a basic example and should be expanded based on your game's specific needs
        
        // Check if the action is within reasonable bounds
        if (action.position.x < -1000 || action.position.x > 1000 ||
            action.position.y < -1000 || action.position.y > 1000 ||
            action.position.z < -1000 || action.position.z > 1000)
        {
            return false;
        }

        // Check if the player's speed is within acceptable limits
        float speed = action.velocity.Length();
        if (speed > MAX_PLAYER_SPEED)
        {
            return false;
        }

        // Add more checks as needed for your game

        return true;
    }

    void Server::ProcessClientAction(int clientId, const ClientAction &action)
    {
        if (ValidateClientAction(clientId, action))
        {
            // Apply the action to the game state
            ApplyClientActionToGameState(clientId, action);
        }
        else
        {
            // Report suspicious activity
            m_securityManager.ReportSuspiciousActivity(std::to_string(clientId), "Invalid client action");
            
            // Optionally, you can correct the client's state here
            CorrectClientState(clientId);
        }
    }

    } // namespace Network
} // namespace GZDoom
void Server::ProcessClockSyncRequests()
{
    for (const auto &request : m_clockSyncRequests)
    {
        Packet response;
        response.type = PacketType::ClockSyncResponse;

        double currentTime = GetCurrentTime();
        double roundTripTime = currentTime - request.clientTime;

        std::vector<uint8_t> data(sizeof(double) * 2);
        memcpy(data.data(), &currentTime, sizeof(double));
        memcpy(data.data() + sizeof(double), &roundTripTime, sizeof(double));

        response.data = data;
        SendPacketToClient(request.clientId, response);
    }
    m_clockSyncRequests.clear();
}

void Server::HandleClockSyncRequest(int clientId, const Packet &packet)
{
    if (packet.data.size() != sizeof(double))
    {
        // Invalid packet size
        return;
    }

    double clientTime;
    memcpy(&clientTime, packet.data.data(), sizeof(double));

    m_clockSyncRequests.push_back({clientId, clientTime});
}
void Server::ProcessGameSettingsRequests()
{
    for (const auto &request : m_gameSettingsRequests)
    {
        SendGameSettings(request.clientId, m_gameSettings);
    }
    m_gameSettingsRequests.clear();
}

void Server::SendGameSettings(int clientId, const GameSettings &settings)
{
    Packet packet;
    packet.type = PacketType::GameSettings;

    packet.data.resize(sizeof(GameSettings));
    memcpy(packet.data.data(), &settings, sizeof(GameSettings));

    SendPacketToClient(clientId, packet);
}

void Server::HandleGameSettingsRequest(int clientId)
{
    m_gameSettingsRequests.push_back({clientId});
}
void Server::ProcessClockSyncRequests()
{
    for (const auto &request : m_clockSyncRequests)
    {
        Packet response;
        response.type = PacketType::ClockSyncResponse;

        double currentTime = GetCurrentTime();
        double roundTripTime = currentTime - request.clientTime;

        std::vector<uint8_t> data(sizeof(double) * 2);
        memcpy(data.data(), &currentTime, sizeof(double));
        memcpy(data.data() + sizeof(double), &roundTripTime, sizeof(double));

        response.data = data;
        SendPacketToClient(request.clientId, response);
    }
    m_clockSyncRequests.clear();
}

void Server::HandleClockSyncRequest(int clientId, const Packet &packet)
{
    if (packet.data.size() != sizeof(double))
    {
        // Invalid packet size
        return;
    }

    double clientTime;
    memcpy(&clientTime, packet.data.data(), sizeof(double));

    m_clockSyncRequests.push_back({clientId, clientTime});
}
void Server::ProcessGameSettingsRequests()
{
    for (const auto &request : m_gameSettingsRequests)
    {
        SendGameSettings(request.clientId, m_gameSettings);
    }
    m_gameSettingsRequests.clear();
}

void Server::SendGameSettings(int clientId, const GameSettings &settings)
{
    Packet packet;
    packet.type = PacketType::GameSettings;

    packet.data.resize(sizeof(GameSettings));
    memcpy(packet.data.data(), &settings, sizeof(GameSettings));

    SendPacketToClient(clientId, packet);
}

void Server::HandleGameSettingsRequest(int clientId)
{
    m_gameSettingsRequests.push_back({clientId});
}
void Server::ProcessClockSyncRequests()
{
    for (const auto &request : m_clockSyncRequests)
    {
        Packet response;
        response.type = PacketType::ClockSyncResponse;

        double currentTime = GetCurrentTime();
        double roundTripTime = currentTime - request.clientTime;

        std::vector<uint8_t> data(sizeof(double) * 2);
        memcpy(data.data(), &currentTime, sizeof(double));
        memcpy(data.data() + sizeof(double), &roundTripTime, sizeof(double));

        response.data = data;
        SendPacketToClient(request.clientId, response);
    }
    m_clockSyncRequests.clear();
}

void Server::HandleClockSyncRequest(int clientId, const Packet &packet)
{
    if (packet.data.size() != sizeof(double))
    {
        // Invalid packet size
        return;
    }

    double clientTime;
    memcpy(&clientTime, packet.data.data(), sizeof(double));

    m_clockSyncRequests.push_back({clientId, clientTime});
}
