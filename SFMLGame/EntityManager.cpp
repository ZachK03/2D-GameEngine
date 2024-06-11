#include "EntityManager.h"

EntityManager::EntityManager() {}
void EntityManager::update() {
	// Adds entities from m_entitiesToAdd the proper locations
	//		- add them to vector of all entities
	//		- add them to vector inside map with tag as key
	for (auto& e : m_entitiesToAdd) {
		m_entities.push_back(e);
		m_entityMap[e->tag()].push_back(e);
	}
	m_entitiesToAdd.clear();

	//Remove dead entities from vector of all
	removeDeadEntities(m_entities);

	//For each entry in map remove dead entities
	for (auto& [tag, entityVec] : m_entityMap) {
		removeDeadEntities(entityVec);
	}
}
void EntityManager::removeDeadEntities(EntityVec& vec) {
	//Remove dead entities with iterator and remove_if
	vec.erase(
		std::remove_if(vec.begin(), vec.end(),
			[](std::shared_ptr<Entity> e) {return !e->isActive(); }
		),
		vec.end()
	);
}
std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag) {
	auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));
	m_entitiesToAdd.push_back(entity);

	return entity;
}
const EntityVec& EntityManager::getEntities() {
	return m_entities;
}
const EntityVec& EntityManager::getEntities(const std::string& tag) {
	return m_entityMap[tag];
}