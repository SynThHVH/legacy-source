#include "../utilities/include.hpp"

Fonts_t Draw::Font;

/* note: maybe it is because of the fact that you are initializing your fonts the shittiest way possible */
void Draw::Initialize( ) // dosnt work fully all the time idk y might be how we are initing interfaces
{
	Font.Default = NULL;
	while ( Font.Default == NULL ) { Font.Default = Interfaces::Surface->FontCreate( ); }
	while ( !Interfaces::Surface->SetFontGlyphSet( Font.Default, "Courier New", 14, 300, 0, 0, FONTFLAG_CUSTOM | FONTFLAG_OUTLINE ) ) { }
	Font.ESP = NULL;
	while (Font.ESP == NULL) { Font.ESP = Interfaces::Surface->FontCreate(); }
	while (!Interfaces::Surface->SetFontGlyphSet(Font.ESP, "Tahoma", 16, 325, 0, 0, FONTFLAG_CUSTOM | FONTFLAG_OUTLINE | FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW )) {}
}

void Draw::String( int x, int y, HFont font, Color col, std::string text, int mode) // i tried to be different idk prob very inefficient
{
	std::wstring wideString;
	for ( int i = 0; i < text.length( ); ++i )
		wideString += wchar_t( text[ i ] );

	Interfaces::Surface->DrawSetTextFont( font );

	Vec2D Pos;

	if ( !mode )
		Pos = Vec2D( x, y );
	else
	{
		Vec2D Size = TextSize( font, wideString );

		//could make an enum 2 lazy rn tho

		if ( mode == 1 ) // center
			Pos = Vec2D( int( x - ( int( .5 + ( Size.x / 2 ) ) ) ), int( y - ( int( .5 + ( Size.y / 2 ) ) ) + 1 ) );
		else if ( mode == 2 ) // x backwards
			Pos = Vec2D( int( x - Size.x ), int( y - ( int( .5 + ( Size.y / 2 ) ) ) + 1 ) );
		else if ( mode == 3 ) // y center only
			Pos = Vec2D( x, int( y - ( int( .5 + ( Size.y / 2 ) ) ) + 1 ) );
		else if ( mode == 4 ) // x center only
			Pos = Vec2D( int( x - ( int( .5 + ( Size.x / 2 ) ) ) ), y );
	}

	Interfaces::Surface->DrawSetTextPos( int( Pos.x ), int( Pos.y ) );
	Interfaces::Surface->DrawSetTextColor( col );
	Interfaces::Surface->DrawPrintText( wideString.c_str( ), wcslen( wideString.c_str( ) ) );
}