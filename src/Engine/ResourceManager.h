#pragma once

#include "Assert.h"

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <typeindex>
#include <unordered_map>

class Cache
{
public:

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

	~ResourceCache()
	{
		for (auto& [path, object] : m_map)
		{
			m_unloadFunction(*object);
		}
	}

	T* Get(const char* path)
	{
		auto it = m_map.find(path);
		if (it != m_map.end())
		{
			return *it->second();
		}

		std::optional<T> opt = m_loadFunction(path);

		if (!opt)
		{
			return nullptr;
		}

		auto ptr = std::make_unique<T>(std::move(opt.value()));

		m_map.emplace(path, std::move(ptr));

		return ptr;
	}

	T& Add(T&& object, const char* name)
	{
		auto it = m_map.find(name);
		if (it != m_map.end())
		{
			m_unloadFunction(*it->second);
		}

		auto ptr = std::make_unique<T>(std::move(object));
		auto& ref = *ptr;

		m_map.emplace(name, std::move(ptr));

		return ref;
	}

	void Remove(const char* path)
	{
		auto it = m_map.find(path);
		if (it != m_map.end())
		{
			m_unloadFunction(*it->second);

			m_map.erase(it);
		}
	}

private:

	std::unordered_map<std::string, std::unique_ptr<T>> m_map;

	std::function<std::optional<T>(const char*)> m_loadFunction;
	std::function<void(T)> m_unloadFunction;
};

class ResourceManager
{
public:

	template <typename T, typename... Args>
	ResourceCache<T>& AddCache(Args&&... args)
	{
		auto it = m_caches.find(typeid(T));
		if (it != m_caches.end())
		{
			return *static_cast<ResourceCache<T>*>(it->second.get());
		}

		auto ptr = std::make_unique<ResourceCache<T>>(std::forward<Args>(args)...);
		ResourceCache<T>& ref = *ptr;

		m_caches.emplace(typeid(T), std::move(ptr));

		return ref;
	}

	template <typename T>
	ResourceCache<T>& GetCache()
	{
		auto it = m_caches.find(typeid(T));
		Assert(it != m_caches.end(), "No cache exists holding type ", typeid(T).name());

		return *static_cast<ResourceCache<T>*>(it->second.get());
	}

	template <typename T>
	void RemoveCache()
	{
		auto it = m_caches.find(typeid(T));
		if (it != m_caches.end())
		{
			m_caches.erase(it);
		}
	}

	void ClearCaches();

private:

	std::unordered_map<std::type_index, std::unique_ptr<Cache>> m_caches;
};