#pragma once

// Because we are gangsters like that
#define PI 3.141592653589793238

namespace Math
{
	static float m_flRadToDeg( float flRadian ) 
	{
		return flRadian * ( 180.f / PI );
	}

	static float m_flDegToRad( float flDegree )
	{
		return flDegree * ( PI / 180.f );
	}

	inline float m_flNormalizePitch( float flPitch )
	{
		while (flPitch > 89.f)
			flPitch -= 180.f;
		while (flPitch < -89.f)
			flPitch += 180.f;

		return flPitch;
	}

	static float m_flNormalizeYaw( float flYaw )
	{
		if ( flYaw > 180.f || flYaw < -180.f )
		{
			auto revolutions = round( abs( flYaw / 360.f ) );

			if ( flYaw < 0.f )
			{
				flYaw += 360.f * revolutions;
			}
			else
			{
				flYaw -= 360.f * revolutions;
			}
		}

		return flYaw;
	}

	static void VectorAngles( const Vec3D& vForward, Vec3D& vAngles ) 
	{
		if ( vForward.x == 0.f && vForward.y == 0.f ) 
		{
			vAngles.x = vForward.z > 0.f ? -90.f : 90.f;
			vAngles.y = 0.f;
		}
		else 
		{
			vAngles.x = m_flRadToDeg( atan2( -vForward.z, vForward.m_flLength2D( ) ) );
			vAngles.y = m_flRadToDeg( atan2( vForward.y, vForward.x ) );
		}

		vAngles.z = 0.f;
	}

	static void VectorAngles( const Vec3D& vForward, Vec3D& vUp, Vec3D& vAngles ) 
	{
		Vec3D vLeft = Vec3D( vForward.y * vUp.z - vForward.z * vUp.y, vForward.z * vUp.x - vForward.x * vUp.z, vForward.x * vUp.y - vForward.y * vUp.x );
		vLeft.Normalize( );

		float flForwardDistance = vForward.m_flLength2D( );
		if ( flForwardDistance > 0.001f ) 
		{
			vAngles.x = m_flRadToDeg( atan2f( -vForward.z, flForwardDistance ) );
			vAngles.y = m_flRadToDeg( atan2f( vForward.y, vForward.x ) );

			float flUpZ = ( vLeft.y * vForward.x ) - ( vLeft.x * vForward.y );
			vAngles.z = m_flRadToDeg( atan2f( vLeft.z, flUpZ ) ) ;
		}
		else 
		{
			vAngles.x = m_flRadToDeg( atan2f( -vForward.z, flForwardDistance ) );
			vAngles.y = m_flRadToDeg( atan2f( -vLeft.x, vLeft.y ) );
			vAngles.z = 0;
		}
	}

	static void VectorTransform( const Vec3D in1, const matrix3x4_t in2, Vec3D &out ) {
		out[ 0 ] = DotProduct( in1, Vec3D( in2[ 0 ][ 0 ], in2[ 0 ][ 1 ], in2[ 0 ][ 2 ] ) ) + in2[ 0 ][ 3 ];
		out[ 1 ] = DotProduct( in1, Vec3D( in2[ 1 ][ 0 ], in2[ 1 ][ 1 ], in2[ 1 ][ 2 ] ) ) + in2[ 1 ][ 3 ];
		out[ 2 ] = DotProduct( in1, Vec3D( in2[ 2 ][ 0 ], in2[ 2 ][ 1 ], in2[ 2 ][ 2 ] ) ) + in2[ 2 ][ 3 ];
	}

	static void SinCos( float flRadian, float* flSin, float* flCos )
	{
		*flSin = std::sin( flRadian );
		*flCos = std::cos( flRadian );
	}

	inline Vec3D NormalizeAngle(Vec3D angle)
	{
		angle.x = m_flNormalizePitch(angle.x);
		angle.y = m_flNormalizeYaw(angle.y);

		if ((angle.z > 50) || (angle.z < 50))
			angle.z = 0;

		return angle;
	}

	static void NormalizeAngles( Vec3D& angles )
	{
		for ( auto i = 0; i < 3; i++ ) {
			while ( angles[ i ] < -180.0f ) angles[ i ] += 360.0f;
			while ( angles[ i ] > 180.0f ) angles[ i ] -= 360.0f;
		}
	}

	static float NormalizeFloat(float angle)
	{
		auto revolutions = angle / 360.f;
		if (angle > 180.f || angle < -180.f)
		{
			revolutions = round(abs(revolutions));
			if (angle < 0.f)
				angle = (angle + 360.f * revolutions);
			else
				angle = (angle - 360.f * revolutions);
			return angle;
		}
		return angle;
	}

	static void AngleVectors( const Vec3D& vAngles, Vec3D* vForward = nullptr, Vec3D* vRight = nullptr, Vec3D* vUp = nullptr )
	{
		// TODO: Gay, fix notation... PASTED!!!
		float sp, sy, sr, cp, cy, cr;

		SinCos( m_flDegToRad( vAngles.x ), &sp, &cp );
		SinCos( m_flDegToRad( vAngles.y ), &sy, &cy );
		SinCos( m_flDegToRad( vAngles.z ), &sr, &cr );

		if ( vForward != nullptr )
		{
			vForward->x = cp * cy;
			vForward->y = cp * sy;
			vForward->z = -sp;
		}

		if ( vRight != nullptr )
		{
			vRight->x = -1 * sr * sp * cy + -1 * cr * -sy;
			vRight->y = -1 * sr * sp * sy + -1 * cr * cy;
			vRight->z = -1 * sr * cp;
		}

		if ( vUp != nullptr )
		{
			vUp->x = cr * sp * cy + -sr * -sy;
			vUp->y = cr * sp * sy + -sr * cy;
			vUp->z = cr * cp;
		}
	}

	static float Distance2D( Vec2D point1, Vec2D point2 )
	{
		float diffY = point1.y - point2.y;
		float diffX = point1.x - point2.x;
		return sqrt( ( diffY * diffY ) + ( diffX * diffX ) );
	}

	static Vec3D CalcAngle( const Vec3D& vecSource, const Vec3D& vecDestination ) // cssimp
	{
		Vec3D vAngle;
		Vec3D delta( ( vecSource.x - vecDestination.x ), ( vecSource.y - vecDestination.y ), ( vecSource.z - vecDestination.z ) );
		double hyp = sqrt( delta.x*delta.x + delta.y*delta.y );

		vAngle.x = float( atanf( float( delta.z / hyp ) ) * 57.295779513082f );
		vAngle.y = float( atanf( float( delta.y / delta.x ) ) * 57.295779513082f );
		vAngle.z = 0.0f;

		if ( delta.x >= 0.0 )
			vAngle.y += 180.0f;

		return vAngle;
	}

	static void RandomSeed( int iSeed )
	{
		static auto iRandomSeed = reinterpret_cast< void( *)( int ) >( GetProcAddress( GetModuleHandleA( "vstdlib.dll" ), "RandomSeed" ) );
		return iRandomSeed( iSeed );
	}

	static float RandomFloat( float flMin, float flMax )
	{
		static auto flRandomFloat = reinterpret_cast< float( *)( float, float ) >( GetProcAddress( GetModuleHandleA( "vstdlib.dll" ), "RandomFloat" ) );
		return flRandomFloat( flMin, flMax );
	}

	static int RandomInt( int iMin, int iMax )
	{
		return iMin + rand( ) / ( RAND_MAX / ( iMax - iMin + 1 ) + 1 );
	}

	static void MatrixGetColumn( const matrix3x4_t& in, int column, Vec3D &out )
	{
		out.x = in[ 0 ][ column ];
		out.y = in[ 1 ][ column ];
		out.z = in[ 2 ][ column ];
	}

	static void MatrixAngles( const matrix3x4_t& matrix, Vec3D &angles ) // valve sdk
	{
		float forward[ 3 ];
		float left[ 3 ];
		float up[ 3 ];

		forward[ 0 ] = matrix[ 0 ][ 0 ];
		forward[ 1 ] = matrix[ 1 ][ 0 ];
		forward[ 2 ] = matrix[ 2 ][ 0 ];
		left[ 0 ] = matrix[ 0 ][ 1 ];
		left[ 1 ] = matrix[ 1 ][ 1 ];
		left[ 2 ] = matrix[ 2 ][ 1 ];
		up[ 2 ] = matrix[ 2 ][ 2 ];

		float xyDist = sqrtf( forward[ 0 ] * forward[ 0 ] + forward[ 1 ] * forward[ 1 ] );

		if ( xyDist > 0.001f )
		{
			angles[ 1 ] = m_flRadToDeg( atan2f( forward[ 1 ], forward[ 0 ] ) );
			angles[ 0 ] = m_flRadToDeg( atan2f( -forward[ 2 ], xyDist ) );
			angles[ 2 ] = m_flRadToDeg( atan2f( left[ 2 ], up[ 2 ] ) );
		}
		else
		{
			angles[ 1 ] = m_flRadToDeg( atan2f( -left[ 0 ], left[ 1 ] ) );
			angles[ 0 ] = m_flRadToDeg( atan2f( -forward[ 2 ], xyDist ) );
			angles[ 2 ] = 0;
		}
	}

	static void MatrixAngles( const matrix3x4_t& matrix, Vec3D &angles, Vec3D &position )
	{
		MatrixGetColumn( matrix, 3, position );
		MatrixAngles( matrix, angles );
	}

	inline float GetYawDelta( float yaw1, float yaw2 )
	{
		return fabs( m_flNormalizeYaw( yaw1 - yaw2 ) );
	}

	template <class T>
	constexpr const T& Min(const T& x, const T& y)
	{
		return (x > y) ? y : x;
	}

	template <class T>
	constexpr const T& Max(const T& x, const T& y)
	{
		return (x < y) ? y : x;
	}
}