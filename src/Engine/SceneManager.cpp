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
}

void SceneManager::RemoveScene(const char* name)
{
	auto it = _scenes.find(name);
	if (it != _scenes.end())
	{
		_scenes.erase(it);
	}
}

void SceneManager::ChangeScene(const char* name)
{
	auto it = _scenes.find(name);
	Assert(it != _scenes.end(), "Scene ", name, " does not exist");

	if (_currentScene)
	{
		_currentScene->OnExit();
	}

	_currentScene = it->second.get();

	_currentScene->OnEnter();
}

void SceneManager::SetContext(Context& context)
{
	_context = &context;
}