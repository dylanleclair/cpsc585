#include <vector>
#include <unordered_map>
#include <memory>

// draws inspiration from:
// - https://www.david-colson.com/2020/02/09/making-a-simple-ecs.html
// - https://austinmorlan.com/posts/entity_component_system/#the-component-manager
// there was few holes in each of them that I patched up, taking the parts I liked of each.

using u64 = uint64_t;
using u32 = uint64_t;
using u16 = uint64_t;
using u8 = uint64_t;

using i64 = int64_t;
using i32 = int64_t;
using i16 = int64_t;
using i8 = int64_t;
/** an entity is just data that stores components. */
using Guid = u64;
using ComponentFlags = u64;
// we need some way of tracking which components are assigned to an entity. we could perhaps reflect on the entity's components, and hash the types?

namespace ecs
{

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
        // TODO add a guard clause to prevent duplicate components
        size_t componentIndex = m_components.size();
        m_components.push_back(component);
        m_mappings.push_back({entityGuid, componentIndex}); // add the mapping to our lookup table for them
      }

      void remove(Guid entityGuid)
      {

        // TODO add a guard clause

        // copy end element into the deleted element's position.
        size_t i{std::numeric_limits<size_t>::max()};
        for (auto const &pair : m_mappings)
        {
          if (pair.first == entityGuid)
          {
            i = pair.second;
          }
        }

        if (i == std::numeric_limits<size_t>::max())
        {
          size_t lastIndex = m_components.size() - 1;

          // if the index is valid...
          // copy the element at end of vector to new position
          m_components[i] = m_components[lastIndex];
          // and remove the one that was copied over!
          m_components.pop_back();
        }

        // update map to point to moved spot
      }

      T &GetComponentData(Guid entityGuid)
      {
        // add guard clause
        for (auto &pair : m_mappings)
        {
          if (pair.first == entityGuid)
          {
            return m_components[pair.second];
          }
        }
      }

    private:
      std::vector<T> m_components = std::vector<T>{};                       // we're *supposed* to use an array here, but a vector will do.
      std::vector<std::pair<Guid, size_t>> m_mappings = std::vector<std::pair<Guid, size_t>>{}; // helps us access components even if others are removed. component guid -> index in pool
    };
  }

  struct EntityComponentSystem
  {
    u64 m_componentCounter{0};
    u64 m_entityCounter{0};
    std::vector<Guid> m_discardedGuids = std::vector<Guid>{};

    // lookup table of component GUIDs
    std::unordered_map<std::string, Guid> m_componentGuids{};
    std::unordered_map<std::string, std::shared_ptr<memory::IComponentPool>> m_componentPools{};
  };

  struct Entity
  {

    Entity(Guid guid, ComponentFlags components) : id(guid), components(components)
    {
    }
    Guid id;
    ComponentFlags components;
  };

  struct Scene
  {

    Scene() : ecs(EntityComponentSystem{}), entities(std::vector<Entity>{}) {}

    /* --------------------------------------

     ENTITY MANAGEMENT

    -------------------------------------- */

    Entity &CreateEntity()
    {

      // first we need to see if there are any discarded guids from destroyed entities
      if (ecs.m_discardedGuids.size() == 0)
      {
        // assign a completely new guid
        Guid entityId = ecs.m_entityCounter++;
        entities.push_back(Entity{entityId, 0});
        return entities[entities.size() - 1];
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

    template <typename T>
    void RegisterComponent()
    {

        std::string typeName = std::string{ typeid(T).name() };
        if (ecs.m_componentPools[typeName] == nullptr)
        {
            Guid componentGuid = GetComponentGuid<T>();
            ecs.m_componentPools.insert({ typeName, std::make_shared<memory::ComponentPool<T>>() });
        }


    }


    template <typename T>
    T &AddComponent(Guid entityGuid, T component)
    {
      // essentially sets a flag telling the engine to
      // update the component with the corresponding system
      // next frame.

      std::string typeName = std::string{typeid(T).name()};
      RegisterComponent<T>(); // attempt to register component
      std::shared_ptr<memory::ComponentPool<T>> componentPool = std::static_pointer_cast<memory::ComponentPool<T>>(ecs.m_componentPools[typeName]);
      


      // when push_back is called, the componentPool[typeName] is an empty vector -> 

      componentPool->push_back(entityGuid, component);

      Guid componentGuid = GetComponentGuid<T>();

      entities[entityGuid].components |= (static_cast<u64>(1) << componentGuid); // mask the bit!

      T &componentData = componentPool->GetComponentData(entityGuid);
      // CONSIDER RETURNING THE COMPONENT
      return componentData;
    }

    template <typename T>
    void RemoveComponent(Guid entityGuid)
    {
      std::string typeName = std::string{typeid(T).name()};
      std::shared_ptr<memory::ComponentPool<T>> componentPool = std::static_pointer_cast<memory::ComponentPool<T>>(ecs.m_componentPools[typeName]);
      componentPool->remove(entityGuid);

      int componentId = GetComponentGuid<T>();
      entities[entityGuid] &= (~(static_cast<u64>(1) << componentId));
    }

    template <typename T>
    T &GetComponent(Guid entityGuid)
    {
      std::string typeName = std::string{typeid(T).name()};
      std::shared_ptr<memory::ComponentPool<T>> componentPool = std::static_pointer_cast<memory::ComponentPool<T>>(ecs.m_componentPools[typeName]);
      return componentPool->GetComponentData(entityGuid);
    }

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
      ecs.m_componentPools[typeName] = std::make_shared<memory::ComponentPool<T>>();

      // TODO make this more robust -> what if we no longer need a particular type of component? make some sort of wrapper that will reuse fully discarded guids

      return componentGuid;
    }

    std::vector<Entity> entities; // basically a list of component masks and names
  private:
    // Member variables
    EntityComponentSystem ecs;
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
        // need to properly debug whatever is up with this!
        // also need to fix whatever issue is causing proper components to be made for each entity.

      Iterator(EntitiesInScene &entitiesInScene) : m_scene(entitiesInScene.m_scene), m_componentMask(entitiesInScene.m_componentMask), m_all(entitiesInScene.m_all) {}

      Iterator(EntitiesInScene &entitiesInScene, Guid index) : m_scene(entitiesInScene.m_scene), m_componentMask(entitiesInScene.m_componentMask), m_all(entitiesInScene.m_all), index(index) {}

      Guid operator*() const
      {
        return m_scene.entities[index].id;
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
        Guid e = m_scene.entities[index].id;
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

        while (index < m_scene.entities.size() && (m_componentMask != entityComponentsMasked || !m_scene.isValidEntity(m_scene.entities[index].id)))
        {
          firstIndex++;
        }

        return Iterator{EntitiesInScene{m_scene}, firstIndex};
      }

      const Iterator end() const
      {
        u64 firstIndex = m_scene.entities.size() - 1;

        ComponentFlags entityComponentsMasked = (m_componentMask & m_scene.entities[index].components);

        while (index > 0 && (m_componentMask != entityComponentsMasked || !m_scene.isValidEntity(m_scene.entities[index].id)))
        {
          firstIndex--;
        }

        return Iterator{EntitiesInScene{m_scene}, firstIndex};
      }
      bool m_all;
      ComponentFlags m_componentMask;
      Scene& m_scene;
      Guid index{0};
    };


    Iterator begin() { return Iterator(*this).begin(); }
    Iterator end() { return Iterator(*this).end(); }
    Scene &m_scene;
    ComponentFlags m_componentMask{0};
    bool m_all{false};
  };

}
