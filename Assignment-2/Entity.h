#pragma once
#include "CTransform.h"
#include "CShape.h"
#include "CName.h"
#include "CBBox.h"
//#include "EntityManager.h"
#include <string>

class Entity
{
	const size_t m_id = 0;
	const std::string m_tag = "Default";
	bool m_alive = true;
	//Entity(const std::string& tag, const size_t id);
	//friend std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag);
	//friend class EntityManager;

public:
	Entity(const std::string& tag, const size_t id);
	std::shared_ptr<CTransform> cTransform;
	std::shared_ptr<CName> cName;
	std::shared_ptr<CShape> cShape;
	std::shared_ptr<CBBox> cBBox;
	
	void destroy();
	size_t id() const;
	std::string tag() const;
	bool isAlive() const;
};
