#pragma once
//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//


//-----------------------------------------------------------------------------
// Purpose: Basic handler for an rgb set of colors
//			This class is fully inline
//-----------------------------------------------------------------------------
class Color
{
public:
	// constructors
	Color( )
	{
		*( ( int * ) this ) = 0;
	}
	Color( int _r, int _g, int _b )
	{
		SetColor( _r, _g, _b, 0 );
	}
	Color( int _r, int _g, int _b, int _a )
	{
		SetColor( _r, _g, _b, _a );
	}

	// set the color
	// r - red component (0-255)
	// g - green component (0-255)
	// b - blue component (0-255)
	// a - alpha component, controls transparency (0 - transparent, 255 - opaque);
	void SetColor( int _r, int _g, int _b, int _a = 0 )
	{
		_color[ 0 ] = ( unsigned char ) _r;
		_color[ 1 ] = ( unsigned char ) _g;
		_color[ 2 ] = ( unsigned char ) _b;
		_color[ 3 ] = ( unsigned char ) _a;
	}

	void GetColor( int &_r, int &_g, int &_b, int &_a ) const
	{
		_r = _color[ 0 ];
		_g = _color[ 1 ];
		_b = _color[ 2 ];
		_a = _color[ 3 ];
	}

	void SetRawColor( int color32 )
	{
		*( ( int * ) this ) = color32;
	}

	int GetRawColor( ) const
	{
		return *( ( int * ) this );
	}

	inline int r( ) const { return _color[ 0 ]; }
	inline int g( ) const { return _color[ 1 ]; }
	inline int b( ) const { return _color[ 2 ]; }
	inline int a( ) const { return _color[ 3 ]; }

	unsigned char &operator[]( int index )
	{
		return _color[ index ];
	}

	const unsigned char &operator[]( int index ) const
	{
		return _color[ index ];
	}

	bool operator == ( const Color &rhs ) const
	{
		return ( *( ( int * ) this ) == *( ( int * ) &rhs ) );
	}

	bool operator != ( const Color &rhs ) const
	{
		return !( operator==( rhs ) );
	}

	Color &operator=( const Color &rhs )
	{
		SetRawColor( rhs.GetRawColor( ) );
		return *this;
	}

	constexpr Color& FromHSV( float h, float s, float v )
	{
		float colOut[ 3 ]{ };
		if ( s == 0.0f )
		{
			_color[0] = _color[1] = _color[2] = static_cast< int >( v * 255 );
			return *this;
		}

		h = std::fmodf( h, 1.0f ) / ( 60.0f / 360.0f );
		int   i = static_cast< int >( h );
		float f = h - static_cast< float >( i );
		float p = v * ( 1.0f - s );
		float q = v * ( 1.0f - s * f );
		float t = v * ( 1.0f - s * ( 1.0f - f ) );

		switch ( i )
		{
		case 0:
			colOut[ 0 ] = v;
			colOut[ 1 ] = t;
			colOut[ 2 ] = p;
			break;
		case 1:
			colOut[ 0 ] = q;
			colOut[ 1 ] = v;
			colOut[ 2 ] = p;
			break;
		case 2:
			colOut[ 0 ] = p;
			colOut[ 1 ] = v;
			colOut[ 2 ] = t;
			break;
		case 3:
			colOut[ 0 ] = p;
			colOut[ 1 ] = q;
			colOut[ 2 ] = v;
			break;
		case 4:
			colOut[ 0 ] = t;
			colOut[ 1 ] = p;
			colOut[ 2 ] = v;
			break;
		case 5: default:
			colOut[ 0 ] = v;
			colOut[ 1 ] = p;
			colOut[ 2 ] = q;
			break;
		}

		_color[0] = static_cast< int >( colOut[ 0 ] * 255 );
		_color[1] = static_cast< int >( colOut[ 1 ] * 255 );
		_color[2] = static_cast< int >( colOut[ 2 ] * 255 );
		return *this;
	}

private:
	unsigned char _color[ 4 ];
};