#include "Entity.h"

Entity::Entity(const std::string& tag, const size_t id)
	: m_tag(tag), m_id(id) {}

void Entity::destroy()
{
	m_alive = false;
}

size_t Entity::id() const
{
	return m_id;
}

const std::string& Entity::tag() const
{
	return m_tag;
}

bool Entity::isAlive() const
{
	return m_alive;
}