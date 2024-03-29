#pragma once
#define DX_BUILD = 1;

#include "windows.h"
#include "Manager.h"

#ifdef DX_BUILD
#include "DXRenderer.h"
#include "DXShader.h"
#include "DXCamera.h"
#include "DXMesh.h"
#else
#include "GLRenderer.h"
#endif

const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class GraphicsManager
	: public Manager
{

public:
	GraphicsManager(int width, int height, HWND);
	virtual ~GraphicsManager();

#ifdef DX_BUILD
	DXRenderer* GetRenderer();
#else
	GLRenderer* GetRenderer();
#endif

	// Platform independent resource setup
	//void CreateShader();
	//void CreateCamera();
	static GraphicsManager* instance;

	//bool OnFrame();

	// Pre-render setup
	void BeginScene();

	// Draw given mesh
	void Draw(DXMesh* mesh);

	// Flip swapchain
	void EndScene();

private:

	//bool Render();


#ifdef DX_BUILD
	DirectX::XMMATRIX world, view, proj;
	DXRenderer* m_renderer;
	DXMesh* m_mesh;
	DXCamera* m_camera;
	DXShader* m_shader;
#else
	GLRenderer* m_renderer;
#endif

	GraphicsManager(const GraphicsManager&) = delete;
	GraphicsManager* operator=(const GraphicsManager&) = delete;
};