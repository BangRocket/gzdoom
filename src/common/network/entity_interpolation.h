#pragma once

#include <vector>
#include <cstdint>
#include "network_architecture.h"

namespace GZDoom
{
    namespace Network
    {

        class EntityInterpolator
        {
        public:
            EntityInterpolator();

            void AddSnapshot(const EntityState &state, double timestamp);
            EntityState Interpolate(double currentTime, double interpolationDelay);
            EntityState Extrapolate(double currentTime);

        private:
            struct Snapshot
            {
                EntityState state;
                double timestamp;
            };

            std::vector<Snapshot> m_snapshots;
            double m_lastInterpolationTime;

            void PruneOldSnapshots(double currentTime);
            EntityState InterpolateBetweenSnapshots(const Snapshot &s1, const Snapshot &s2, double t);
        };

    } // namespace Network
} // namespace GZDoom
