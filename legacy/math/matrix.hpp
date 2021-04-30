#pragma once

// Modifying notation for SourceSDK consistancy
class matrix3x4_t 
{
public:
	float* operator[]( int iIndex ) 
	{
		return m[ iIndex ];
	}
	const float* operator[]( int iIndex ) const 
	{
		return m[ iIndex ];
	}

	inline void SetOrigin( Vec3D const & p )
	{
		m[ 0 ][ 3 ] = p.x;
		m[ 1 ][ 3 ] = p.y;
		m[ 2 ][ 3 ] = p.z;
	}

	inline Vec3D GetOrigin( )
	{
		return Vec3D( m[ 0 ][ 3 ], m[ 1 ][ 3 ], m[ 2 ][ 3 ] );
	}


	float m[ 3 ][ 4 ];
};

class matrix4x4_t 
{
public:
	float* operator[]( int iIndex )
	{
		return m[ iIndex ];
	}
	const float* operator[]( int iIndex ) const 
	{
		return m[ iIndex ];
	}

	float m[ 4 ][ 4 ];
};

class VMatrix
{
public:

	VMatrix( );
	VMatrix(
		vec_t m00, vec_t m01, vec_t m02, vec_t m03,
		vec_t m10, vec_t m11, vec_t m12, vec_t m13,
		vec_t m20, vec_t m21, vec_t m22, vec_t m23,
		vec_t m30, vec_t m31, vec_t m32, vec_t m33
	);

	// Creates a matrix where the X axis = forward
	// the Y axis = left, and the Z axis = up
	VMatrix( const Vec3D& forward, const Vec3D& left, const Vec3D& up );

	// Construct from a 3x4 matrix
	VMatrix( const matrix3x4_t& matrix3x4 );

	// Set the values in the matrix.
	void		Init(
		vec_t m00, vec_t m01, vec_t m02, vec_t m03,
		vec_t m10, vec_t m11, vec_t m12, vec_t m13,
		vec_t m20, vec_t m21, vec_t m22, vec_t m23,
		vec_t m30, vec_t m31, vec_t m32, vec_t m33
	);


	// Initialize from a 3x4
	void		Init( const matrix3x4_t& matrix3x4 );

	// array access
	inline float* operator[]( int i )
	{
		return m[ i ];
	}

	inline const float* operator[]( int i ) const
	{
		return m[ i ];
	}

	// Get a pointer to m[0][0]
	inline float *Base( )
	{
		return &m[ 0 ][ 0 ];
	}

	inline const float *Base( ) const
	{
		return &m[ 0 ][ 0 ];
	}

	void		SetLeft( const Vec3D &vLeft );
	void		SetUp( const Vec3D &vUp );
	void		SetForward( const Vec3D &vForward );

	void		GetBasisVec3Ds( Vec3D &vForward, Vec3D &vLeft, Vec3D &vUp ) const;
	void		SetBasisVec3Ds( const Vec3D &vForward, const Vec3D &vLeft, const Vec3D &vUp );

	// Get/set the translation.
	Vec3D &	GetTranslation( Vec3D &vTrans ) const;
	void		SetTranslation( const Vec3D &vTrans );

	void		PreTranslate( const Vec3D &vTrans );
	void		PostTranslate( const Vec3D &vTrans );

	matrix3x4_t& As3x4( );
	const matrix3x4_t& As3x4( ) const;
	void		CopyFrom3x4( const matrix3x4_t &m3x4 );
	void		Set3x4( matrix3x4_t& matrix3x4 ) const;

	bool		operator==( const VMatrix& src ) const;
	bool		operator!=( const VMatrix& src ) const { return !( *this == src ); }

	// Access the basis Vec3Ds.
	Vec3D		GetLeft( ) const;
	Vec3D		GetUp( ) const;
	Vec3D		GetForward( ) const;
	Vec3D		GetTranslation( ) const;


	// Matrix->Vec3D operations.
public:
	// Multiply by a 3D Vec3D (same as operator*).
	void		V3Mul( const Vec3D &vIn, Vec3D &vOut ) const;

	// Multiply by a 4D Vec3D.
	//void		V4Mul( const Vec3D4D &vIn, Vec3D4D &vOut ) const;

	// Applies the rotation (ignores translation in the matrix). (This just calls VMul3x3).
	Vec3D		ApplyRotation( const Vec3D &vVec ) const;

	// Multiply by a Vec3D (divides by w, assumes input w is 1).
	Vec3D		operator*( const Vec3D &vVec ) const;

	// Multiply by the upper 3x3 part of the matrix (ie: only apply rotation).
	Vec3D		VMul3x3( const Vec3D &vVec ) const;

	// Apply the inverse (transposed) rotation (only works on pure rotation matrix)
	Vec3D		VMul3x3Transpose( const Vec3D &vVec ) const;

	// Multiply by the upper 3 rows.
	Vec3D		VMul4x3( const Vec3D &vVec ) const;

	// Apply the inverse (transposed) transformation (only works on pure rotation/translation)
	Vec3D		VMul4x3Transpose( const Vec3D &vVec ) const;


	// Matrix->plane operations.
	//public:
	// Transform the plane. The matrix can only contain translation and rotation.
	//void		TransformPlane( const VPlane &inPlane, VPlane &outPlane ) const;

	// Just calls TransformPlane and returns the result.
	//VPlane		operator*(const VPlane &thePlane) const;

	// Matrix->matrix operations.
public:

	VMatrix&	operator=( const VMatrix &mOther );

	// Multiply two matrices (out = this * vm).
	void		MatrixMul( const VMatrix &vm, VMatrix &out ) const;

	// Add two matrices.
	const VMatrix& operator+=( const VMatrix &other );

	// Just calls MatrixMul and returns the result.	
	VMatrix		operator*( const VMatrix &mOther ) const;

	// Add/Subtract two matrices.
	VMatrix		operator+( const VMatrix &other ) const;
	VMatrix		operator-( const VMatrix &other ) const;

	// Negation.
	VMatrix		operator-( ) const;

	// Return inverse matrix. Be careful because the results are undefined 
	// if the matrix doesn't have an inverse (ie: InverseGeneral returns false).
	VMatrix		operator~( ) const;

	// Matrix operations.
public:
	// Set to identity.
	void		Identity( );

	bool		IsIdentity( ) const;

	// Setup a matrix for origin and angles.
	void		SetupMatrixOrgAngles( const Vec3D &origin, const Vec3D &vAngles );

	// General inverse. This may fail so check the return!
	bool		InverseGeneral( VMatrix &vInverse ) const;

	// Does a fast inverse, assuming the matrix only contains translation and rotation.
	void		InverseTR( VMatrix &mRet ) const;

	// Usually used for debug checks. Returns true if the upper 3x3 contains
	// unit Vec3Ds and they are all orthogonal.
	bool		IsRotationMatrix( ) const;

	// This calls the other InverseTR and returns the result.
	VMatrix		InverseTR( ) const;

	// Get the scale of the matrix's basis Vec3Ds.
	Vec3D		GetScale( ) const;

	// (Fast) multiply by a scaling matrix setup from vScale.
	VMatrix		Scale( const Vec3D &vScale );

	// Normalize the basis Vec3Ds.
	VMatrix		NormalizeBasisVec3Ds( ) const;

	// Transpose.
	VMatrix		Transpose( ) const;

	// Transpose upper-left 3x3.
	VMatrix		Transpose3x3( ) const;

public:
	// The matrix.
	vec_t		m[ 4 ][ 4 ];
};