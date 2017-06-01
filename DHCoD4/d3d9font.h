#ifndef _D3D9FONT_H
#define _D3D9FONT_H

#include "d3d9.h"

class CD3D9Font
{
public:
	CD3D9Font()
	{
		m_pDev = NULL;
		m_pFont = NULL;
	}

	CD3D9Font(IDirect3DDevice9* dev, LPCTSTR FontName, UINT Weight=FW_NORMAL, int Height=15, BOOL Italic=FALSE)
	{
		Initialize(dev, FontName, Weight, Height, Italic);
	}

	~CD3D9Font()
	{
		Release();
	}

	void Initialize(IDirect3DDevice9* dev, LPCTSTR FontName, UINT Weight=FW_NORMAL, int Height=15, BOOL Italic=FALSE)
	{
		m_pDev = dev;
		m_FontName = FontName;
		m_Weight = Weight;
		m_Height = Height;
		m_Italic = Italic;

		D3DVIEWPORT9 vp;
		dev->GetViewport(&vp);
		m_fontRect.bottom = vp.X;
		m_fontRect.left = vp.Y;
		m_fontRect.right = vp.X+vp.Width;
		m_fontRect.bottom = vp.Y+vp.Height;
		D3DXCreateFontA( dev, Height, 0, Weight, 1, Italic, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, FontName, &m_pFont );
	}

	void Release()
	{
		_SAFE_RELEASE(m_pFont)
	}

	INT Draw(LPCTSTR text, int x, int y, D3DCOLOR col, DWORD fmt = 0)
	{
		m_fontRect.left = x;
		m_fontRect.top = y;
		return m_pFont->DrawTextA(NULL, text, -1, &m_fontRect, fmt, col);
	}

	INT DrawOutlined(LPCTSTR text, int x, int y, D3DCOLOR col, D3DCOLOR outlinecol, DWORD fmt = 0)
	{
		Draw(text, x-1, y-1, outlinecol, fmt);
		Draw(text, x+1, y+1, outlinecol, fmt);
		Draw(text, x-1, y, outlinecol, fmt);
		Draw(text, x+1, y, outlinecol, fmt);
		Draw(text, x-1, y+1, outlinecol, fmt);
		Draw(text, x, y-1, outlinecol, fmt);
		Draw(text, x, y+1, outlinecol, fmt);
		Draw(text, x+1, y-1, outlinecol, fmt);

		Draw(text,x,y,col,fmt);
	}

	INT DrawShadowed(LPCTSTR text, int x, int y, D3DCOLOR col, D3DCOLOR outlinecol, DWORD fmt = 0)
	{
		Draw(text, x+1, y+1, outlinecol, fmt);

		Draw(text,x,y,col,fmt);
	}

	void Lost()
	{
		_SAFE_RELEASE(m_pFont);
	}

	void Reset()
	{
		D3DVIEWPORT9 vp;
		m_pDev->GetViewport(&vp);
		m_fontRect.bottom = vp.X;
		m_fontRect.left = vp.Y;
		m_fontRect.right = vp.X+vp.Width;
		m_fontRect.bottom = vp.Y+vp.Height;
		D3DXCreateFontA( m_pDev, m_Height, 0, m_Weight, 1, m_Italic, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, m_FontName, &m_pFont );
	}

	RECT m_fontRect;

private:
	IDirect3DDevice9* m_pDev;
	LPCTSTR m_FontName;
	UINT m_Weight;
	int m_Height;
	BOOL m_Italic;
	ID3DXFont* m_pFont;
};

#endif
