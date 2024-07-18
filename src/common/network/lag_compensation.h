#pragma once

#include "network_architecture.h"
#include <vector>
#include <unordered_map>

namespace GZDoom
{
    namespace Network
    {

        class LagCompensator
        {
        public:
            LagCompensator();
            ~LagCompensator();

            void StoreEntityState(int entityId, const EntityState &state, double timestamp);
            EntityState RewindState(int entityId, double targetTime);
            void ApplyServerSideRewind(double currentTime, double rewindTime);

        private:
            struct TimestampedState
            {
                EntityState state;
                double timestamp;
            };

            std::unordered_map<int, std::vector<TimestampedState>> m_entityHistory;
            static constexpr double MAX_REWIND_TIME = 1.0; // Maximum rewind time in seconds

            void PruneOldStates(double currentTime);
        };

    } // namespace Network
} // namespace GZDoom
