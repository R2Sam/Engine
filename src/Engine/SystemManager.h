#pragma once

// Forward
struct Context;

#include <type_traits>
#include <vector>
#include <memory>

class System
{
public:

	System(const Context& context) :
	_context(context)
	{

	}

	virtual ~System() = default;

	virtual void Update(const float deltaT);
	virtual void Draw();

private:

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

		auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
		_systems.push_back(std::move(ptr));
	}

private:

	std::vector<std::unique_ptr<System>> _systems;
};