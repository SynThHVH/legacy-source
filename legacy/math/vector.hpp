#pragma once

typedef float vec_t;

#define ALIGN16 __declspec(align(16))
#define CHECK_VALID( _v ) 0
#define FastSqrt(x)	(sqrt)(x)

class Vec2D 
{
public:
	// The only notation exception, for clarity of course.
	float x, y;

	constexpr explicit Vec2D( float flX = 0.f, float flY = 0.f ) : x( flX ), y( flY ) { }
	constexpr explicit Vec2D( const float* flV ) : x( flV[ 0 ] ), y( flV[ 1 ] ) { }

	~Vec2D( ) { }

	// Basic
	bool m_bIsEmpty( ) const
	{
		return x == 0.f && y == 0.f;
	}

	void Clear( )
	{
		x = y = 0.f;
	}

	// Operators
	bool operator==( const Vec2D& vOther ) const
	{
		return ( x == vOther.x && y == vOther.y );
	}

	bool operator!=( const Vec2D& vOther ) const
	{
		return !( *this == vOther );
	}

	template <typename A>
	Vec2D operator+( A tOther ) const 
	{
		Vec2D vBuffer = *this;
		vBuffer += static_cast< float >( tOther );
		return vBuffer;
	}

	template <typename A>
	Vec2D operator-( A tOther ) const
	{
		Vec2D vBuffer = *this;
		vBuffer -= static_cast< float >( tOther );
		return vBuffer;
	}

	template <typename A>
	Vec2D operator*( A tOther ) const
	{
		Vec2D vBuffer = *this;
		vBuffer *= static_cast< float >( tOther );
		return vBuffer;
	}

	template <typename A>
	Vec2D operator/( A tOther ) const
	{
		Vec2D vBuffer = *this;
		vBuffer /= static_cast< float >( tOther );
		return vBuffer;
	}

	template <typename A>
	Vec2D& operator+=( A tOther )
	{
		x += static_cast< float >( tOther );
		y += static_cast< float >( tOther );

		return *this;
	}

	template <typename A>
	Vec2D& operator-=( A tOther )
	{
		x -= static_cast< float >( tOther );
		y -= static_cast< float >( tOther );

		return *this;
	}

	template <typename A>
	Vec2D& operator*=( A tOther )
	{
		x *= static_cast< float >( tOther );
		y *= static_cast< float >( tOther );

		return *this;
	}

	template <typename A>
	Vec2D& operator/=( A tOther )
	{
		x /= static_cast< float >( tOther );
		y /= static_cast< float >( tOther );

		return *this;
	}

	Vec2D operator+( const Vec2D& tOther ) const
	{
		Vec2D vBuffer = *this;

		vBuffer.x += tOther.x;
		vBuffer.y += tOther.y;

		return vBuffer;
	}

	Vec2D operator-( const Vec2D& tOther ) const
	{
		Vec2D vBuffer = *this;

		vBuffer.x -= tOther.x;
		vBuffer.y -= tOther.y;

		return vBuffer;
	}

	Vec2D& operator+=( const Vec2D& tOther )
	{
		x += tOther.x;
		y += tOther.y;

		return ( *this );
	}

	Vec2D& operator-=( const Vec2D& tOther )
	{
		x -= tOther.x;
		y -= tOther.y;

		return ( *this );
	}

	// Math
	float m_flLength( ) const 
	{
		return sqrt( m_flLengthSqr( ) );
	}

	float m_flLengthSqr( ) const
	{
		return x * x + y * y;
	}

	float m_flDistance( const Vec2D& vOther ) const
	{
		return ( vOther - ( *this ) ).m_flLength( );
	}

	void Normalize( )
	{
		*this /= m_flLength( );
	}

	Vec2D m_vNormalized( ) const
	{
		Vec2D vBase = *this;
		vBase.Normalize( );
		return vBase;
	}
};

class Vec3D 
{
public:
	// The only notation exception, for clarity of course.
	float x, y, z;
	
	constexpr explicit Vec3D( float flX = 0.f, float flY = 0.f, float flZ = 0.f ) : x( flX ), y( flY ), z( flZ ) { }
	constexpr explicit Vec3D( const float* flV ) : x( flV[ 0 ] ), y( flV[ 1 ] ), z( flV[ 2 ] ) { }

	~Vec3D( ) { }

	void Init( float ix = 0.0f, float iy = 0.0f, float iz = 0.0f );

	// Basic
	bool m_bIsEmpty( ) const 
	{
		return x == 0.f && y == 0.f && z == 0.f;
	}

	void Clear( ) 
	{
		x = y = z = 0.f;
	}

	bool m_bIsValid( ) 
	{
		return !std::isnan( x ) || !std::isnan( y ) || !std::isnan( z );
	}

	// Operators
	Vec3D& operator=( const Vec2D& vOther )
	{
		x = vOther.x;
		y = vOther.y;
		z = 0.f;
		return *this;
	}

	bool operator==( const Vec3D& vOther ) const
	{
		return ( x == vOther.x && y == vOther.y && z == vOther.z );
	}

	bool operator!=( const Vec3D& vOther ) const
	{
		return !( *this == vOther );
	}

	float& operator[]( int iMember ) 
	{
		assert( ( iMember >= 0 ) && ( iMember < 3 ) );
		return ( ( float* ) this )[ iMember ];
	}

	float operator[]( int iMember ) const
	{
		assert( ( iMember >= 0 ) && ( iMember < 3 ) );
		return ( ( float* ) this )[ iMember ];
	}

	template <typename A>
	Vec3D operator+( A tOther ) const
	{
		Vec3D vBuffer = *this;
		vBuffer += static_cast< float >( tOther );
		return vBuffer;
	}

	template <typename A>
	Vec3D operator-( A tOther ) const
	{
		Vec3D vBuffer = *this;
		vBuffer -= static_cast< float >( tOther );
		return vBuffer;
	}

	template <typename A>
	Vec3D operator*( A tOther ) const
	{
		Vec3D vBuffer = *this;
		vBuffer *= static_cast< float >( tOther );
		return vBuffer;
	}

	template <typename A>
	Vec3D operator/( A tOther ) const
	{
		Vec3D vBuffer = *this;
		vBuffer /= static_cast< float >( tOther );
		return vBuffer;
	}

	template <typename A>
	Vec3D& operator+=( A tOther )
	{
		x += static_cast< float >( tOther );
		y += static_cast< float >( tOther );
		z += static_cast< float >( tOther );

		return *this;
	}

	template <typename A>
	Vec3D& operator-=( A tOther )
	{
		x -= static_cast< float >( tOther );
		y -= static_cast< float >( tOther );
		z -= static_cast< float >( tOther );

		return *this;
	}

	template <typename A>
	Vec3D& operator*=( A tOther )
	{
		x *= static_cast< float >( tOther );
		y *= static_cast< float >( tOther );
		z *= static_cast< float >( tOther );

		return *this;
	}

	template <typename A>
	Vec3D& operator/=( A tOther )
	{
		x /= static_cast< float >( tOther );
		y /= static_cast< float >( tOther );
		z /= static_cast< float >( tOther );

		return *this;
	}

	Vec3D operator+( const Vec3D& tOther ) const
	{
		Vec3D vBuffer = *this;

		vBuffer.x += tOther.x;
		vBuffer.y += tOther.y;
		vBuffer.z += tOther.z;

		return vBuffer;
	}

	Vec3D operator-( const Vec3D& tOther ) const
	{
		Vec3D vBuffer = *this;

		vBuffer.x -= tOther.x;
		vBuffer.y -= tOther.y;
		vBuffer.z -= tOther.z;

		return vBuffer;
	}

	Vec3D& operator+=( const Vec3D& tOther )
	{
		x += tOther.x;
		y += tOther.y;
		z += tOther.z;

		return ( *this );
	}

	Vec3D& operator-=( const Vec3D& tOther )
	{
		x -= tOther.x;
		y -= tOther.y;
		z -= tOther.z;

		return ( *this );
	}

	// Math
	float m_flLength2D( ) const 
	{
		return sqrt( m_flLengthSqr2D( ) );
	}

	float m_flLengthSqr2D( ) const
	{
		return x * x + y * y;
	}

	float m_flLength( ) const 
	{
		return sqrt( m_flLengthSqr( ) );
	}

	float m_flLengthSqr( ) const
	{
		return x * x + y * y + z * z;
	}

	float m_flDistance( const Vec3D& vOther ) const
	{
		return abs( ( vOther - *this ).m_flLength( ) );
	}

	void Normalize( )
	{
		*this /= m_flLength( ) + std::numeric_limits< float >::epsilon( );
	}

	Vec3D m_vNormalized( ) const 
	{
		Vec3D vBase = *this;
		vBase.Normalize( );
		return vBase;
	}

	float m_flDot( const Vec3D& vOther ) const 
	{
		return x * vOther.x + y * vOther.y + z * vOther.z;
	}

	float m_flDot( float* flOther ) const
	{
		return x * flOther[ 0 ] + y * flOther[ 1 ] + z * flOther[ 2 ];
	}

	Vec3D m_vCrossProduct( const Vec3D& vA, const Vec3D& vB ) const
	{
		return Vec3D( vA.y*vB.z - vA.z*vB.y, vA.z*vB.x - vA.x*vB.z, vA.x*vB.y - vA.y*vB.x );
	}

	Vec3D m_vCross( const Vec3D& vOther ) const
	{
		return m_vCrossProduct( *this, vOther );
	}

	Vec3D m_vAngle( )
	{
		Vec3D vForward, vAngles;
		vForward = *this;

		if ( vForward.x == 0.f && vForward.y == 0.f ) 
		{
			vAngles.x = vForward.z > 0.f ? -90.f : 90.f;
			vAngles.y = 0.f;
		}
		else 
		{
			vAngles.x = atan2( -vForward.z, vForward.m_flLength2D( ) ) * ( 180.f / 3.1415926 );
			vAngles.y = atan2( vForward.y, vForward.x ) * ( 180.f / 3.1415926 );
		}

		vAngles.z = 0.f;

		return vAngles;
	}

	void Clamp( )
	{
		x = std::clamp( x, -89.0f, 89.0f );
		y = std::clamp( std::remainderf( y, 360.0f ), -180.f, 180.f );
		z = std::clamp( z, -50.f, 50.f );
	}

	float NormalizeInPlace( )
	{
		float radius = FastSqrt( x * x + y * y + z * z );

		// FLT_EPSILON is added to the radius to eliminate the possibility of divide by zero.
		float iradius = 1.f / ( radius + FLT_EPSILON );

		x *= iradius;
		y *= iradius;
		z *= iradius;

		return radius;
	}

	bool __inline IsZero()
	{
		return x == 0.0f && y == 0.0f && z == 0.0f; //-V550
	}

	void __inline Zero()
	{
		x = y = z = 0.0f;
	}
};

FORCEINLINE vec_t DotProduct( const Vec3D& a, const Vec3D& b )
{
	return ( a.x * b.x + a.y * b.y + a.z * b.z );
}

inline void Vec3D::Init( float ix, float iy, float iz )
{
	x = ix; y = iy; z = iz;
	CHECK_VALID( *this );
}

class ALIGN16 VectorAligned : public Vec3D
{
public:
	inline VectorAligned( void ) {};
	inline VectorAligned( float X, float Y, float Z )
	{
		Init( X, Y, Z );
	}

#ifdef VECTOR_NO_SLOW_OPERATIONS

private:
	// No copy constructors allowed if we're in optimal mode
	VectorAligned( const VectorAligned& vOther );
	VectorAligned( const Vector &vOther );

#else
public:
	explicit VectorAligned( const Vec3D &vOther )
	{
		Init( vOther.x, vOther.y, vOther.z );
	}

	VectorAligned& operator=( const Vec3D &vOther )
	{
		Init( vOther.x, vOther.y, vOther.z );
		return *this;
	}

#endif
	float w;	// this space is used anyway
};


class Vec4D 
{
public:
	// The only notation exception, for clarity of course.
	float x, y, z, w;

	constexpr explicit Vec4D( float _x = 0.f, float _y = 0.f, float _z = 0.f, float _w = 0.f ) : x( _x ), y( _y ), z( _z ), w( _w ) { }
	constexpr explicit Vec4D( const float* v ) : x( v[ 0 ] ), y( v[ 1 ] ), z( v[ 2 ] ), w( v[ 3 ] ) { }

	~Vec4D( ) { }

	// Basic
	bool m_bIsEmpty( ) const
	{
		return x == 0.f && y == 0.f && z == 0.f && w == 0.f;
	}

	void Clear( )
	{
		x = y = z = w = 0.f;
	}

	// Operators
	Vec4D& operator=( const Vec2D& vOther ) 
	{
		x = vOther.x;
		y = vOther.y;
		z = 0.f;
		w = 0.f;
		return *this;
	}

	Vec4D& operator=( const Vec3D& vOther )
	{
		x = vOther.x;
		y = vOther.y;
		z = vOther.z;
		w = 0.f;
		return *this;
	}

	Vec4D& operator=( const Vec4D& vOther )
	{
		x = vOther.x;
		y = vOther.y;
		z = vOther.z;
		w = vOther.w;
		return *this;
	}

	bool operator==( const Vec4D& vOther ) const
	{
		return ( x == vOther.x && y == vOther.y && z == vOther.z && w == vOther.w );
	}

	bool operator!=( const Vec4D& vOther ) const
	{
		return !( *this == vOther );
	}

	template <typename A>
	Vec4D operator+( A vOther ) const
	{
		Vec4D vBuffer = *this;
		vBuffer += static_cast< float >( vOther );
		return vBuffer;
	}

	template <typename A>
	Vec4D operator-( A vOther ) const
	{
		Vec4D vBuffer = *this;
		vBuffer -= static_cast< float >( vOther );
		return vBuffer;
	}

	template <typename A>
	Vec4D operator*( A vOther ) const
	{
		Vec4D vBuffer = *this;
		vBuffer *= static_cast< float >( vOther );
		return vBuffer;
	}

	template <typename A>
	Vec4D operator/( A vOther ) const
	{
		Vec4D vBuffer = *this;
		vBuffer /= static_cast< float >( vOther );
		return vBuffer;
	}

	template <typename A>
	Vec4D& operator+=( A vOther )
	{
		x += static_cast< float >( vOther );
		y += static_cast< float >( vOther );
		z += static_cast< float >( vOther );
		w += static_cast< float >( vOther );

		return *this;
	}

	template <typename A>
	Vec4D& operator-=( A vOther )
	{
		x -= static_cast< float >( vOther );
		y -= static_cast< float >( vOther );
		z -= static_cast< float >( vOther );
		w -= static_cast< float >( vOther );

		return *this;
	}

	template <typename A>
	Vec4D& operator*=( A vOther )
	{
		x *= static_cast< float >( vOther );
		y *= static_cast< float >( vOther );
		z *= static_cast< float >( vOther );
		w *= static_cast< float >( vOther );

		return *this;
	}

	template <typename A>
	Vec4D& operator/=( A vOther )
	{
		x /= static_cast< float >( vOther );
		y /= static_cast< float >( vOther );
		z /= static_cast< float >( vOther );
		w /= static_cast< float >( vOther );

		return *this;
	}

	Vec4D operator+( const Vec4D& vOther ) const
	{
		Vec4D vBuffer = *this;

		vBuffer.x += vOther.x;
		vBuffer.y += vOther.y;
		vBuffer.z += vOther.z;
		vBuffer.w += vOther.w;

		return vBuffer;
	}

	Vec4D operator-( const Vec4D& vOther ) const
	{
		Vec4D vBuffer = *this;

		vBuffer.x -= vOther.x;
		vBuffer.y -= vOther.y;
		vBuffer.z -= vOther.z;
		vBuffer.w -= vOther.w;

		return vBuffer;
	}

	Vec4D& operator+=( const Vec4D& vOther )
	{
		x += vOther.x;
		y += vOther.y;
		z += vOther.z;
		w += vOther.w;

		return ( *this );
	}

	Vec4D& operator-=( const Vec4D& vOther )
	{
		x -= vOther.x;
		y -= vOther.y;
		z -= vOther.z;
		w -= vOther.w;

		return ( *this );
	}

	// Math
	float m_flLength2D( ) const
	{
		return std::sqrt( m_flLengthSqr2D( ) );
	}

	float m_flLengthSqr2D( ) const
	{
		return x * x + y * y;
	}

	float m_flLength3D( ) const
	{
		return std::sqrt( m_flLengthSqr3D( ) );
	}

	float m_flLengthSqr3D( ) const
	{
		return x * x + y * y + z * z;
	}

	float m_flLength( ) const
	{
		return std::sqrt( m_flLengthSqr( ) );
	}

	float m_flLengthSqr( ) const
	{
		return x * x + y * y + z * z + w * w;
	}

	float m_flDistance( const Vec4D& vOther ) const 
	{
		return ( vOther - ( *this ) ).m_flLength( );
	}

	void Normalize( )
	{
		*this /= m_flLength( );
	}

	Vec4D m_vNormalized( ) const 
	{
		Vec4D pBase = *this;
		pBase.Normalize( );
		return pBase;
	}
};