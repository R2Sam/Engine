#pragma once

#include "Assert.hpp"
#include "NonCopyable.hpp"

#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <raylib.h>
#include <shared_mutex>
#include <string>
#include <typeindex>
#include <unordered_map>

/**
 * @file ResourceManager.hpp
 * @brief Engine resource caches and manager.
 */

/**
 * @brief Abstract base for type-erased resource caches
 *
 * Allows ResourceManager to store heterogeneous caches in a single map.
 */
class Cache : public NonCopyable<>
{
public:

	virtual ~Cache() = default;
};

/**
 * @brief Thread-safe cache for a single resource type
 *
 * Resources are loaded on demand, stored by path, and automatically
 * unloaded when the last shared_ptr to them is released.
 *
 * All public methods are safe to call from multiple threads concurrently.
 *
 * @tparam Resource The resource type to cache (e.g. Texture2D, Sound)
 */
template <class Resource>
class ResourceCache : public Cache
{
public:

	/**
	 * @brief Constructs a cache with the given load and unload functions
	 *
	 * @param loadFunction  Called with a file path; returns the resource or nullopt on failure
	 * @param unloadFunction Called when the last reference to a resource is dropped
	 */
	ResourceCache(const std::function<std::optional<Resource>(const std::string&)>& loadFunction,
	const std::function<void(Resource)>& unloadFunction) :
	m_loadFunction(loadFunction),
	m_unloadFunction(unloadFunction)
	{
	}

	ResourceCache(const ResourceCache&) = delete;
	ResourceCache& operator=(const ResourceCache&) = delete;

	/**
	 * @brief Looks up a cached resource by name without loading it
	 *
	 * @param name Key the resource was stored under (typically its file path)
	 * @return Shared pointer to the resource, or empty if not cached
	 */
	std::shared_ptr<Resource> Get(const std::string& name)
	{
		std::shared_lock lock(m_mutex);

		auto it = m_map.find(name);
		if (it != m_map.end())
		{
			return it->second;
		}

		return {};
	}

	/**
	 * @brief Loads a resource from disk and caches it under its path
	 *
	 * If the resource is already cached it is returned immediately without reloading.
	 *
	 * @param path File path passed to the load function
	 * @return Shared pointer to the resource, or empty if loading failed
	 */
	std::shared_ptr<Resource> Load(const std::string& path)
	{
		std::optional<Resource> opt = m_loadFunction(path);

		if (!opt)
		{
			return {};
		}

		auto unload = m_unloadFunction;

		auto ptr = std::shared_ptr<Resource>(new Resource(std::move(*opt)), [unload](Resource* resource)
		{
			unload(*resource);
			delete resource;
		});

		{
			std::unique_lock lock(m_mutex);

			m_map[path] = ptr;
		}

		return ptr;
	}

	/**
	 * @brief Stores an already-constructed resource under an arbitrary name
	 *
	 * Useful for runtime-generated resources that have no associated file path.
	 *
	 * @param object Resource to store (moved into the cache)
	 * @param name   Key to store the resource under
	 * @return Shared pointer to the stored resource
	 */
	std::shared_ptr<Resource> Add(Resource&& object, const std::string& name)
	{
		auto unload = m_unloadFunction;

		auto ptr = std::shared_ptr<Resource>(new Resource(std::move(object)), [unload](Resource* resource)
		{
			unload(*resource);
			delete resource;
		});

		{
			std::unique_lock lock(m_mutex);

			m_map[name] = ptr;
		}

		return ptr;
	}

	/**
	 * @brief Removes a resource from the cache
	 *
	 * The resource is unloaded when the last shared_ptr to it is released.
	 * Existing shared_ptrs remain valid after removal.
	 *
	 * @param path Key the resource was stored under
	 */
	void Remove(const std::string& path)
	{
		std::unique_lock lock(m_mutex);

		m_map.erase(path);
	}

private:

	std::unordered_map<std::string, std::shared_ptr<Resource>> m_map;

	std::function<std::optional<Resource>(const std::string&)> m_loadFunction;
	std::function<void(Resource)> m_unloadFunction;

	std::shared_mutex m_mutex;
};

/**
 * @brief Owns a collection of typed resource caches
 *
 * Each resource type gets its own ResourceCache. Caches are added once at
 * startup and then accessed by type anywhere in the engine.
 *
 * All public methods are safe to call from multiple threads concurrently.
 */
class ResourceManager
{
public:

	/**
	 * @brief Creates and registers a cache for a resource type
	 *
	 * If a cache for this type already exists it is returned without modification.
	 *
	 * @tparam Resource Resource type to cache
	 * @tparam Args ResourceCache constructor argument types
	 * @param args Load and unload functions forwarded to ResourceCache
	 * @return Shared pointer to the cache
	 */
	template <typename Resource, typename... Args>
	std::shared_ptr<ResourceCache<Resource>> AddCache(Args&&... args)
	{
		std::unique_lock lock(m_mutex);

		auto it = m_caches.find(typeid(Resource));
		if (it != m_caches.end())
		{
			return std::static_pointer_cast<ResourceCache<Resource>>(it->second);
		}

		auto ptr = std::make_shared<ResourceCache<Resource>>(std::forward<Args>(args)...);

		m_caches.emplace(typeid(Resource), ptr);

		return ptr;
	}

	/**
	 * @brief Returns the cache for the given resource type
	 *
	 * Asserts if no cache has been registered for this type.
	 *
	 * @tparam Resource Resource type
	 * @return Shared pointer to the ResourceCache
	 */
	template <typename Resource>
	std::shared_ptr<ResourceCache<Resource>> GetCache()
	{
		std::shared_lock lock(m_mutex);

		auto it = m_caches.find(typeid(Resource));
		Assert(it != m_caches.end(), "No cache exists holding type ", typeid(Resource).name());

		return std::static_pointer_cast<ResourceCache<Resource>>(it->second);
	}

	/**
	 * @brief Removes the cache for the given resource type
	 *
	 * Any resources still referenced by shared_ptrs remain alive until those
	 * pointers are released.
	 *
	 * @tparam Resource Resource type whose cache should be removed
	 */
	template <typename Resource>
	void RemoveCache()
	{
		std::unique_lock lock(m_mutex);

		m_caches.erase(typeid(Resource));
	}

	/**
	 * @brief Removes all registered caches
	 *
	 * Called by the Engine destructor. Resources still held by shared_ptrs
	 * remain alive until those pointers are released.
	 */
	void ClearCaches();

private:

	ResourceManager() = default;

	std::shared_mutex m_mutex;

	std::unordered_map<std::type_index, std::shared_ptr<Cache>> m_caches;

	friend class Engine;
};