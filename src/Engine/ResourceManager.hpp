#pragma once

#include "Assert.hpp"
#include "NonCopyable.hpp"

#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <string>
#include <typeindex>
#include <unordered_map>

class Cache : public NonCopyable<>
{
public:

	virtual ~Cache() = default;
};

template <class Resource>
class ResourceCache : public Cache
{
public:

	ResourceCache(const std::function<std::optional<Resource>(const std::string&)> loadFunction,
	const std::function<void(Resource)> unloadFunction) :
	m_loadFunction(loadFunction),
	m_unloadFunction(unloadFunction)
	{
	}

	ResourceCache(const ResourceCache&) = delete;
	ResourceCache& operator=(const ResourceCache&) = delete;

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

class ResourceManager
{
public:

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

	template <typename Resource>
	std::shared_ptr<ResourceCache<Resource>> GetCache()
	{
		std::shared_lock lock(m_mutex);

		auto it = m_caches.find(typeid(Resource));
		Assert(it != m_caches.end(), "No cache exists holding type ", typeid(Resource).name());

		return std::static_pointer_cast<ResourceCache<Resource>>(it->second);
	}

	template <typename Resource>
	void RemoveCache()
	{
		std::unique_lock lock(m_mutex);

		auto it = m_caches.find(typeid(Resource));
		if (it != m_caches.end())
		{
			m_caches.erase(it);
		}
	}

	void ClearCaches();

private:

	ResourceManager() = default;

	std::shared_mutex m_mutex;

	std::unordered_map<std::type_index, std::shared_ptr<Cache>> m_caches;

	friend class Engine;
};