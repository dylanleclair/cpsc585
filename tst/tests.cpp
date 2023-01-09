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

TEST(ecs, register_component_guids)
{
  // tests
  ecs::Guid guid1 = ecs::GetComponentGuid<DummyData>();
  ecs::Guid guid2 = ecs::GetComponentGuid<DummyDataAlternate>();
  ecs::Guid guid3 = ecs::GetComponentGuid<DummyData>();

  ASSERT_TRUE(guid1 == 0);
  ASSERT_TRUE(guid2 == 1);
  ASSERT_TRUE(guid3 == 0);
}