#include "zscript_network_bridge.h"
#include "network_manager.h"
#include <iostream>

namespace GZDoom
{
    namespace Network
    {

        ZScriptNetworkBridge::ZScriptNetworkBridge(NetworkManager *networkManager)
            : m_networkManager(networkManager) {}

        void ZScriptNetworkBridge::SyncZScriptVariable(const std::string &varName, const std::vector<uint8_t> &value)
        {
            std::vector<uint8_t> packet;
            packet.push_back(static_cast<uint8_t>(PacketType::ZScriptSync));
            packet.push_back(static_cast<uint8_t>(ZScriptPacketType::VariableSync));

            AppendString(packet, varName);
            packet.insert(packet.end(), value.begin(), value.end());

            m_networkManager->SendPacket(packet);
        }

        void ZScriptNetworkBridge::ExecuteZScriptFunction(const std::string &funcName, const std::vector<std::vector<uint8_t>> &args)
        {
            std::vector<uint8_t> packet;
            packet.push_back(static_cast<uint8_t>(PacketType::ZScriptSync));
            packet.push_back(static_cast<uint8_t>(ZScriptPacketType::FunctionExecution));

            AppendString(packet, funcName);
            packet.push_back(static_cast<uint8_t>(args.size()));

            for (const auto &arg : args)
            {
                packet.insert(packet.end(), arg.begin(), arg.end());
            }

            m_networkManager->SendPacket(packet);
        }

        void ZScriptNetworkBridge::HandleZScriptPacket(const std::vector<uint8_t> &packet)
        {
            if (packet.size() < 2)
            {
                std::cerr << "Invalid ZScript packet received" << std::endl;
                return;
            }

            ZScriptPacketType packetType = static_cast<ZScriptPacketType>(packet[1]);
            size_t offset = 2;

            switch (packetType)
            {
            case ZScriptPacketType::VariableSync:
                HandleVariableSync(packet, offset);
                break;
            case ZScriptPacketType::FunctionExecution:
                HandleFunctionExecution(packet, offset);
                break;
            default:
                std::cerr << "Unknown ZScript packet type received" << std::endl;
            }
        }

        void ZScriptNetworkBridge::HandleVariableSync(const std::vector<uint8_t> &packet, size_t &offset)
        {
            std::string varName = ExtractString(packet, offset);
            std::vector<uint8_t> value(packet.begin() + offset, packet.end());

            // TODO: Implement the actual variable sync in the ZScript VM
            std::cout << "ZScript Variable Sync: " << varName << " = [" << value.size() << " bytes]" << std::endl;
        }

        void ZScriptNetworkBridge::HandleFunctionExecution(const std::vector<uint8_t> &packet, size_t &offset)
        {
            std::string funcName = ExtractString(packet, offset);
            uint8_t argCount = packet[offset++];

            std::vector<std::vector<uint8_t>> args;
            for (int i = 0; i < argCount; ++i)
            {
                size_t argSize = packet.size() - offset;
                args.emplace_back(packet.begin() + offset, packet.end());
                offset += argSize;
            }

            // TODO: Implement the actual function execution in the ZScript VM
            std::cout << "ZScript Function Execution: " << funcName << " with " << argCount << " arguments" << std::endl;
        }

        void ZScriptNetworkBridge::AppendString(std::vector<uint8_t> &packet, const std::string &str)
        {
            packet.push_back(static_cast<uint8_t>(str.length()));
            packet.insert(packet.end(), str.begin(), str.end());
        }

        std::string ZScriptNetworkBridge::ExtractString(const std::vector<uint8_t> &packet, size_t &offset)
        {
            uint8_t length = packet[offset++];
            std::string str(packet.begin() + offset, packet.begin() + offset + length);
            offset += length;
            return str;
        }

    } // namespace Network
} // namespace GZDoom
