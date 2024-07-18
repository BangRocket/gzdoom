#pragma once

#include <vector>
#include <cstdint>

namespace GZDoom
{
    namespace Network
    {

        class NetworkManager;

        enum class ACSPacketType : uint8_t
        {
            VariableSync,
            FunctionExecution
        };

        class ACSNetworkBridge
        {
        public:
            ACSNetworkBridge(NetworkManager *networkManager);

            void SyncACSVariable(int scriptNum, int varNum, int value);
            void ExecuteACSFunction(int scriptNum, const std::vector<int> &args);
            void HandleACSPacket(const std::vector<uint8_t> &packet);

        private:
            NetworkManager *m_networkManager;

            void HandleVariableSync(const std::vector<uint8_t> &packet, size_t &offset);
            void HandleFunctionExecution(const std::vector<uint8_t> &packet, size_t &offset);

            void AppendInt(std::vector<uint8_t> &packet, int value);
            int ExtractInt(const std::vector<uint8_t> &packet, size_t &offset);
        };

    } // namespace Network
} // namespace GZDoom
