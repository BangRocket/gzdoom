#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include <unordered_map>

namespace GZDoom
{
    namespace Network
    {

        class NetworkManager;

        enum class ZScriptPacketType : uint8_t
        {
            VariableSync,
            FunctionExecution
        };

        class ZScriptNetworkBridge
        {
        public:
            ZScriptNetworkBridge(NetworkManager *networkManager);

            void SyncZScriptVariable(const std::string &varName, const std::vector<uint8_t> &value);
            void ExecuteZScriptFunction(const std::string &funcName, const std::vector<std::vector<uint8_t>> &args);
            void HandleZScriptPacket(const std::vector<uint8_t> &packet);

        private:
            NetworkManager *m_networkManager;

            void HandleVariableSync(const std::vector<uint8_t> &packet, size_t &offset);
            void HandleFunctionExecution(const std::vector<uint8_t> &packet, size_t &offset);

            void AppendString(std::vector<uint8_t> &packet, const std::string &str);
            std::string ExtractString(const std::vector<uint8_t> &packet, size_t &offset);
        };

    } // namespace Network
} // namespace GZDoom
