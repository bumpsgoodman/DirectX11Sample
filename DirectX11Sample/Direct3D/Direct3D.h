#pragma once

#include <d3d11_1.h>
#include <Windows.h>

#pragma comment(lib, "d3d11.lib")

class Direct3D final
{
public:
	Direct3D() = default;
	Direct3D(const Direct3D&) = delete;
	Direct3D& operator=(const Direct3D&) = delete;
	~Direct3D();

	bool Init(HWND hWnd, HINSTANCE hInstance);
	void Cleanup();

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
};