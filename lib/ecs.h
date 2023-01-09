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

// we need some way of tracking which components are assigned to an entity. we could perhaps reflect on the entity's components, and hash the types?

namespace ecs
{

  /** an entity is just data that stores components. */
  using Guid = u64;
  using ComponentFlags = u64;

  static u64 s_componentCounter{0};
  static u64 s_entityCounter{0};
  static std::vector<Guid> s_discardedGuids{};

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
          if (pair.first == guid)
          {
            return m_components[pair.second];
          }
        }
      }

    private:
      std::vector<T> m_components;                       // we're *supposed* to use an array here, but a vector will do.
      std::vector<std::pair<Guid, size_t>> m_mappings{}; // helps us access components even if others are removed.
    };
  }

  // lookup table of component GUIDs
  static std::unordered_map<std::string, Guid> s_componentGuids{};

  static std::unordered_map<std::string, std::shared_ptr<memory::IComponentPool>> s_componentPools;

  template <typename T>
  Guid GetComponentGuid()
  {

    // lookup the typename
    std::string typeName = std::string{typeid(T).name()};

    // see if a Guid already exists for this type of component
    if (auto typeLookup = s_componentGuids.find(typeName); typeLookup != s_componentGuids.end())
    { // if the type is already found, return it.
      return typeLookup->second;
    }

    // if not yet created, add the new component Guid to the lookup table.
    Guid componentGuid = s_componentCounter++;
    s_componentGuids[typeName] = componentGuid;
    s_componentPools[typeName] = std::make_shared<memory::ComponentPool<T>>();

    // TODO make this more robust -> what if we no longer need a particular type of component? make some sort of wrapper that will reuse fully discarded guids

    return componentGuid;
  }

  struct Scene
  {

    struct Entity
    {
      Guid id;
      ComponentFlags components;

      template <typename T>
      T &AddComponent(Guid entityGuid, T component)
      {
        // essentially sets a flag telling the engine to
        // update the component with the corresponding system
        // next frame.

        std::string typeName = std::string{typeid(T).name()};
        memory::ComponentPool<T> *componentPool = std::static_pointer_cast<memory::ComponentPool<T>>(s_componentPools[typeName]);
        auto component = componentPool->push_back(entityGuid, component);

        int componentGuid = GetComponentGuid<T>();

        entities[guid] |= 1 << componentId; // mask the bit!

        T &componentData = componentPool->GetComponentData(entityGuid);
        // CONSIDER RETURNING THE COMPONENT
        return componentData;
      }

      template <typename T>
      void RemoveComponent(Guid entityGuid)
      {

        std::string typeName = std::string{typeid(T).name()};
        memory::ComponentPool<T> *componentPool = std::static_pointer_cast<memory::ComponentPool<T>>(s_componentPools[typeName]);
        componentPool->remove(entityGuid);

        int componentId = GetComponentGuid<T>();
        entities[guid] &= (~(1 << componentId));
      }

      template <typename T>
      T &GetComponent(Guid entityGuid)
      {
        std::string typeName = std::string{typeid(T).name()};
        memory::ComponentPool<T> *componentPool = std::static_pointer_cast<memory::ComponentPool<T>>(s_componentPools[typeName]);
        componentPool->GetComponentData(entityGuid);
      }
    };

    Guid CreateEntity()
    {

      // first we need to see if there are any discarded guids from destroyed entities
      if (s_discardedGuids.size() != 0)
      {
        // assign a completely new guid
        Guid entityId = s_entityCounter++;
        entities.push_back({entityId, 0});
        return entityId;
      }
      else
      {
        // take the most recently discarded guid, return it.
        Guid assignedGuid = s_discardedGuids.back();
        s_discardedGuids.pop_back();
        return assignedGuid;
      }
    }

    void DestroyEntity(Guid entityGuid)
    {
      s_discardedGuids.push_back(entityGuid);
      entities[entityGuid].components = 0; // reset the component flags to 0 (empty it)
    }

    bool isValidEntity(Guid entityGuid)
    {
      for (const auto &guid : s_discardedGuids)
      {
        if (entityGuid == guid)
        {
          return false;
        }
      }
      return true;
    }

    std::vector<Entity> entities; // basically a list of component masks and names
  };

  template <typename... ComponentTypes>
  struct EntitiesInScene
  {
    EntitiesInScene(Scene &scene) : m_scene(&scene)
    {
      if (sizeof(ComponentTypes) == 0)
      {
        all = true;
      }
      else
      {
        Guid[] componentIds = {0, GetComponentGuid<ComponentTypes>()...};
        for (const auto &id : componentIds)
        {
          ComponentFlags |= id; // all we have to do is or it in~!
        }
      }
    }

    struct Iterator
    {
      Iterator(EntitiesInScene &entitiesInScene) : m_entities(entitiesInScene) {}

      Iterator(EntitiesInScene &entitiesInScene, Guid index) : m_entities(entitiesInScene), index(index) {}

      Guid operator*() const
      {
        m_entities.m_scene.entities[index].id;
      };

      bool operator==(const Iterator &other) const
      {
        return index == other.index || index == m_scene.entities.size();
      }
      bool operator!=(const Iterator &other) const
      {
        return index != other.index && index != m_scene.entities.size();
      }

      void isValidIndex()
      {
        Guid e = m_scene.entities[index].;
        if (m_scene.isValidEntity(e))
          return (m_entities.m_all) ? true : m_componentMask == m_scene.entities[index].components;
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
        } while (index < m_scene.entities.size() && !isValidIndex()) // advances index until next element with the right components is found
      }

      const Iterator begin() const
      {
        int firstIndex = 0;

        ComponentFlags entityComponentsMasked = (m_componentMask & m_scene.entities[index].components);

        while (index < m_scene.entities.size() && (m_componentMask != entityComponentsMasked || !m_scene.isValidEntity(m_scene.entities[index].id)))
        {
          firstIndex++;
        }

        return Iterator{EntitiesInScene{m_scene}, firstIndex};
      }

      EntitiesInScene &m_entities;
      Guid index{0};
    };

    Scene &m_scene;
    ComponentFlags m_componentMask{0};
    bool m_all{false};
  };

}
