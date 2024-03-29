#pragma once
#include "windows.h"
#include "windowsx.h"
#define WIN32_LEAN_AND_MEAN

#pragma comment(lib, "GAnC_ACW_Engine.lib")

#include "TestGame.h"

#if DX_BUILD
#include "DXWindow.h"

const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;
const float SCREEN_WIDTH = 800;
const float SCREEN_HEIGHT = 600;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	TestGame game;
	bool result;

	DXWindow* window = new DXWindow(&game, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DEPTH, SCREEN_NEAR, hInstance, nShowCmd);

	// Run the game
	window->Run();

	// Cleanup
	delete window;
	window = 0;
}
#endif

#if GL_BUILD
// Do GL stuff
#endif