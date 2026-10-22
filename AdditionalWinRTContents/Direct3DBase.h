#pragma once

#include "DirectXHelper.h"

// Helper class that initializes DirectX APIs
ref class Direct3DBase abstract
{
public:
    Direct3DBase();

    virtual void Initialize(Windows::UI::Core::CoreWindow^ window);
    virtual void CreateDeviceResources();
    virtual void CreateWindowSizeDependentResources();
    virtual void UpdateForWindowSizeChange();
    virtual void Render() = 0;
    virtual void Present();

internal:
	ID3D11Device1*          GetDevice()           { return m_d3dDevice.Get(); }
	ID3D11DeviceContext1*   GetContext()          { return m_d3dContext.Get(); }
	ID3D11RenderTargetView* GetRenderTargetView() { return m_renderTargetView.Get(); }
	D3D_FEATURE_LEVEL       GetFeatureLevel()     { return m_featureLevel; }
	float                   GetBackBufferWidth()  { return m_renderTargetSize.Width; }
	float                   GetBackBufferHeight() { return m_renderTargetSize.Height; }

protected:

    // Direct3D Objects
    Microsoft::WRL::ComPtr<ID3D11Device1>           m_d3dDevice;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext1>    m_d3dContext;
    Microsoft::WRL::ComPtr<IDXGISwapChain1>         m_swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  m_renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>  m_depthStencilView;

    D3D_FEATURE_LEVEL                               m_featureLevel;
    Windows::Foundation::Size                       m_renderTargetSize;
    Windows::Foundation::Rect                       m_windowBounds;
    Windows::UI::Core::CoreWindow^                  m_window;
};
