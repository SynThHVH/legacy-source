#include "../features.hpp"

Config_t Menu::Config;
std::deque<PopUp_t> Menu::PopUp;

/*
note:
	i have plans to get rid of this menu and add a new one because this one is dogshit and completely breaks randomly if i edit it
	but first i want to make sure the cheat works good, and it looks good because this menu works and it doesn't need to be changed
	if anything ill add the actual styles menu because i want to make this cheat like styles but better because that cheat was the best
	welp, that never happened either, i had a lot of plans for this cheat but some shit doesn't happen
*/

namespace Menu
{
	std::string m_sMenuName = "stylesche.at legacy";

	POINT m_uPos = { 100, 100 };
	POINT m_uSize = { 400 , 340 };
}

bool Menu::m_bMouseInRegion( int x, int y, int x2, int y2 ) 
{
	return ( !m_bDisableBound && m_uMousePos( ).x >= x && m_uMousePos( ).y >= y && m_uMousePos( ).x <= x2 + x && m_uMousePos( ).y <= y2 + y );
}

void Menu::Render( )
{
	static bool LoadSave = true;
	if ( GetKeyState( VK_INSERT ) )
	{
		if ( !LoadSave )
		{
			SaveConfig( );
			LoadSave = true;
		}
		return;
	}

	if ( LoadSave )
	{
		int NewBuild = Config.BuildVer;
		LoadConfig( );
		if ( Config.BuildVer != NewBuild )
		{
			for ( int i = 0; i < 20; i++ )
				PopUp.push_back( { "build version is different make sure config is correct",  Color( 255.f,0.f,0.f,255.f ), Interfaces::GlobalVars->realtime + 30.f } );
			Config.BuildVer = NewBuild;
		}
		LoadSave = false;
	}

	m_bClick = GetAsyncKeyState( VK_LBUTTON );

	ResetFrame( );

	Frame( m_sMenuName );
	{

		Tab( "rage" );
		{
			SubTab( "aimbot" );
			{
				CheckBox( "enabled", Config.ragebot );
				CheckBox( "autostop", Config.autostop );
				Slider( "hitchance", 0.0, 100.0, Config.hitchance );
				Slider( "penetration damage", 0.0, 125.0, Config.primarydmg );
				Slider( "visable damage", 0.0, 125.0, Config.seconddmg );
			}
			SubTab( "selection" );
			{
				MultiBox( "hitboxes", { "head", "pelvis", "stomach", "lower chest", "upper chest", "left thigh", "right thigh", "left foot", "right foot" }, Config.hitboxes );
				CheckBox( "multipoint", Config.multipoint );
				if ( Config.multipoint )
				{
					CheckBox( "dynamic", Config.smartex );
					Slider( "head scale", 0.0, 0.95, Config.headpointscale );
					Slider( "body scale", 0.0, 0.95, Config.bodypointscale );
				}
				MultiBox( "backtrack hitboxes", { "head", "pelvis", "lower chest", "upper chest" }, Config.backhitboxes );
				ComboBox( "onshot", { "do nothing", "backtrack", "baim", "remove record" }, Config.onshotmode );
				CheckBox( "ignore moving limbs", Config.ignorelimbs );
				MultiBox( "baim", { "in air", "lethal" }, Config.baim );
				CheckBox("backtrack lby", Config.backtracklby);
			}
		}

		/* note: premium shit right here ma nigga */
		Tab( "anti aim" );
		{
			SubTab( "angles" );
			{
				CheckBox( "enabled", Config.antiaim );
				CheckBox( "at targets", Config.attargets );
				ComboBox( "pitch", { "disabled", "down", "up", "random" }, Config.pitch );
				ComboBox( "standing yaw", { "disabled", "180", "lower body", "spin" }, Config.realyaw );
				ComboBox( "moving yaw", { "disabled", "180", "lower body", "spin" }, Config.movingyaw );
				ComboBox( "jumping yaw", { "disabled", "180", "lower body", "spin" }, Config.airyaw );
				ComboBox( "fake yaw", { "disabled", "180", "lower body", "lby inverse", "spin" }, Config.fakeyaw );
			}

			SubTab("fakelag");
			{
				CheckBox("enabled", Config.fakelag);
				Slider("choked ticks", 0.0, 14.0, Config.fakelagamnt);
				CheckBox("on peek", Config.onpeek);
			}
		}

		Tab( "visuals" );
		{
			SubTab( "esp" );
			{
				CheckBox( "enable", Config.visuals );
				CheckBox( "box", Config.box );
				CheckBox( "name", Config.name );
				CheckBox( "health bar", Config.healthbar );
				CheckBox( "weapon", Config.weaponname );
				CheckBox( "resolver info", Config.resolvermode );
			}

			SubTab( "chams" );
			{
				CheckBox( "chams", Config.chams );
				ColorPicker( "chamscolor", Config.chamscolor );
				CheckBox( "shot chams", Config.shotcham );
				ColorPicker( "shotchamscolor", Config.shotchamscolor );
			}

			SubTab("world");
			{
				CheckBox("bullet trails", Config.bullettrails);
			}

			SubTab("extras");
			{
				CheckBox("novisrecoil", Config.novisrecoil);
				KeyBind("thirdperson", Config.tpkey);
				Slider("tp distance", 60.0, 200.0, Config.tpdistance);
				Slider("fov", 0.0, 150.0, Config.fov);
				Slider("viewmodelfov", 0.0, 150.0, Config.viewmodelfov);
			}
		}
		
		Tab( "misc" );
		{
			CheckBox( "bhop", Config.bhop );
			CheckBox( "autostrafer", Config.strafer );
			KeyBind( "slow walk", Config.slwwlk );
			Slider( "weapon volume", 0.0, 1.0, Config.weaponvlm );
		}
	}

	Focus( );

	EndFrame( );
}

void Menu::ResetFrame( )
{
//	cInfo.prevOffset = POINT{0,0};
//	cInfo.prevControl = none;
	m_iFocusIndex = 0;
	m_uOffset.x = m_uPos.x;
	m_uOffset.y = m_uPos.y;

	m_bDisableBound = false;
	m_bCloseBoxes = false;
	m_bSubTabReturn = false;
	m_bControlsReturn = false;

	m_iTabCount = 0;
	m_iSubTabCount = 0;
}

void Menu::EndFrame( )
{
	m_iTabSize = m_iTabCount;
	m_iSubTabSize = m_iSubTabCount;
	m_iSubTabLastY = m_iSubOffsetY;
}

void Menu::Frame( std::string sName ) 
{
	static bool bGrab = false;
	static POINT uLastMousePos, uLastMenuPos;

	if ( m_bClick )
	{
		if ( !bGrab && m_bMouseInRegion( m_uPos.x, m_uPos.y, m_uSize.x, m_uSize.y ) && !m_bMouseInRegion( m_uPos.x + 9, m_uPos.y + 19, m_uSize.x - 18, m_uSize.y - 28 ) )
		{
			bGrab = true;
			uLastMousePos = m_uMousePos( );
			uLastMenuPos = m_uPos;
		}

		if ( bGrab )
		{
			m_uPos.x = uLastMenuPos.x + ( m_uMousePos( ).x - uLastMousePos.x);
			m_uPos.y = uLastMenuPos.y + ( m_uMousePos( ).y - uLastMousePos.y);
		}
	}
	else
	{
		bGrab = false;
	}

	static int iScreenHeight, iScreenWidth;
	Interfaces::Engine->GetScreenSize( iScreenWidth, iScreenHeight );

	if ( m_uPos.x < 0 )
		m_uPos.x = 0;
	if ( m_uPos.y < 0 )
		m_uPos.y = 0;
	if ( ( m_uPos.x + m_uSize.x ) > iScreenWidth )
		m_uPos.x = iScreenWidth - m_uSize.x;
	if ( ( m_uPos.y + m_uSize.y ) > iScreenHeight )
		m_uPos.y = iScreenHeight - m_uSize.y;

	Draw::Rect( m_uPos.x, m_uPos.y, m_uSize.x, m_uSize.y, Color( 33, 33, 33, 255 ) );
	Draw::OutlinedRect( m_uPos.x, m_uPos.y, m_uSize.x, m_uSize.y, Color( 0, 0, 0, 255 ) );
	Draw::Line( m_uPos.x, m_uPos.y + 20, m_uPos.x + m_uSize.x, m_uPos.y + 20, Color( 0, 0, 0, 255 ) );
	Draw::Line( m_uPos.x + 79, m_uPos.y + 20, m_uPos.x + 79, m_uPos.y + ( m_uSize.y - 20 ), Color( 0, 0, 0, 255 ) );
	Draw::Line( m_uPos.x, m_uPos.y + ( m_uSize.y - 20 ), m_uPos.x + m_uSize.x, m_uPos.y + ( m_uSize.y - 20 ), Color( 0, 0, 0, 255 ) );

	Draw::String( m_uPos.x + 6, m_uPos.y + 2, Draw::Font.Default, Color( 255, 255, 255, 255 ), sName, false );
	Draw::String( m_uPos.x + 6, m_uPos.y + ( m_uSize.y - 18 ), Draw::Font.Default, Color( 255, 255, 255, 255 ), "alpha build | last update: 4/29/2021", false );
}

void Menu::Tab( std::string sName )
{
	if ( m_iTabCount == 0 )
		Draw::Rect( m_uPos.x, m_uPos.y + 20, 70, ( m_iTabSize * 25 ), Color( 119, 119, 119, 255 ) );

	if ( m_iTabSize != 0 )
	{
		RECT TabArea = { m_uPos.x, m_uPos.y + 20 + ( m_iTabCount * 25 ), 80, 26 };

		bool MouseInBound = m_bMouseInRegion( TabArea.left, TabArea.top, TabArea.right, TabArea.bottom - 2 );

		static bool Done = false;

		if ( m_bClick && MouseInBound )
		{
			m_iSelectedTab = m_iTabCount;
			m_bCloseBoxes = true;
			if ( !Done )
			{
				m_bSubTabReturn = true;
				m_bControlsReturn = true;
				Done = true;
			}
		}
		else if ( MouseInBound )
			Done = false;

		if ( m_iSelectedTab == m_iTabCount )
		{
			Draw::Rect( TabArea.left + 70, TabArea.top, 10, TabArea.bottom, Color( 81, 115, 244, 255 ) );
			Draw::Rect( TabArea.left, TabArea.top, 70, TabArea.bottom, Color( 53, 53, 53, 255 ) );
		}
		else
			Draw::Rect( TabArea.left, TabArea.top, 70, TabArea.bottom, Color( 119, 119, 119, 255 ) );

		Draw::OutlinedRect( TabArea.left, TabArea.top, TabArea.right, TabArea.bottom, Color( 0, 0, 0, 255 ) );
		Draw::Line( TabArea.left + 70, TabArea.top, TabArea.left + 70, TabArea.top + TabArea.bottom, Color( 0, 0, 0, 255 ) );

		Draw::String( TabArea.left + 6, TabArea.top + 2, Draw::Font.Default, Color( 255, 255, 255, 255 ), sName, false );
	}

	m_iMainControl = none;
//	cInfo.prevControl = none;

	m_uOffset.x = m_uPos.x + 85;
	m_uOffset.y = m_uPos.y + 25;

	m_iTabCount++;
}

void Menu::SubTab( std::string sName )
{
	if ( m_bSubTabReturn )
		return;

	if ( m_iTabCount - 1 != m_iSelectedTab )
	{
		return;
	}

	RECT SubTabArea = { m_uOffset.x, m_uPos.y + 25, m_uSize.x - 91, m_uSize.y - 50 };

	if ( m_iSubTabCount == 0 )
	{
		m_uOffset.y += 27;
	}

	static int iFrameSkip = 0;

	if ( m_iSubTabSize != 0 && iFrameSkip == m_iSelectedTab )
	{
		if ( m_iSelectedSubTab > m_iSubTabSize )
			m_iSelectedSubTab = 0;

		int TabLength = ( SubTabArea.right / float( m_iSubTabSize ));

		RECT InTab = { ( SubTabArea.left ) + ( TabLength * float( m_iSubTabCount ) ), SubTabArea.top, TabLength, 21 };

		bool MouseInBound = m_bMouseInRegion( InTab.left + 1 , InTab.top, InTab.right - 2, InTab.bottom );

		static bool Done = false;

		if ( m_bClick && MouseInBound )
		{
			m_iSelectedSubTab = m_iSubTabCount;
			m_bCloseBoxes = true;
			if ( !Done )
			{
				m_bControlsReturn = true;
				Done = true;
			}
		}
		else if ( MouseInBound )
			Done = false;

		int Offset = ( ( m_iSubTabSize - 1 ) == m_iSubTabCount ) ? 0 : 1;

		if ( ( ( m_iSubTabSize - 1 ) == m_iSubTabCount ) && ( ( ( TabLength * m_iSubTabSize ) > SubTabArea.right ) || ( ( TabLength * m_iSubTabSize ) < SubTabArea.right ) ) )
			Offset = ( SubTabArea.right - ( TabLength * m_iSubTabSize ) );

		if ( m_iSelectedSubTab == m_iSubTabCount )
			Draw::Rect( InTab.left, InTab.top, InTab.right + Offset, InTab.bottom, Color( 53, 53, 53, 255 ) );
		else
			Draw::Rect( InTab.left, InTab.top, InTab.right + Offset, InTab.bottom, Color( 119, 119, 119, 255 ) );

		Draw::OutlinedRect( InTab.left, InTab.top, InTab.right + Offset, InTab.bottom, Color( 0, 0, 0, 255 ) );
		Draw::String( InTab.left + ( InTab.right / 2 ), InTab.top + ( InTab.bottom / 2 ), Draw::Font.Default, Color( 255, 255, 255, 255 ), sName, true );
	}

	if ( iFrameSkip != m_iSelectedTab )
		iFrameSkip = m_iSelectedTab;

	m_iSubTabCount++;
}

void Menu::CheckBox( std::string sName, bool& bItem )
{
	if ( m_iTabCount - 1 != m_iSelectedTab )
	{
		return;
	}

	if ( m_iSubTabCount != 0 && m_iSelectedSubTab != m_iSubTabCount - 1 )
	{
		return;
	}

	static bool bBuffer = false;
	static int iWidth = 16, iHeight = 16;

	bool MouseInBound = m_bMouseInRegion( m_uOffset.x + 277, m_uOffset.y, iWidth, iHeight );

	Draw::String( m_uOffset.x + 6, m_uOffset.y, Draw::Font.Default, Color( 255, 255, 255, 255 ), sName, false );

	if ( !m_bDisableBound )
	{
		if ( m_bClick && MouseInBound )
		{
			bBuffer ? ( bItem = !bItem, bBuffer = false ) : NULL;
		}
		else if ( MouseInBound )
		{
			bBuffer = true;
		}
	}

	if ( bItem )
		Draw::Rect( m_uOffset.x + 277, m_uOffset.y, iWidth , iHeight , Color( 120, 209, 109, 255 ));
	else
		Draw::Rect( m_uOffset.x + 277, m_uOffset.y, iWidth, iHeight, Color( 192, 97, 108, 255 ) );

	Draw::OutlinedRect( m_uOffset.x + 277, m_uOffset.y, iWidth, iHeight, Color( 0, 0, 0, 255 ) );

	m_uOffset.y += 26;

	m_iLastControl = check_box;
}

void Menu::Slider( std::string sName, double dMin, double dMax, double& dItem )
{
	if ( m_bControlsReturn )
		return;

	if ( m_iTabCount - 1 != m_iSelectedTab )
	{
		return;
	}

	if ( m_iSubTabCount != 0 && m_iSelectedSubTab != m_iSubTabCount - 1 )
	{
		return;
	}

	static int Width = 120, Height = 8;

	bool MouseInBound = m_bMouseInRegion( m_uOffset.x + 153, m_uOffset.y + 5, Width, Height );

	Draw::Rect( m_uOffset.x + 153, m_uOffset.y + 5, Width, Height, Color( 62, 62, 62, 255 ) );
	Draw::OutlinedRect( m_uOffset.x + 153, m_uOffset.y + 5, Width, Height, Color( 0, 0, 0, 255 ) );

	float Size = 0;

	if ( dMax >= 0 && dMin >= 0 )
	{
		Size = dMax - dMin;
	}
	else if ( dMax >= 0 && dMin < 0 )
	{
		Size = dMax + fabs( dMin );
	}
	else if ( dMax < 0 && dMin < 0 )
	{
		Size = fabs( dMin ) - fabs( dMax );
	}

	float Scale = float(Width - 6) / Size;
	float Delta = ( m_uMousePos( ).x - ( m_uOffset.x + 155.f ) );

	if ( !m_bDisableBound )
	{
		if ( MouseInBound )
		{
			if ( m_bClick )
			{
				dItem = dMin + ( Delta / Scale );
			}
		}
	}

	if ( dItem < dMin )
	{
		dItem = dMin;
	}
	else if ( dItem > dMax )
	{
		dItem = dMax;
	}

	if ( dItem == dMax )
	{
		Draw::Rect( m_uOffset.x + Width + 153 - 6, m_uOffset.y + 5, 6, Height, Color( 119, 119, 119, 255 ) );
		Draw::OutlinedRect( m_uOffset.x + Width + 153 - 6, m_uOffset.y + 5, 6, Height, Color( 0, 0, 0, 255 ) );
	}
	else
	{
		Draw::Rect( m_uOffset.x + (( dItem * Scale ) - ( dMin * Scale )) + 153, m_uOffset.y + 5, 6, Height, Color( 119, 119, 119, 255 ) );
		Draw::OutlinedRect( m_uOffset.x + ( ( dItem * Scale ) - ( dMin * Scale ) ) + 153, m_uOffset.y + 5, 6, Height, Color( 0, 0, 0, 255 ) );
	}

	std::string sItem = std::to_string( dItem );

	int RemoveSize = 4;

	if ( fabs( dItem ) >= 100.00 )
		RemoveSize = 7;

	sItem.resize( sItem.length( ) - RemoveSize );

	Draw::String( m_uOffset.x + 6, m_uOffset.y, Draw::Font.Default, Color( 255, 255, 255, 255 ), sName, false);

	Draw::String( m_uOffset.x + 292, m_uOffset.y + 1, Draw::Font.Default, Color( 255, 255, 255, 255 ), sItem, 4 );

	m_uOffset.y += 26;

	m_iLastControl = slider;
}

void Menu::ComboBox( std::string sName, std::vector< std::string > uItemNames, int& iItem )
{
	if ( m_bControlsReturn )
		return;

	if ( m_iTabCount - 1 != m_iSelectedTab )
	{
		return;
	}

	if ( m_iSubTabCount != 0 && m_iSelectedSubTab != m_iSubTabCount - 1 )
	{
		return;
	}

	static bool bBuffer;
	static int iWidth = 140, iHeight = 16;

	bool bMouseInBound = m_bMouseInRegion( m_uOffset.x + 153, m_uOffset.y, iWidth, iHeight );

	Draw::Rect( m_uOffset.x + 153, m_uOffset.y, iWidth, iHeight, Color( 119, 119, 119, 255 ) );
	Draw::OutlinedRect( m_uOffset.x + 153, m_uOffset.y, iWidth, iHeight, Color( 0, 0, 0, 255 ) );

	if ( !m_bDisableBound )
	{
		bool bSkip = false;
		if ( m_bClick && bMouseInBound )
		{
			bBuffer ? ( Info.m_bOpen = !Info.m_bOpen, Info.m_sName = sName, bBuffer = false ) : NULL;
			bSkip = true;
		}
		else if ( bMouseInBound )
		{
			bBuffer = true;
		}

		if ( Info.m_bOpen && Info.m_sName == sName )
		{
			int iClickedInBounds = 0;

			for ( int i = 0; i < uItemNames.size( ); i++ )
			{
				bool bMouseInComBound = m_bMouseInRegion( m_uOffset.x + 153, m_uOffset.y + 16 + (i * 16), iWidth, iHeight );

				if ( m_bClick && bMouseInComBound )
				{
					iItem = i;
					iClickedInBounds++;
				}
			}

			if ( ( m_bClick && iClickedInBounds <= 0 && !bSkip ) || m_bCloseBoxes )
				Info.m_bOpen = false;
		}
		else
		{

		}

		if ( ( bMouseInBound || Info.m_bOpen ) && Info.m_sName == sName )
		{
			Info.m_iPrevControl = combo_box;
		//	cInfo.Name = Name;
			Info.m_iItem = iItem;
			Info.m_uItems.assign( uItemNames.begin( ), uItemNames.end( ) );
			Info.m_uItems = uItemNames;
			Info.m_uPrevOffset = m_uOffset;

			m_bDisableBound = true;
		}
	}

	Draw::String( m_uOffset.x + 159, m_uOffset.y, Draw::Font.Default, Color( 255, 255, 255, 255 ), ( iItem >= 0 ) ? uItemNames.at( iItem ) : "none", false );
	Draw::String( m_uOffset.x + 6, m_uOffset.y, Draw::Font.Default, Color( 255, 255, 255, 255 ), sName, false );

	m_uOffset.y += 26;

	m_iLastControl = combo_box;
}

void Menu::MultiBox( std::string sName, std::vector< std::string > uItemNames, bool* bItem )
{
	if ( m_bControlsReturn )
		return;

	if ( m_iTabCount - 1 != m_iSelectedTab )
	{
		return;
	}

	if ( m_iSubTabCount != 0 && m_iSelectedSubTab != m_iSubTabCount - 1 )
	{
		return;
	}

	static bool bBuffer, bBuffer2 = false;
	static int iWidth = 140, iHeight = 16;

	bool bMouseInBound = m_bMouseInRegion( m_uOffset.x + 153, m_uOffset.y, iWidth, iHeight );

	Draw::Rect( m_uOffset.x + 153, m_uOffset.y, iWidth, iHeight, Color( 119, 119, 119, 255 ) );
	Draw::OutlinedRect( m_uOffset.x + 153, m_uOffset.y, iWidth, iHeight, Color( 0, 0, 0, 255 ) );

	if ( !m_bDisableBound )
	{
		bool bSkip = false;
		if ( m_bClick && bMouseInBound )
		{
			bBuffer ? ( Info.m_bOpen = !Info.m_bOpen, Info.m_sName = sName, bBuffer = false ) : NULL;
			bSkip = true;
		}
		else if ( bMouseInBound )
		{
			bBuffer = true;
		}

		if ( Info.m_bOpen && Info.m_sName == sName )
		{
			int iClickedInBounds = 0;

			for ( int i = 0; i < uItemNames.size( ); i++ )
			{
				bool bMouseInComBound = m_bMouseInRegion( m_uOffset.x + 153, m_uOffset.y + 16 + ( i * 16 ), iWidth, iHeight );

				if ( m_bClick && bMouseInComBound )
				{
					bBuffer2 ? ( bItem[ i ] = !bItem[ i ], bBuffer2 = false ) : NULL;
					iClickedInBounds++;
				}
				else if ( bMouseInComBound )
				{
					bBuffer2 = true;
				}
			}

			if ( (m_bClick && iClickedInBounds <= 0 && !bSkip) || m_bCloseBoxes )
				Info.m_bOpen = false;
		}
		else
		{

		}

		if ( ( bMouseInBound || Info.m_bOpen ) && Info.m_sName == sName )
		{
			Info.m_iPrevControl = multi_box;
			//	cInfo.Name = Name;
			Info.m_bItems = bItem;
			Info.m_uItems.assign( uItemNames.begin( ), uItemNames.end( ) );
			Info.m_uItems = uItemNames;
			Info.m_uPrevOffset = m_uOffset;

			m_bDisableBound = true;
		}
	}

	std::string sItemsSelected = "";

	bool bPastFirst = false;
	int iPicked = -1;

	for ( int i = 0; i < uItemNames.size( ); i++ )
	{
		if ( bItem[ i ] )
		{					
			if ( !bPastFirst )
				sItemsSelected += uItemNames.at( i );
			bPastFirst = true;
			iPicked++;
		}
	}
	
	if ( iPicked == -1 )
		sItemsSelected = "off";
	else if ( iPicked != 0 )
		sItemsSelected += " + " + std::to_string( iPicked );

	Draw::String( m_uOffset.x + 159, m_uOffset.y, Draw::Font.Default, Color( 255, 255, 255, 255 ), sItemsSelected, false );
	Draw::String( m_uOffset.x + 6, m_uOffset.y, Draw::Font.Default, Color( 255, 255, 255, 255 ), sName, false );

	m_uOffset.y += 26;

	m_iLastControl = multi_box;
}

void Menu::ColorPicker( std::string sName, ColorV2& cItem ) // best coder in the universe
{
	if ( m_bControlsReturn )
		return;

	if ( m_iTabCount - 1 != m_iSelectedTab )
	{
		return;
	}

	if ( m_iSubTabCount != 0 && m_iSelectedSubTab != m_iSubTabCount - 1 )
	{
		return;
	}

	int CtrXoffset = 0;

	if ( m_iLastControl != check_box )
		CtrXoffset = 127;
	else
		CtrXoffset = 251;

	int yoffset = (m_uOffset.y - 26) + 10;
	int xoffset = m_uOffset.x + 330;

	int xoffsetoverscreen = 430;

	Color rainbow;

	bool pressed = false;
	bool open = false;
	static bool selectedOpened = false;
	static bool clickRest;
	static bool rest;
	static std::string nameSelected;

	bool bMouseInBound = m_bMouseInRegion( m_uOffset.x + CtrXoffset, m_uOffset.y - 26, 16, 16 );

	if ( m_bClick && bMouseInBound && !clickRest )
	{
		nameSelected = sName;
		pressed = true;
		clickRest = true;
	}
	else if ( !m_bClick && bMouseInBound )
		clickRest = false;

	if ( pressed )
	{
		if ( !rest )
			selectedOpened = !selectedOpened;

		rest = true;
	}
	else
		rest = false;

	if ( nameSelected == sName )
		open = selectedOpened;

	if ( open )
	{
		Draw::Rect( xoffset, yoffset - 10, 100, 20, Color( 0, 0, 0, 255 ) );
		Draw::Rect( xoffset, yoffset + 90, 100, 20, Color( 255, 255, 255, 255 ) );

		if ( m_bClick && m_bMouseInRegion( xoffset, ( m_uOffset.y - 26 ), 100, 10 ) )
		{
			cItem.red = 0;
			cItem.green = 0;
			cItem.blue = 0;
			cItem.alpha = 255;
		}

		if ( m_bClick && m_bMouseInRegion( xoffset, ( m_uOffset.y - 26 ) + 110, 100, 10 ) )
		{
			cItem.red = 255;
			cItem.green = 255;
			cItem.blue = 255;
			cItem.alpha = 255;
		}

		Draw::OutlinedRect( xoffset - 1, ( m_uOffset.y - 26 ) - 1, 102, 122, Color( 0, 0, 0, 255 ) );

		for ( int i = 0; i < 100; i++ )
		{
			if ( xoffset >= m_uOffset.x + xoffsetoverscreen )
			{
				xoffset -= 100;
				yoffset += 10;
			}

			float hue = ( i * .01f );

			rainbow.FromHSV( hue, 1.f, 1.f );
			rainbow.SetColor( rainbow.r(), rainbow.g(), rainbow.b(), 255 );
			Draw::Rect( xoffset, yoffset, 10, 10, rainbow );

			if ( m_bClick && m_bMouseInRegion( xoffset, yoffset, 10, 10 ) )
			{
				cItem.red = rainbow.r();
				cItem.green = rainbow.g();
				cItem.blue = rainbow.b();
				cItem.alpha = 255.f;
			}

			xoffset += 10;
		}
	}

	rainbow.SetColor( cItem.red, cItem.green, cItem.blue, 255 );

	Draw::Rect( m_uOffset.x + CtrXoffset, ( m_uOffset.y - 26 ), 16, 16, rainbow );
	Draw::OutlinedRect( m_uOffset.x + CtrXoffset, ( m_uOffset.y - 26 ), 16, 16, Color( 0, 0, 0, 255 ) );
}

void Menu::KeyBind( std::string sName, int& iItem )
{
	if ( m_bControlsReturn )
		return;

	if ( m_iTabCount - 1 != m_iSelectedTab )
	{
		return;
	}

	if ( m_iSubTabCount != 0 && m_iSelectedSubTab != m_iSubTabCount - 1 )
	{
		return;
	}

	static bool gotKey = false;
	bool pressed = false;
	if ( gotKey )
		pressed = true;
	gotKey = false;
	bool open = false;
	static bool selectedOpened = false;
	static bool clickRest;
	static bool rest;
	static std::string nameSelected;

	static int iWidth = 140, iHeight = 16;
	bool bMouseInBound = m_bMouseInRegion( m_uOffset.x + 153, m_uOffset.y, iWidth, iHeight );

	if ( m_bClick && bMouseInBound && !clickRest )
	{
		nameSelected = sName;
		pressed = true;
		clickRest = true;
	}
	else if ( !m_bClick && bMouseInBound )
		clickRest = false;

	if ( pressed )
	{
		if ( !rest )
			selectedOpened = !selectedOpened;

		rest = true;
	}
	else
		rest = false;

	if ( nameSelected == sName )
		open = selectedOpened;

	Draw::String( m_uOffset.x + 6, m_uOffset.y, Draw::Font.Default, Color( 255, 255, 255, 255 ), sName, false );

	Draw::Rect( m_uOffset.x + 153, m_uOffset.y, iWidth, iHeight, Color( 119, 119, 119, 255 ) );

	if ( open )
	{
		for ( int i = 0; i < 255; i++ )
		{
			if ( ( GetAsyncKeyState( i ) & 0x8000 ) && i != 1 && i != 45 )
			{
				iItem = i;
				gotKey = true;
			}
		}
		Draw::Rect( m_uOffset.x + 153, m_uOffset.y, iWidth, iHeight, Color( 53, 53, 53, 255 ) );
	}

	Draw::String( m_uOffset.x + 6, m_uOffset.y, Draw::Font.Default, Color( 255, 255, 255, 255 ), sName, false );
	Draw::String( m_uOffset.x + 159, m_uOffset.y, Draw::Font.Default, Color( 255, 255, 255, 255 ),  std::string( 1, char( iItem ) ), false ); // K LOL

	Draw::OutlinedRect( m_uOffset.x + 153, m_uOffset.y, iWidth, iHeight, Color( 0, 0, 0, 255 ) );

	m_uOffset.y += 26;

	m_iLastControl = keybind;
}

void Menu::Focus( )
{
	if ( Info.m_iPrevControl == combo_box || Info.m_iPrevControl == multi_box )
	{
		if ( Info.m_uItems.size( ) > 0 )
		{
			if ( Info.m_bOpen )
			{
				if ( m_bCloseBoxes )
				{
					Info.m_bOpen = false;
				}

				Draw::Rect( Info.m_uPrevOffset.x + 153, Info.m_uPrevOffset.y + 16 , 140, ( Info.m_uItems.size( ) * 16 ), Color( 119, 119, 119, 255 ) );

				for ( int i = 0; i < Info.m_uItems.size( ); i++ )
				{
					std::string wtf = Info.m_uItems.at( i );
					RECT ComboArea = { Info.m_uPrevOffset.x + 153, Info.m_uPrevOffset.y + 16 + ( i * 16 ), 140, 16 };

					if ( Info.m_iPrevControl == combo_box && Info.m_iItem == i )
						Draw::Rect( ComboArea.left + 1, ComboArea.top, ComboArea.right - 2, ComboArea.bottom, Color( 53, 53, 53, 255 ) );
					else if ( Info.m_iPrevControl == multi_box && Info.m_bItems[i] == 1)
						Draw::Rect( ComboArea.left + 1, ComboArea.top, ComboArea.right - 2, ComboArea.bottom, Color( 120, 209, 109, 255 ) );
					else if ( Info.m_iPrevControl == multi_box )
						Draw::Rect( ComboArea.left + 1, ComboArea.top, ComboArea.right - 2, ComboArea.bottom, Color( 192, 97, 108, 255 ) );

					Draw::String( ComboArea.left + 6, ComboArea.top, Draw::Font.Default, Color( 255, 255, 255, 255 ), wtf, false );
				}

				Draw::OutlinedRect( Info.m_uPrevOffset.x + 153, Info.m_uPrevOffset.y + 16, 140, ( Info.m_uItems.size( ) * 16 ), Color( 0, 0, 0, 255 ) );
			}
		}
	}
}

void Menu::PopUpHandle()
{
	if ( PopUp.size( ) <= 0 )
		return;

	int Offset = 0;

	for ( int index = 1; index < PopUp.size( ); index++ )
	{
		if ( PopUp.at( index ).flLength < Interfaces::GlobalVars->realtime )
		{
			PopUp.erase( PopUp.begin( ) + index );
			continue;
		}

		auto Message = PopUp.at(index);

		Draw::String( 10, 10 + Offset, Draw::Font.Default, Message.cColor, Message.sMessage );
		Offset += 10;
	}

}