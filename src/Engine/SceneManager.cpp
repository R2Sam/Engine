#include "SceneManager.h"

#include "Assert.h"

void SceneManager::Update(const float deltaT)
{
	if (m_currentScene)
	{
		m_currentScene->Update(deltaT);
	}
}

void SceneManager::Draw()
{
	if (m_currentScene)
	{
		m_currentScene->Draw();
	}

	if (!m_nextSceneName.empty())
	{
		if (m_currentScene)
		{
			m_currentScene->OnExit();
		}

		auto it = m_scenes.find(m_nextSceneName);
		Assert(m_scenes.find(m_nextSceneName) != m_scenes.end(), "Scene ", m_nextSceneName.c_str(), " does not exist");

		m_currentScene = it->second.get();

		m_currentScene->OnEnter();

		m_nextSceneName = "";
	}
}

void SceneManager::RemoveScene(const char* name)
{
	auto it = m_scenes.find(name);
	if (it != m_scenes.end())
	{
		Assert(it->second.get() != m_currentScene, "Cannot delete the current scene");

		m_scenes.erase(it);
	}
}

void SceneManager::ChangeScene(const char* name)
{
	auto it = m_scenes.find(name);
	Assert(it != m_scenes.end(), "Scene ", name, " does not exist");
	Assert(it->second.get() != m_currentScene, "Cannot change to the current scene");

	m_nextSceneName = name;
}