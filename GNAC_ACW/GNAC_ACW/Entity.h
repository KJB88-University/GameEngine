#pragma once
#include <vector>

#include "Component.h"

class Entity
{

public:

	Entity(int id, const std::string& name);
	virtual ~Entity();

	const int id;
	std::string name;

	Component* const GetComponent(const std::string& componentName);
	void AddComponent(Component* component);

private:

	// Component container
	std::vector<Component*> components;

	Entity(const Entity& other);
};