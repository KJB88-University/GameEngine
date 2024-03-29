#include "DXRenderer.h"
#include "Logger.h"

#include "DX_VBO.h"

using namespace DirectX;

DXRenderer::DXRenderer(int screenWidth, int screenHeight, HWND hWnd, float screenDepth, float screenNear)
	: Renderer(screenWidth, screenHeight, screenDepth, screenNear)
{
	InitializeCoreD3D(screenWidth, screenHeight, hWnd);
	InitializeSecondaryD3D(screenWidth, screenHeight);
	InitializeRenderSystem();

	// World
	m_world = XMMatrixIdentity();

	m_camera = new DXCamera(screenWidth, screenHeight, screenNear, screenDepth);
	//m_camera->SetPosition(0.0f, 0.0f, -5.0f);

	m_shader = new DXShader(this);
}

DXRenderer::~DXRenderer()
{
	if (m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}

	if (m_rasterState)
	{
		m_rasterState->Release();
		m_rasterState = nullptr;
	}

	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = nullptr;
	}

	if (m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = nullptr;
	}

	if (m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = nullptr;
	}

	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = nullptr;
	}

	if (m_context)
	{
		m_context->Release();
		m_context = nullptr;
	}

	if (m_device)
	{
		m_device->Release();
		m_device = nullptr;
	}

	if (m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = nullptr;
	}
}

ID3D11Device* DXRenderer::GetDevice() const
{
	return m_device;
}

ID3D11DeviceContext* DXRenderer::GetContext() const
{
	return m_context;
}

void DXRenderer::InitializeCoreD3D(int screenWidth, int screenHeight, HWND hwnd)
{
	HRESULT result;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;

	// Init. SC descriptor
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = true;

	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	featureLevel = D3D_FEATURE_LEVEL_11_0;

	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;

	// Create Device, DeviceContext and SwapChain
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, creationFlags, &featureLevel, 1, D3D11_SDK_VERSION,
		&swapChainDesc, &m_swapChain, &m_device, NULL, &m_context);
	if (FAILED(result))
	{
		Logger::LogToConsole("DIRECTX: Failed to create graphics device, swap chain or graphics context.");
	}

	// Setup back buffer
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result))
	{
		Logger::LogToConsole("DIRECTX: Failed to get back buffer.");
	}

	// Create Render Target View
	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if (FAILED(result))
	{
		Logger::LogToConsole("DIRECTX: Failed to create render target view.");
	}

	backBufferPtr->Release();
	backBufferPtr = nullptr;
}

void DXRenderer::InitializeSecondaryD3D(int screenWidth, int screenHeight)
{
	HRESULT result;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_VIEWPORT viewport;

	// Init. depth buffer
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(result))
	{
		Logger::LogToConsole("DIRECTX: Failed to create depth stencil buffer.");
	}

	// Init. Stencil State
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil ops for front-facing
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil ops for back-facing
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(result))
	{
		Logger::LogToConsole("DIRECTX: Failed to create depth stencil state.");
	}

	m_context->OMSetDepthStencilState(m_depthStencilState, 1);

	// Init. Depth Stencil View
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result))
	{
		Logger::LogToConsole("DIRECTX: Failed to create depth stencil view.");
	}

	// Bind RTV and DSB to render pipeline
	m_context->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	// Init Rasterizer
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if (FAILED(result))
	{
		Logger::LogToConsole("DIRECTX: Failed to create rasterizer state.");
	}

	m_context->RSSetState(m_rasterState);

	// Init. viewport
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	m_context->RSSetViewports(1, &viewport);

}

void DXRenderer::InitializeRenderSystem()
{
	renderSystem = new RenderSystem(this);
}

Camera* DXRenderer::GetMainCam()
{
	return m_camera;
}

RenderSystem* DXRenderer::GetSystem()
{
	return renderSystem;
}

void DXRenderer::Draw(const Vector3& pos, const Vector3& rotation, const Vector3& scale, const Mesh* mesh)
{
	DX_VBO* vbo = dynamic_cast<DX_VBO*>(mesh->GetVBO());

	// TRANSLATE
	m_world = XMMatrixTranslation(pos.x, pos.y, pos.z);

	// ROTATE
	// SCALE
	m_world *= XMMatrixScaling(scale.x, scale.y, scale.z);

	
	vbo->Draw(this);

	m_shader->Render(GetContext(), vbo->GetIndexCount(), m_world, m_view, m_projection);
}

void DXRenderer::BeginScene(float r, float g, float b, float a)
{
	float color[4];

	color[0] = r;
	color[1] = g;
	color[2] = b;
	color[3] = a;

	GetWorldMatrix(m_world);
	GetProjectionMatrix(m_projection);
	GetViewMatrix(m_view);

	m_context->ClearRenderTargetView(m_renderTargetView, color);
	m_context->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_camera->Render();
}

void DXRenderer::EndScene()
{
	m_swapChain->Present(0, 0);
}

void DXRenderer::GetProjectionMatrix(DirectX::XMMATRIX & proj)
{
	m_camera->GetProjectionMatrix(proj);
}

void DXRenderer::GetWorldMatrix(DirectX::XMMATRIX & world)
{
	world = m_world;
}

void DXRenderer::GetOrthographicMatrix(DirectX::XMMATRIX & ortho)
{
	m_camera->GetProjectionMatrix(ortho);
}

void DXRenderer::GetViewMatrix(DirectX::XMMATRIX & view)
{
	m_camera->GetViewMatrix(view);
}