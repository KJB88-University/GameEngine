#pragma once
#include "GameObject.h"

class TestObject
	: public GameObject
{
public:

	TestObject();
	TestObject(const std::string& name);
	TestObject(const TestObject& other);

	virtual ~TestObject();

	virtual void Initialize();
	virtual void Update();
	virtual void Render();

private:

};