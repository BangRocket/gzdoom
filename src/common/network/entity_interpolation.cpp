#include "entity_interpolation.h"
#include <algorithm>

namespace GZDoom
{
    namespace Network
    {

        EntityInterpolator::EntityInterpolator() : m_lastInterpolationTime(0.0) {}

        void EntityInterpolator::AddSnapshot(const EntityState &state, double timestamp)
        {
            m_snapshots.push_back({state, timestamp});
            std::sort(m_snapshots.begin(), m_snapshots.end(),
                      [](const Snapshot &a, const Snapshot &b)
                      { return a.timestamp < b.timestamp; });
            PruneOldSnapshots(timestamp);
        }

        EntityState EntityInterpolator::Interpolate(double currentTime, double interpolationDelay)
        {
            if (m_snapshots.size() < 2)
            {
                return m_snapshots.empty() ? EntityState() : m_snapshots.back().state;
            }

            double targetTime = currentTime - interpolationDelay;

            auto it = std::lower_bound(m_snapshots.begin(), m_snapshots.end(), targetTime,
                                       [](const Snapshot &s, double t)
                                       { return s.timestamp < t; });

            if (it == m_snapshots.begin())
            {
                return it->state;
            }
            if (it == m_snapshots.end())
            {
                return (--it)->state;
            }

            auto next = it;
            auto prev = --it;

            double t = (targetTime - prev->timestamp) / (next->timestamp - prev->timestamp);
            return InterpolateBetweenSnapshots(*prev, *next, t);
        }

        EntityState EntityInterpolator::Extrapolate(double currentTime)
        {
            if (m_snapshots.size() < 2)
            {
                return m_snapshots.empty() ? EntityState() : m_snapshots.back().state;
            }

            const auto &latest = m_snapshots.back();
            const auto &previous = m_snapshots[m_snapshots.size() - 2];

            double dt = currentTime - latest.timestamp;
            double snapshotDt = latest.timestamp - previous.timestamp;

            EntityState result = latest.state;
            result.position += result.velocity * dt;
            result.velocity += (latest.state.velocity - previous.state.velocity) * (dt / snapshotDt);

            return result;
        }

        void EntityInterpolator::PruneOldSnapshots(double currentTime)
        {
            constexpr double MAX_SNAPSHOT_AGE = 1.0; // 1 second
            m_snapshots.erase(
                std::remove_if(m_snapshots.begin(), m_snapshots.end(),
                               [currentTime](const Snapshot &s)
                               { return currentTime - s.timestamp > MAX_SNAPSHOT_AGE; }),
                m_snapshots.end());
        }

        EntityState EntityInterpolator::InterpolateBetweenSnapshots(const Snapshot &s1, const Snapshot &s2, double t)
        {
            EntityState result;
            result.position = s1.state.position * (1 - t) + s2.state.position * t;
            result.velocity = s1.state.velocity * (1 - t) + s2.state.velocity * t;
            result.rotation = s1.state.rotation * (1 - t) + s2.state.rotation * t;
            return result;
        }

    } // namespace Network
} // namespace GZDoom
