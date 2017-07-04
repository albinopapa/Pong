#pragma once

#include "ChiliWin.h"
#include "GDIPlusManager.h"
#include "ChiliException.h"
#include "Surface.h"
#include "Colors.h"
#include "Vec2.h"
#include "TexVertex.h"
#include <d2d1_1.h>
#include <d3d11.h>
#include <dwrite.h>
#include <wincodec.h>
#include <wrl.h>

#define CHILI_GFX_EXCEPTION( hr,note ) Graphics::Exception( hr,note,_CRT_WIDE(__FILE__),__LINE__ )



class Graphics
{
public:
	class Exception : public ChiliException
	{
	public:
		Exception( HRESULT hr,const std::wstring& note,const wchar_t* file,unsigned int line );
		std::wstring GetErrorName() const;
		std::wstring GetErrorDescription() const;
		virtual std::wstring GetFullMessage() const override;
		virtual std::wstring GetExceptionType() const override;
	private:
		HRESULT hr;
	};
private:
	// vertex format for the framebuffer fullscreen textured quad
	struct FSQVertex
	{
		float x,y,z;		// position
		float u,v;			// texcoords
	};
public:
	Graphics( class HWNDKey& key );
	Graphics( const Graphics& ) = delete;
	Graphics& operator=( const Graphics& ) = delete;

	void InitDirect3D( HWND WinHandle );
	void InitWIC();
	void InitDirect2D();
	void InitDirectWrite();

	void EndFrame();
	void BeginFrame();
	void DrawString( const std::wstring &Str,
					 const RectF &Field,
					 Color C);
	void DrawTriangle( const Vec2& v0,const Vec2& v1,const Vec2& v2,Color c );
	void DrawTriangleTex( const TexVertex& v0,const TexVertex& v1,const TexVertex& v2,const Surface& tex );
	void DrawTriangleTexWrap( const TexVertex& v0,const TexVertex& v1,const TexVertex& v2,const Surface& tex );
	void DrawLine( const Vec2& p1,const Vec2& p2,Color c )
	{
		DrawLine( p1.x,p1.y,p2.x,p2.y,c );
	}
	void DrawLine( float x1,float y1,float x2,float y2,Color c );
	void DrawRectangle( const RectF &Rect, Color c );
	void DrawDisc( const Vec2f &Center, float Radius, Color C );
	void PutPixel( unsigned int x, unsigned int y, unsigned int r, unsigned int g, unsigned int b )
	{
		PutPixel( x,y,{ unsigned char( r ),unsigned char( g ),unsigned char( b ) } );
	}
	void PutPixel( unsigned int x, unsigned int y,Color c )
	{
		sysBuffer.PutPixel( x,y,c );
	}
private:
	void DrawFlatTopTriangle( const Vec2& v0,const Vec2& v1,const Vec2& v2,Color c );
	void DrawFlatBottomTriangle( const Vec2& v0,const Vec2& v1,const Vec2& v2,Color c );
	void DrawFlatTopTriangleTex( const TexVertex& v0,const TexVertex& v1,const TexVertex& v2,const Surface& tex );
	void DrawFlatBottomTriangleTex( const TexVertex& v0,const TexVertex& v1,const TexVertex& v2,const Surface& tex );
	void DrawFlatTriangleTex( const TexVertex& v0,const TexVertex& v1,const TexVertex& v2,const Surface& tex,
							  const TexVertex& dv0,const TexVertex& dv1,TexVertex& itEdge1 );
	void DrawFlatTopTriangleTexWrap( const TexVertex& v0,const TexVertex& v1,const TexVertex& v2,const Surface& tex );
	void DrawFlatBottomTriangleTexWrap( const TexVertex& v0,const TexVertex& v1,const TexVertex& v2,const Surface& tex );
	void DrawFlatTriangleTexWrap( const TexVertex& v0,const TexVertex& v1,const TexVertex& v2,const Surface& tex,
							  const TexVertex& dv0,const TexVertex& dv1,TexVertex& itEdge1 );
private:
	GDIPlusManager										gdipMan;
	class COMManager
	{
	public:
		COMManager()
		{
			CoInitialize( nullptr );
		}
		~COMManager()
		{
			CoUninitialize();
		}
	}comManager;

	// Direct3D vars
	Microsoft::WRL::ComPtr<IDXGISwapChain>				pSwapChain;
	Microsoft::WRL::ComPtr<ID3D11Device>				pDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>			pImmediateContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>		pRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>				pSysBufferTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	pSysBufferTextureView;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			pPixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>			pVertexShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer>				pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>			pInputLayout;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>			pSamplerState;
	D3D11_MAPPED_SUBRESOURCE							mappedSysBufferTexture;

	// Direct2D vars
	Microsoft::WRL::ComPtr<ID2D1Factory>				pFactory2D;
	Microsoft::WRL::ComPtr<ID2D1RenderTarget>			pRenderTarget;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>		pBrush2D;

	// DirectWrite vars
	Microsoft::WRL::ComPtr<IDWriteFactory>				pFactoryWrite;
	Microsoft::WRL::ComPtr<IDWriteTextFormat>			pFormat;

	// WIC vars
	Microsoft::WRL::ComPtr<IWICImagingFactory>			pFactoryImage;
	Microsoft::WRL::ComPtr<IWICBitmap>					pRenderTargetWIC;
	
	Surface												sysBuffer;
public:
	static constexpr unsigned int ScreenWidth = 640u;
	static constexpr unsigned int ScreenHeight = 640u;
};

inline D2D1_COLOR_F ConvertChiliColor( Color C )
{
	const auto invValue = 1.f / 255.f;
	return D2D1::ColorF(
		static_cast< float >( C.GetR() ) * invValue,
		static_cast< float >( C.GetG() ) * invValue,
		static_cast< float >( C.GetB() ) * invValue,
		static_cast< float >( C.GetA() ) * invValue
	);
}
inline D2D1_RECT_F ToD2DRectF( const RectF &Rect )
{
	return *( reinterpret_cast< const D2D1_RECT_F* >( &Rect ) );
}
inline WICRect ToWICRect( const RectF &Rect )
{
	WICRect rect;
	rect.X = static_cast<int>( Rect.left );
	rect.Y = static_cast<int>( Rect.top );
	rect.Width = static_cast<int>( Rect.GetWidth() );
	rect.Height = static_cast<int>( Rect.GetHeight() );
	return rect;
}

inline void ThrowIfFail( HRESULT Hr, std::wstring &&Note, unsigned int LineNumber = __LINE__, const wchar_t *Filename = _CRT_WIDE( __FILE__ ) )
{
	if( FAILED( Hr ) )
		throw Graphics::Exception{ Hr, std::move( Note ), Filename, LineNumber };
}