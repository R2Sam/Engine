#include "SceneManager.h"

#include "Assert.h"

void SceneManager::Update(const float deltaT)
{
	if (_currentScene)
	{
		_currentScene->Update(deltaT);
	}
}

void SceneManager::Draw()
{
	if (_currentScene)
	{
		_currentScene->Draw();
	}

	if (!_nextSceneName.empty())
	{
		if (_currentScene)
		{
			_currentScene->OnExit();
		}

		auto it = _scenes.find(_nextSceneName);
		Assert(_scenes.find(_nextSceneName) != _scenes.end(), "Scene ", _nextSceneName.c_str(), " does not exist");

		_currentScene = it->second.get();

		_currentScene->OnEnter();

		_nextSceneName = "";
	}
}

void SceneManager::RemoveScene(const char* name)
{
	auto it = _scenes.find(name);
	if (it != _scenes.end())
	{
		Assert(it->second.get() != _currentScene, "Cannot delete the current scene");

		_scenes.erase(it);
	}
}

void SceneManager::ChangeScene(const char* name)
{
	auto it = _scenes.find(name);
	Assert(it != _scenes.end(), "Scene ", name, " does not exist");
	Assert(it->second.get() != _currentScene, "Cannot change to the current scene");

	_nextSceneName = name;
}