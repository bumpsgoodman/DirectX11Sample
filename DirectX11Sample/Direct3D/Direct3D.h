#pragma once

#include <cstddef>
#include <cstdint>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <DirectXColors.h>
#include <Windows.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

class Direct3D final
{
public:
	Direct3D() = default;
	Direct3D(const Direct3D&) = delete;
	Direct3D(const Direct3D&&) = delete;
	Direct3D& operator=(const Direct3D&) = delete;
	~Direct3D();

	bool Initialize(const HWND hWnd, const HINSTANCE hInstance);
	void Cleanup();
	void Render() const;
			
private:
	bool initDevice();

private:
	HWND					mhWnd = nullptr;
	HINSTANCE				mhInstance = nullptr;

	D3D_DRIVER_TYPE			mDriverType	= D3D_DRIVER_TYPE_HARDWARE;
	D3D_FEATURE_LEVEL		mFeatureLevel = D3D_FEATURE_LEVEL_11_1;

	ID3D11Device*			mDevice = nullptr;
	ID3D11DeviceContext*	mImmediateContext = nullptr;
	IDXGISwapChain*			mSwapChain = nullptr;
	ID3D11RenderTargetView* mRenderTargetView = nullptr;
	ID3D11VertexShader*		mVertexShader = nullptr;
	ID3D11InputLayout*		mVertexLayout = nullptr;
	ID3D11PixelShader*		mPixelShader = nullptr;
	ID3D11Buffer*			mVertexBuffer = nullptr;

	uint32_t				mWidth = 0;
	uint32_t				mHeight = 0;
};

HRESULT CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);