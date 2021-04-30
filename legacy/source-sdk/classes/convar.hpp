#pragma once

class ICvar;
class ConCommandBase;
class CCommand;
struct characterset_t;
typedef int CVarDLLIdentifier_t;

#define FORCEINLINE_CVAR FORCEINLINE

//-----------------------------------------------------------------------------
// Any executable that wants to use ConVars need to implement one of
// these to hook up access to console variables.
//-----------------------------------------------------------------------------
class IConCommandBaseAccessor; 

//-----------------------------------------------------------------------------
// Called when a ConCommand needs to execute
//-----------------------------------------------------------------------------
typedef void( *FnCommandCallbackVoid_t )( void );
typedef void( *FnCommandCallback_t )( const CCommand &command );

#define COMMAND_COMPLETION_MAXITEMS		64
#define COMMAND_COMPLETION_ITEM_LENGTH	64

//-----------------------------------------------------------------------------
// Returns 0 to COMMAND_COMPLETION_MAXITEMS worth of completion strings
//-----------------------------------------------------------------------------
typedef int( *FnCommandCompletionCallback )( const char *partial, char commands[ COMMAND_COMPLETION_MAXITEMS ][ COMMAND_COMPLETION_ITEM_LENGTH ] );


//-----------------------------------------------------------------------------
// Interface version
//-----------------------------------------------------------------------------
class ICommandCallback;

class ICommandCompletionCallback;
class ConCommandBase
{
	friend class CCvar;
	friend class ConVar;
	friend class ConCommand;
	friend void ConVar_Register( int nCVarFlag, IConCommandBaseAccessor *pAccessor );

	// FIXME: Remove when ConVar changes are done
	friend class CDefaultCvar;

public:
	ConCommandBase( void );
	ConCommandBase( const char *pName, const char *pHelpString = 0, int flags = 0 );

	virtual                     ~ConCommandBase( void );
	virtual    bool             IsCommand( void ) const;
	virtual bool                IsFlagSet( int flag ) const;
	virtual void                AddFlags( int flags );
	virtual void                RemoveFlags( int flags );
	virtual int                 GetFlags( ) const;
	virtual const char*         GetName( void ) const;
	virtual const char*         GetHelpText( void ) const;
	const ConCommandBase*       GetNext( void ) const;
	ConCommandBase*             GetNext( void );
	virtual bool                IsRegistered( void ) const;
	virtual CVarDLLIdentifier_t GetDLLIdentifier( ) const;

	//protected:
	virtual void                Create( const char *pName, const char *pHelpString = 0, int flags = 0 );
	virtual void                Init( );
	void                        Shutdown( );
	char*                       CopyString( const char *from );

	//private:
	// Next ConVar in chain
	// Prior to register, it points to the next convar in the DLL.
	// Once registered, though, m_pNext is reset to point to the next
	// convar in the global list
	ConCommandBase*             m_pNext;
	bool                        m_bRegistered;
	const char*                 m_pszName;
	const char*                 m_pszHelpString;
	int                         m_nFlags;

protected:
	// ConVars add themselves to this list for the executable. 
	// Then ConVar_Register runs through  all the console variables 
	// and registers them into a global list stored in vstdlib.dll
	static ConCommandBase* s_pConCommandBases;

	// ConVars in this executable use this 'global' to access values.
	static IConCommandBaseAccessor* s_pAccessor;
};

//-----------------------------------------------------------------------------
// Command tokenizer
//-----------------------------------------------------------------------------
class CCommand;
class ConCommand;

//-----------------------------------------------------------------------------
// Purpose: A console variable
//-----------------------------------------------------------------------------
struct CCvarValue {
	char* m_cString;
	int m_iLength;
	float m_flFloat;
	int m_iInt;
};

//-----------------------------------------------------------------------------
// Purpose: A console variable
//-----------------------------------------------------------------------------
class ConVar
{
public:

	VF( ReadString, const char*, 11 );

	VF( ReadFloat, float, 12 );

	VF( ReadInt, int, 13 );

	VF( SetString, void, 14, const char* value );

	VF( SetFloat, void, 15, float value );

	VF( SetInt, void, 16, int value );

	char pad_0x0000[ 0x4 ]; //0x0000
	ConVar* pNext; //0x0004 
	__int32 bRegistered; //0x0008 
	char* pszName; //0x000C 
	char* pszHelpString; //0x0010 
	__int32 nFlags; //0x0014 
	char pad_0x0018[ 0x4 ]; //0x0018
	ConVar* pParent; //0x001C 
	char* pszDefaultValue; //0x0020 
	char* strString; //0x0024 
	__int32 StringLength; //0x0028 
	float fValue; //0x002C 
	__int32 nValue; //0x0030 
	__int32 bHasMin; //0x0034 
	float fMinVal; //0x0038 
	__int32 bHasMax; //0x003C 
	float fMaxVal; //0x0040 
	void *fnChangeCallback; //0x0044 
};