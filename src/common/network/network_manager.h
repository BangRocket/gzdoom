#pragma once

#include "network_architecture.h"
#include "packet_handler.h"
#include "server_reconciliation.h"
#include "entity_interpolation.h"
#include "network_compression.h"
#include "acs_network_bridge.h"
#include "zscript_network_bridge.h"
#include "lag_compensation.h"
#include "network_security.h"
#include "network_diagnostics.h"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

namespace GZDoom
{
    namespace Network
    {

        class Client;
        class Server;

        /**
         * @class NetworkManager
         * @brief Manages all network operations for GZDoom's modern netcode implementation.
         */
        class NetworkManager
        {
        public:
            NetworkManager();
            ~NetworkManager();

            void Initialize();
            void Update();
            void Shutdown();
            bool CheckModCompatibility(const std::vector<ModInfo> &clientMods, std::string &errorMessage);
            void SetCompatibilityMode(bool enabled);
            bool IsCompatibilityModeEnabled() const;

            // New methods
            bool IsServer() const { return m_server != nullptr; }
            bool IsClient() const { return m_client != nullptr; }
            void StartServer(uint16_t port);
            void ConnectToServer(const std::string &serverAddress, uint16_t port);
            void DisconnectFromServer();
            void KickPlayer(int playerId, const std::string &reason);
            void BroadcastMessage(const std::string &message);
            void SendPrivateMessage(int playerId, const std::string &message);

        private:
            bool m_compatibilityMode;

            void DetectNetworkMode();
            void SwitchToLegacyMode();
            void SwitchToModernMode();
            void SendPacket(const std::vector<uint8_t> &data);
            void ReceivePackets();

            ConnectionState GetConnectionState() const;

            ACSNetworkBridge &GetACSNetworkBridge() { return *m_acsNetworkBridge; }
            ZScriptNetworkBridge &GetZScriptNetworkBridge() { return *m_zscriptNetworkBridge; }
            NetworkStateManager &GetNetworkStateManager() { return *m_networkStateManager; }
            LagCompensator &GetLagCompensator() { return *m_lagCompensator; }
            SecurityManager &GetSecurityManager() { return *m_securityManager; }
            NetworkDiagnostics &GetNetworkDiagnostics() { return *m_networkDiagnostics; }

        private:
            std::unique_ptr<Client> m_client;
            std::unique_ptr<Server> m_server;
            std::unique_ptr<PacketHandler> m_packetHandler;
            std::unique_ptr<ServerReconciliation> m_serverReconciliation;
            std::unique_ptr<Compressor> m_compressor;
            std::unique_ptr<ACSNetworkBridge> m_acsNetworkBridge;
            std::unique_ptr<ZScriptNetworkBridge> m_zscriptNetworkBridge;
            std::unique_ptr<NetworkStateManager> m_networkStateManager;
            std::unique_ptr<LagCompensator> m_lagCompensator;
            std::unique_ptr<SecurityManager> m_securityManager;
            std::unique_ptr<NetworkDiagnostics> m_networkDiagnostics;
            std::unique_ptr<ModSyncManager> m_modSyncManager;
            std::unordered_map<int, EntityInterpolator> m_entityInterpolators;
            ConnectionState m_connectionState;

            void UpdateEntityInterpolation(double currentTime);
            void UpdateLagCompensation(double currentTime);
            void UpdateNetworkDiagnostics();
            std::vector<uint8_t> CompressData(const std::vector<uint8_t> &data);
            std::vector<uint8_t> DecompressData(const std::vector<uint8_t> &compressedData);
            std::vector<uint8_t> EncryptData(const std::vector<uint8_t> &data);
            std::vector<uint8_t> DecryptData(const std::vector<uint8_t> &encryptedData);

            void SynchronizeClocks();
            void UpdateClockSync();
            void ProcessPacketSequencing();
            void HandlePacketAcknowledgement(uint32_t sequenceNumber);

            // Simple networked game state for testing
            struct TestGameState
            {
                int playerCount;
                std::vector<float> playerPositions;
            };

            void UpdateTestGameState(const TestGameState &newState);
            TestGameState GetTestGameState() const;

        private:
            TestGameState m_testGameState;
            double m_lastClockSyncTime;
            uint32_t m_nextSequenceNumber;
            std::unordered_map<uint32_t, Packet> m_unacknowledgedPackets;
        };

    } // namespace Network
} // namespace GZDoom
