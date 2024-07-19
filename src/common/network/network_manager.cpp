#include "network_manager.h"
#include "client.h"
#include "server.h"

namespace GZDoom
{
    namespace Network
    {

        NetworkManager::NetworkManager() : m_connectionState(ConnectionState::Disconnected), m_compatibilityMode(false) {}
        NetworkManager::~NetworkManager() = default;

        void NetworkManager::Initialize()
        {
            DetectNetworkMode();
            if (m_compatibilityMode)
            {
                SwitchToLegacyMode();
            }
            else
            {
                SwitchToModernMode();
            }

            m_client = std::make_unique<Client>();
            m_server = std::make_unique<Server>();
            m_packetHandler = std::make_unique<PacketHandler>();
            m_serverReconciliation = std::make_unique<ServerReconciliation>();
            m_compressor = std::make_unique<Compressor>();
            m_acsNetworkBridge = std::make_unique<ACSNetworkBridge>(this);
            m_zscriptNetworkBridge = std::make_unique<ZScriptNetworkBridge>(this);
            m_networkStateManager = std::make_unique<NetworkStateManager>();
            m_lagCompensator = std::make_unique<LagCompensator>();
            m_networkDiagnostics = std::make_unique<NetworkDiagnostics>();
        }

        void NetworkManager::DetectNetworkMode()
        {
            // Implement logic to detect whether to use legacy or modern netcode
            // This could be based on server settings, client preferences, or other factors
            m_compatibilityMode = false; // Default to modern netcode
        }

        void NetworkManager::SwitchToLegacyMode()
        {
            // Implement code to switch to legacy netcode
            // This might involve initializing old network structures or loading legacy network modules
        }

        void NetworkManager::SwitchToModernMode()
        {
            // Implement code to switch to modern netcode
            // This might involve initializing new network structures or loading modern network modules
        }

        void NetworkManager::SetCompatibilityMode(bool enabled)
        {
            m_compatibilityMode = enabled;
            // Add any necessary logic to switch between modern and legacy netcode
        }

        bool NetworkManager::IsCompatibilityModeEnabled() const
        {
            return m_compatibilityMode;
        }

        void NetworkManager::Update()
        {
            double currentTime = GetCurrentTime();

            if (m_client)
                m_client->Update(currentTime);
            if (m_server)
                m_server->Update(currentTime);
            ReceivePackets();

            // Update game state
            m_gameState.Update(currentTime);

            // Perform server-side reconciliation
            if (m_server && m_serverReconciliation)
            {
                m_serverReconciliation->UpdateAuthoritativeState(m_gameState);

                std::vector<StateCorrection> corrections = m_serverReconciliation->GenerateCorrections();
                for (const auto &correction : corrections)
                {
                    m_server->SendCorrection(correction);
                }

                // Send delta updates to all clients
                for (const auto &client : m_server->GetConnectedClients())
                {
                    std::vector<uint8_t> deltaUpdate = m_gameState.GenerateDelta(m_lastSentStates[client.id]);
                    m_server->SendDeltaUpdate(client.id, deltaUpdate);
                    m_lastSentStates[client.id] = m_gameState;
                }
            }

            // Update entity interpolation and lag compensation
            UpdateEntityInterpolation(currentTime);
            UpdateLagCompensation(currentTime);

            // Update network diagnostics
            UpdateNetworkDiagnostics();

            // Perform clock synchronization
            SynchronizeClocks();

            // Synchronize game settings
            SynchronizeGameSettings();

            // Update clock synchronization
            UpdateClockSync();

            // Update tic-based synchronization
            UpdateTicSync();

            // Update clock synchronization
            UpdateClockSync();

            // Perform tic-based synchronization
            SynchronizeTics();

            // Process packet sequencing and acknowledgments
            ProcessPacketSequencing();

            // Update player management
            UpdatePlayerManagement();

            // Handle error logging
            ProcessErrorLogs();

            // Update console messages
            ProcessConsoleMessages();

            // Update game launch and start sequence
            UpdateGameLaunchSequence();

            // Process resend requests
            ProcessResendRequests();

            // Update compatibility mode
            UpdateCompatibilityMode();

            // Update tic-based synchronization
            UpdateTicSync();
        }

        void NetworkManager::UpdateTicSync()
        {
            m_currentTic++;
            if (m_server)
            {
                // Server: Send current tic to all clients
                for (const auto &client : m_server->GetConnectedClients())
                {
                    m_server->SendTicUpdate(client.id, m_currentTic);
                }
            }
            else if (m_client)
            {
                // Client: Check if we're behind or ahead of the server
                int ticDifference = m_currentTic - m_client->GetServerTic();
                if (std::abs(ticDifference) > MAX_TIC_DIFFERENCE)
                {
                    // Adjust client tic to match server
                    m_currentTic = m_client->GetServerTic();
                }
            }
        }

        void NetworkManager::SynchronizeTics()
        {
            unsigned int currentTic = m_gameState.GetCurrentTic();
            for (const auto &client : m_server->GetConnectedClients())
            {
                if (client.lastAcknowledgedTic < currentTic - MAX_TIC_LAG)
                {
                    m_server->SendTicSync(client.id, currentTic);
                }
            }
        }

        void NetworkManager::ProcessPacketSequencing()
        {
            if (m_server)
            {
                for (const auto &client : m_server->GetConnectedClients())
                {
                    std::vector<uint32_t> missingPackets = client.connection.GetMissingPackets();
                    for (uint32_t packetId : missingPackets)
                    {
                        m_server->ResendPacket(client.id, packetId);
                    }
                    client.connection.AcknowledgePackets();
                }
            }
            else if (m_client)
            {
                std::vector<uint32_t> missingPackets = m_client->GetMissingPackets();
                for (uint32_t packetId : missingPackets)
                {
                    m_client->RequestPacketResend(packetId);
                }
                m_client->AcknowledgePackets();
            }
        }

        void NetworkManager::SendPacket(const Packet &packet)
        {
            packet.sequenceNumber = m_nextSequenceNumber++;
            if (m_server)
            {
                for (const auto &client : m_server->GetConnectedClients())
                {
                    m_server->SendPacket(client.id, packet);
                }
            }
            else if (m_client)
            {
                m_client->SendPacket(packet);
            }
            m_unacknowledgedPackets[packet.sequenceNumber] = packet;
        }

        void NetworkManager::HandlePacketAcknowledgement(uint32_t sequenceNumber)
        {
            m_unacknowledgedPackets.erase(sequenceNumber);
        }

        void NetworkManager::UpdatePlayerManagement()
        {
            if (m_server)
            {
                for (const auto &client : m_server->GetConnectedClients())
                {
                    if (client.lastActivityTime + PLAYER_TIMEOUT < GetCurrentTime())
                    {
                        m_server->DisconnectPlayer(client.id, "Timed out");
                    }
                }
                m_server->ProcessJoinRequests();
                m_server->UpdatePlayerStates();
                m_server->SyncPlayerData();
            }
            else if (m_client)
            {
                m_client->UpdateLocalPlayerState();
                m_client->SyncWithServer();
            }
        }

        void NetworkManager::ProcessErrorLogs()
        {
            std::vector<NetworkError> errors = m_errorLogger.GetRecentErrors();
            for (const auto &error : errors)
            {
                LogError(error);
                if (error.severity == ErrorSeverity::Critical)
                {
                    BroadcastErrorMessage(error);
                }
            }
        }

        void NetworkManager::ProcessConsoleMessages()
        {
            std::vector<ConsoleMessage> messages = m_consoleMessageQueue.GetPendingMessages();
            for (const auto &message : messages)
            {
                if (message.recipient == MessageRecipient::All)
                {
                    BroadcastConsoleMessage(message.content);
                }
                else
                {
                    SendConsoleMessage(message.recipientId, message.content);
                }
            }
        }

        void NetworkManager::UpdateGameLaunchSequence()
        {
            switch (m_launchState)
            {
            case LaunchState::WaitingForPlayers:
                if (m_server->GetConnectedClientCount() >= m_minPlayersToStart)
                {
                    m_launchState = LaunchState::CountdownStarted;
                    m_launchCountdown = LAUNCH_COUNTDOWN_DURATION;
                }
                break;
            case LaunchState::CountdownStarted:
                m_launchCountdown--;
                BroadcastLaunchCountdown(m_launchCountdown);
                if (m_launchCountdown <= 0)
                {
                    m_launchState = LaunchState::GameStarting;
                    StartGame();
                }
                break;
            case LaunchState::GameStarting:
                // Game is already starting, no action needed
                break;
            }
        }

        void NetworkManager::ProcessResendRequests()
        {
            for (const auto &client : m_server->GetConnectedClients())
            {
                std::vector<ResendRequest> requests = client.connection.GetPendingResendRequests();
                for (const auto &request : requests)
                {
                    m_server->ResendGameState(client.id, request.startTic, request.endTic);
                }
            }
        }

        void NetworkManager::UpdateCompatibilityMode()
        {
            if (m_compatibilityMode)
            {
                // Apply compatibility fixes
                m_gameState.ApplyCompatibilityFixes();
                for (const auto &client : m_server->GetConnectedClients())
                {
                    if (client.protocolVersion < CURRENT_PROTOCOL_VERSION)
                    {
                        SendCompatibilityUpdate(client.id);
                    }
                }
            }
        }

        void NetworkManager::SynchronizeClocks()
        {
            if (m_client)
            {
                m_client->SendClockSyncRequest();
            }
            if (m_server)
            {
                m_server->ProcessClockSyncRequests();
            }
        }

        void NetworkManager::UpdateClockSync()
        {
            if (GetCurrentTime() - m_lastClockSyncTime > CLOCK_SYNC_INTERVAL)
            {
                if (m_client)
                {
                    m_client->SendClockSyncRequest();
                }
                if (m_server)
                {
                    m_server->ProcessClockSyncRequests();
                }
                m_lastClockSyncTime = GetCurrentTime();
            }
        }

        void NetworkManager::UpdateNetworkDiagnostics()
        {
            if (m_client)
            {
                float ping = m_client->GetPing();
                float packetLoss = m_client->GetPacketLoss();
                m_networkDiagnostics->UpdatePing(m_client->GetClientId(), ping);
                m_networkDiagnostics->UpdatePacketLoss(m_client->GetClientId(), packetLoss);
            }

            if (m_server)
            {
                for (const auto &client : m_server->GetConnectedClients())
                {
                    float ping = client.GetPing();
                    float packetLoss = client.GetPacketLoss();
                    m_networkDiagnostics->UpdatePing(client.GetClientId(), ping);
                    m_networkDiagnostics->UpdatePacketLoss(client.GetClientId(), packetLoss);
                }
            }

            int bytesReceived = m_packetHandler->GetBytesReceived();
            int bytesSent = m_packetHandler->GetBytesSent();
            m_networkDiagnostics->UpdateBandwidthUsage(bytesReceived, bytesSent);
        }

        void NetworkManager::UpdateLagCompensation(double currentTime)
        {
            // Store current entity states for lag compensation
            for (const auto &entity : m_server->GetEntities())
            {
                m_lagCompensator->StoreEntityState(entity.id, entity.state, currentTime);
            }

            // Apply server-side rewind for hit detection
            double rewindTime = currentTime - m_server->GetAverageClientLatency();
            m_lagCompensator->ApplyServerSideRewind(currentTime, rewindTime);
        }

        void NetworkManager::UpdateEntityInterpolation(double currentTime)
        {
            const double INTERPOLATION_DELAY = 0.1; // 100ms interpolation delay

            for (auto &[entityId, interpolator] : m_entityInterpolators)
            {
                EntityState interpolatedState = interpolator.Interpolate(currentTime, INTERPOLATION_DELAY);
                // Apply the interpolated state to the entity in the game world
                ApplyEntityState(entityId, interpolatedState);
            }
        }

        void NetworkManager::ApplyEntityState(int entityId, const EntityState &state)
        {
            // Implement this function to apply the interpolated state to the entity in the game world
            // This might involve updating the entity's position, velocity, and rotation in the game engine
        }

        void NetworkManager::Shutdown()
        {
            DisconnectFromServer();
            m_client.reset();
            m_server.reset();
            m_packetHandler.reset();
        }

        void NetworkManager::ConnectToServer(const std::string &serverAddress)
        {
            if (m_client)
            {
                m_client->Connect(serverAddress);
                m_connectionState = ConnectionState::Connecting;
            }
        }

        void NetworkManager::DisconnectFromServer()
        {
            if (m_client)
            {
                m_client->Disconnect();
                m_connectionState = ConnectionState::Disconnected;
            }
        }

        void NetworkManager::SendPacket(const std::vector<uint8_t> &data)
        {
            if (m_client && m_connectionState == ConnectionState::Connected)
            {
                std::vector<uint8_t> compressedData = CompressData(data);
                std::vector<uint8_t> encryptedData = EncryptData(compressedData);
                Packet packet = m_packetHandler->CreatePacket(encryptedData);
                m_client->SendPacket(packet);
            }
        }

        void NetworkManager::ReceivePackets()
        {
            if (m_client)
            {
                std::vector<Packet> receivedPackets = m_client->ReceivePackets();
                for (const auto &packet : receivedPackets)
                {
                    std::vector<uint8_t> decryptedData = DecryptData(packet.data);
                    std::vector<uint8_t> decompressedData = DecompressData(decryptedData);
                    m_packetHandler->ProcessPacket(Packet(packet.sequence, packet.ack, packet.ack_bits, decompressedData));
                }
            }
        }

        std::vector<uint8_t> NetworkManager::CompressData(const std::vector<uint8_t> &data)
        {
            return m_compressor->Compress(data);
        }

        std::vector<uint8_t> NetworkManager::EncryptData(const std::vector<uint8_t> &data)
        {
            return m_securityManager->EncryptData(data);
        }

        std::vector<uint8_t> NetworkManager::DecryptData(const std::vector<uint8_t> &encryptedData)
        {
            return m_securityManager->DecryptData(encryptedData);
        }

        void NetworkManager::SendPacket(const std::vector<uint8_t> &data)
        {
            if (m_client && m_connectionState == ConnectionState::Connected)
            {
                std::vector<uint8_t> compressedData = CompressData(data);
                std::vector<uint8_t> encryptedData = EncryptData(compressedData);
                Packet packet = m_packetHandler->CreatePacket(encryptedData);
                m_client->SendPacket(packet);
            }
        }

        void NetworkManager::ReceivePackets()
        {
            if (m_client)
            {
                std::vector<Packet> receivedPackets = m_client->ReceivePackets();
                for (const auto &packet : receivedPackets)
                {
                    std::vector<uint8_t> decryptedData = DecryptData(packet.data);
                    std::vector<uint8_t> decompressedData = DecompressData(decryptedData);
                    m_packetHandler->ProcessPacket(Packet(packet.sequence, packet.ack, packet.ack_bits, decompressedData));
                }
            }
        }

        std::vector<uint8_t> NetworkManager::DecompressData(const std::vector<uint8_t> &compressedData)
        {
            return m_compressor->Decompress(compressedData);
        }

        ConnectionState NetworkManager::GetConnectionState() const
        {
            return m_connectionState;
        }

        void NetworkManager::SetCompatibilityMode(bool enabled)
        {
            m_compatibilityMode = enabled;
            // Add any necessary logic to switch between modern and legacy netcode
        }

        bool NetworkManager::IsCompatibilityModeEnabled() const
        {
            return m_compatibilityMode;
        }

        void NetworkManager::UpdateTestGameState(const TestGameState &newState)
        {
            m_testGameState = newState;
            // In a real implementation, you would send this update to connected clients
            if (m_server)
            {
                std::vector<uint8_t> stateData = SerializeTestGameState(newState);
                for (const auto &client : m_server->GetConnectedClients())
                {
                    m_server->SendPacket(client.id, stateData);
                }
            }
        }

        NetworkManager::TestGameState NetworkManager::GetTestGameState() const
        {
            return m_testGameState;
        }

        std::vector<uint8_t> NetworkManager::SerializeTestGameState(const TestGameState &state)
        {
            std::vector<uint8_t> data;
            data.reserve(sizeof(int) + state.playerPositions.size() * sizeof(float));

            // Serialize playerCount
            const uint8_t *countBytes = reinterpret_cast<const uint8_t *>(&state.playerCount);
            data.insert(data.end(), countBytes, countBytes + sizeof(int));

            // Serialize playerPositions
            for (float pos : state.playerPositions)
            {
                const uint8_t *posBytes = reinterpret_cast<const uint8_t *>(&pos);
                data.insert(data.end(), posBytes, posBytes + sizeof(float));
            }

            return data;
        }

    } // namespace Network
} // namespace GZDoom
void NetworkManager::UpdateNetworkDiagnostics()
{
    if (m_client)
    {
        float ping = m_client->GetPing();
        float packetLoss = m_client->GetPacketLoss();
        m_networkDiagnostics->UpdatePing(m_client->GetClientId(), ping);
        m_networkDiagnostics->UpdatePacketLoss(m_client->GetClientId(), packetLoss);
    }

    if (m_server)
    {
        for (const auto &client : m_server->GetConnectedClients())
        {
            float ping = client.GetPing();
            float packetLoss = client.GetPacketLoss();
            m_networkDiagnostics->UpdatePing(client.GetClientId(), ping);
            m_networkDiagnostics->UpdatePacketLoss(client.GetClientId(), packetLoss);
        }
    }

    int bytesReceived = m_packetHandler->GetBytesReceived();
    int bytesSent = m_packetHandler->GetBytesSent();
    m_networkDiagnostics->UpdateBandwidthUsage(bytesReceived, bytesSent);
}
void NetworkManager::SynchronizeClocks()
{
    if (m_client)
    {
        m_client->SendClockSyncRequest();
    }
    if (m_server)
    {
        m_server->ProcessClockSyncRequests();
    }
}
void NetworkManager::SynchronizeGameSettings()
{
    if (m_server)
    {
        // Server sends game settings to all clients
        for (const auto &client : m_server->GetConnectedClients())
        {
            m_server->SendGameSettings(client.id, m_gameSettings);
        }
    }
    else if (m_client)
    {
        // Client requests game settings from server
        m_client->RequestGameSettings();
    }
}

void NetworkManager::UpdateGameSettings(const GameSettings &newSettings)
{
    m_gameSettings = newSettings;
    if (m_server)
    {
        // Notify all clients about the updated settings
        for (const auto &client : m_server->GetConnectedClients())
        {
            m_server->SendGameSettings(client.id, m_gameSettings);
        }
    }
}

void NetworkManager::UpdateGameSettings(const GameSettings &newSettings)
{
    m_gameSettings = newSettings;
    if (m_server)
    {
        // Notify all clients about the updated settings
        for (const auto &client : m_server->GetConnectedClients())
        {
            m_server->SendGameSettings(client.id, m_gameSettings);
        }
    }
}
void NetworkManager::UpdateClockSync()
{
    if (GetCurrentTime() - m_lastClockSyncTime > CLOCK_SYNC_INTERVAL)
    {
        if (m_client)
        {
            m_client->SendClockSyncRequest();
        }
        if (m_server)
        {
            m_server->ProcessClockSyncRequests();
        }
        m_lastClockSyncTime = GetCurrentTime();
    }
}
void NetworkManager::UpdateClockSync()
{
    if (GetCurrentTime() - m_lastClockSyncTime > CLOCK_SYNC_INTERVAL)
    {
        if (m_client)
        {
            m_client->SendClockSyncRequest();
        }
        if (m_server)
        {
            m_server->ProcessClockSyncRequests();
        }
        m_lastClockSyncTime = GetCurrentTime();
    }
}
void NetworkManager::UpdateTicSync()
{
    m_currentTic++;
    if (m_server)
    {
        // Server: Send current tic to all clients
        for (const auto &client : m_server->GetConnectedClients())
        {
            m_server->SendTicUpdate(client.id, m_currentTic);
        }
    }
    else if (m_client)
    {
        // Client: Check if we're behind or ahead of the server
        int ticDifference = m_currentTic - m_client->GetServerTic();
        if (std::abs(ticDifference) > MAX_TIC_DIFFERENCE)
        {
            // Adjust client tic to match server
            m_currentTic = m_client->GetServerTic();
        }
    }
}
