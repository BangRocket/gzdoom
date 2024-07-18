#include "acs_network_bridge.h"
#include "network_manager.h"
#include <iostream>

namespace GZDoom
{
    namespace Network
    {

        ACSNetworkBridge::ACSNetworkBridge(NetworkManager *networkManager)
            : m_networkManager(networkManager) {}

        void ACSNetworkBridge::SyncACSVariable(int scriptNum, int varNum, int value)
        {
            // Create a packet to sync the ACS variable
            std::vector<uint8_t> packet;
            packet.push_back(static_cast<uint8_t>(PacketType::ACSSync));
            packet.push_back(static_cast<uint8_t>(ACSPacketType::VariableSync));

            // Add script number, variable number, and value to the packet
            AppendInt(packet, scriptNum);
            AppendInt(packet, varNum);
            AppendInt(packet, value);

            // Send the packet through the network manager
            m_networkManager->SendPacket(packet);
        }

        void ACSNetworkBridge::ExecuteACSFunction(int scriptNum, const std::vector<int> &args)
        {
            // Create a packet to execute an ACS function
            std::vector<uint8_t> packet;
            packet.push_back(static_cast<uint8_t>(PacketType::ACSSync));
            packet.push_back(static_cast<uint8_t>(ACSPacketType::FunctionExecution));

            // Add script number and argument count to the packet
            AppendInt(packet, scriptNum);
            AppendInt(packet, static_cast<int>(args.size()));

            // Add all arguments to the packet
            for (int arg : args)
            {
                AppendInt(packet, arg);
            }

            // Send the packet through the network manager
            m_networkManager->SendPacket(packet);
        }

        void ACSNetworkBridge::HandleACSPacket(const std::vector<uint8_t> &packet)
        {
            if (packet.size() < 2)
            {
                std::cerr << "Invalid ACS packet received" << std::endl;
                return;
            }

            ACSPacketType packetType = static_cast<ACSPacketType>(packet[1]);
            size_t offset = 2;

            switch (packetType)
            {
            case ACSPacketType::VariableSync:
                HandleVariableSync(packet, offset);
                break;
            case ACSPacketType::FunctionExecution:
                HandleFunctionExecution(packet, offset);
                break;
            default:
                std::cerr << "Unknown ACS packet type received" << std::endl;
            }
        }

        void ACSNetworkBridge::HandleVariableSync(const std::vector<uint8_t> &packet, size_t &offset)
        {
            if (packet.size() - offset < 12)
            {
                std::cerr << "Invalid ACS variable sync packet received" << std::endl;
                return;
            }

            int scriptNum = ExtractInt(packet, offset);
            int varNum = ExtractInt(packet, offset);
            int value = ExtractInt(packet, offset);

            // TODO: Implement the actual variable sync in the ACS VM
            std::cout << "ACS Variable Sync: Script " << scriptNum << ", Variable " << varNum << " = " << value << std::endl;
        }

        void ACSNetworkBridge::HandleFunctionExecution(const std::vector<uint8_t> &packet, size_t &offset)
        {
            if (packet.size() - offset < 8)
            {
                std::cerr << "Invalid ACS function execution packet received" << std::endl;
                return;
            }

            int scriptNum = ExtractInt(packet, offset);
            int argCount = ExtractInt(packet, offset);

            if (packet.size() - offset < static_cast<size_t>(argCount * 4))
            {
                std::cerr << "Invalid ACS function execution packet: not enough arguments" << std::endl;
                return;
            }

            std::vector<int> args;
            for (int i = 0; i < argCount; ++i)
            {
                args.push_back(ExtractInt(packet, offset));
            }

            // TODO: Implement the actual function execution in the ACS VM
            std::cout << "ACS Function Execution: Script " << scriptNum << " with " << argCount << " arguments" << std::endl;
        }

        void ACSNetworkBridge::AppendInt(std::vector<uint8_t> &packet, int value)
        {
            packet.push_back(static_cast<uint8_t>((value >> 24) & 0xFF));
            packet.push_back(static_cast<uint8_t>((value >> 16) & 0xFF));
            packet.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
            packet.push_back(static_cast<uint8_t>(value & 0xFF));
        }

        int ACSNetworkBridge::ExtractInt(const std::vector<uint8_t> &packet, size_t &offset)
        {
            int value = (packet[offset] << 24) | (packet[offset + 1] << 16) | (packet[offset + 2] << 8) | packet[offset + 3];
            offset += 4;
            return value;
        }

    } // namespace Network
} // namespace GZDoom
