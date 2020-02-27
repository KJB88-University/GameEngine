#pragma once
#include "VBO.h"

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <DirectXMath.h>

class DX_VBO
	: public VBO
{
public:

	DX_VBO();
	virtual ~DX_VBO();

	// Inherited via VBO
	virtual void Create
	(IRenderDevice* device, std::vector<Vertex> vertices, int noOfVerts) override;

	ID3D11Buffer* const* GetVBuffer() const { return &m_vBuffer; }

private:
	struct DXVertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
	};

	ID3D11Buffer* m_vBuffer;
	int m_vCount;

	//ID3D11Buffer* m_iBuffer;
	//int m_iCount;
};