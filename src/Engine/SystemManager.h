#pragma once

// Forward
struct Context;

#include "Assert.h"

#include <type_traits>
#include <vector>
#include <memory>

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

	template<typename T, typename... Args>
	void AddSystem(Args&&... args)
	{
		Assert((std::is_base_of_v<System, T>), "Systems must derive from System");
		Assert(_context, "Context must be set first");

		auto ptr = std::make_unique<T>(*_context, std::forward<Args>(args)...);
		_systems.push_back(std::move(ptr));
	}

	void SetContext(Context& context);

private:

	Context* _context = nullptr;

	std::vector<std::unique_ptr<System>> _systems;
};