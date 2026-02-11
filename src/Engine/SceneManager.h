#pragma once

#include <memory>
#include <unordered_map>

/**
 * @brief Base class for all scenes
 *
 * All scenes must derive from this class,
 * and implement Update, Draw, OnEnter and OnExit.
 */

class Scene
{
public:

	virtual ~Scene()
	{
	}

	/**
	 * @brief Updates the scene
	 *
	 * Called after systems
	 *
	 * @param deltaT Duration of previous frame
	 */

	virtual void Update(const float deltaT) = 0;

	/**
	 * @brief Renders the scene
	 *
	 * Called after systems
	 */

	virtual void Draw() = 0;

	/**
	 * @brief Called by the engine when a scene is entered
	 */

	virtual void OnEnter() = 0;

	/**
	 * @brief Called by the engine when a scene is left
	 */

	virtual void OnExit() = 0;
};

/**
 * @brief Manages scenes and the the execution of the current scene
 *
 * Only one scene can be active at the time.
 * Scenes update and render after systems.
 */

class SceneManager
{
public:

	/**
	 * @brief Updates current scene
	 *
	 * @param deltaT Duration of previous frame
	 */

	void Update(const float deltaT);

	/**
	 * @brief Renders the current scene
	 */

	void Draw();

	/**
	 * @brief Adds a scene to the manager
	 *
	 * The scene is constructed in within the manager and owned by it.
	 * Scenes must derive from the Scene base class.
	 *
	 * @tparam T Scene type
	 * @tparam Args Constructor arguments types
	 * @param name Scene name which it can be referenced with
	 * @param args Scene constructor arguments
	 *
	 * Usage:
	 * @code
	 * sceneManager.AddScene<MenuScene>("Menu", menuTitle);
	 * @endcode
	 */

	template <typename T, typename... Args>
		requires std::is_base_of_v<Scene, T>
	void AddScene(const char* name, Args&&... args)
	{

		auto ptr = std::make_unique<T>(std::forward<Args>(args)...);

		_scenes.emplace(name, std::move(ptr));
	}

	/**
	 * @brief Removes a scene from the manager
	 *
	 * @param name Scene reference name
	 */

	void RemoveScene(const char* name);

	/**
	 * @brief Changed the current scene
	 *
	 * OnExit of the current scene will be called.
	 * OnEnter of the next scene will be called.
	 *
	 * @param name Scene reference name
	 */

	void ChangeScene(const char* name);

private:

	Scene* _currentScene = nullptr;

	std::unordered_map<std::string, std::unique_ptr<Scene>> _scenes;
};