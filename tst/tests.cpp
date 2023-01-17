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

  // proper behavior is a unique integer per component type.

  ASSERT_TRUE(guid1 == 0);
  ASSERT_TRUE(guid2 == 1);
  ASSERT_TRUE(guid3 == 0);
  
}

TEST(ecs, entities_in_scene)
{

  ecs::Scene scene;

  ecs::Entity entity = scene.CreateEntity();

  ASSERT_TRUE(scene.getComponentFlags(entity.guid) == 0); // no components yet

  scene.AddComponent<Transform>(entity.guid, Transform{0.0f, 0.0f, 0.0f});

  Guid componentGuid = scene.GetComponentGuid<Transform>();
  ComponentFlags componentMask = (static_cast<u64>(1) << componentGuid);

  std::cout << "Component GUID: " << componentGuid << std::endl;
  std::cout << "Expected component mask: " << componentMask << std::endl;
  std::cout << ">ACTUAL< component mask: " << scene.getComponentFlags(entity.guid) << std::endl;

  ASSERT_TRUE(scene.getComponentFlags(entity.guid) == componentMask); // a component has been added!
}

TEST(ecs, entity_reuse)
{

  ecs::Scene scene;

  ecs::Entity entity = scene.CreateEntity();

  ASSERT_TRUE(scene.getComponentFlags(entity.guid) == 0); // no components yet

  scene.AddComponent<Transform>(entity.guid, Transform{0.0f, 0.0f, 0.0f});
  scene.DestroyEntity(entity.guid);

  // make sure component flags destroyed / reset so invalid components are not fetched
  ASSERT_TRUE(scene.getComponentFlags(entity.guid) == 0);

  // create a new entity. this should reuse the guid of the destroyed entity.
  entity = scene.CreateEntity();

  ASSERT_TRUE(scene.getComponentFlags(entity.guid) == 0);
  ASSERT_TRUE(entity.guid == 0);

  std::cout << "Expect entity GUID: " << 0 << std::endl;
  std::cout << "Actual entity GUID: " << entity.guid << std::endl;

  std::cout << "Expected component mask: " << 0 << std::endl;
  std::cout << ">ACTUAL< component mask: " << scene.getComponentFlags(entity.guid) << std::endl;
}

TEST(ecs, remove_component)
{

  ecs::Scene scene;

  ecs::Entity entity = scene.CreateEntity();

  ASSERT_TRUE(scene.getComponentFlags(entity.guid) == 0); // no components yet

  scene.AddComponent<Transform>(entity.guid, Transform{0.0f, 0.0f, 0.0f}); // component guid 0
  scene.AddComponent<DummyData>(entity.guid, {0,1}); // component guid 1

  for (int i = 1; i < 5; i++)
  {

    float fi = static_cast<float>(i);
    ecs::Entity e1 = scene.CreateEntity();
    scene.AddComponent(e1.guid, Transform{fi, fi, fi});
  }

  ASSERT_TRUE(scene.getComponentFlags(entity.guid) == 3); // ...11 is component field

  scene.RemoveComponent<Transform>(entity.guid); // "remove" the component from memory pool

  ASSERT_TRUE(scene.getComponentFlags(entity.guid) == 2); // only bit 0 should be set now.

  std::cout << "Expect entity GUID: " << 0 << std::endl;
  std::cout << "Actual entity GUID: " << entity.guid << std::endl;

  std::cout << "Expected component mask: " << 1 << std::endl;
  std::cout << ">ACTUAL< component mask: " << scene.getComponentFlags(entity.guid) << std::endl;
}