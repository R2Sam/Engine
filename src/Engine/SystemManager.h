#pragma once

// Forward
#include <utility>
struct Context;

#include "Assert.h"
#include "Types.h"

#include <type_traits>
#include <vector>
#include <memory>
#include <algorithm>

class System
{
public:

	// First arguement of any derived class must be the same as here
	System(const Context& context) :
	_context(context)
	{

	}

	virtual ~System()
	{

	}

	virtual void Update(const float deltaT) = 0;
	virtual void Draw() = 0;

protected:

	const Context& _context;
};

class SystemManager
{
public:

	void Update(const float deltaT);
	void Draw();

	// Smaller priority done first
	template<typename T, typename... Args>
	T& AddSystem(const u32 priority, Args&&... args)
	{
		Assert((std::is_base_of_v<System, T>), "Systems must derive from System");
		Assert(_context, "Context must be set first");

		auto ptr = std::make_unique<T>(*_context, std::forward<Args>(args)...);
		T& ref = *ptr;

		_systems.push_back(std::make_pair(priority, std::move(ptr)));

		std::sort(_systems.begin(), _systems.end(), [](const auto& a, const auto& b)
		{
			return a.first < b.first;
		});

		return ref;
	}

	void SetContext(Context& context);

private:

	Context* _context = nullptr;

	std::vector<std::pair<u32, std::unique_ptr<System>>> _systems;
};