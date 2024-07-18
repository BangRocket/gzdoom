#pragma once

#include <vector>
#include <string>
#include <cstdint>

namespace GZDoom
{
    namespace Network
    {

        struct ModInfo
        {
            std::string name;
            std::string version;
            uint8_t checksum[16]; // MD5 checksum
        };

        class ModSyncManager
        {
        public:
            ModSyncManager();
            ~ModSyncManager();

            void AddMod(const ModInfo &mod);
            std::vector<uint8_t> PackageModData() const;
            bool VerifyModPackage(const std::vector<uint8_t> &package) const;
            std::vector<ModInfo> UnpackageModData(const std::vector<uint8_t> &package);

        private:
            std::vector<ModInfo> m_mods;
        };

    } // namespace Network
} // namespace GZDoom
