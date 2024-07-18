#include <gtest/gtest.h>
#include "../src/common/network/network_manager.h"
#include "../src/common/network/game_state.h"

namespace GZDoom
{
    namespace Network
    {
        namespace Tests
        {

            TEST(NetworkTests, GameStateSerializationDeserialization)
            {
                GameState originalState;

                Entity entity1{1, {1.0f, 2.0f, 3.0f}, {0.1f, 0.2f, 0.3f}};
                Entity entity2{2, {4.0f, 5.0f, 6.0f}, {0.4f, 0.5f, 0.6f}};

                originalState.AddEntity(entity1);
                originalState.AddEntity(entity2);

                std::vector<uint8_t> serializedData = originalState.Serialize();

                GameState deserializedState;
                deserializedState.Deserialize(serializedData);

                ASSERT_EQ(originalState.GetEntity(1)->position.x, deserializedState.GetEntity(1)->position.x);
                ASSERT_EQ(originalState.GetEntity(1)->position.y, deserializedState.GetEntity(1)->position.y);
                ASSERT_EQ(originalState.GetEntity(1)->position.z, deserializedState.GetEntity(1)->position.z);

                ASSERT_EQ(originalState.GetEntity(2)->velocity.x, deserializedState.GetEntity(2)->velocity.x);
                ASSERT_EQ(originalState.GetEntity(2)->velocity.y, deserializedState.GetEntity(2)->velocity.y);
                ASSERT_EQ(originalState.GetEntity(2)->velocity.z, deserializedState.GetEntity(2)->velocity.z);
            }

            TEST(NetworkTests, GameStateDeltaGeneration)
            {
                GameState oldState;
                GameState newState;

                Entity entity1{1, {1.0f, 2.0f, 3.0f}, {0.1f, 0.2f, 0.3f}};
                Entity entity2{2, {4.0f, 5.0f, 6.0f}, {0.4f, 0.5f, 0.6f}};

                oldState.AddEntity(entity1);
                oldState.AddEntity(entity2);

                Entity updatedEntity2{2, {4.5f, 5.5f, 6.5f}, {0.5f, 0.6f, 0.7f}};
                newState.AddEntity(entity1);
                newState.AddEntity(updatedEntity2);

                std::vector<uint8_t> delta = newState.GenerateDelta(oldState);

                ASSERT_EQ(delta.size(), sizeof(Entity));

                Entity deltaEntity;
                memcpy(&deltaEntity, delta.data(), sizeof(Entity));

                ASSERT_EQ(deltaEntity.id, 2);
                ASSERT_EQ(deltaEntity.position.x, 4.5f);
                ASSERT_EQ(deltaEntity.position.y, 5.5f);
                ASSERT_EQ(deltaEntity.position.z, 6.5f);
                ASSERT_EQ(deltaEntity.velocity.x, 0.5f);
                ASSERT_EQ(deltaEntity.velocity.y, 0.6f);
                ASSERT_EQ(deltaEntity.velocity.z, 0.7f);
            }

        } // namespace Tests
    } // namespace Network
} // namespace GZDoom

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
