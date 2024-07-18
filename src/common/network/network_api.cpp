#include "network_api.h"
#include "network_manager.h"
#include <iostream>
#include <cstring>

namespace GZDoom
{
    namespace Network
    {

        NetworkAPI::NetworkAPI(NetworkManager *networkManager) : m_networkManager(networkManager) {}

        NetworkAPI::~NetworkAPI() {}

        bool NetworkAPI::SendMessageToAll(const std::string &message)
        {
            if (!ValidateInput("SendMessageToAll", SerializeString(message)))
            {
                return false;
            }

            // Use the NetworkManager to send the message to all clients
            return m_networkManager->BroadcastMessage(message);
        }

        bool NetworkAPI::SendMessageToPlayer(int playerId, const std::string &message)
        {
            if (!ValidateInput("SendMessageToPlayer", SerializeString(message)))
            {
                return false;
            }

            // Use the NetworkManager to send the message to a specific player
            return m_networkManager->SendMessageToPlayer(playerId, message);
        }

        bool NetworkAPI::SyncVariable(const std::string &varName, const std::vector<uint8_t> &value)
        {
            if (!ValidateInput("SyncVariable", value))
            {
                return false;
            }

            // Use the NetworkManager to synchronize the variable across the network
            return m_networkManager->SyncNetworkVariable(varName, value);
        }

        bool NetworkAPI::RegisterNetworkEvent(const std::string &eventName, std::function<void(const std::vector<uint8_t> &)> callback)
        {
            if (m_eventCallbacks.find(eventName) != m_eventCallbacks.end())
            {
                std::cerr << "Event '" << eventName << "' is already registered." << std::endl;
                return false;
            }

            m_eventCallbacks[eventName] = callback;
            return true;
        }

        bool NetworkAPI::TriggerNetworkEvent(const std::string &eventName, const std::vector<uint8_t> &data)
        {
            if (!ValidateInput("TriggerNetworkEvent", data))
            {
                return false;
            }

            // Use the NetworkManager to trigger the event across the network
            return m_networkManager->TriggerNetworkEvent(eventName, data);
        }

        std::vector<uint8_t> NetworkAPI::SerializeInt(int value)
        {
            std::vector<uint8_t> result(sizeof(int));
            std::memcpy(result.data(), &value, sizeof(int));
            return result;
        }

        std::vector<uint8_t> NetworkAPI::SerializeFloat(float value)
        {
            std::vector<uint8_t> result(sizeof(float));
            std::memcpy(result.data(), &value, sizeof(float));
            return result;
        }

        std::vector<uint8_t> NetworkAPI::SerializeString(const std::string &value)
        {
            std::vector<uint8_t> result(value.size() + 1);
            std::memcpy(result.data(), value.c_str(), value.size() + 1);
            return result;
        }

        int NetworkAPI::DeserializeInt(const std::vector<uint8_t> &data)
        {
            if (data.size() < sizeof(int))
            {
                throw std::runtime_error("Not enough data to deserialize int");
            }
            int result;
            std::memcpy(&result, data.data(), sizeof(int));
            return result;
        }

        float NetworkAPI::DeserializeFloat(const std::vector<uint8_t> &data)
        {
            if (data.size() < sizeof(float))
            {
                throw std::runtime_error("Not enough data to deserialize float");
            }
            float result;
            std::memcpy(&result, data.data(), sizeof(float));
            return result;
        }

        std::string NetworkAPI::DeserializeString(const std::vector<uint8_t> &data)
        {
            return std::string(reinterpret_cast<const char *>(data.data()));
        }

        bool NetworkAPI::ValidateInput(const std::string &functionName, const std::vector<uint8_t> &data)
        {
            if (data.empty())
            {
                std::cerr << "Error in " << functionName << ": Empty data" << std::endl;
                return false;
            }
            if (data.size() > 1024)
            { // Example size limit
                std::cerr << "Error in " << functionName << ": Data too large" << std::endl;
                return false;
            }
            return true;
        }

    } // namespace Network
} // namespace GZDoom
#include "network_api.h"
#include "network_manager.h"
#include <iostream>
#include <cstring>

namespace GZDoom
{
    namespace Network
    {

        NetworkAPI::NetworkAPI(NetworkManager *networkManager) : m_networkManager(networkManager) {}

        NetworkAPI::~NetworkAPI() {}

        bool NetworkAPI::SendMessageToAll(const std::string &message)
        {
            if (!ValidateInput("SendMessageToAll", SerializeString(message)))
            {
                return false;
            }

            // Use the NetworkManager to send the message to all clients
            return m_networkManager->BroadcastMessage(message);
        }

        bool NetworkAPI::SendMessageToPlayer(int playerId, const std::string &message)
        {
            if (!ValidateInput("SendMessageToPlayer", SerializeString(message)))
            {
                return false;
            }

            // Use the NetworkManager to send the message to a specific player
            return m_networkManager->SendMessageToPlayer(playerId, message);
        }

        bool NetworkAPI::SyncVariable(const std::string &varName, const std::vector<uint8_t> &value)
        {
            if (!ValidateInput("SyncVariable", value))
            {
                return false;
            }

            // Use the NetworkManager to synchronize the variable across the network
            return m_networkManager->SyncNetworkVariable(varName, value);
        }

        bool NetworkAPI::RegisterNetworkEvent(const std::string &eventName, std::function<void(const std::vector<uint8_t> &)> callback)
        {
            if (m_eventCallbacks.find(eventName) != m_eventCallbacks.end())
            {
                std::cerr << "Event '" << eventName << "' is already registered." << std::endl;
                return false;
            }

            m_eventCallbacks[eventName] = callback;
            return true;
        }

        bool NetworkAPI::TriggerNetworkEvent(const std::string &eventName, const std::vector<uint8_t> &data)
        {
            if (!ValidateInput("TriggerNetworkEvent", data))
            {
                return false;
            }

            // Use the NetworkManager to trigger the event across the network
            return m_networkManager->TriggerNetworkEvent(eventName, data);
        }

        std::vector<uint8_t> NetworkAPI::SerializeInt(int value)
        {
            std::vector<uint8_t> result(sizeof(int));
            std::memcpy(result.data(), &value, sizeof(int));
            return result;
        }

        std::vector<uint8_t> NetworkAPI::SerializeFloat(float value)
        {
            std::vector<uint8_t> result(sizeof(float));
            std::memcpy(result.data(), &value, sizeof(float));
            return result;
        }

        std::vector<uint8_t> NetworkAPI::SerializeString(const std::string &value)
        {
            std::vector<uint8_t> result(value.size() + 1);
            std::memcpy(result.data(), value.c_str(), value.size() + 1);
            return result;
        }

        int NetworkAPI::DeserializeInt(const std::vector<uint8_t> &data)
        {
            if (data.size() < sizeof(int))
            {
                throw std::runtime_error("Not enough data to deserialize int");
            }
            int result;
            std::memcpy(&result, data.data(), sizeof(int));
            return result;
        }

        float NetworkAPI::DeserializeFloat(const std::vector<uint8_t> &data)
        {
            if (data.size() < sizeof(float))
            {
                throw std::runtime_error("Not enough data to deserialize float");
            }
            float result;
            std::memcpy(&result, data.data(), sizeof(float));
            return result;
        }

        std::string NetworkAPI::DeserializeString(const std::vector<uint8_t> &data)
        {
            return std::string(reinterpret_cast<const char *>(data.data()));
        }

        bool NetworkAPI::ValidateInput(const std::string &functionName, const std::vector<uint8_t> &data)
        {
            if (data.empty())
            {
                std::cerr << "Error in " << functionName << ": Empty data" << std::endl;
                return false;
            }
            if (data.size() > 1024)
            { // Example size limit
                std::cerr << "Error in " << functionName << ": Data too large" << std::endl;
                return false;
            }
            return true;
        }

    } // namespace Network
} // namespace GZDoom
