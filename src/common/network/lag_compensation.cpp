#include "lag_compensation.h"
#include <algorithm>

namespace GZDoom
{
    namespace Network
    {

        LagCompensator::LagCompensator() {}

        LagCompensator::~LagCompensator() {}

        void LagCompensator::StoreEntityState(int entityId, const EntityState &state, double timestamp)
        {
            m_entityHistory[entityId].push_back({state, timestamp});
            PruneOldStates(timestamp);
        }

        EntityState LagCompensator::RewindState(int entityId, double targetTime)
        {
            auto &history = m_entityHistory[entityId];

            auto it = std::lower_bound(history.begin(), history.end(), targetTime,
                                       [](const TimestampedState &ts, double time)
                                       { return ts.timestamp < time; });

            if (it == history.begin())
            {
                return it->state;
            }
            if (it == history.end())
            {
                return (--it)->state;
            }

            auto next = it;
            auto prev = --it;

            double t = (targetTime - prev->timestamp) / (next->timestamp - prev->timestamp);
            return InterpolateStates(prev->state, next->state, t);
        }

        void LagCompensator::ApplyServerSideRewind(double currentTime, double rewindTime)
        {
            // Implement server-side rewind logic here
            // This would involve rewinding all relevant entities to the specified time
            // and then re-running the game simulation up to the current time
        }

        void LagCompensator::PruneOldStates(double currentTime)
        {
            for (auto &[entityId, history] : m_entityHistory)
            {
                history.erase(
                    std::remove_if(history.begin(), history.end(),
                                   [currentTime](const TimestampedState &ts)
                                   {
                                       return currentTime - ts.timestamp > MAX_REWIND_TIME;
                                   }),
                    history.end());
            }
        }

        EntityState LagCompensator::InterpolateStates(const EntityState &s1, const EntityState &s2, double t)
        {
            EntityState result;
            result.position = s1.position * (1 - t) + s2.position * t;
            result.velocity = s1.velocity * (1 - t) + s2.velocity * t;
            result.rotation = s1.rotation * (1 - t) + s2.rotation * t;
            // Interpolate other relevant properties as needed
            return result;
        }

    } // namespace Network
} // namespace GZDoom
