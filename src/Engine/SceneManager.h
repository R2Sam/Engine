#pragma once

//Forward
struct Context;

#include "Assert.h"

#include <unordered_map>
#include <memory>

class Scene
{
public:

	// First arguement of any derived class must be the same as here
	Scene(const Context& context) :
	_context(context)
	{

	}

	virtual ~Scene()
	{

	}

	virtual void Update(const float deltaT) = 0;
	virtual void Draw() = 0;

	virtual void OnEnter() = 0;
	virtual void OnExit() = 0;

protected:

	const Context& _context;
};

class SceneManager
{
public:

	void Update(const float deltaT);
	void Draw();

	template<typename T, typename... Args>
	T& AddScene(const char* name, Args&&... args)
	{
		Assert((std::is_base_of_v<Scene, T>), "Scenes must derive from Scene");
		Assert(_context, "Context must be set first");

		auto ptr = std::make_unique<T>(*_context, std::forward<Args>(args)...);
		T& ref = *ptr;

		_scenes.emplace(name, std::move(ptr));

		return ref;
	}

	void RemoveScene(const char* name);
	void ChangeScene(const char* name);

	void SetContext(Context& context);

private:

	Context* _context = nullptr;

	Scene* _currentScene = nullptr;

	std::unordered_map<std::string, std::unique_ptr<Scene>> _scenes;
};