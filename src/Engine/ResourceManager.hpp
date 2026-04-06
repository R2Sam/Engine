#pragma once

#include "Assert.hpp"

#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <string>
#include <string_view>
#include <typeindex>
#include <unordered_map>

class Cache
{
public:

	Cache() = default;

	Cache(const Cache&) = delete;
	Cache& operator=(const Cache&) = delete;

	virtual ~Cache() = default;
};

template <class T>
class ResourceCache : public Cache
{
public:

	ResourceCache(const std::function<std::optional<T>(const char*)> loadFunction,
	const std::function<void(T)> unloadFunction) :
	m_loadFunction(loadFunction),
	m_unloadFunction(unloadFunction)
	{
	}

	ResourceCache(const ResourceCache&) = delete;
	ResourceCache& operator=(const ResourceCache&) = delete;

	std::shared_ptr<T> Get(std::string_view name)
	{
		std::shared_lock lock(m_mutex);

		auto it = m_map.find(name);
		if (it != m_map.end())
		{
			return it->second;
		}

		return {};
	}

	std::shared_ptr<T> Load(std::string_view path)
	{
		std::optional<T> opt = m_loadFunction(path);

		if (!opt)
		{
			return {};
		}

		auto unload = m_unloadFunction;

		auto ptr = std::shared_ptr<T>(new T(std::move(*opt)), [unload](T* t)
		{
			unload(*t);
			delete t;
		});

		{
			std::unique_lock lock(m_mutex);

			m_map[path] = ptr;
		}

		return ptr;
	}

	std::shared_ptr<T> Add(T&& object, std::string_view name)
	{
		auto unload = m_unloadFunction;

		auto ptr = std::shared_ptr<T>(new T(std::move(object)), [unload](T* t)
		{
			unload(*t);
			delete t;
		});

		{
			std::unique_lock lock(m_mutex);

			m_map[name] = ptr;
		}

		return ptr;
	}

	void Remove(std::string_view path)
	{
		std::unique_lock lock(m_mutex);

		m_map.erase(path);
	}

private:

	std::unordered_map<std::string, std::shared_ptr<T>> m_map;

	std::function<std::optional<T>(const char*)> m_loadFunction;
	std::function<void(T)> m_unloadFunction;

	std::mutex m_mutex;
};

class ResourceManager
{
public:

	template <typename T, typename... Args>
	std::shared_ptr<ResourceCache<T>> AddCache(Args&&... args)
	{
		std::unique_lock lock(m_mutex);

		auto it = m_caches.find(typeid(T));
		if (it != m_caches.end())
		{
			return it->second;
		}

		auto ptr = std::make_shared<ResourceCache<T>>(std::forward<Args>(args)...);

		m_caches.emplace(typeid(T), ptr);

		return ptr;
	}

	template <typename T>
	std::shared_ptr<ResourceCache<T>> GetCache()
	{
		std::shared_lock lock(m_mutex);

		auto it = m_caches.find(typeid(T));
		Assert(it != m_caches.end(), "No cache exists holding type ", typeid(T).name());

		return std::static_pointer_cast<ResourceCache<T>>(it->second);
	}

	template <typename T>
	void RemoveCache()
	{
		std::unique_lock lock(m_mutex);

		auto it = m_caches.find(typeid(T));
		if (it != m_caches.end())
		{
			m_caches.erase(it);
		}
	}

	void ClearCaches();

private:

	ResourceManager() = default;

	std::mutex m_mutex;

	std::unordered_map<std::type_index, std::shared_ptr<Cache>> m_caches;

	friend class Engine;
};