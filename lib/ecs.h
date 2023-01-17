#pragma once

#include "int_types.h"
#include <vector>
#include <array>
#include <unordered_map>
#include <memory>

// draws inspiration from:
// - https://www.david-colson.com/2020/02/09/making-a-simple-ecs.html
// - https://austinmorlan.com/posts/entity_component_system/#the-component-manager
// there was few holes in each of them that I patched up, taking the parts I liked of each.

/** an entity is just data that stores components. */
using Guid = u64;
using ComponentFlags = u64;
// we need some way of tracking which components are assigned to an entity. we could perhaps reflect on the entity's components, and hash the types?

namespace ecs
{

  const u32 MAX_ENTITIES = 50000;

  template <typename... ComponentTypes>
  struct EntitiesInScene;
  struct Scene;
  struct Entity;

  /// @brief systems interface proof of concept
  struct ISystem
  {
    virtual void Update(Scene &scene, float deltaTime) = 0;
  };

  /// @brief Gets the mask for the component by shifting according to the provided GUID.
  /// @param componentGuid 
  /// @return ComponentFlags with only this component enabled
  ComponentFlags componentMaskFromGuid(Guid componentGuid)
  {
    return (static_cast<u64>(1) << componentGuid);
  }

  namespace memory
  {

    class IComponentPool
    {
    public:
      virtual ~IComponentPool() = default;
      virtual void remove(Guid entityGuid) = 0;
    };

    template <typename T>
    class ComponentPool : public IComponentPool
    {

    public:

      void push_back(Guid entityGuid, T component)
      {
        size_t componentIndex = m_size;
        m_components[componentIndex] = component;
        m_mappings[entityGuid] = componentIndex; // add the mapping to our lookup table for them
        m_size++;
      }

      void remove(Guid entityGuid)
      {

        // GOAL: copy last element into deleted elements position

        // mappings[i] is the component index of entity guid i

        // first, we need to find the element to remove.

        size_t removeIndex = m_mappings[entityGuid];
        size_t lastIndex = m_size - 1;

        // copy data from the end of array to newly "freed" location
        m_components[removeIndex] = m_components[lastIndex];

        // update the mapping so data is still coherent
        m_mappings[lastIndex] = removeIndex;

        m_size--;

      }

      T &GetComponentData(Guid entityGuid)
      {
        // TODO: add guard clause
        return m_components[m_mappings[entityGuid]];
      }

    private:
      // std::vector<T> m_components = std::vector<T>{};                       // we're *supposed* to use an array here, but a vector will do.
      size_t m_size{0};
      std::array<T,MAX_ENTITIES> m_components;
      std::array<size_t, MAX_ENTITIES> m_mappings; // helps us access components even if others are removed. component guid -> index in pool
    };
  }

  struct EntityComponentSystem
  {
    u64 m_componentCounter{0};
    u64 m_entityCounter{0};
    std::vector<Guid> m_discardedGuids = std::vector<Guid>{};

    // lookup table of component GUIDs
    std::unordered_map<std::string, Guid> m_componentGuids{};
    // std::unordered_map<std::string, std::shared_ptr<memory::IComponentPool>> m_componentPools{};

    std::vector<std::shared_ptr<memory::IComponentPool>> m_componentPools{};
  };

  struct Entity
  {
    friend struct Scene;
    template <typename... ComponentTypes>
    friend struct EntitiesInScene;

    Guid guid;
    Entity(Guid guid, ComponentFlags components) : guid(guid), components(components) {}
  private:
    ComponentFlags components;
  };

  /// @brief The primary data structure of the ECS. Manages entities, components, and their interactions.
  struct Scene
  {

    template <typename... ComponentTypes>
    friend struct EntitiesInScene;

    Scene() : ecs(EntityComponentSystem{}), entities(std::vector<Entity>{}) {}

    /* --------------------------------------

     ENTITY MANAGEMENT

    -------------------------------------- */

    /**
     * Creates an entity, returning a reference to the entity.
     */
    Entity CreateEntity()
    {

      // first we need to see if there are any discarded guids from destroyed entities
      if (ecs.m_discardedGuids.size() == 0)
      {
        // assign a completely new guid
        Guid entityId = ecs.m_entityCounter++;
        entities.push_back(Entity{entityId, 0});
        return entities[entityId];
      }
      else
      {
        // take the most recently discarded guid, return it.
        Guid assignedGuid = ecs.m_discardedGuids.back();
        ecs.m_discardedGuids.pop_back();
        entities[assignedGuid].components = 0;
        return entities[assignedGuid];
      }
    }

    void DestroyEntity(Guid entityGuid)
    {
      ecs.m_discardedGuids.push_back(entityGuid);
      entities[entityGuid].components = 0; // reset the component flags to 0 (empty it)

      // we want to keep the array packed

    }

    bool isValidEntity(Guid entityGuid)
    {
      for (const auto &guid : ecs.m_discardedGuids)
      {
        if (entityGuid == guid)
        {
          return false;
        }
      }
      return true;
    }

    /* --------------------------------------

     COMPONENT MANAGEMENT

    -------------------------------------- */

    /// @brief Add the provided component to the entity with provided GUID.
    /// @tparam T the type of component to add
    /// @param entityGuid the entity guid to add a component to
    /// @param component the component data to initialize the entity with
    /// @return a reference to the component data in ECS memory
    template <typename T>
    T &AddComponent(Guid entityGuid, T component)
    {
      std::string typeName = std::string{typeid(T).name()};
      Guid componentGuid = GetComponentGuid<T>();
      ComponentFlags newComponentMask = componentMaskFromGuid(componentGuid);

      Entity &entity = entities[entityGuid];

      std::shared_ptr<memory::ComponentPool<T>> componentPool = std::static_pointer_cast<memory::ComponentPool<T>>(ecs.m_componentPools[componentGuid]);

      // add the component to memory pool reponsible for storing it.
      componentPool->push_back(entityGuid, component);

      // update the entity's components.
      entities[entityGuid].components |= (static_cast<u64>(1) << componentGuid); // mask the bit!

      T &componentData = componentPool->GetComponentData(entityGuid);
      // CONSIDER RETURNING THE COMPONENT
      return componentData;
    }

    /// @brief Removes a component from the specified entity
    /// @tparam T the type of the component
    /// @param entityGuid the guid of the entity to remove data from
    template <typename T>
    void RemoveComponent(Guid entityGuid)
    {
      std::string typeName = std::string{typeid(T).name()};
      u64 componentId = GetComponentGuid<T>();
      std::shared_ptr<memory::ComponentPool<T>> componentPool = std::static_pointer_cast<memory::ComponentPool<T>>(ecs.m_componentPools[componentId]);
      componentPool->remove(entityGuid);

      Entity e = entities[entityGuid];

      u64 mask = (~(static_cast<u64>(1) << componentId));

      entities[entityGuid].components &= (~(static_cast<u64>(1) << componentId));
    }

    /// @brief Gets a component specified by template on an entity.
    /// @tparam T the desired component type
    /// @param entityGuid 
    /// @return 
    template <typename T>
    T &GetComponent(Guid entityGuid)
    {
      std::string typeName = std::string{typeid(T).name()};
      Guid componentGuid = GetComponentGuid<T>();
      std::shared_ptr<memory::ComponentPool<T>> componentPool = std::static_pointer_cast<memory::ComponentPool<T>>(ecs.m_componentPools[componentGuid]);
      return componentPool->GetComponentData(entityGuid);
    }

    /// @brief Gets the component flags -> DO NOT USE unless for tests / absolutely needed.
    /// @param entityGuid the GUID of the entity to lookup the component flags for
    /// @return the component flags
    ComponentFlags getComponentFlags(Guid entityGuid)
    {
      for (auto& entity : entities)
      {
        if (entity.guid == entityGuid)
        {
          return entity.components;
        }
      }
      return 0;
    }

    /// @brief Gets the component GUID for the templated component type. Component GUIDs are unique per type. Calling this function with the same type will provide the same result.
    /// @tparam T the component type
    /// @return the component GUID corresponding to the type. 
    template <typename T>
    Guid GetComponentGuid()
    {
      // lookup the typename
      std::string typeName = std::string{typeid(T).name()};

      // see if a Guid already exists for this type of component
      if (auto typeLookup = ecs.m_componentGuids.find(typeName); typeLookup != ecs.m_componentGuids.end())
      { // if the type is already found, return it.
        return typeLookup->second;
      }

      // if not yet created, add the new component Guid to the lookup table.
      Guid componentGuid = ecs.m_componentCounter++;
      ecs.m_componentGuids[typeName] = componentGuid;
      ecs.m_componentPools.push_back(std::make_shared<memory::ComponentPool<T>>()); // setup the component pool.

      assert(componentGuid < 63 && "Max number of components exceeded.");
      // TODO make this more robust -> what if we no longer need a particular type of component? make some sort of wrapper that will reuse fully discarded guids

      return componentGuid;
    }

  private:
    // Member variables
    EntityComponentSystem ecs{};
    std::vector<Entity> entities{}; // basically a list of component masks and names

    /* --------------------------------------
     Private helper functions
    -------------------------------------- */

    /// @brief Registers a component in the system.
    /// @tparam T the new type of component to register in the system. max of 64 components.
    template <typename T>
    void RegisterComponent()
    {
      // Guid componentGuid = GetComponentGuid<T>();

      // std::string typeName = std::string{typeid(T).name()};
      // if (ecs.m_componentPools[componentGuid] == nullptr)
      // {
      //   ecs.m_componentPools.push({typeName, std::make_shared<memory::ComponentPool<T>>()});
      // }
    }

  };

  template <typename... ComponentTypes>
  struct EntitiesInScene
  {
    EntitiesInScene(Scene &scene) : m_scene(scene)
    {
      if (sizeof...(ComponentTypes) == 0)
      {
        m_all = true;
      }
      else
      {
        Guid componentIds[] = {0, scene.GetComponentGuid<ComponentTypes>()...};
        for (const auto &id : componentIds)
        {
            m_componentMask |= (static_cast<ComponentFlags>(1) << id); // all we have to do is or it in~!
        }
      }
    }

    struct Iterator
    {

      Iterator(EntitiesInScene &entitiesInScene) : m_wrapper(entitiesInScene), m_scene(entitiesInScene.m_scene), m_componentMask(entitiesInScene.m_componentMask), m_all(entitiesInScene.m_all) {}

      Iterator(EntitiesInScene &entitiesInScene, size_t index) : m_wrapper(entitiesInScene),  m_scene(entitiesInScene.m_scene), m_componentMask(entitiesInScene.m_componentMask), m_all(entitiesInScene.m_all), index(index) {}

      Guid operator*() const
      {
        return m_scene.entities[index].guid;
      };

      bool operator==(const Iterator &other) const
      {
        return index == other.index || index == m_scene.entities.size();
      }
      bool operator!=(const Iterator &other) const
      {
        return index != other.index && index != m_scene.entities.size();
      }

      bool isValidIndex()
      {
        Guid e = m_scene.entities[index].guid;
        if (m_scene.isValidEntity(e))
          return (m_all) ? true : m_componentMask == m_scene.entities[index].components;
        else
        {
          return false;
        }
      }

      Iterator &operator++()
      {
        do
        {
          index++;
        } while (index < m_scene.entities.size() && !isValidIndex()); // advances index until next element with the right components is found
        return *this;
      }

      const Iterator begin() const
      {
        u64 firstIndex = 0;

        ComponentFlags entityComponentsMasked = (m_componentMask & m_scene.entities[index].components);

        while (index < m_scene.entities.size() && (m_componentMask != entityComponentsMasked || !m_scene.isValidEntity(m_scene.entities[index].guid)))
        {
          firstIndex++;
        }

        return Iterator{m_wrapper, firstIndex};
      }

      const Iterator end() const
      {
        u64 firstIndex = m_scene.entities.size() - 1;

        ComponentFlags entityComponentsMasked = (m_componentMask & m_scene.entities[index].components);

        while (index > 0 && (m_componentMask != entityComponentsMasked || !m_scene.isValidEntity(m_scene.entities[index].guid)))
        {
          firstIndex--;
        }

        return Iterator{m_wrapper, firstIndex};
      }
      bool m_all;
      ComponentFlags m_componentMask;
      Scene& m_scene;
      size_t index{0};
      EntitiesInScene& m_wrapper;
    };


    Iterator begin() { return Iterator(*this).begin(); }
    Iterator end() { return Iterator(*this).end(); }
    Scene &m_scene;
    ComponentFlags m_componentMask{0};
    bool m_all{false};
  };

}
