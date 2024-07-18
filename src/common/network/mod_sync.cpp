#include "mod_sync.h"
#include <algorithm>
#include <cstring>

namespace GZDoom
{
    namespace Network
    {

        ModSyncManager::ModSyncManager() {}

        ModSyncManager::~ModSyncManager() {}

        void ModSyncManager::AddMod(const ModInfo &mod)
        {
            m_mods.push_back(mod);
        }

        std::vector<uint8_t> ModSyncManager::PackageModData() const
        {
            std::vector<uint8_t> package;
            for (const auto &mod : m_mods)
            {
                // Add mod name
                package.insert(package.end(), mod.name.begin(), mod.name.end());
                package.push_back('\0'); // Null-terminate the string

                // Add mod version
                package.insert(package.end(), mod.version.begin(), mod.version.end());
                package.push_back('\0'); // Null-terminate the string

                // Add mod checksum
                for (int i = 0; i < 16; ++i)
                {
                    package.push_back(mod.checksum[i]);
                }
            }
            return package;
        }

        bool ModSyncManager::VerifyModPackage(const std::vector<uint8_t> &package) const
        {
            size_t offset = 0;
            for (const auto &mod : m_mods)
            {
                // Verify mod name
                if (std::strcmp(reinterpret_cast<const char *>(&package[offset]), mod.name.c_str()) != 0)
                {
                    return false;
                }
                offset += mod.name.length() + 1; // +1 for null terminator

                // Verify mod version
                if (std::strcmp(reinterpret_cast<const char *>(&package[offset]), mod.version.c_str()) != 0)
                {
                    return false;
                }
                offset += mod.version.length() + 1; // +1 for null terminator

                // Verify mod checksum
                if (std::memcmp(&package[offset], mod.checksum, 16) != 0)
                {
                    return false;
                }
                offset += 16;
            }
            return true;
        }

        std::vector<ModInfo> ModSyncManager::UnpackageModData(const std::vector<uint8_t> &package)
        {
            std::vector<ModInfo> mods;
            size_t offset = 0;
            while (offset < package.size())
            {
                ModInfo mod;

                // Extract mod name
                mod.name = reinterpret_cast<const char *>(&package[offset]);
                offset += mod.name.length() + 1; // +1 for null terminator

                // Extract mod version
                mod.version = reinterpret_cast<const char *>(&package[offset]);
                offset += mod.version.length() + 1; // +1 for null terminator

                // Extract mod checksum
                std::memcpy(mod.checksum, &package[offset], 16);
                offset += 16;

                mods.push_back(mod);
            }
            return mods;
        }

    } // namespace Network
} // namespace GZDoom
