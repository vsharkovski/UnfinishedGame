#include "EntityManager.h"

EntityManager::EntityManager()
{

}

void EntityManager::update()
{
	for (auto e : m_toAdd)
	{
		m_entities.push_back(e);
		m_entityMap[e->tag()].push_back(e);
	}
	m_toAdd.clear();

	EntityVec nextEntities;
	for (auto& e : m_entities)
	{
		if (e->isAlive())
		{
			nextEntities.push_back(e);
		}
	}
	m_entities.swap(nextEntities);

	for (auto& kvp : m_entityMap)
	{
		nextEntities.clear();
		for (auto& e : kvp.second)
		{
			if (e->isAlive())
			{
				nextEntities.push_back(e);
			}
		}
		kvp.second.swap(nextEntities);
	}
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag)
{
	auto e = std::shared_ptr<Entity>(new Entity(tag, m_totalEntities++));
	m_toAdd.push_back(e);
	return e;
}

EntityVec& EntityManager::getEntities()
{
	return m_entities;
}

EntityVec& EntityManager::getEntities(const std::string& tag)
{
	return m_entityMap[tag];
}