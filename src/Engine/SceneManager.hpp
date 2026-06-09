#pragma once

#include "Assert.hpp"
#include "Log/Log.hpp"
#include "NonCopyable.hpp"

#include <memory>
#include <mutex>
#include <typeindex>
#include <unordered_map>

/**
 * @file SceneManager.hpp
 * @brief Scene organization.
 */

/**
 * @brief Base class for all scenes
 *
 * All scenes must derive from this class and implement Update.
 */
class Scene : public NonCopyable<>
{
public:

	virtual ~Scene() = default;

	/**
	 * @brief Updates the scene
	 *
	 * Called after systems each fixed-timestep step.
	 *
	 * @param deltaT Duration of the previous frame in seconds
	 */
	virtual void Update(const float deltaT) = 0;

	/**
	 * @brief Renders the scene
	 *
	 * Called once per frame after systems. Default implementation does nothing.
	 */
	virtual void Draw() const;

	/**
	 * @brief Called by the engine when this scene becomes the active scene
	 *
	 * Use this for one-time setup that depends on the scene being visible
	 * (e.g. spawning entities, starting music). Default implementation does nothing.
	 */
	virtual void OnEnter();

	/**
	 * @brief Called by the engine just before this scene is replaced by another
	 *
	 * Use this for cleanup (e.g. destroying entities, stopping music).
	 * Default implementation does nothing.
	 */
	virtual void OnExit();
};

/**
 * @brief Manages scenes and the execution of the current active scene
 *
 * Only one scene can be active at a time. Scene transitions are queued and
 * applied at the end of the current Draw call so that OnExit and OnEnter are
 * never called mid-update.
 *
 * Scenes update and draw after systems each frame.
 */
class SceneManager
{
public:

	/**
	 * @brief Adds a scene to the manager
	 *
	 * The scene is constructed within the manager and owned by it.
	 * Scenes must derive from the Scene base class.
	 *
	 * @tparam SceneT Scene type
	 * @tparam Args Constructor argument types
	 * @param args SceneT constructor arguments
	 *
	 * Usage:
	 * @code
	 * sceneManager.AddScene<MenuScene>("Menu", menuTitle);
	 * @endcode
	 */
	template <typename SceneT, typename... Args>
		requires std::is_base_of_v<Scene, SceneT>
	void AddScene(Args&&... args)
	{
		auto ptr = std::make_unique<SceneT>(std::forward<Args>(args)...);

		std::unique_lock lock(m_mutex);

		m_scenes.emplace(typeid(SceneT), std::move(ptr));
	}

	/**
	 * @brief Removes a scene from the manager
	 *
	 * Asserts if the scene to remove is the currently active scene.
	 *
	 * @tparam SceneT Scene type to remove
	 */
	template <typename SceneT>
		requires std::is_base_of_v<Scene, SceneT>
	void RemoveScene()
	{
		std::unique_lock lock(m_mutex);

		auto it = m_scenes.find(typeid(SceneT));
		if (it != m_scenes.end())
		{
			Assert(it->second.get() != m_currentScene, "Cannot delete the current scene");

			m_scenes.erase(it);
		}
	}

	/**
	 * @brief Queues a scene transition to be applied at the end of the current frame
	 *
	 * OnExit of the current scene is called followed by OnEnter of the next scene.
	 * The transition happens inside CheckForChange at the end of Draw.
	 * Asserts if the target scene does not exist or is already the active scene.
	 *
	 * @tparam SceneT Type of the scene to transition to
	 */
	template <typename SceneT>
		requires std::is_base_of_v<Scene, SceneT>
	void ChangeScene()
	{
		std::unique_lock lock(m_mutex);

		auto it = m_scenes.find(typeid(SceneT));
		Assert(it != m_scenes.end(), "Scene ", Demangle<SceneT>().c_str(), " does not exist");
		Assert(it->second.get() != m_currentScene, "Cannot change to the current scene");

		m_nextSceneType = typeid(SceneT);
		m_changeScene = true;
	}

	/**
	 * @brief Removes all scenes and clears the active scene pointer
	 *
	 * Called by the Engine destructor.
	 */
	void ClearScenes();

private:

	/**
	 * @brief Updates the current scene
	 *
	 * @param deltaT Duration of the previous frame in seconds
	 */
	void Update(const float deltaT);

	/**
	 * @brief Draws the current scene and applies any pending scene transition
	 */
	void Draw();

	/**
	 * @brief Applies a queued scene transition if one is pending
	 *
	 * Calls OnExit on the outgoing scene and OnEnter on the incoming one.
	 */
	void CheckForChange();

	Scene* m_currentScene = nullptr;

	bool m_changeScene = false;
	std::type_index m_nextSceneType = typeid(void);

	std::mutex m_mutex;

	std::unordered_map<std::type_index, std::unique_ptr<Scene>> m_scenes;

	friend class Engine;
};