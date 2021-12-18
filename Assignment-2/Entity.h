#pragma once

#include "Common.h"
#include "Components.h"

class EntityManager;

class Entity
{
	bool m_active = true;
	const size_t m_id = 0;
	const std::string m_tag = "default";
	
	// private constructor
	Entity(const size_t id, const std::string& tag);

	friend EntityManager;
	//friend std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag);

public:
	// component pointers
	std::shared_ptr<CTransform> cTransform;
	std::shared_ptr<CShape> cShape;
	std::shared_ptr<CCollision> cCollision;
	std::shared_ptr<CInput> cInput;
	std::shared_ptr<CScore> cScore;
	std::shared_ptr<CLifespan> cLifespan;

	// private member access functions
	bool isActive() const;
	const size_t id() const;
	const std::string& tag() const;
	
	void destroy();
};
