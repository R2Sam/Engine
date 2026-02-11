#include "SystemManager.h"

void SystemManager::Update(const float deltaT)
{
	for (auto& pair : _systems)
	{
		pair.second->Update(deltaT);
	}
}

void SystemManager::Draw()
{
	for (auto& pair : _systems)
	{
		pair.second->Draw();
	}
}