#include "gtest/gtest.h"
#include "ecs.h"

struct DummyData
{
  int hello0{0};
  int hello1{1};
};

struct DummyDataAlternate
{
  int why{1};
  int oh{0};
};

struct Transform
{
  float posX, posY, posZ;
};

TEST(ecs, register_component_guids)
{
  ecs::Scene scene;
  // tests
  Guid guid1 = scene.GetComponentGuid<DummyData>();
  Guid guid2 = scene.GetComponentGuid<DummyDataAlternate>();
  Guid guid3 = scene.GetComponentGuid<DummyData>();

  ASSERT_TRUE(guid1 == 0);
  ASSERT_TRUE(guid2 == 1);
  ASSERT_TRUE(guid3 == 0);
}

TEST(ecs, entities_in_scene)
{

  ecs::Scene scene;

  ecs::Entity& entity = scene.CreateEntity();

  ASSERT_TRUE(entity.components == 0); // no components yet

  scene.AddComponent<Transform>(entity.id, Transform{0.0f, 0.0f, 0.0f});

  Guid componentGuid = scene.GetComponentGuid<Transform>();
  ComponentFlags componentMask = (static_cast<u64>(1) << componentGuid);

  std::cout << "Component GUID: " << componentGuid << std::endl;
  std::cout << "Expected component mask: " << componentMask << std::endl;
  std::cout << ">ACTUAL< component mask: " << entity.components << std::endl;

  ASSERT_TRUE(entity.components == componentMask); // a component has been added!
}

// TEST(ecs, test_system_basic)
// {

//   ecs::Scene scene;

//   ecs::Scene::Entity entity = scene.CreateEntity();

//   entity.AddComponent<Transform>(entity.id, Transform{0.0f, 0.0f, 0.0f});

//   // tests
//   ecs::Guid guid1 = ecs::GetComponentGuid<DummyData>();
//   ecs::Guid guid2 = ecs::GetComponentGuid<DummyDataAlternate>();
//   ecs::Guid guid3 = ecs::GetComponentGuid<DummyData>();

//   ASSERT_TRUE(guid1 == 0);
//   ASSERT_TRUE(guid2 == 1);
//   ASSERT_TRUE(guid3 == 0);
// }