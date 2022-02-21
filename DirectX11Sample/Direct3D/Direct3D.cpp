#include "Direct3D.h"
#include "SimpleVertex.h"
#include "../Util/Assert.h"
#include "../Util/SafeDelete.h"

Direct3D::~Direct3D()
{
	Cleanup();
}

bool Direct3D::Initialize(const HWND hWnd, const HINSTANCE hInstance)
{
	AssertW(hWnd != nullptr, L"hWnd is nullptr");

	HRESULT hr;

	mhWnd = hWnd;
	mhInstance = hInstance;

	RECT windowRect = {};
	GetClientRect(hWnd, &windowRect);
	mWidth = windowRect.right - windowRect.left;
	mHeight = windowRect.bottom - windowRect.top;

	if (!initDevice())
	{
		AssertW(false, L"Failed to initDevice");
		return false;
	}

	// Demo
	ID3DBlob* vsBlob = nullptr;
	hr = CompileShaderFromFile(L"Shaders/SimpleTriangle_VS.hlsl", "main", "vs_5_0", &vsBlob);
	if (FAILED(hr))
	{
		SAFE_RELEASE(vsBlob);
		AssertW(false, L"Failed to compile shader from file");
		return false;
	}

	hr = mDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &mVertexShader);
	if (FAILED(hr))
	{
		AssertW(false, L"Failed to create vertex shader");
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	const uint32_t numElements = ARRAYSIZE(layout);

	hr = mDevice->CreateInputLayout(layout, numElements, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &mVertexLayout);
	SAFE_RELEASE(vsBlob);
	if (FAILED(hr))
	{
		AssertW(false, L"Failed to create input layout");
		return false;
	}

	mImmediateContext->IASetInputLayout(mVertexLayout);

	ID3DBlob* psBlob = nullptr;
	hr = CompileShaderFromFile(L"Shaders/SimpleTriangle_PS.hlsl", "main", "ps_5_0", &psBlob);
	if (FAILED(hr))
	{
		SAFE_RELEASE(vsBlob);
		AssertW(false, L"Failed to compile shader from file");
		return false;
	}

	hr = mDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &mPixelShader);
	SAFE_RELEASE(psBlob);
	if (FAILED(hr))
	{
		AssertW(false, L"Failed to create pixel shader");
		return false;
	}

	SimpleVertex vertices[] =
	{
		{ 0.0f, 0.5f, 0.5f },
		{ 0.5f, -0.5f, 0.5f },
		{ -0.5f, -0.5f, 0.5f }
	};
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 3;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = vertices;
	hr = mDevice->CreateBuffer(&bd, &initData, &mVertexBuffer);

	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	mImmediateContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);

	mImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return true;
}

void Direct3D::Cleanup()
{
	SAFE_RELEASE(mVertexBuffer);
	SAFE_RELEASE(mVertexLayout);
	SAFE_RELEASE(mVertexShader);
	SAFE_RELEASE(mPixelShader);
	SAFE_RELEASE(mRenderTargetView);
	SAFE_RELEASE(mSwapChain);
	SAFE_RELEASE(mImmediateContext);
	SAFE_RELEASE(mDevice);
}

void Direct3D::Render() const
{
	mImmediateContext->ClearRenderTargetView(mRenderTargetView, DirectX::Colors::MidnightBlue);

	mImmediateContext->VSSetShader(mVertexShader, nullptr, 0);
	mImmediateContext->PSSetShader(mPixelShader, nullptr, 0);
	mImmediateContext->Draw(3, 0);

	mSwapChain->Present(0, 0);
}

bool Direct3D::initDevice()
{
	HRESULT hr;

	uint32_t createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	const D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};
	const uint32_t numDriverTypes = ARRAYSIZE(driverTypes);

	const D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	const uint32_t numFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferCount = 1;
	sd.BufferDesc.Width = mWidth;
	sd.BufferDesc.Height = mHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = mhWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = true;

	for (uint32_t driverTypeIndex = 0; driverTypeIndex < numDriverTypes; ++driverTypeIndex)
	{
		const D3D_DRIVER_TYPE driverType = driverTypes[driverTypeIndex];

		hr = D3D11CreateDeviceAndSwapChain(nullptr, driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &mSwapChain, &mDevice, &mFeatureLevel, &mImmediateContext);
		if (SUCCEEDED(hr))
		{
			break;
		}
	}
	if (FAILED(hr))
	{
		AssertW(false, L"Failed to create device and swapchain");
		return false;
	}

	ID3D11Texture2D* backBuffer = nullptr;
	hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	if (FAILED(hr))
	{
		AssertW(false, L"Failed to get backbuffer");
		return false;
	}

	hr = mDevice->CreateRenderTargetView(backBuffer, nullptr, &mRenderTargetView);
	SAFE_RELEASE(backBuffer);
	if (FAILED(hr))
	{
		AssertW(false, L"Failed to create RenderTargetView");
		return false;
	}
	mImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, nullptr);

	D3D11_VIEWPORT vp = {};
	vp.Width = (FLOAT)mWidth;
	vp.Height = (FLOAT)mHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	mImmediateContext->RSSetViewports(1, &vp);

	return true;
}

HRESULT CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;

	// Disable optimizations to further improve shader debugging
	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* pErrorBlob = nullptr;
	hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
	if (FAILED(hr))
	{
		if (pErrorBlob)
		{
			OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
			pErrorBlob->Release();
		}
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();

	return S_OK;
}