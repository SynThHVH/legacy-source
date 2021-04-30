#pragma once

struct Fonts_t
{
	HFont Default = NULL;
	HFont ESP = NULL;
};

namespace Draw
{
	extern Fonts_t Font;

	void Initialize( );

	static void Rect( int x, int y, int width, int height, Color col )
	{
		Interfaces::Surface->DrawSetColor( col );
		Interfaces::Surface->DrawFilledRect( x, y, x + width, y + height );
	}

	static void Line( int x, int y, int x2, int y2, Color col )
	{
		Interfaces::Surface->DrawSetColor( col );
		Interfaces::Surface->DrawLine( x, y, x2, y2 );
	}

	static void OutlinedRect( int x, int y, int width, int height, Color col )
	{
		Interfaces::Surface->DrawSetColor( col );
		Interfaces::Surface->DrawOutlinedRect( x, y, x + width, y + height );
	}

	static Vec2D TextSize( HFont font, std::wstring wtext, std::string text = "")
	{
		std::wstring wideString;

		if ( text == "" )
			wideString = wtext;
		else
		{
			for ( int i = 0; i < text.length( ); ++i )
				wideString += wchar_t( text[ i ] );
		}

		int Width, Height;
		Interfaces::Surface->GetTextSize( font, wideString.c_str( ), Width, Height );

		return Vec2D( Width, Height );
	}

	void String( int x, int y, HFont font, Color col, std::string text, int mode = 0 );
}