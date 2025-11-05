#include "SystemManager.h"

void SystemManager::Update(const float deltaT)
{
	for (auto& ptr : _systems)
	{
		ptr->Update(deltaT);
	}
}

void SystemManager::Draw()
{
	for (auto& ptr : _systems)
	{
		ptr->Draw();
	}
}

void SystemManager::SetContext(Context& context)
{
	_context = &context;
}