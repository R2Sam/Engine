#include "ResourceManager.hpp"

void ResourceManager::ClearCaches()
{
	std::unique_lock lock(m_mutex);

	m_caches.clear();
}