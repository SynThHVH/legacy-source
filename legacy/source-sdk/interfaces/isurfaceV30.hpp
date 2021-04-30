#pragma once
//========= Copyright Valve Corporation, All rights reserved. ============//
//
// The copyright to the contents herein is the property of Valve, L.L.C.
// The contents may be used and/or copied only with the written permission of
// Valve, L.L.C., or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

typedef unsigned long HFont;

struct Vertex_t
{
	Vertex_t( ) {}
	Vertex_t( const Vec2D &pos, const Vec2D &coord = Vec2D( 0, 0 ) )
	{
		m_Position = pos;
		m_TexCoord = coord;
	}
	void Init( const Vec2D &pos, const Vec2D &coord = Vec2D( 0, 0 ) )
	{
		m_Position = pos;
		m_TexCoord = coord;
	}

	Vec2D	m_Position;
	Vec2D	m_TexCoord;
};

typedef  Vertex_t FontVertex_t;

enum FontDrawType_t
{
	// Use the "additive" value from the scheme file
	FONT_DRAW_DEFAULT = 0,

	// Overrides
	FONT_DRAW_NONADDITIVE,
	FONT_DRAW_ADDITIVE,

	FONT_DRAW_TYPE_COUNT = 2,
};


// Refactor these two
struct CharRenderInfo
{
	// In:
	FontDrawType_t	drawType;
	wchar_t			ch;

	// Out
	bool			valid;

	// In/Out (true by default)
	bool			shouldclip;
	// Text pos
	int				x, y;
	// Top left and bottom right
	Vertex_t		verts[ 2 ];
	int				textureId;
	int				abcA;
	int				abcB;
	int				abcC;
	int				fontTall;
	HFont		currentFont;
};

enum SurfaceFeature_e
{
	ANTIALIASED_FONTS = 1,
	DROPSHADOW_FONTS = 2,
	ESCAPE_KEY = 3,
	OPENING_NEW_HTML_WINDOWS = 4,
	FRAME_MINIMIZE_MAXIMIZE = 5,
	OUTLINE_FONTS = 6,
	DIRECT_HWND_RENDER = 7,
};

struct IntRect
{
	int x0;
	int y0;
	int x1;
	int y1;
};

// adds to the font
enum EFontFlags
{
	FONTFLAG_NONE,
	FONTFLAG_ITALIC = 0x001,
	FONTFLAG_UNDERLINE = 0x002,
	FONTFLAG_STRIKEOUT = 0x004,
	FONTFLAG_SYMBOL = 0x008,
	FONTFLAG_ANTIALIAS = 0x010,
	FONTFLAG_GAUSSIANBLUR = 0x020,
	FONTFLAG_ROTARY = 0x040,
	FONTFLAG_DROPSHADOW = 0x080,
	FONTFLAG_ADDITIVE = 0x100,
	FONTFLAG_OUTLINE = 0x200,
	FONTFLAG_CUSTOM = 0x400,		// custom generated font - never fall back to asian compatibility mode
	FONTFLAG_BITMAP = 0x800,		// compiled bitmap font - no fallbacks
};
//-----------------------------------------------------------------------------
// Purpose: Wraps contextless windows system functions
//-----------------------------------------------------------------------------
class ISurface
{
public:
	VF( UnLockCursor, void, 66 );
	VF( DrawSetColor, void, 14, Color col );
	VF( DrawSetColor, void, 15, int r, int g, int b, int a );
	VF( DrawFilledRect, void, 16, int x0, int y0, int x1, int y1 );
	VF( DrawOutlinedRect, void, 18, int x0, int y0, int x1, int y1 );
	VF( DrawLine, void, 19, int x0, int y0, int x1, int y1 );
	VF( DrawPolyLine, void, 20, int* x, int* y, int count);
	VF( DrawSetTextFont, void, 23, HFont );
	VF( DrawSetTextColor, void, 24, Color col );
	VF( DrawSetTextColor, void, 25, int r, int g, int b, int a );
	VF( DrawSetTextPos, void, 26, int x, int y );
	VF( DrawPrintText, void, 28, const wchar_t* text, int size );
	VF( DrawPrintText, void, 37, int iTextureID, unsigned char const* col, int width, int height );
	VF( DrawPrintText, void, 38, int iTextureID );
	VF( CreateNewTextureID, int, 43, bool procedural );
	VF( FontCreate, HFont, 71 );
	VF( SetFontGlyphSet, bool, 72, HFont font, const char* csFontName, int iHeight, int iWeight, int iBlur, int iScanLinesAmount, int iFlags, int iRangeMinimum = 0, int iRangeMaximum = 0 );
	VF( GetTextSize, void, 79, HFont font, const wchar_t *text, int &wide, int &tall );
	VF( DrawOutlinedCircle, void, 103, int x, int y, int radius, int size );
	VF( DrawTexturedPolygon, void, 106, int size, FontVertex_t *vertex, bool clipVertices = true );
};