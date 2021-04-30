#pragma once

enum controls
{
	none,
	sub_tab,
	group_box,
	check_box,
	combo_box,
	multi_box,
	slider,
	color_box,
	keybind
};

enum side
{
	left,
	right,
	wide
};

enum slider_mode
{
	INTE,
	FLT
};

struct ColorV2
{
	float red, green, blue, alpha;
};

struct PopUp_t
{
	std::string sMessage = "k";
	Color cColor;
	float flLength = 0.f;
};

struct Config_t // DO NOT PUT CONFIG SHIT IN ORDER OR U WILL FUCK THE MEMORY SIZE JUST PUT IT UNDER THE NEXT
{
	int BuildVer = 8;
	bool ragebot = false; 
	//bool multitest[ 15 ] = {false,false,false,false, false, false, false, false, false, false, false, false, false, false, false};
	bool autostop = false;
	int combotest = 0;
	double hitchance = 0.0;
	double seconddmg = 0.0;
	bool hitboxes[ 9 ] = { false,false,false,false,false, false, false, false, false };
	bool multipoint = false;
	bool smartex = false;
	double headpointscale = 0.0;
	double bodypointscale = 0.0;
	bool backhitboxes[ 4 ] = { false,false,false,false };
	bool ignorelimbs = false;
	bool baim[ 3 ] = { false,false,false };
	bool delayshot[ 2 ] = { false,false };
	bool onshotbackunused = false;
	bool legitback = false;
	bool antiaim = false;
	bool fakelag = false;
	double fakelagamnt = 0.0;
	bool onpeek = false;
	bool bhop = false;
	bool strafer = false;
	int tpkey = -1;
	double tpdistance = 0.0;
	bool visuals = false;
	bool box = false;
	ColorV2 boxcolor = { 255.f,255.f,255.f,255.f };
	bool novisrecoil;
	double fov;
	double viewmodelfov;
	bool chams = false;
	ColorV2 chamscolor = { 255.f,255.f,255.f,255.f };
	bool name = false;
	bool weaponname = false;
	bool resolvermode = false;
	bool healthbar = false;
	double primarydmg = 0.0;
	double rslvrfix = 0.0;
	int slwwlk = -1;
	double weaponvlm = 1.0;
	bool desync = false;
	double desyncoffset = 0.0;
	int desyncmove = 0;
	bool shotcham = false;
	ColorV2 shotchamscolor = { 0.f,255.f,0.f,255.f };
	bool bullettrails = false;
	int onshotmode = 0;
	bool doubletap = false;
	bool backtracklby = false;
	int pitch = 0;
	int realyaw = 0;
	int fakeyaw = 0;
	int aamode = 0;
	bool attargets = false;
	int movingyaw = 0;
	int airyaw = 0;
};

namespace Menu
{
	static struct
	{
		POINT m_uPrevOffset;
		int m_iPrevControl;

		bool m_bOpen;
		std::string m_sName;
		int m_iItem;
		bool* m_bItems;
		std::vector<std::string> m_uItems;
		int m_iMax = 0;

	}
	Info;

	extern Config_t Config;
	extern std::deque<PopUp_t> PopUp;

	static void LoadConfig( ) // kmeth  (ily toast)
	{
		CreateDirectory( "configs" , NULL );
		std::string ConfigName;
		ConfigName =  "configs/" ;
		ConfigName += "config";
		ConfigName += std::to_string( 1 ).c_str( );
		ConfigName += ".de" ;

		std::ifstream f( ConfigName.c_str( ) );
		if ( !f.good( ) ) return;

		FILE* fin = fopen( ConfigName.c_str( ), "r" );
		fread( &Config, sizeof( Config_t ), 1, fin );
		fclose( fin );
	}

	static void SaveConfig( ) // kmeth
	{
		CreateDirectory(  "configs" , NULL );
		std::string ConfigName;
		ConfigName = "configs/";
		ConfigName += "config";
		ConfigName += std::to_string( 1 ).c_str( );
		ConfigName += ".de";
		FILE* fout = fopen( ConfigName.c_str( ), "w");
		fwrite( &Config, sizeof( Config_t ), 1, fout );
		fclose( fout );
	}

	extern std::string m_sMenuName;
	extern POINT m_uPos;
	extern POINT m_uSize;
	static Color m_uCtrColor = Color( 59, 107, 23, 255 );

	static int m_iFocusIndex;
	static int m_iMainControl;
	static POINT m_uOffset;

	static bool m_bClick;

	static bool m_bDisableBound;
	static bool m_bCloseBoxes;
	static bool m_bSubTabReturn;
	static bool m_bControlsReturn;
	static int m_iLastControl;

	//selects
	static int m_iSelectedTab = 0;
	static int m_iSelectedSubTab = 0;

	//sizes
	static int m_iTabSize = 0;
	static int m_iSubTabSize = 0;
	static int m_iSubTabLastY = 0;
	static int m_iSubOffsetY = 0;

	//count
	static int m_iTabCount = 0;
	static int m_iSubTabCount = 0;

	static POINT m_uMousePos( )
	{
		static POINT uPoint;
		GetCursorPos( &uPoint );
		ScreenToClient( FindWindowA( 0, "Counter-Strike: Global Offensive" ), &uPoint );
		return uPoint;
	}

	static std::vector< std::pair< Vec2D, std::string > > m_uTopText;

	bool m_bMouseInRegion( int x, int y, int x2, int y2 );

	void Render( );

	void ResetFrame( );
	void EndFrame( );

	void Frame( std::string sName );

	void Tab( std::string sName );
	void SubTab( std::string sName );

	void CheckBox( std::string sName, bool& bItem );
	void Slider( std::string sName, double dMin, double dMax, double& dItem );
	void ComboBox( std::string sName, std::vector< std::string > uItemNames, int& iItem );
	void MultiBox( std::string sName, std::vector< std::string > uItemNames, bool* bItem );
	void ColorPicker( std::string sName, ColorV2& cItem );
	void KeyBind( std::string sName, int& iItem );

	void Focus( );
	void PopUpHandle( );
}