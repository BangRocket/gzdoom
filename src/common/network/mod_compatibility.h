#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace GZDoom
{
    namespace Network
    {

        struct ModInfo
        {
            std::string name;
            std::string version;
            std::vector<std::string> dependencies;
        };

        class ModCompatibilityChecker
        {
        public:
            ModCompatibilityChecker();
            ~ModCompatibilityChecker();

            void AddMod(const ModInfo &mod);
            bool CheckCompatibility(const std::vector<ModInfo> &clientMods, std::string &errorMessage);
            std::vector<ModInfo> GetMissingMods(const std::vector<ModInfo> &clientMods);
            bool SynchronizeMods(const std::vector<ModInfo> &missingMods);

        private:
            std::unordered_map<std::string, ModInfo> m_serverMods;
            bool CheckVersionCompatibility(const std::string &serverVersion, const std::string &clientVersion);
            bool CheckDependencies(const ModInfo &mod);
        };

    } // namespace Network
} // namespace GZDoom
