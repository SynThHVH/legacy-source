#pragma once

typedef unsigned char ubyte_t;
typedef ubyte_t *pubyte_t;
typedef unsigned short uword_t;
typedef uword_t *puword_t;

namespace Memory {
	static uintptr_t Signature( uintptr_t dwAddress, uintptr_t dwLength, const char* csPattern ) {
		if ( !dwAddress || !dwLength || !csPattern )
		{
			return 0;
		}

		auto* pPattern = csPattern;
		uintptr_t dwMatchData = 0;
		uintptr_t dwReturnData = 0;

		for ( auto dwData{ dwAddress }; dwData < dwLength; ++dwData )
		{
			if ( !*pPattern )
			{
				dwReturnData = dwMatchData;
			}

			if ( *pubyte_t( pPattern ) == '\?' || *reinterpret_cast< ubyte_t * >( dwData ) == ( ( pPattern[ 0 ] >= '0' && pPattern[ 0 ] <= '9' ? pPattern[ 0 ] - '0' : ( pPattern[ 0 ] & ~32 ) - 'A' + 10 ) << 4 | ( pPattern[ 1 ] >= '0' && pPattern[ 1 ] <= '9' ? pPattern[ 1 ] - '0' : ( pPattern[ 1 ] & ~32 ) - 'A' + 10 ) ) )
			{
				if ( !dwMatchData )
				{
					dwMatchData = dwData;
				}

				if ( !pPattern[ 2 ] ) 
				{
					dwReturnData = dwMatchData;
				}

				pPattern += *puword_t( pPattern ) == 0x3F3F || *pubyte_t( pPattern ) != '\?' ? 0x3 : 0x2;
			}
			else 
			{
				pPattern = csPattern;
				dwMatchData = 0;
			}
		}

		return ( dwReturnData );
	}

	template < typename T >
	static T Signature( void* pModule, const char* csPattern ) 
	{
		auto dwBase = reinterpret_cast< uintptr_t >( pModule );
		auto dataHeader = reinterpret_cast< PIMAGE_NT_HEADERS >( dwBase + reinterpret_cast< PIMAGE_DOS_HEADER >( dwBase )->e_lfanew )->OptionalHeader;

		return ( T )( Signature( dwBase + dataHeader.BaseOfCode, dwBase + dataHeader.SizeOfCode, csPattern ) );
	}
}