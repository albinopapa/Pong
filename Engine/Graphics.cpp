#include "MainWindow.h"
#include "Graphics.h"
#include "DXErr.h"
#include "ChiliException.h"
#include <assert.h>
#include <string>
#include <array>
#include <functional>

// Ignore the intellisense error "cannot open source file" for .shh files.
// They will be created during the build sequence before the preprocessor runs.
namespace FramebufferShaders
{
#include "FramebufferPS.shh"
#include "FramebufferVS.shh"
}

#pragma comment( lib,"d3d11.lib" )
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "windowscodecs.lib")

using Microsoft::WRL::ComPtr;

Graphics::Graphics( HWNDKey& key )
	:
	sysBuffer( ScreenWidth,ScreenHeight )
{
	assert( key.hWnd != nullptr );
	InitDirect3D( key.hWnd );
	InitWIC();
	InitDirect2D();
	InitDirectWrite();
}

void Graphics::InitDirect3D( HWND WinHandle )
{
	//////////////////////////////////////////////////////
	// create device and swap chain/get render target view
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferCount = 1;
	sd.BufferDesc.Width = Graphics::ScreenWidth;
	sd.BufferDesc.Height = Graphics::ScreenHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 1;
	sd.BufferDesc.RefreshRate.Denominator = 60;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = WinHandle;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	D3D_FEATURE_LEVEL	featureLevelsRequested = D3D_FEATURE_LEVEL_11_0;
	UINT				numLevelsRequested = 1;
	D3D_FEATURE_LEVEL	featureLevelsSupported;
	UINT				createFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
#ifdef USE_DIRECT3D_DEBUG_RUNTIME
	createFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
#endif

	// create device and front/back buffers
	ThrowIfFail(
		D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			createFlags,
			&featureLevelsRequested,
			numLevelsRequested,
			D3D11_SDK_VERSION,
			&sd,
			&pSwapChain,
			&pDevice,
			&featureLevelsSupported,
			&pImmediateContext ),
		L"Creating device and swap chain"
	);

	// get handle to backbuffer
	ComPtr<ID3D11Resource> pBackBuffer;
	ThrowIfFail( pSwapChain->GetBuffer( 0, IID_PPV_ARGS( &pBackBuffer ) ), 
				 L"Getting back buffer" );

	// create a view on backbuffer that we can render to
	ThrowIfFail( 
		pDevice->CreateRenderTargetView( pBackBuffer.Get(),nullptr, &pRenderTargetView ), 
		L"Creating render target view on backbuffer" );


	// set backbuffer as the render target using created view
	pImmediateContext->OMSetRenderTargets( 1, pRenderTargetView.GetAddressOf(), nullptr );


	// set viewport dimensions
	D3D11_VIEWPORT vp;
	vp.Width = float( Graphics::ScreenWidth );
	vp.Height = float( Graphics::ScreenHeight );
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	pImmediateContext->RSSetViewports( 1, &vp );


	///////////////////////////////////////
	// create texture for cpu render target
	// create the texture
	D3D11_TEXTURE2D_DESC sysTexDesc;
	sysTexDesc.Width = Graphics::ScreenWidth;
	sysTexDesc.Height = Graphics::ScreenHeight;
	sysTexDesc.MipLevels = 1;
	sysTexDesc.ArraySize = 1;
	sysTexDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sysTexDesc.SampleDesc.Count = 1;
	sysTexDesc.SampleDesc.Quality = 0;
	sysTexDesc.Usage = D3D11_USAGE_DYNAMIC;
	sysTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	sysTexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	sysTexDesc.MiscFlags = 0;

	ThrowIfFail( pDevice->CreateTexture2D( &sysTexDesc, nullptr, &pSysBufferTexture ), 
				 L"Creating sysbuffer texture" );

	// create the resource view on the texture
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = sysTexDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	ThrowIfFail(
		pDevice->CreateShaderResourceView( pSysBufferTexture.Get(), &srvDesc, &pSysBufferTextureView ), 
		L"Creating view on sysBuffer texture" );


	////////////////////////////////////////////////
	// create pixel shader for framebuffer
	// Ignore the intellisense error "namespace has no member"
	ThrowIfFail( 
		pDevice->CreatePixelShader(
			FramebufferShaders::FramebufferPSBytecode,
			sizeof( FramebufferShaders::FramebufferPSBytecode ),
			nullptr,
			&pPixelShader ),
		L"Creating pixel shader" );


	/////////////////////////////////////////////////
	// create vertex shader for framebuffer
	// Ignore the intellisense error "namespace has no member"
	ThrowIfFail(
		pDevice->CreateVertexShader(
			FramebufferShaders::FramebufferVSBytecode,
			sizeof( FramebufferShaders::FramebufferVSBytecode ),
			nullptr,
			&pVertexShader ),
		L"Creating vertex shader" );


	//////////////////////////////////////////////////////////////
	// create and fill vertex buffer with quad for rendering frame
	const FSQVertex vertices[] =
	{
		{ -1.0f, 1.0f, 0.5f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 0.5f, 1.0f, 0.0f },
		{ 1.0f, -1.0f, 0.5f, 1.0f, 1.0f },
		{ -1.0f, 1.0f, 0.5f, 0.0f, 0.0f },
		{ 1.0f, -1.0f, 0.5f, 1.0f, 1.0f },
		{ -1.0f, -1.0f, 0.5f, 0.0f, 1.0f },
	};

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( FSQVertex ) * 6;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0u;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = vertices;

	ThrowIfFail(
		pDevice->CreateBuffer( &bd, &initData, &pVertexBuffer ), 
		L"Creating vertex buffer" );


	//////////////////////////////////////////
	// create input layout for fullscreen quad
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// Ignore the intellisense error "namespace has no member"
	ThrowIfFail(
		pDevice->CreateInputLayout( 
			ied, 2, FramebufferShaders::FramebufferVSBytecode,
			sizeof( FramebufferShaders::FramebufferVSBytecode ),
			&pInputLayout ), 
		L"Creating input layout" );


	////////////////////////////////////////////////////
	// Create sampler state for fullscreen textured quad
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	ThrowIfFail(
		pDevice->CreateSamplerState( &sampDesc, &pSamplerState ),
		L"Creating sampler state" );
}

void Graphics::InitWIC()
{
	ThrowIfFail(
		CoCreateInstance(
			CLSID_WICImagingFactory,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS( pFactoryImage.GetAddressOf() )
		), L"Failed to create WIC imaging factory."
	);
	
	ThrowIfFail(
		pFactoryImage->CreateBitmap(
			ScreenWidth, 
			ScreenHeight, 
			GUID_WICPixelFormat32bppPBGRA, 
			WICBitmapCacheOnDemand,
			&pRenderTargetWIC ),
		L"Failed to create the WIC bitmap render target." );
}

void Graphics::InitDirect2D()
{
	ThrowIfFail( D2D1CreateFactory( D2D1_FACTORY_TYPE_SINGLE_THREADED, pFactory2D.GetAddressOf() ),
				 L"Failed to create Direct2D factory." );

	D2D1_RENDER_TARGET_PROPERTIES props{};
	props.pixelFormat = { DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED };
	ThrowIfFail( 
		pFactory2D->CreateWicBitmapRenderTarget( pRenderTargetWIC.Get(), props, &pRenderTarget ),
		L"Failed to create WicBitmapRenderTarget." );

	ThrowIfFail( pRenderTarget->CreateSolidColorBrush( D2D1::ColorF( 1.f, 1.f, 1.f ), &pBrush2D ),
				 L"Failed to create Direct2D solid color brush." );
}

void Graphics::InitDirectWrite()
{
	ThrowIfFail(
		DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED, __uuidof( IDWriteFactory ), &pFactoryWrite
		), L"Failed to create DirectWrite factory."
	);

	const std::wstring &FontName = L"Consolas";
	float FontSize = 32.f;
	DWRITE_FONT_WEIGHT Weight = DWRITE_FONT_WEIGHT_NORMAL;
	DWRITE_FONT_STRETCH Stretch = DWRITE_FONT_STRETCH_NORMAL;
	DWRITE_FONT_STYLE Style = DWRITE_FONT_STYLE_NORMAL;

	ThrowIfFail(
		pFactoryWrite->CreateTextFormat(
			FontName.c_str(), nullptr, Weight, Style, Stretch, FontSize, L"en-us", &pFormat
		), L"Failed to create font format." );
}

void Graphics::EndFrame()
{
	HRESULT hr;

	// lock and map the adapter memory for copying over the sysbuffer
	ThrowIfFail( 
		pImmediateContext->Map( 
			pSysBufferTexture.Get(), 
			0u, D3D11_MAP_WRITE_DISCARD, 
			0u, &mappedSysBufferTexture ), 
		L"Mapping sysbuffer" );

	// perform the copy line-by-line
	sysBuffer.Present( mappedSysBufferTexture.RowPitch,
		reinterpret_cast<BYTE*>(mappedSysBufferTexture.pData) );

	// release the adapter memory
	pImmediateContext->Unmap( pSysBufferTexture.Get(),0u );

	// render offscreen scene texture to back buffer
	pImmediateContext->IASetInputLayout( pInputLayout.Get() );
	pImmediateContext->VSSetShader( pVertexShader.Get(),nullptr,0u );
	pImmediateContext->PSSetShader( pPixelShader.Get(),nullptr,0u );
	pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	const UINT stride = sizeof( FSQVertex );
	const UINT offset = 0u;
	pImmediateContext->IASetVertexBuffers( 0u,1u,pVertexBuffer.GetAddressOf(),&stride,&offset );
	pImmediateContext->PSSetShaderResources( 0u,1u,pSysBufferTextureView.GetAddressOf() );
	pImmediateContext->PSSetSamplers( 0u,1u,pSamplerState.GetAddressOf() );
	pImmediateContext->Draw( 6u,0u );

	// flip back/front buffers
	if( FAILED( hr = pSwapChain->Present( 1u,0u ) ) )
	{
		throw CHILI_GFX_EXCEPTION( hr,L"Presenting back buffer" );
	}
}

void Graphics::BeginFrame()
{
	sysBuffer.Clear( Colors::Red );
}

void Graphics::DrawString( const std::wstring & Str, const RectF &Field, Color C )
{
	{
		pBrush2D->SetColor( ConvertChiliColor( C ) );
		const auto field = ToD2DRectF( Field );

		pRenderTarget->BeginDraw();
		pRenderTarget->Clear( ConvertChiliColor( Colors::Black ) );

		pRenderTarget->DrawText(
			Str.c_str(), static_cast<UINT32>( Str.length() ),
			pFormat.Get(), field, pBrush2D.Get() );

		ThrowIfFail( pRenderTarget->EndDraw(), L"Drawing to Direct2D render target." );
	}
	{
		WICRect rect = ToWICRect( Field );
		WicSurface surface( pRenderTargetWIC, rect, WICBitmapLockRead );
		
		for( UINT iy = 0; iy < surface.Height(); ++iy )
		{
			for( UINT ix = 0; ix < surface.Width(); ++ix )
			{
				const auto x = ix + static_cast<UINT>( Field.left );
				const auto y = iy + static_cast<UINT>( Field.top );
				const auto idx = x + ( y * ScreenWidth );

				const auto color = surface.Pixel( ix, iy );
				if( color != Color( 0, 0, 0 ) )
				{
					PutPixel( x, y, color );
				}				
			}
		}
	}
}


//////////////////////////////////////////////////
//           Graphics Exception
Graphics::Exception::Exception( HRESULT hr,const std::wstring& note,const wchar_t* file,unsigned int line )
	:
	ChiliException( file,line,note ),
	hr( hr )
{}

std::wstring Graphics::Exception::GetFullMessage() const
{
	const std::wstring empty = L"";
	const std::wstring errorName = GetErrorName();
	const std::wstring errorDesc = GetErrorDescription();
	const std::wstring& note = GetNote();
	const std::wstring location = GetLocation();
	return    (!errorName.empty() ? std::wstring( L"Error: " ) + errorName + L"\n"
		: empty)
		+ (!errorDesc.empty() ? std::wstring( L"Description: " ) + errorDesc + L"\n"
			: empty)
		+ (!note.empty() ? std::wstring( L"Note: " ) + note + L"\n"
			: empty)
		+ (!location.empty() ? std::wstring( L"Location: " ) + location
			: empty);
}

std::wstring Graphics::Exception::GetErrorName() const
{
	return DXGetErrorString( hr );
}

std::wstring Graphics::Exception::GetErrorDescription() const
{
	std::array<wchar_t,512> wideDescription;
	DXGetErrorDescription( hr,wideDescription.data(),wideDescription.size() );
	return wideDescription.data();
}

std::wstring Graphics::Exception::GetExceptionType() const
{
	return L"Chili Graphics Exception";
}

void Graphics::DrawLine( float x1,float y1,float x2,float y2,Color c )
{
	const float dx = x2 - x1;
	const float dy = y2 - y1;

	if( dy == 0.0f && dx == 0.0f )
	{
		PutPixel( int( x1 ),int( y1 ),c );
	}
	else if( abs( dy ) > abs( dx ) )
	{
		if( dy < 0.0f )
		{
			std::swap( x1,x2 );
			std::swap( y1,y2 );
		}

		const float m = dx / dy;
		float y = y1;
		int lastIntY;
		for( float x = x1; y < y2; y += 1.0f,x += m )
		{
			lastIntY = int( y );
			PutPixel( int( x ),lastIntY,c );
		}
		if( int( y2 ) > lastIntY )
		{
			PutPixel( int( x2 ),int( y2 ),c );
		}
	}
	else
	{
		if( dx < 0.0f )
		{
			std::swap( x1,x2 );
			std::swap( y1,y2 );
		}

		const float m = dy / dx;
		float x = x1;
		int lastIntX;
		for( float y = y1; x < x2; x += 1.0f,y += m )
		{
			lastIntX = int( x );
			PutPixel( lastIntX,int( y ),c );
		}
		if( int( x2 ) > lastIntX )
		{
			PutPixel( int( x2 ),int( y2 ),c );
		}
	}
}

void Graphics::DrawRectangle( const RectF & Rect, Color C )
{
	const auto xStart = std::max( -Rect.left, 0.f );
	const auto xEnd = std::min( Graphics::ScreenWidth - Rect.left, Rect.GetWidth() );
	const auto yStart = std::max( -Rect.top, 0.f );
	const auto yEnd = std::min( Graphics::ScreenHeight - Rect.top, Rect.GetHeight() );

	for( auto y = yStart; y < yEnd; ++y )
	{
		for( auto x = xStart; x < xEnd; ++x )
		{
			PutPixel( static_cast< unsigned >( x + Rect.left ),
					  static_cast< unsigned >( y + Rect.top ), C );
		}
	}
}

void Graphics::DrawDisc( const Vec2f & Center, float Radius, Color C )
{
	const auto xStart = std::max( -( Center.x - Radius ), -Radius );
	const auto xEnd = std::min( Graphics::ScreenWidth - ( Center.x - Radius ), Radius );
	const auto yStart = std::max( -( Center.y - Radius ), -Radius );
	const auto yEnd = std::min( Graphics::ScreenHeight - ( Center.y - Radius ), Radius );

	const auto rSq = sq( Radius );

	for( auto y = yStart; y < yEnd; ++y )
	{
		for( auto x = xStart; x < xEnd; ++x )
		{
			if( sq( x ) + sq( y ) <= rSq )
			{
				PutPixel(
					static_cast<unsigned>( x + Center.x ),
					static_cast<unsigned>( y + Center.y ), C );
			}
		}
	}
}

void Graphics::DrawTriangle( const Vec2& v0,const Vec2& v1,const Vec2& v2,Color c )
{
	// using pointers so we can swap (for sorting purposes)
	const Vec2* pv0 = &v0;
	const Vec2* pv1 = &v1;
	const Vec2* pv2 = &v2;

	// sorting vertices by y
	if( pv1->y < pv0->y ) std::swap( pv0,pv1 );
	if( pv2->y < pv1->y ) std::swap( pv1,pv2 );
	if( pv1->y < pv0->y ) std::swap( pv0,pv1 );

	if( pv0->y == pv1->y ) // natural flat top
	{
		// sorting top vertices by x
		if( pv1->x < pv0->x ) std::swap( pv0,pv1 );
		DrawFlatTopTriangle( *pv0,*pv1,*pv2,c );
	}
	else if( pv1->y == pv2->y ) // natural flat bottom
	{
		// sorting bottom vertices by x
		if( pv2->x < pv1->x ) std::swap( pv1,pv2 );
		DrawFlatBottomTriangle( *pv0,*pv1,*pv2,c );
	}
	else // general triangle
	{
		// find splitting vertex
		const float alphaSplit =
			(pv1->y - pv0->y) /
			(pv2->y - pv0->y);
		const Vec2 vi = *pv0 + (*pv2 - *pv0) * alphaSplit;

		if( pv1->x < vi.x ) // major right
		{
			DrawFlatBottomTriangle( *pv0,*pv1,vi,c );
			DrawFlatTopTriangle( *pv1,vi,*pv2,c );
		}
		else // major left
		{
			DrawFlatBottomTriangle( *pv0,vi,*pv1,c );
			DrawFlatTopTriangle( vi,*pv1,*pv2,c );
		}
	}
}

void Graphics::DrawTriangleTex( const TexVertex& v0,const TexVertex& v1,const TexVertex& v2,const Surface& tex )
{
	// using pointers so we can swap (for sorting purposes)
	const TexVertex* pv0 = &v0;
	const TexVertex* pv1 = &v1;
	const TexVertex* pv2 = &v2;

	// sorting vertices by y
	if( pv1->pos.y < pv0->pos.y ) std::swap( pv0,pv1 );
	if( pv2->pos.y < pv1->pos.y ) std::swap( pv1,pv2 );
	if( pv1->pos.y < pv0->pos.y ) std::swap( pv0,pv1 );

	if( pv0->pos.y == pv1->pos.y ) // natural flat top
	{
		// sorting top vertices by x
		if( pv1->pos.x < pv0->pos.x ) std::swap( pv0,pv1 );
		DrawFlatTopTriangleTex( *pv0,*pv1,*pv2,tex );
	}
	else if( pv1->pos.y == pv2->pos.y ) // natural flat bottom
	{
		// sorting bottom vertices by x
		if( pv2->pos.x < pv1->pos.x ) std::swap( pv1,pv2 );
		DrawFlatBottomTriangleTex( *pv0,*pv1,*pv2,tex );
	}
	else // general triangle
	{
		// find splitting vertex
		const float alphaSplit =
			(pv1->pos.y - pv0->pos.y) /
			(pv2->pos.y - pv0->pos.y);
		const TexVertex vi = pv0->InterpolateTo( *pv2,alphaSplit );

		if( pv1->pos.x < vi.pos.x ) // major right
		{
			DrawFlatBottomTriangleTex( *pv0,*pv1,vi,tex );
			DrawFlatTopTriangleTex( *pv1,vi,*pv2,tex );
		}
		else // major left
		{
			DrawFlatBottomTriangleTex( *pv0,vi,*pv1,tex );
			DrawFlatTopTriangleTex( vi,*pv1,*pv2,tex );
		}
	}
}

void Graphics::DrawTriangleTexWrap( const TexVertex& v0,const TexVertex& v1,const TexVertex& v2,const Surface& tex )
{
	// using pointers so we can swap (for sorting purposes)
	const TexVertex* pv0 = &v0;
	const TexVertex* pv1 = &v1;
	const TexVertex* pv2 = &v2;

	// sorting vertices by y
	if( pv1->pos.y < pv0->pos.y ) std::swap( pv0,pv1 );
	if( pv2->pos.y < pv1->pos.y ) std::swap( pv1,pv2 );
	if( pv1->pos.y < pv0->pos.y ) std::swap( pv0,pv1 );

	if( pv0->pos.y == pv1->pos.y ) // natural flat top
	{
		// sorting top vertices by x
		if( pv1->pos.x < pv0->pos.x ) std::swap( pv0,pv1 );
		DrawFlatTopTriangleTexWrap( *pv0,*pv1,*pv2,tex );
	}
	else if( pv1->pos.y == pv2->pos.y ) // natural flat bottom
	{
		// sorting bottom vertices by x
		if( pv2->pos.x < pv1->pos.x ) std::swap( pv1,pv2 );
		DrawFlatBottomTriangleTexWrap( *pv0,*pv1,*pv2,tex );
	}
	else // general triangle
	{
		// find splitting vertex
		const float alphaSplit =
			(pv1->pos.y - pv0->pos.y) /
			(pv2->pos.y - pv0->pos.y);
		const TexVertex vi = pv0->InterpolateTo( *pv2,alphaSplit );

		if( pv1->pos.x < vi.pos.x ) // major right
		{
			DrawFlatBottomTriangleTexWrap( *pv0,*pv1,vi,tex );
			DrawFlatTopTriangleTexWrap( *pv1,vi,*pv2,tex );
		}
		else // major left
		{
			DrawFlatBottomTriangleTexWrap( *pv0,vi,*pv1,tex );
			DrawFlatTopTriangleTexWrap( vi,*pv1,*pv2,tex );
		}
	}
}

void Graphics::DrawFlatTopTriangle( const Vec2& v0,const Vec2& v1,const Vec2& v2,Color c )
{
	// calulcate slopes in screen space
	const float m0 = (v2.x - v0.x) / (v2.y - v0.y);
	const float m1 = (v2.x - v1.x) / (v2.y - v1.y);

	// calculate start and end scanlines
	const int yStart = (int)ceil( v0.y - 0.5f );
	const int yEnd = (int)ceil( v2.y - 0.5f ); // the scanline AFTER the last line drawn

	for( int y = yStart; y < yEnd; y++ )
	{
		// caluclate start and end points (x-coords)
		// add 0.5 to y value because we're calculating based on pixel CENTERS
		const float px0 = m0 * (float( y ) + 0.5f - v0.y) + v0.x;
		const float px1 = m1 * (float( y ) + 0.5f - v1.y) + v1.x;

		// calculate start and end pixels
		const int xStart = (int)ceil( px0 - 0.5f );
		const int xEnd = (int)ceil( px1 - 0.5f ); // the pixel AFTER the last pixel drawn

		for( int x = xStart; x < xEnd; x++ )
		{
			PutPixel( x,y,c );
		}
	}
}

void Graphics::DrawFlatBottomTriangle( const Vec2& v0,const Vec2& v1,const Vec2& v2,Color c )
{
	// calulcate slopes in screen space
	const float m0 = (v1.x - v0.x) / (v1.y - v0.y);
	const float m1 = (v2.x - v0.x) / (v2.y - v0.y);

	// calculate start and end scanlines
	const int yStart = (int)ceil( v0.y - 0.5f );
	const int yEnd = (int)ceil( v2.y - 0.5f ); // the scanline AFTER the last line drawn

	for( int y = yStart; y < yEnd; y++ )
	{
		// caluclate start and end points
		// add 0.5 to y value because we're calculating based on pixel CENTERS
		const float px0 = m0 * (float( y ) + 0.5f - v0.y) + v0.x;
		const float px1 = m1 * (float( y ) + 0.5f - v0.y) + v0.x;

		// calculate start and end pixels
		const int xStart = (int)ceil( px0 - 0.5f );
		const int xEnd = (int)ceil( px1 - 0.5f ); // the pixel AFTER the last pixel drawn

		for( int x = xStart; x < xEnd; x++ )
		{
			PutPixel( x,y,c );
		}
	}
}

void Graphics::DrawFlatTopTriangleTex( const TexVertex& v0,const TexVertex& v1,const TexVertex& v2,const Surface& tex )
{
	// calulcate dVertex / dy
	const float delta_y = v2.pos.y - v0.pos.y;
	const TexVertex dv0 = (v2 - v0) / delta_y;
	const TexVertex dv1 = (v2 - v1) / delta_y;

	// create right edge interpolant
	TexVertex itEdge1 = v1;

	// call the flat triangle render routine
	DrawFlatTriangleTex( v0,v1,v2,tex,dv0,dv1,itEdge1 );
}

void Graphics::DrawFlatBottomTriangleTex( const TexVertex& v0,const TexVertex& v1,const TexVertex& v2,const Surface& tex )
{
	// calulcate dVertex / dy
	const float delta_y = v2.pos.y - v0.pos.y;
	const TexVertex dv0 = (v1 - v0) / delta_y;
	const TexVertex dv1 = (v2 - v0) / delta_y;

	// create right edge interpolant
	TexVertex itEdge1 = v0;

	// call the flat triangle render routine
	DrawFlatTriangleTex( v0,v1,v2,tex,dv0,dv1,itEdge1 );
}

void Graphics::DrawFlatTriangleTex( const TexVertex& v0,const TexVertex& v1,const TexVertex& v2,const Surface& tex,
									const TexVertex& dv0,const TexVertex& dv1,TexVertex& itEdge1 )
{
	// create edge interpolant for left edge (always v0)
	TexVertex itEdge0 = v0;

	// calculate start and end scanlines
	const int yStart = (int)ceil( v0.pos.y - 0.5f );
	const int yEnd = (int)ceil( v2.pos.y - 0.5f ); // the scanline AFTER the last line drawn
	
	// do interpolant prestep
	itEdge0 += dv0 * (float( yStart ) + 0.5f - v0.pos.y);
	itEdge1 += dv1 * (float( yStart ) + 0.5f - v0.pos.y);

	// init tex width/height and clamp values
	const float tex_width = float( tex.GetWidth() );
	const float tex_height = float( tex.GetHeight() );
	const float tex_clamp_x = tex_width - 1.0f;
	const float tex_clamp_y = tex_height - 1.0f;

	for( int y = yStart; y < yEnd; y++,itEdge0 += dv0,itEdge1 += dv1 )
	{
		// calculate start and end pixels
		const int xStart = (int)ceil( itEdge0.pos.x - 0.5f );
		const int xEnd = (int)ceil( itEdge1.pos.x - 0.5f ); // the pixel AFTER the last pixel drawn
		
		// calculate scanline dTexCoord / dx
		const Vec2 dtcLine = (itEdge1.tc - itEdge0.tc) / (itEdge1.pos.x - itEdge0.pos.x);

		// create scanline tex coord interpolant and prestep
		Vec2 itcLine = itEdge0.tc + dtcLine * (float( xStart ) + 0.5f - itEdge0.pos.x);

		for( int x = xStart; x < xEnd; x++,itcLine += dtcLine )
		{
			PutPixel( x,y,tex.GetPixel( 
				int( std::min( itcLine.x * tex_width,tex_clamp_x ) ),
				int( std::min( itcLine.y * tex_height,tex_clamp_y ) ) ) );
			// need std::min b/c tc.x/y == 1.0, we'll read off edge of tex
			// and with fp err, tc.x/y can be > 1.0 (by a tiny amount)
		}
	}
}

void Graphics::DrawFlatTopTriangleTexWrap( const TexVertex& v0,const TexVertex& v1,const TexVertex& v2,const Surface& tex )
{
	// calulcate dVertex / dy
	const float delta_y = v2.pos.y - v0.pos.y;
	const TexVertex dv0 = (v2 - v0) / delta_y;
	const TexVertex dv1 = (v2 - v1) / delta_y;

	// create right edge interpolant
	TexVertex itEdge1 = v1;

	// call the flat triangle render routine
	DrawFlatTriangleTexWrap( v0,v1,v2,tex,dv0,dv1,itEdge1 );
}

void Graphics::DrawFlatBottomTriangleTexWrap( const TexVertex& v0,const TexVertex& v1,const TexVertex& v2,const Surface& tex )
{
	// calulcate dVertex / dy
	const float delta_y = v2.pos.y - v0.pos.y;
	const TexVertex dv0 = (v1 - v0) / delta_y;
	const TexVertex dv1 = (v2 - v0) / delta_y;

	// create right edge interpolant
	TexVertex itEdge1 = v0;

	// call the flat triangle render routine
	DrawFlatTriangleTexWrap( v0,v1,v2,tex,dv0,dv1,itEdge1 );
}

void Graphics::DrawFlatTriangleTexWrap( const TexVertex& v0,const TexVertex& v1,const TexVertex& v2,const Surface& tex,
									const TexVertex& dv0,const TexVertex& dv1,TexVertex& itEdge1 )
{
	// create edge interpolant for left edge (always v0)
	TexVertex itEdge0 = v0;

	// calculate start and end scanlines
	const int yStart = (int)ceil( v0.pos.y - 0.5f );
	const int yEnd = (int)ceil( v2.pos.y - 0.5f ); // the scanline AFTER the last line drawn

												   // do interpolant prestep
	itEdge0 += dv0 * (float( yStart ) + 0.5f - v0.pos.y);
	itEdge1 += dv1 * (float( yStart ) + 0.5f - v0.pos.y);

	// init tex width/height and clamp values
	const float tex_width = float( tex.GetWidth() );
	const float tex_height = float( tex.GetHeight() );
	const float tex_clamp_x = tex_width - 1.0f;
	const float tex_clamp_y = tex_height - 1.0f;

	for( int y = yStart; y < yEnd; y++,itEdge0 += dv0,itEdge1 += dv1 )
	{
		// calculate start and end pixels
		const int xStart = (int)ceil( itEdge0.pos.x - 0.5f );
		const int xEnd = (int)ceil( itEdge1.pos.x - 0.5f ); // the pixel AFTER the last pixel drawn

															// calculate scanline dTexCoord / dx
		const Vec2 dtcLine = (itEdge1.tc - itEdge0.tc) / (itEdge1.pos.x - itEdge0.pos.x);

		// create scanline tex coord interpolant and prestep
		Vec2 itcLine = itEdge0.tc + dtcLine * (float( xStart ) + 0.5f - itEdge0.pos.x);

		for( int x = xStart; x < xEnd; x++,itcLine += dtcLine )
		{
			PutPixel( x,y,tex.GetPixel(
				int( std::fmod( itcLine.x * tex_width,tex_clamp_x ) ),
				int( std::fmod( itcLine.y * tex_height,tex_clamp_y ) ) ) );
			// need std::min b/c tc.x/y == 1.0, we'll read off edge of tex
			// and with fp err, tc.x/y can be > 1.0 (by a tiny amount)
		}
	}
}