#pragma once

#include <vector>
#include "network_architecture.h"

namespace GZDoom
{
    namespace Network
    {
        class GameState
        {
        public:
            GameState();
            ~GameState();

            void Update(double currentTime);
            void AddEntity(const Entity &entity);
            void RemoveEntity(int entityId);
            void UpdateEntity(const Entity &entity);
            const Entity* GetEntity(int entityId) const;

            std::vector<uint8_t> Serialize() const;
            void Deserialize(const std::vector<uint8_t> &data);

            std::vector<uint8_t> GenerateDelta(const GameState &oldState) const;
            void ApplyDelta(const std::vector<uint8_t> &delta);

        private:
            std::unordered_map<int, Entity> m_entities;
            double m_lastUpdateTime;
        };
    }
}
