#pragma once
#include "CTransform.h"
#include "CShape.h"
#include "CCollision.h"
#include "CInput.h"
#include "CScore.h"
#include "CLifespan.h"
#include "EntityManager.h"
#include <string>

class EntityManager;

class Entity
{
	const size_t m_id = 0;
	const std::string m_tag = "Default";
	bool m_alive = true;

	Entity(const std::string& tag, const size_t id);
	
public:
	std::shared_ptr<CTransform> cTransform;
	std::shared_ptr<CShape> cShape;
	std::shared_ptr<CCollision> cCollision;
	std::shared_ptr<CInput> cInput;
	std::shared_ptr<CScore> cScore;
	std::shared_ptr<CLifespan> cLifespan;

	void destroy();
	size_t id() const;
	const std::string& tag() const;
	bool isAlive() const;



	friend EntityManager;
	//friend std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag);
};
