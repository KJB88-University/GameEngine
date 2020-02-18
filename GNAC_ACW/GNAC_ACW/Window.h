#pragma once
#include "windows.h"
#include "windowsx.h"
#define WIN32_LEAN_AND_MEAN

#include "Input.h"

class Window
{
public:

	Window(int& screenWidth, int& screenHeight, Input* input);
	~Window();

	HWND GetHWND();
	LPCSTR GetAppName();
	HINSTANCE GetHINSTANCE();

	bool Messaging();
	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:

	int screenWidth;
	int screenHeight;

	LPCSTR appName;
	HINSTANCE hInstance;
	HWND hWnd;
	MSG msg;

	Input* input;

};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static Window* appHandle = 0;

