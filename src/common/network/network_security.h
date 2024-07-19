#pragma once

#include <vector>
#include <string>
#include <cstdint>

namespace GZDoom
{
    namespace Network
    {

        class SecurityManager
        {
        public:
            SecurityManager();
            ~SecurityManager();

            std::vector<uint8_t> EncryptData(const std::vector<uint8_t> &data);
            std::vector<uint8_t> DecryptData(const std::vector<uint8_t> &encryptedData);
            bool AuthenticateClient(const std::string &clientId, const std::string &authToken);
            void UpdateRateLimit(const std::string &clientId);
            bool CheckRateLimit(const std::string &clientId);

            // New methods for anti-cheat measures
            bool VerifyClientIntegrity(const std::string &clientId, const std::vector<uint8_t> &clientChecksum);
            void ReportSuspiciousActivity(const std::string &clientId, const std::string &reason);

        private:
            // Add private members for encryption, authentication, and rate limiting
            void GenerateAESKey();
    
            uint8_t m_aesKey[16];
            uint8_t m_aesIv[16];
            std::unordered_map<std::string, int> m_rateLimitData;
            const int MAX_REQUESTS_PER_MINUTE = 100;

            // New members for anti-cheat measures
            std::unordered_map<std::string, std::vector<uint8_t>> m_clientChecksums;
            std::unordered_map<std::string, int> m_suspiciousActivityCount;
        };

    } // namespace Network
} // namespace GZDoom
