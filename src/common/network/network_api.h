#pragma once

#include <string>
#include <vector>
#include <functional>

namespace GZDoom
{
    namespace Network
    {

        class NetworkManager;

        /**
         * @class NetworkAPI
         * @brief Provides a high-level interface for modders to interact with GZDoom's netcode.
         */
        class NetworkAPI
        {
        public:
            NetworkAPI(NetworkManager *networkManager);
            ~NetworkAPI();

            /**
             * @brief Sends a message to all connected clients.
             * @param message The message to send.
             * @return True if the message was sent successfully, false otherwise.
             */
            bool SendMessageToAll(const std::string &message);

            /**
             * @brief Sends a message to a specific player.
             * @param playerId The ID of the player to send the message to.
             * @param message The message to send.
             * @return True if the message was sent successfully, false otherwise.
             */
            bool SendMessageToPlayer(int playerId, const std::string &message);

            /**
             * @brief Synchronizes a variable across the network.
             * @param varName The name of the variable to synchronize.
             * @param value The value of the variable.
             * @return True if the variable was synchronized successfully, false otherwise.
             */
            bool SyncVariable(const std::string &varName, const std::vector<uint8_t> &value);

            /**
             * @brief Registers a network event callback.
             * @param eventName The name of the event to register.
             * @param callback The function to call when the event is triggered.
             * @return True if the event was registered successfully, false otherwise.
             */
            bool RegisterNetworkEvent(const std::string &eventName, std::function<void(const std::vector<uint8_t> &)> callback);

            /**
             * @brief Triggers a network event.
             * @param eventName The name of the event to trigger.
             * @param data The data to send with the event.
             * @return True if the event was triggered successfully, false otherwise.
             */
            bool TriggerNetworkEvent(const std::string &eventName, const std::vector<uint8_t> &data);

            // Helper functions
            std::vector<uint8_t> SerializeInt(int value);
            std::vector<uint8_t> SerializeFloat(float value);
            std::vector<uint8_t> SerializeString(const std::string &value);

            int DeserializeInt(const std::vector<uint8_t> &data);
            float DeserializeFloat(const std::vector<uint8_t> &data);
            std::string DeserializeString(const std::vector<uint8_t> &data);

        private:
            NetworkManager *m_networkManager;
            std::unordered_map<std::string, std::function<void(const std::vector<uint8_t> &)>> m_eventCallbacks;

            bool ValidateInput(const std::string &functionName, const std::vector<uint8_t> &data);
        };

    } // namespace Network
} // namespace GZDoom
#pragma once

#include <string>
#include <vector>
#include <functional>

namespace GZDoom
{
    namespace Network
    {

        class NetworkManager;

        class NetworkAPI
        {
        public:
            NetworkAPI(NetworkManager *networkManager);
            ~NetworkAPI();

            // High-level networking functions for modders
            bool SendMessageToAll(const std::string &message);
            bool SendMessageToPlayer(int playerId, const std::string &message);
            bool SyncVariable(const std::string &varName, const std::vector<uint8_t> &value);
            bool RegisterNetworkEvent(const std::string &eventName, std::function<void(const std::vector<uint8_t> &)> callback);
            bool TriggerNetworkEvent(const std::string &eventName, const std::vector<uint8_t> &data);

            // Helper functions
            std::vector<uint8_t> SerializeInt(int value);
            std::vector<uint8_t> SerializeFloat(float value);
            std::vector<uint8_t> SerializeString(const std::string &value);

            int DeserializeInt(const std::vector<uint8_t> &data);
            float DeserializeFloat(const std::vector<uint8_t> &data);
            std::string DeserializeString(const std::vector<uint8_t> &data);

        private:
            NetworkManager *m_networkManager;
            std::unordered_map<std::string, std::function<void(const std::vector<uint8_t> &)>> m_eventCallbacks;

            bool ValidateInput(const std::string &functionName, const std::vector<uint8_t> &data);
        };

    } // namespace Network
} // namespace GZDoom
