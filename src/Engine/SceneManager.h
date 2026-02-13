#pragma once

#include "Assert.h"
#include "Lua/MyLua.h"

#include <memory>
#include <typeindex>
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
	 * @param args Scene constructor arguments
	 *
	 * Usage:
	 * @code
	 * sceneManager.AddScene<MenuScene>("Menu", menuTitle);
	 * @endcode
	 */

	template <typename T, typename... Args>
		requires std::is_base_of_v<Scene, T>
	void AddScene(Args&&... args)
	{
		auto ptr = std::make_unique<T>(std::forward<Args>(args)...);

		m_scenes.emplace(typeid(T), std::move(ptr));
	}

	/**
	 * @brief Removes a scene from the manager
	 *
	 * @tparam Scene type
	 */

	template <typename T>
		requires std::is_base_of_v<Scene, T>
	void RemoveScene()
	{
		auto it = m_scenes.find(typeid(T));
		if (it != m_scenes.end())
		{
			Assert(it->second.get() != m_currentScene, "Cannot delete the current scene");

			m_scenes.erase(it);
		}
	}

	/**
	 * @brief Queues a scene change at the end of the frame
	 *
	 * OnExit of the current scene will be called.
	 * OnEnter of the next scene will be called.
	 *
	 * @tparam Scene type
	 */

	template <typename T>
		requires std::is_base_of_v<Scene, T>
	void ChangeScene()
	{
		auto it = m_scenes.find(typeid(T));
		Assert(it != m_scenes.end(), "Scene ", Demangle<T>().c_str(), " does not exist");
		Assert(it->second.get() != m_currentScene, "Cannot change to the current scene");

		m_nextSceneType = typeid(T);
		m_changeScene = true;
	}

private:

	void CheckForChange();

	Scene* m_currentScene = nullptr;

	bool m_changeScene = false;
	std::type_index m_nextSceneType = typeid(void);

	std::unordered_map<std::type_index, std::unique_ptr<Scene>> m_scenes;
};