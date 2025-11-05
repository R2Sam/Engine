#pragma once

//Forward
struct Context;

#include <unordered_map>
#include <memory>

class Scene
{
public:

	Scene(const Context& context) : 
	_context(context)
	{

	}

	virtual ~Scene() = default;

	virtual void Update(float deltaT);
	virtual void Draw();

	virtual void OnEnter();
	virtual void OnExit();

private:

	const Context& _context;
};

class SceneManager
{
public:

	void Update(const float deltaT);
	void Draw();

	template<typename T, typename... Args>
	void AddScene(const char* name, Args&&... args)
	{
		Assert((std::is_base_of_v<Scene, T>), "Scenes must derive from Scene");

		auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
		_scenes.emplace(name, std::move(ptr));
	}

	void RemoveScene(const char* name);
	void ChangeScene(const char* name);

private:

	Scene* _currentScene = nullptr;

	std::unordered_map<std::string, std::unique_ptr<Scene>> _scenes;
};