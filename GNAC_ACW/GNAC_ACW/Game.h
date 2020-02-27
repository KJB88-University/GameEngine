#pragma once
#include "windows.h"
#include "windowsx.h"
#define WIN32_LEAN_AND_MEAN

// Window
#include "Window.h"


// Management
#include "Input.h"
#include "Renderer.h"
#include "Time.h"
#include "SceneManager.h"

// Systems
#include "RenderSystem.h"

class Game
{
public:
	Game();
	~Game();

	void Run();

private:

	bool OnFrame();
	void CreateSystems();

	Window* window;

	// Core Managers
	Input* input;
	Renderer* renderer;
	Time* time;
	SceneManager* sceneManager;

	// Systems
	RenderSystem* renderSystem;


	Game(const Game&) = delete;
	Game* operator=(const Game&) = delete;
};