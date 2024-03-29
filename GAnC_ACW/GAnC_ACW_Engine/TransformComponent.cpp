#include "TransformComponent.h"
#include "Entity.h"

TransformComponent::TransformComponent(Entity* owner)
	: BaseComponent("Transform", COMPONENT_TRANSFORM, owner), 
	position(0.0f, 0.0f, 0.0f), rotation(0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f)
{
	m_owner->AddComponent(this);
}

TransformComponent::~TransformComponent()
{

}
