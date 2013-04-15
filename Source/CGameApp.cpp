//-----------------------------------------------------------------------------
// File: CGameApp.cpp
//
// Desc: Game Application class, this is the central hub for all app processing
//
// Original design by Adam Hoult & Gary Simmons. Modified by Mihai Popescu.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CGameApp Specific Includes
//-----------------------------------------------------------------------------
#include "CGameApp.h"

extern HINSTANCE g_hInst;

//-----------------------------------------------------------------------------
// CGameApp Member Functions
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Name : CGameApp () (Constructor)
// Desc : CGameApp Class Constructor
//-----------------------------------------------------------------------------
CGameApp::CGameApp()
{
	// Reset / Clear all required values
	m_hWnd			= NULL;
	m_hIcon			= NULL;
	m_hMenu			= NULL;
	m_pBBuffer		= NULL;
	m_pPlayer		= NULL;
	m_pPlayer1		= NULL;
	m_pPlayer2		= NULL;
	m_pPlayer3		= NULL;
	m_pPlayer4		= NULL;
	m_pPlayer5		= NULL;
	m_pPlayer6		= NULL;
	
	missile			= NULL;
	bullet			= NULL;

	m_LastFrameRate = 0;
}

//-----------------------------------------------------------------------------
// Name : ~CGameApp () (Destructor)
// Desc : CGameApp Class Destructor
//-----------------------------------------------------------------------------
CGameApp::~CGameApp()
{
	// Shut the engine down
	ShutDown();
}

//-----------------------------------------------------------------------------
// Name : InitInstance ()
// Desc : Initialises the entire Engine here.
//-----------------------------------------------------------------------------
bool CGameApp::InitInstance( LPCTSTR lpCmdLine, int iCmdShow )
{
	// Create the primary display device
	if (!CreateDisplay()) { ShutDown(); return false; }

	// Build Objects
	if (!BuildObjects()) 
	{ 
		MessageBox( 0, _T("Failed to initialize properly. Reinstalling the application may solve this problem.\nIf the problem persists, please contact technical support."), _T("Fatal Error"), MB_OK | MB_ICONSTOP);
		ShutDown(); 
		return false; 
	}

	// Set up all required game states
	SetupGameState();

	// Success!
	return true;
}

//-----------------------------------------------------------------------------
// Name : CreateDisplay ()
// Desc : Create the display windows, devices etc, ready for rendering.
//-----------------------------------------------------------------------------
bool CGameApp::CreateDisplay()
{
	HMONITOR hmon = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi = { sizeof(mi) };
	if (!GetMonitorInfo(hmon, &mi)) return NULL;

	LPTSTR			WindowTitle		= _T("GameFramework");
	LPCSTR			WindowClass		= _T("GameFramework_Class");
	USHORT			Width			= mi.rcMonitor.right;
	USHORT			Height			= mi.rcMonitor.left;
	RECT			rc;
	WNDCLASSEX		wcex;

	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= CGameApp::StaticWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= g_hInst;
	wcex.hIcon			= LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_ICON));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= WindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON));

	if(RegisterClassEx(&wcex)==0)
		return false;

	// Retrieve the final client size of the window
	::GetClientRect( m_hWnd, &rc );
	m_nViewX		= rc.left;
	m_nViewY		= rc.top;
	m_nViewWidth	= rc.right - rc.left;
	m_nViewHeight	= rc.bottom - rc.top;

	

	m_hWnd = CreateWindow(WindowClass, WindowTitle, WS_POPUP | WS_VISIBLE,
	mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left,
	mi.rcMonitor.bottom - mi.rcMonitor.top, NULL, NULL, g_hInst, this);

	if (!m_hWnd)
		return false;

	// Show the window
	ShowWindow(m_hWnd, SW_SHOW);

	// Success!!
	return true;
}

//-----------------------------------------------------------------------------
// Name : BeginGame ()
// Desc : Signals the beginning of the physical post-initialisation stage.
//		From here on, the game engine has control over processing.
//-----------------------------------------------------------------------------
int CGameApp::BeginGame()
{
	MSG		msg;

	// Start main loop
	while(true) 
	{
		// Did we recieve a message, or are we idling ?
		if ( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) ) 
		{
			if (msg.message == WM_QUIT) break;
			TranslateMessage( &msg );
			DispatchMessage ( &msg );
		} 
		else 
		{
			// Advance Game Frame.
			FrameAdvance();

		} // End If messages waiting
	
	} // Until quit message is receieved

	return 0;
}

//-----------------------------------------------------------------------------
// Name : ShutDown ()
// Desc : Shuts down the game engine, and frees up all resources.
//-----------------------------------------------------------------------------
bool CGameApp::ShutDown()
{
	// Release any previously built objects
	ReleaseObjects ( );
	
	// Destroy menu, it may not be attached
	if ( m_hMenu ) DestroyMenu( m_hMenu );
	m_hMenu		 = NULL;

	// Destroy the render window
	SetMenu( m_hWnd, NULL );
	if ( m_hWnd ) DestroyWindow( m_hWnd );
	m_hWnd		  = NULL;
	
	// Shutdown Success
	return true;
}

//-----------------------------------------------------------------------------
// Name : StaticWndProc () (Static Callback)
// Desc : This is the main messge pump for ALL display devices, it captures
//		the appropriate messages, and routes them through to the application
//		class for which it was intended, therefore giving full class access.
// Note : It is VITALLY important that you should pass your 'this' pointer to
//		the lpParam parameter of the CreateWindow function if you wish to be
//		able to pass messages back to that app object.
//-----------------------------------------------------------------------------
LRESULT CALLBACK CGameApp::StaticWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	// If this is a create message, trap the 'this' pointer passed in and store it within the window.
	if ( Message == WM_CREATE ) SetWindowLong( hWnd, GWL_USERDATA, (LONG)((CREATESTRUCT FAR *)lParam)->lpCreateParams);

	// Obtain the correct destination for this message
	CGameApp *Destination = (CGameApp*)GetWindowLong( hWnd, GWL_USERDATA );
	
	// If the hWnd has a related class, pass it through
	if (Destination) return Destination->DisplayWndProc( hWnd, Message, wParam, lParam );
	
	// No destination found, defer to system...
	return DefWindowProc( hWnd, Message, wParam, lParam );
}

//-----------------------------------------------------------------------------
// Name : DisplayWndProc ()
// Desc : The display devices internal WndProc function. All messages being
//		passed to this function are relative to the window it owns.
//-----------------------------------------------------------------------------
LRESULT CGameApp::DisplayWndProc( HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam )
{
	static UINT			fTimer;	

	// Determine message type
	switch (Message)
	{
		case WM_CREATE:
			break;
		
		case WM_CLOSE:
			PostQuitMessage(0);
			break;
		
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		
		case WM_SIZE:
			if ( wParam == SIZE_MINIMIZED )
			{
				// App is inactive
				m_bActive = false;
			
			} // App has been minimized
			else
			{
				// App is active
				m_bActive = true;

				// Store new viewport sizes
				m_nViewWidth  = LOWORD( lParam );
				m_nViewHeight = HIWORD( lParam );
		
			
			} // End if !Minimized

			break;

		case WM_LBUTTONDOWN:
			// Capture the mouse
			SetCapture( m_hWnd );
			GetCursorPos( &m_OldCursorPos );
			break;

		case WM_LBUTTONUP:
			// Release the mouse
			ReleaseCapture( );
			break;

		case WM_KEYDOWN:
			switch(wParam)
			{
			case VK_ESCAPE:
				PostQuitMessage(0);
				break;
			case VK_RETURN:
				fTimer = SetTimer(m_hWnd, 1, 250, NULL);
				m_pPlayer->Explode();
				break;
			case VK_CONTROL:
				fTimer= SetTimer(m_hWnd, 1, 250, NULL);
				m_pPlayer1->Explode();
				break;
			}
			break;

		case WM_TIMER:
			switch(wParam)
			{
			case 1:
				if(!m_pPlayer->AdvanceExplosion())
					KillTimer(m_hWnd, 1);
				if(!m_pPlayer1->AdvanceExplosion())
					KillTimer(m_hWnd, 1);
				break;
			}

		case WM_COMMAND:
			break;

		default:
			return DefWindowProc(hWnd, Message, wParam, lParam);

	} // End Message Switch
	
	return 0;
}

//-----------------------------------------------------------------------------
// Name : BuildObjects ()
// Desc : Build our demonstration meshes, and the objects that instance them
//-----------------------------------------------------------------------------
bool CGameApp::BuildObjects()
{
	m_pBBuffer = new BackBuffer(m_hWnd, m_nViewWidth, m_nViewHeight);
	m_pPlayer = new CPlayer(m_pBBuffer, CPlayer::image);
	m_pPlayer1 = new CPlayer(m_pBBuffer, CPlayer::image1);
	m_pPlayer2 = new CPlayer(m_pBBuffer, CPlayer::image1);
	m_pPlayer2 = new CPlayer(m_pBBuffer, CPlayer::image1);
	m_pPlayer3 = new CPlayer(m_pBBuffer, CPlayer::image1);
	m_pPlayer4 = new CPlayer(m_pBBuffer, CPlayer::image1);
	m_pPlayer5 = new CPlayer(m_pBBuffer, CPlayer::image1);
	m_pPlayer6 = new CPlayer(m_pBBuffer, CPlayer::image1);
	m_pPlayer7= new CPlayer(m_pBBuffer, CPlayer::image1);
   
	missile  = new CPlayer(m_pBBuffer,CPlayer::image2);
	missile1  = new CPlayer(m_pBBuffer,CPlayer::image2);
	missile2  = new CPlayer(m_pBBuffer,CPlayer::image2);
	missile3  = new CPlayer(m_pBBuffer,CPlayer::image2);
	missile4  = new CPlayer(m_pBBuffer,CPlayer::image2);
	missile5  = new CPlayer(m_pBBuffer,CPlayer::image2);
	missile6  = new CPlayer(m_pBBuffer,CPlayer::image2);
	missile7  = new CPlayer(m_pBBuffer,CPlayer::image2);

	bullet = new CPlayer(m_pBBuffer,CPlayer::image3);

	if(!m_imgBackground.LoadBitmapFromFile("data/background.bmp", GetDC(m_hWnd)))
		return false;

	// Success!
	return true;
}

//-----------------------------------------------------------------------------
// Name : SetupGameState ()
// Desc : Sets up all the initial states required by the game.
//-----------------------------------------------------------------------------
void CGameApp::SetupGameState()
{
	m_pPlayer->Position() = Vec2(700, 650);
	m_pPlayer1->Position() = Vec2(100, 100);
	m_pPlayer2->Position() = Vec2(300, 100);
	m_pPlayer3->Position() = Vec2(500, 100);
	m_pPlayer4->Position() = Vec2(700, 100);
	m_pPlayer5->Position() = Vec2(900, 100);
	m_pPlayer6->Position() = Vec2(1100, 100);
	m_pPlayer7->Position() = Vec2(1300, 100);

	missile->Velocity().y = 0;
	missile1->Velocity().y = 0;
	missile2->Velocity().y = 0;
	missile3->Velocity().y = 0;
	missile4->Velocity().y = 0;
	missile5->Velocity().y = 0;
	missile6->Velocity().y = 0;
	missile7->Velocity().y = 0;

	bullet->Velocity().y  = 0;
}

//-----------------------------------------------------------------------------
// Name : ReleaseObjects ()
// Desc : Releases our objects and their associated memory so that we can
//		rebuild them, if required, during our applications life-time.
//-----------------------------------------------------------------------------
void CGameApp::ReleaseObjects( )
{
	if(m_pPlayer != NULL)
	{
		delete m_pPlayer;
		m_pPlayer = NULL;
	}

	if(m_pPlayer1 != NULL)
	{
		delete m_pPlayer1;
		m_pPlayer1 = NULL;
	}
	if(m_pPlayer2 != NULL)
	{
		delete m_pPlayer2;
		m_pPlayer2 = NULL;
	}
	if(m_pPlayer3 != NULL)
	{
		delete m_pPlayer3;
		m_pPlayer3 = NULL;
	}
	if(m_pPlayer4 != NULL)
	{
		delete m_pPlayer4;
		m_pPlayer4 = NULL;
	}
	if(m_pPlayer5 != NULL)
	{
		delete m_pPlayer5;
		m_pPlayer5 = NULL;
	}
	if(m_pPlayer6 != NULL)
	{
		delete m_pPlayer6;
		m_pPlayer6 = NULL;
	}
	if(m_pPlayer7 != NULL)
	{
		delete m_pPlayer7;
		m_pPlayer7 = NULL;
	}

	if(missile!= NULL)
	{
		delete missile;
		missile = NULL;
	}
	
	if(bullet!= NULL)
	{
		delete bullet;
		bullet = NULL;
	}
	if(m_pBBuffer != NULL)
	{
		delete m_pBBuffer;
		m_pBBuffer = NULL;
	}
}

//-----------------------------------------------------------------------------
// Name : FrameAdvance () (Private)
// Desc : Called to signal that we are now rendering the next frame.
//-----------------------------------------------------------------------------
void CGameApp::FrameAdvance()
{
	static TCHAR FrameRate[ 50 ];
	static TCHAR TitleBuffer[ 255 ];

	// Advance the timer
	m_Timer.Tick( );

	// Skip if app is inactive
	if ( !m_bActive ) return;
	
	// Get / Display the framerate
	if ( m_LastFrameRate != m_Timer.GetFrameRate() )
	{
		m_LastFrameRate = m_Timer.GetFrameRate( FrameRate, 50 );
		sprintf_s( TitleBuffer, _T("Game : %s"), FrameRate );
		SetWindowText( m_hWnd, TitleBuffer );

	} // End if Frame Rate Altered

	// Poll & Process input devices
	ProcessInput();

	// Animate the game objects
	AnimateObjects();

	// Drawing the game objects
	DrawObjects();
}

//-----------------------------------------------------------------------------
// Name : ProcessInput () (Private)
// Desc : Simply polls the input devices and performs basic input operations
//-----------------------------------------------------------------------------

void CGameApp::ProcessInput( )
{
	HMONITOR hmon = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi = { sizeof(mi) };
	GetMonitorInfo(hmon, &mi);
	int				x			= mi.rcMonitor.right;
	int				y			= mi.rcMonitor.bottom;

	static UCHAR pKeyBuffer[ 256 ];
	ULONG		Direction = 0, Direction1 = 0;
	POINT		CursorPos;
	float		X = 0.0f, Y = 0.0f;

	// Retrieve keyboard state
	if ( !GetKeyboardState( pKeyBuffer ) ) return;

	// Check the relevant keys for the first player
	if ( pKeyBuffer[ VK_UP	] & 0xF0 ) Direction |= CPlayer::DIR_FORWARD;
	if ( pKeyBuffer[ VK_DOWN  ] & 0xF0 ) Direction |= CPlayer::DIR_BACKWARD;
	if ( pKeyBuffer[ VK_LEFT  ] & 0xF0 ) Direction |= CPlayer::DIR_LEFT;
	if ( pKeyBuffer[ VK_RIGHT ] & 0xF0 ) Direction |= CPlayer::DIR_RIGHT;
	

	// Move the player
	Vec2 pos;
	Vec2 pos2;
	pos2 = bullet->Position();
	pos = m_pPlayer->Position();

	if(pos2.y <= 0 && bullet->Velocity().y != 0)
	{
		bullet->stop();
		attack(m_pPlayer, bullet, 300);
	}

	if(missile1->Position().y >= y && missile1->Velocity().y != 0)
	{
		missile1->stop();
		attack(m_pPlayer1, missile1, 100);
	}
	if(missile2->Position().y >= y && missile2->Velocity().y != 0)
	{
		missile2->stop();
		attack(m_pPlayer2, missile2, 100);
	}
	if(missile3->Position().y >= y && missile3->Velocity().y != 0)
	{
		missile3->stop();
		attack(m_pPlayer3, missile3, 100);
	}
	if(missile4->Position().y >= y && missile4->Velocity().y != 0)
	{
		missile4->stop();
		attack(m_pPlayer4, missile4, 100);
	}
	if(missile5->Position().y >= y && missile5->Velocity().y != 0)
	{
		missile5->stop();
		attack(m_pPlayer5, missile5, 100);
	}
	if(missile6->Position().y >= y && missile6->Velocity().y != 0)
	{
		missile6->stop();
		attack(m_pPlayer6, missile6, 100);
	}
	if(missile7->Position().y >= y && missile7->Velocity().y != 0)
	{
		missile7->stop();
		attack(m_pPlayer7, missile7, 100);
	}

	if(pos.x <= x - 50)
		m_pPlayer->Move(Direction);
	else
	{
		m_pPlayer->stop();
		m_pPlayer->Move(4);
	}

	if(pos.x >= 50)
		m_pPlayer->Move(Direction);
	else
	{
		m_pPlayer->stop();
		m_pPlayer->Move(8);
	}
	
	if(pos.y <= y - 70)
		m_pPlayer->Move(Direction);
	else
	{
		m_pPlayer->stop();
		m_pPlayer->Move(1);
	}
	
	if(pos.y >= 70)
		m_pPlayer->Move(Direction);
	else
	{
		m_pPlayer->stop();
		m_pPlayer->Move(2);
	}

	// Now process the mouse (if the button is pressed)
	if ( GetCapture() == m_hWnd )
	{
		// Hide the mouse pointer
		SetCursor( NULL );

		// Retrieve the cursor position
		GetCursorPos( &CursorPos );

		// Reset our cursor position so we can keep going forever :)
		SetCursorPos( m_OldCursorPos.x, m_OldCursorPos.y );

	} // End if Captured
}

//-----------------------------------------------------------------------------
// Name : AnimateObjects () (Private)
// Desc : Animates the objects we currently have loaded.
//-----------------------------------------------------------------------------
void CGameApp::AnimateObjects()
{
	m_pPlayer->Update(m_Timer.GetTimeElapsed());
	m_pPlayer1->Update(m_Timer.GetTimeElapsed());
	m_pPlayer2->Update(m_Timer.GetTimeElapsed());
	m_pPlayer3->Update(m_Timer.GetTimeElapsed());
	m_pPlayer4->Update(m_Timer.GetTimeElapsed());
	m_pPlayer5->Update(m_Timer.GetTimeElapsed());
	m_pPlayer6->Update(m_Timer.GetTimeElapsed());
	m_pPlayer7->Update(m_Timer.GetTimeElapsed());

	missile->Update(m_Timer.GetTimeElapsed());
	missile1->Update(m_Timer.GetTimeElapsed());
	missile2->Update(m_Timer.GetTimeElapsed());
	missile3->Update(m_Timer.GetTimeElapsed());
	missile4->Update(m_Timer.GetTimeElapsed());
	missile5->Update(m_Timer.GetTimeElapsed());
	missile6->Update(m_Timer.GetTimeElapsed());
	missile7->Update(m_Timer.GetTimeElapsed());

	bullet->Update(m_Timer.GetTimeElapsed());
	
	colision(m_pPlayer1,m_pPlayer);
	colision(m_pPlayer2,m_pPlayer);
	colision(m_pPlayer3,m_pPlayer);
	colision(m_pPlayer4,m_pPlayer);
	colision(m_pPlayer5,m_pPlayer);
	colision(m_pPlayer6,m_pPlayer);
	colision(m_pPlayer7,m_pPlayer);

	colision(missile1,m_pPlayer);
	colision(missile2,m_pPlayer);
	colision(missile3,m_pPlayer);
	colision(missile4,m_pPlayer);
	colision(missile5,m_pPlayer);
	colision(missile6,m_pPlayer);
	colision(missile7,m_pPlayer);

	colision(bullet, m_pPlayer1);
	colision(bullet, m_pPlayer2);
	colision(bullet, m_pPlayer3);
	colision(bullet, m_pPlayer4);
	colision(bullet, m_pPlayer5);
	colision(bullet, m_pPlayer6);
	colision(bullet, m_pPlayer7);
}

void CGameApp::AI()
{
	m_pPlayer1->Velocity() = Vec2(0,25);
	m_pPlayer2->Velocity() = Vec2(0,25);
	m_pPlayer3->Velocity() = Vec2(0,25);
	m_pPlayer4->Velocity() = Vec2(0,25);
	m_pPlayer5->Velocity() = Vec2(0,25);
	m_pPlayer6->Velocity() = Vec2(0,25);
	m_pPlayer7->Velocity() = Vec2(0,25);

	m_pPlayer1->Draw();	
	m_pPlayer2->Draw();
	m_pPlayer3->Draw();
	m_pPlayer4->Draw();
	m_pPlayer5->Draw();
	m_pPlayer6->Draw();
	m_pPlayer7->Draw();
	
	missile1->Draw();
	attack(m_pPlayer1, missile1, -100);

	missile2->Draw();
	attack(m_pPlayer2, missile2, -100);

	missile3->Draw();
	attack(m_pPlayer3, missile3, -100);

	missile4->Draw();
	attack(m_pPlayer4, missile4, -100);

	missile5->Draw();
	attack(m_pPlayer5, missile5, -100);

	missile6->Draw();
	attack(m_pPlayer6, missile6, -100);

	missile7->Draw();
	attack(m_pPlayer7, missile7, -100);
}

void CGameApp::attack(CPlayer* m_pPlayer, CPlayer* obj, int val)
{
	Vec2 pos;
	pos = m_pPlayer->Position();
	if(obj->Velocity().y == 0)
		obj->Position() = Vec2(pos.x, pos.y);

	obj->Velocity() = Vec2 (0,-val);
}

void CGameApp::colision(CPlayer* obj, CPlayer* obj2)
{
	Vec2 pos1;
	Vec2 pos2;
	pos1 = obj2->Position();
	pos2 = obj->Position();
	if(abs(pos2.x - pos1.x) < 60 && abs(pos2.y - pos1.y) < 60)
	{
		obj2->Explode();
		obj2->AdvanceExplosion();
		obj2->stop();
		obj2->Position() = Vec2(5000, 5000);
	}
}

//-----------------------------------------------------------------------------
// Name : DrawObjects () (Private)
// Desc : Draws the game objects
//-----------------------------------------------------------------------------
void CGameApp::DrawObjects()
{
	m_pBBuffer->reset();

	m_imgBackground.Paint(m_pBBuffer->getDC(), 0, 0);

	m_pPlayer->Draw();

	AI();

	if(GetKeyState( VK_NUMPAD0 ))
	{
		bullet->Draw();
		attack(m_pPlayer, bullet, 300);
	}
	m_pBBuffer->present();
}
