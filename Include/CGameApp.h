//-----------------------------------------------------------------------------
// File: CGameApp.h
//
// Desc: Game Application class, this is the central hub for all app processing
//
// Original design by Adam Hoult & Gary Simmons. Modified by Mihai Popescu.
//-----------------------------------------------------------------------------

#ifndef _CGAMEAPP_H_
#define _CGAMEAPP_H_

//-----------------------------------------------------------------------------
// CGameApp Specific Includes
//-----------------------------------------------------------------------------
#include "Main.h"
#include "CTimer.h"
#include "CPlayer.h"
#include "BackBuffer.h"
#include "ImageFile.h"

//-----------------------------------------------------------------------------
// Forward Declarations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Main Class Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Name : CGameApp (Class)
// Desc : Central game engine, initialises the game and handles core processes.
//-----------------------------------------------------------------------------
class CGameApp
{
public:
	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//-------------------------------------------------------------------------
			 CGameApp();
	virtual ~CGameApp();

	//-------------------------------------------------------------------------
	// Public Functions for This Class
	//-------------------------------------------------------------------------
	LRESULT	 DisplayWndProc( HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam );
	bool		InitInstance( LPCTSTR lpCmdLine, int iCmdShow );
	int		 BeginGame( );
	bool		ShutDown( );
	
private:
	//-------------------------------------------------------------------------
	// Private Functions for This Class
	//-------------------------------------------------------------------------
	bool		BuildObjects	  ( );
	void		ReleaseObjects	( );
	void		FrameAdvance	  ( );
	bool		CreateDisplay	 ( );
	void		ChangeDevice	  ( );
	void		SetupGameState	( );
	void		AnimateObjects	( );
	void		DrawObjects	   ( );
	void		DrawMenu()		();
	void		ProcessInput	  ( );
	void		attack			(CPlayer* m_pPlayer, CPlayer* obj, int val);
	void		AI				();
	void	   colision         (CPlayer* obj, CPlayer* obj2);

	//-------------------------------------------------------------------------
	// Private Static Functions For This Class
	//-------------------------------------------------------------------------
	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

	//-------------------------------------------------------------------------
	// Private Variables For This Class
	//-------------------------------------------------------------------------
	CTimer				  m_Timer;			// Game timer
	ULONG				   m_LastFrameRate;	// Used for making sure we update only when fps changes.
	
	HWND					m_hWnd;			 // Main window HWND
	HICON				   m_hIcon;			// Window Icon
	HMENU				   m_hMenu;			// Window Menu
	
	bool					m_bActive;		  // Is the application active ?

	ULONG				   m_nViewX;		   // X Position of render viewport
	ULONG				   m_nViewY;		   // Y Position of render viewport
	ULONG				   m_nViewWidth;	   // Width of render viewport
	ULONG				   m_nViewHeight;	  // Height of render viewport

	POINT				   m_OldCursorPos;	 // Old cursor position for tracking
	HINSTANCE				m_hInstance;

	CImageFile				m_imgBackground;
	CImageFile				m_imgBackground2;

	BackBuffer*				m_pBBuffer;
	CPlayer*				m_pPlayer;

	CPlayer*				m_pPlayer1;
	CPlayer*				m_pPlayer2;
	CPlayer*				m_pPlayer3;
	CPlayer*				m_pPlayer4;
	CPlayer*				m_pPlayer5;
	CPlayer*				m_pPlayer6;
	CPlayer*				m_pPlayer7;

	CPlayer*				missile;
	CPlayer*				missile1;
	CPlayer*				missile2;
	CPlayer*				missile3;
	CPlayer*				missile4;
	CPlayer*				missile5;
	CPlayer*				missile6;
	CPlayer*				missile7;
	CPlayer*				bullet;
};

#endif // _CGAMEAPP_H_