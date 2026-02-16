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

	CheckForChange();
}

void SceneManager::ClearScenes()
{
	m_currentScene = nullptr;
	m_changeScene = false;
	m_scenes.clear();
}

void SceneManager::CheckForChange()
{
	if (m_changeScene)
	{
		if (m_currentScene)
		{
			m_currentScene->OnExit();
		}

		auto it = m_scenes.find(m_nextSceneType);
		Assert(m_scenes.contains(m_nextSceneType), "Scene ", Demangle(m_nextSceneType).c_str(), " does not exist");

		m_currentScene = it->second.get();

		m_currentScene->OnEnter();

		m_changeScene = false;
	}
}