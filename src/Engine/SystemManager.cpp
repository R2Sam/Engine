#include "SystemManager.hpp"

void System::Draw()
{
}

void SystemManager::Update(const float deltaT)
{
	std::unique_lock lock(m_mutex);

	for (auto& pair : m_systems)
	{
		pair.second->Update(deltaT);
	}
}

void SystemManager::Draw()
{
	std::unique_lock lock(m_mutex);

	for (auto& pair : m_systems)
	{
		pair.second->Draw();
	}
}

void SystemManager::ClearSystems()
{
	std::unique_lock lock(m_mutex);

	m_systemsMap.clear();
	m_systems.clear();
}