#include "mod_compatibility.h"
#include <algorithm>
#include <sstream>
#include <iostream>

namespace GZDoom
{
    namespace Network
    {

        ModCompatibilityChecker::ModCompatibilityChecker() {}

        ModCompatibilityChecker::~ModCompatibilityChecker() {}

        void ModCompatibilityChecker::AddMod(const ModInfo &mod)
        {
            m_serverMods[mod.name] = mod;
        }

        bool ModCompatibilityChecker::CheckCompatibility(const std::vector<ModInfo> &clientMods, std::string &errorMessage)
        {
            for (const auto &clientMod : clientMods)
            {
                auto serverModIt = m_serverMods.find(clientMod.name);
                if (serverModIt == m_serverMods.end())
                {
                    errorMessage = "Server is missing mod: " + clientMod.name;
                    return false;
                }

                if (!CheckVersionCompatibility(serverModIt->second.version, clientMod.version))
                {
                    errorMessage = "Version mismatch for mod: " + clientMod.name;
                    return false;
                }

                if (!CheckDependencies(clientMod))
                {
                    errorMessage = "Dependency missing for mod: " + clientMod.name;
                    return false;
                }
            }

            return true;
        }

        std::vector<ModInfo> ModCompatibilityChecker::GetMissingMods(const std::vector<ModInfo> &clientMods)
        {
            std::vector<ModInfo> missingMods;
            for (const auto &serverMod : m_serverMods)
            {
                auto it = std::find_if(clientMods.begin(), clientMods.end(),
                                       [&](const ModInfo &clientMod)
                                       { return clientMod.name == serverMod.first; });
                if (it == clientMods.end())
                {
                    missingMods.push_back(serverMod.second);
                }
            }
            return missingMods;
        }

        bool ModCompatibilityChecker::SynchronizeMods(const std::vector<ModInfo> &missingMods)
        {
            // This is a placeholder implementation. In a real-world scenario, this would involve
            // downloading missing mods from the server or a centralized repository.
            for (const auto &mod : missingMods)
            {
                // Simulate mod download and installation
                std::cout << "Downloading and installing mod: " << mod.name << " (version " << mod.version << ")" << std::endl;
            }
            return true;
        }

        bool ModCompatibilityChecker::CheckVersionCompatibility(const std::string &serverVersion, const std::string &clientVersion)
        {
            // Implement a more sophisticated version comparison
            std::vector<int> serverVersionParts;
            std::vector<int> clientVersionParts;

            std::istringstream serverStream(serverVersion);
            std::istringstream clientStream(clientVersion);
            std::string part;

            while (std::getline(serverStream, part, '.'))
            {
                serverVersionParts.push_back(std::stoi(part));
            }

            while (std::getline(clientStream, part, '.'))
            {
                clientVersionParts.push_back(std::stoi(part));
            }

            // Compare version numbers
            for (size_t i = 0; i < std::min(serverVersionParts.size(), clientVersionParts.size()); ++i)
            {
                if (serverVersionParts[i] != clientVersionParts[i])
                {
                    return serverVersionParts[i] <= clientVersionParts[i];
                }
            }

            // If all compared parts are equal, the version with more parts is considered newer
            return serverVersionParts.size() <= clientVersionParts.size();
        }

        bool ModCompatibilityChecker::CheckDependencies(const ModInfo &mod)
        {
            for (const auto &dependency : mod.dependencies)
            {
                if (m_serverMods.find(dependency) == m_serverMods.end())
                {
                    return false;
                }
            }
            return true;
        }

    } // namespace Network
} // namespace GZDoom
