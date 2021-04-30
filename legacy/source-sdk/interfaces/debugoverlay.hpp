#pragma once

// TODO: Fix notation

class unk_0;
class CDebugOverlay
{
public:
	virtual void            AddEntityTextOverlay( int entIndex, int lineOffset, float duration, int r, int g, int b, int a, const char *format, ... ) = 0;
	virtual void            AddBoxOverlay( const Vec3D& origin, const Vec3D& mins, const Vec3D& max, Vec3D const& orientation, int r, int g, int b, int a, float duration ) = 0;
	virtual void            AddSphereOverlay( const Vec3D& origin, float radius, int theta, int phi, int r, int g, int b, int a, float duration ) = 0;
	virtual void            AddTriangleOverlay( const Vec3D& p1, const Vec3D& p2, const Vec3D& p3, int r, int g, int b, int a, bool noDepthTest, float duration ) = 0;
	virtual void            AddLineOverlay( const Vec3D& origin, const Vec3D& dest, int r, int g, int b, bool noDepthTest, float duration ) = 0;
	virtual void            AddTextOverlay( const Vec3D& origin, float duration, const char *format, ... ) = 0;
	virtual void            AddTextOverlay( const Vec3D& origin, int lineOffset, float duration, const char *format, ... ) = 0;
	virtual void            AddScreenTextOverlay( float xPos, float yPos, float duration, int r, int g, int b, int a, const char *text ) = 0;
	virtual void            AddSweptBoxOverlay( const Vec3D& start, const Vec3D& end, const Vec3D& mins, const Vec3D& max, const Vec3D & angles, int r, int g, int b, int a, float flDuration ) = 0;
	virtual void            AddGridOverlay( const Vec3D& origin ) = 0;
	virtual void            AddCoordFrameOverlay( const matrix3x4_t& frame, float scale, int colorTable[ 3 ][ 3 ] = nullptr ) = 0;
	virtual int             ScreenPosition( const Vec3D& point, Vec3D& screen ) = 0;
	virtual int             ScreenPosition( float xPos, float yPos, Vec3D& screen ) = 0;
	virtual unk_0*			GetFirst( void ) = 0;
	virtual unk_0*			GetNext( unk_0 *current ) = 0;
	virtual void            ClearDeadOverlays( void ) = 0;
	virtual void            ClearAllOverlays( void ) = 0;
	virtual void            AddTextOverlayRGB( const Vec3D& origin, int lineOffset, float duration, float r, float g, float b, float alpha, const char *format, ... ) = 0;
	virtual void            AddTextOverlayRGB( const Vec3D& origin, int lineOffset, float duration, int r, int g, int b, int a, const char *format, ... ) = 0;
	virtual void            AddLineOverlayAlpha( const Vec3D& origin, const Vec3D& dest, int r, int g, int b, int a, bool noDepthTest, float duration ) = 0;
	virtual void            AddBoxOverlaySecond( const Vec3D& origin, const Vec3D& mins, const Vec3D& max, Vec3D const& orientation, const uint8_t* faceColor, const uint8_t* edgeColor, float duration ) = 0;
	virtual void			AddLineOverlay( const Vec3D& origin, const Vec3D& dest, int r, int g, int b, int a, float, float ) = 0;
	virtual void            PurgeTextOverlays( ) = 0;

	VF( AddCapsuleOverlay, void, 24, Vec3D& mins, Vec3D& maxs, float pillradius, int r, int g, int b, int a, float duration );

	bool m_bWorldToScreen( Vec3D world, Vec3D& screen ) 
	{
		return ( ScreenPosition( world, screen ) != 1 );
	}
};