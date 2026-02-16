#include "SystemManager.h"

void SystemManager::Update(const float deltaT)
{
	for (auto& pair : m_systems)
	{
		pair.second->Update(deltaT);
	}
}

void SystemManager::Draw()
{
	for (auto& pair : m_systems)
	{
		pair.second->Draw();
	}
}

void SystemManager::ClearSystems()
{
	m_systemsMap.clear();
	m_systems.clear();
}