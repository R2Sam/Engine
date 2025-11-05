#pragma once

#include "Assert.h"

#include <string>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <memory>
#include <functional>

class Cache
{
public:

	virtual ~Cache() = default;
};

template<class T>
class ResourceCache : public Cache
{
public:

	ResourceCache(const std::function<T (const char*)>& loadFunction, const std::function<void (T)>& unloadFunction) :
	_loadFunction(loadFunction),
	_unloadFunction(unloadFunction)
	{

	}

	~ResourceCache()
	{
		for (auto& [path, object] : _map)
		{
			unloadFunction(object);
		}
	}

	T& Get(const char* path)
	{
		auto it = _map.find(path);
		if (it != _map.end())
		{
			return *it->second();
		}

		auto ptr = std::make_unique<T>(_loadFunction(path));
		auto& ref = *ptr;

		_map.emplace(path, std::move(ptr));

		return ref;
	}

	void Remove(const char* path)
	{
		auto it = _map.find(path);
		if (it != _map.end())
		{
			_unloadFunction(*it->second);

			_map.erase(it);
		}
	}

private:

	std::unordered_map<std::string, std::unique_ptr<T>> _map;

	std::function<T(const char*)> _loadFunction;
	std::function<void (T)> _unloadFunction;
};

class ResourceManger
{
public:

	template<typename T, typename T2, typename... Args>
	T& AddCache(Args&&... args)
	{
		Assert((std::is_base_of_v<Cache, T>), "ResourceCache must derive from Cache");

		auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
		T& ref = *ptr;

		_caches.emplace(typeid(T2), std::move(ptr));

		return ref;
	}

	template<typename T>
	T& GetCache()
	{
		Assert(_caches.find(typeid(T)) != _caches.end(), "No cache exists holding type ", typeid(T).name());

		return *static_cast<ResourceCache<T>*>(_caches[typeid(T)].get());
	}

private:

	std::unordered_map<std::type_index, std::unique_ptr<Cache>> _caches;
};