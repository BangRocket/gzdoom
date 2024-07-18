#include "game_state.h"
#include "network_manager.h"
#include <algorithm>

namespace GZDoom
{
    namespace Network
    {

        GameState::GameState() : m_lastUpdateTime(0.0) {}

        void GameState::Update(double currentTime)
        {
            // Update game state logic here
            m_lastUpdateTime = currentTime;
        }

        void GameState::AddEntity(const Entity &entity)
        {
            m_entities[entity.id] = entity;
        }

        void GameState::RemoveEntity(int entityId)
        {
            m_entities.erase(entityId);
        }

        void GameState::UpdateEntity(const Entity &entity)
        {
            m_entities[entity.id] = entity;
        }

        const Entity *GameState::GetEntity(int entityId) const
        {
            auto it = m_entities.find(entityId);
            return it != m_entities.end() ? &it->second : nullptr;
        }

        std::vector<uint8_t> GameState::Serialize() const
        {
            std::vector<uint8_t> data;
            // Serialize game state data
            // This is a simplified example; you'd need to implement proper serialization
            for (const auto &pair : m_entities)
            {
                const Entity &entity = pair.second;
                data.insert(data.end(), reinterpret_cast<const uint8_t *>(&entity.id), reinterpret_cast<const uint8_t *>(&entity.id) + sizeof(int));
                data.insert(data.end(), reinterpret_cast<const uint8_t *>(&entity.position), reinterpret_cast<const uint8_t *>(&entity.position) + sizeof(Vector3));
                data.insert(data.end(), reinterpret_cast<const uint8_t *>(&entity.velocity), reinterpret_cast<const uint8_t *>(&entity.velocity) + sizeof(Vector3));
            }
            return data;
        }

        void GameState::Deserialize(const std::vector<uint8_t> &data)
        {
            m_entities.clear();
            // Deserialize game state data
            // This is a simplified example; you'd need to implement proper deserialization
            for (size_t i = 0; i < data.size(); i += sizeof(int) + 2 * sizeof(Vector3))
            {
                Entity entity;
                memcpy(&entity.id, &data[i], sizeof(int));
                memcpy(&entity.position, &data[i + sizeof(int)], sizeof(Vector3));
                memcpy(&entity.velocity, &data[i + sizeof(int) + sizeof(Vector3)], sizeof(Vector3));
                m_entities[entity.id] = entity;
            }
        }

        std::vector<uint8_t> GameState::GenerateDelta(const GameState &oldState) const
        {
            std::vector<uint8_t> delta;
            // Generate delta between this state and oldState
            // This is a simplified example; you'd need to implement proper delta generation
            for (const auto &pair : m_entities)
            {
                const Entity &newEntity = pair.second;
                const Entity *oldEntity = oldState.GetEntity(newEntity.id);
                if (!oldEntity || *oldEntity != newEntity)
                {
                    delta.insert(delta.end(), reinterpret_cast<const uint8_t *>(&newEntity), reinterpret_cast<const uint8_t *>(&newEntity) + sizeof(Entity));
                }
            }
            return delta;
        }

        void GameState::ApplyDelta(const std::vector<uint8_t> &delta)
        {
            // Apply delta to this state
            // This is a simplified example; you'd need to implement proper delta application
            for (size_t i = 0; i < delta.size(); i += sizeof(Entity))
            {
                Entity entity;
                memcpy(&entity, &delta[i], sizeof(Entity));
                m_entities[entity.id] = entity;
            }
        }

    } // namespace Network
} // namespace GZDoom
