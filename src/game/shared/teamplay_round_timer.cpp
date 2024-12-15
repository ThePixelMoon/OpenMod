//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Team gamerules round timer 
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "teamplay_round_timer.h"
#include "teamplayroundbased_gamerules.h"

#ifdef CLIENT_DLL
#include "iclientmode.h"
#include "vgui_controls/AnimationController.h"
#include "c_playerresource.h"
#include "c_team_objectiveresource.h"
#if defined( TF_CLIENT_DLL )
#include "tf_gamerules.h"
#include "c_tf_player.h"
#endif // TF_CLIENT_DLL
#else
#include "team.h"
#include "team_objectiveresource.h"
#if defined( TF_DLL )
#include "tf_player.h"
#endif // TF_DLL
#endif

#define ROUND_TIMER_60SECS	"Announcer.RoundEnds60seconds"
#define ROUND_TIMER_30SECS	"Announcer.RoundEnds30seconds"
#define ROUND_TIMER_10SECS	"Announcer.RoundEnds10seconds"
#define ROUND_TIMER_5SECS	"Announcer.RoundEnds5seconds"
#define ROUND_TIMER_4SECS	"Announcer.RoundEnds4seconds"
#define ROUND_TIMER_3SECS	"Announcer.RoundEnds3seconds"
#define ROUND_TIMER_2SECS	"Announcer.RoundEnds2seconds"
#define ROUND_TIMER_1SECS	"Announcer.RoundEnds1seconds"

#define ROUND_SETUP_60SECS	"Announcer.RoundBegins60Seconds"
#define ROUND_SETUP_30SECS	"Announcer.RoundBegins30Seconds"
#define ROUND_SETUP_10SECS	"Announcer.RoundBegins10Seconds"
#define ROUND_SETUP_5SECS	"Announcer.RoundBegins5Seconds"
#define ROUND_SETUP_4SECS	"Announcer.RoundBegins4Seconds"
#define ROUND_SETUP_3SECS	"Announcer.RoundBegins3Seconds"
#define ROUND_SETUP_2SECS	"Announcer.RoundBegins2Seconds"
#define ROUND_SETUP_1SECS	"Announcer.RoundBegins1Seconds"

#ifdef TF_CLIENT_DLL
#define MERASMUS_SETUP_5SECS	"Merasmus.RoundBegins5Seconds"
#define MERASMUS_SETUP_4SECS	"Merasmus.RoundBegins4Seconds"
#define MERASMUS_SETUP_3SECS	"Merasmus.RoundBegins3Seconds"
#define MERASMUS_SETUP_2SECS	"Merasmus.RoundBegins2Seconds"
#define MERASMUS_SETUP_1SECS	"Merasmus.RoundBegins1Seconds"
#endif

#define ROUND_START_BELL	"Ambient.Siren"

#define ROUND_TIMER_TIME_ADDED			"Announcer.TimeAdded"
#define ROUND_TIMER_TIME_ADDED_LOSER	"Announcer.TimeAddedForEnemy"
#define ROUND_TIMER_TIME_ADDED_WINNER	"Announcer.TimeAwardedForTeam"

enum
{
	RT_THINK_SETUP,
	RT_THINK_NORMAL,	
};

enum
{
	RT_WARNING_60SECS,
	RT_WARNING_30SECS,
	RT_WARNING_10SECS,
	RT_WARNING_5SECS,
	RT_WARNING_4SECS,
	RT_WARNING_3SECS,
	RT_WARNING_2SECS,
	RT_WARNING_1SECS,
	RT_WARNING_TIME_START,
};

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern bool IsInCommentaryMode();

#if defined( GAME_DLL ) && defined( TF_DLL )
ConVar tf_overtime_nag( "tf_overtime_nag", "0", FCVAR_NOTIFY, "Announcer overtime nag." );
#endif

#ifdef CLIENT_DLL

// Use this proxy to flash the round timer whenever the timer is restarted
// because trapping the round start event doesn't work ( the event also flushes
// all hud events and obliterates our TimerFlash event )
static void RecvProxy_TimerPaused( const CRecvProxyData *pData, void *pStruct, void *pOut )
{
	CTeamRoundTimer *pTimer = (CTeamRoundTimer *) pStruct;

	bool bTimerPaused = ( pData->m_Value.m_Int > 0 );

	if ( bTimerPaused == false )
	{
		g_pClientMode->GetViewportAnimationController()->StartAnimationSequence( "TimerFlash" ); 
	}

	if ( pTimer )
	{
		pTimer->InternalSetPaused( bTimerPaused );
	}
}

#endif

LINK_ENTITY_TO_CLASS( team_round_timer, CTeamRoundTimer );
PRECACHE_REGISTER( team_round_timer );

IMPLEMENT_NETWORKCLASS_ALIASED( TeamRoundTimer, DT_TeamRoundTimer )

BEGIN_NETWORK_TABLE_NOBASE( CTeamRoundTimer, DT_TeamRoundTimer )
#ifdef CLIENT_DLL

	RecvPropInt( RECVINFO( m_bTimerPaused ), 0, RecvProxy_TimerPaused ),
	RecvPropTime( RECVINFO( m_flTimeRemaining ) ),
	RecvPropTime( RECVINFO( m_flTimerEndTime ) ),
	RecvPropInt( RECVINFO( m_nTimerMaxLength ) ),
	RecvPropBool( RECVINFO( m_bIsDisabled ) ),
	RecvPropBool( RECVINFO( m_bShowInHUD ) ),
	RecvPropInt( RECVINFO( m_nTimerLength ) ),
	RecvPropInt( RECVINFO( m_nTimerInitialLength ) ),
	RecvPropBool( RECVINFO( m_bAutoCountdown ) ),
	RecvPropInt( RECVINFO( m_nSetupTimeLength ) ),
	RecvPropInt( RECVINFO( m_nState ) ),
	RecvPropBool( RECVINFO( m_bStartPaused ) ),
	RecvPropBool( RECVINFO( m_bShowTimeRemaining ) ),
	RecvPropBool( RECVINFO( m_bInCaptureWatchState ) ),
	RecvPropBool( RECVINFO( m_bStopWatchTimer ) ),
	RecvPropTime( RECVINFO( m_flTotalTime ) ),

#else

	SendPropBool( SENDINFO( m_bTimerPaused ) ),
	SendPropTime( SENDINFO( m_flTimeRemaining ) ),
	SendPropTime( SENDINFO( m_flTimerEndTime ) ),
	SendPropInt( SENDINFO( m_nTimerMaxLength ) ),
	SendPropBool( SENDINFO( m_bIsDisabled ) ),
	SendPropBool( SENDINFO( m_bShowInHUD ) ),
	SendPropInt( SENDINFO( m_nTimerLength ) ),
	SendPropInt( SENDINFO( m_nTimerInitialLength ) ),
	SendPropBool( SENDINFO( m_bAutoCountdown ) ),
	SendPropInt( SENDINFO( m_nSetupTimeLength ) ),
	SendPropInt( SENDINFO( m_nState ) ),
	SendPropBool( SENDINFO( m_bStartPaused ) ),
	SendPropBool( SENDINFO( m_bShowTimeRemaining ) ),
	SendPropBool( SENDINFO( m_bStopWatchTimer ) ),
	SendPropBool( SENDINFO( m_bInCaptureWatchState ) ),
	SendPropTime( SENDINFO( m_flTotalTime ) ),

#endif
END_NETWORK_TABLE()

#ifndef CLIENT_DLL
BEGIN_DATADESC(CTeamRoundTimer)
	DEFINE_KEYFIELD( m_nTimerInitialLength,		FIELD_INTEGER,	"timer_length" ),
	DEFINE_KEYFIELD( m_nTimerMaxLength,			FIELD_INTEGER,	"max_length" ),
	DEFINE_KEYFIELD( m_bShowInHUD,				FIELD_BOOLEAN,	"show_in_hud" ),
	DEFINE_KEYFIELD( m_bIsDisabled,				FIELD_BOOLEAN,	"StartDisabled" ),
	DEFINE_KEYFIELD( m_bAutoCountdown,			FIELD_BOOLEAN,	"auto_countdown" ),
	DEFINE_KEYFIELD( m_nSetupTimeLength,		FIELD_INTEGER,	"setup_length" ),
	DEFINE_KEYFIELD( m_bResetTimeOnRoundStart,	FIELD_BOOLEAN,	"reset_time" ),
	DEFINE_KEYFIELD( m_bStartPaused,			FIELD_BOOLEAN,	"start_paused" ),
	DEFINE_KEYFIELD( m_bShowTimeRemaining,			FIELD_BOOLEAN,	"show_time_remaining" ),

	DEFINE_FUNCTION( RoundTimerSetupThink ),
	DEFINE_FUNCTION( RoundTimerThink ),

	DEFINE_INPUTFUNC( FIELD_VOID,		"Enable",			InputEnable ),
	DEFINE_INPUTFUNC( FIELD_VOID,		"Disable",			InputDisable ),
	DEFINE_INPUTFUNC( FIELD_VOID,		"Pause",			InputPause ),
	DEFINE_INPUTFUNC( FIELD_VOID,		"Resume",			InputResume ),
	DEFINE_INPUTFUNC( FIELD_INTEGER,	"SetTime",			InputSetTime ),
	DEFINE_INPUTFUNC( FIELD_INTEGER,	"AddTime",			InputAddTime ),
	DEFINE_INPUTFUNC( FIELD_VOID,		"Restart",			InputRestart ),
	DEFINE_INPUTFUNC( FIELD_INTEGER,	"ShowInHUD",		InputShowInHUD ),
	DEFINE_INPUTFUNC( FIELD_VOID,		"RoundSpawn",		InputRoundSpawn ),
	DEFINE_INPUTFUNC( FIELD_INTEGER,	"SetMaxTime",		InputSetMaxTime ),
	DEFINE_INPUTFUNC( FIELD_INTEGER,	"AutoCountdown",	InputAutoCountdown ),
	DEFINE_INPUTFUNC( FIELD_STRING,		"AddTeamTime",		InputAddTeamTime ),
	DEFINE_INPUTFUNC( FIELD_INTEGER,	"SetSetupTime",		InputSetSetupTime ),

	DEFINE_OUTPUT(	m_OnRoundStart,		"OnRoundStart" ),
	DEFINE_OUTPUT(	m_OnFinished,		"OnFinished" ),
	DEFINE_OUTPUT(	m_On5MinRemain,		"On5MinRemain" ),
	DEFINE_OUTPUT(	m_On4MinRemain,		"On4MinRemain" ),
	DEFINE_OUTPUT(	m_On3MinRemain,		"On3MinRemain" ),
	DEFINE_OUTPUT(	m_On2MinRemain,		"On2MinRemain" ),
	DEFINE_OUTPUT(	m_On1MinRemain,		"On1MinRemain" ),
	DEFINE_OUTPUT(	m_On30SecRemain,	"On30SecRemain" ),
	DEFINE_OUTPUT(	m_On10SecRemain,	"On10SecRemain" ),
	DEFINE_OUTPUT(	m_On5SecRemain,		"On5SecRemain" ),
	DEFINE_OUTPUT(	m_On4SecRemain,		"On4SecRemain" ),
	DEFINE_OUTPUT(	m_On3SecRemain,		"On3SecRemain" ),
	DEFINE_OUTPUT(	m_On2SecRemain,		"On2SecRemain" ),
	DEFINE_OUTPUT(	m_On1SecRemain,		"On1SecRemain" ),
	DEFINE_OUTPUT(	m_OnSetupStart,		"OnSetupStart" ),
	DEFINE_OUTPUT(	m_OnSetupFinished,	"OnSetupFinished" ),

END_DATADESC();
#endif

#ifndef CLIENT_DLL
#define ROUND_TIMER_THINK			"CTeamplayRoundTimerThink"
#define ROUND_TIMER_SETUP_THINK		"CTeamplayRoundTimerSetupThink"
#endif

//-----------------------------------------------------------------------------
// Purpose: constructor
//-----------------------------------------------------------------------------
CTeamRoundTimer::CTeamRoundTimer( void )
{
	m_bTimerPaused = false;
	m_flTimeRemaining = 0;
	m_nTimerLength = 0;
	m_nTimerInitialLength = 0;
	m_nTimerMaxLength = 0;
	m_flTimerEndTime = 0;
	m_bIsDisabled = false;
	m_bAutoCountdown = true;
	m_nState.Set( RT_STATE_NORMAL );        // we'll assume no setup time for now
	m_bStartPaused = true;
	m_bShowTimeRemaining = true;

	m_bFireFinished = true;
	m_bFire5MinRemain = true;
	m_bFire4MinRemain = true;
	m_bFire3MinRemain = true;
	m_bFire2MinRemain = true;
	m_bFire1MinRemain = true;
	m_bFire30SecRemain = true;
	m_bFire10SecRemain = true;
	m_bFire5SecRemain = true;
	m_bFire4SecRemain = true;
	m_bFire3SecRemain = true;
	m_bFire2SecRemain = true;
	m_bFire1SecRemain = true;

	m_bStopWatchTimer = false;

	m_flTotalTime = 0.0f;

	m_nSetupTimeLength = 0;

#ifndef CLIENT_DLL
	m_bPauseDueToWin = false;
	m_bResetTimeOnRoundStart = false;
	m_nTimeToUseAfterSetupFinished = 0;
	m_flNextOvertimeNag = 0;
	m_flLastTime = 0.f;
#endif
}

//-----------------------------------------------------------------------------
// Purpose: destructor
//-----------------------------------------------------------------------------
CTeamRoundTimer::~CTeamRoundTimer( void )
{

}

//-----------------------------------------------------------------------------
// Purpose: destructor
//-----------------------------------------------------------------------------
void CTeamRoundTimer::Precache( void )
{
#if defined( TF_DLL ) || defined( TF_CLIENT_DLL ) 
	PrecacheScriptSound( ROUND_TIMER_60SECS );
	PrecacheScriptSound( ROUND_TIMER_30SECS );
	PrecacheScriptSound( ROUND_TIMER_10SECS );
	PrecacheScriptSound( ROUND_TIMER_5SECS );
	PrecacheScriptSound( ROUND_TIMER_4SECS );
	PrecacheScriptSound( ROUND_TIMER_3SECS );
	PrecacheScriptSound( ROUND_TIMER_2SECS );
	PrecacheScriptSound( ROUND_TIMER_1SECS );
	PrecacheScriptSound( ROUND_SETUP_60SECS );
	PrecacheScriptSound( ROUND_SETUP_30SECS );
	PrecacheScriptSound( ROUND_SETUP_10SECS );
	PrecacheScriptSound( ROUND_SETUP_5SECS );
	PrecacheScriptSound( ROUND_SETUP_4SECS );
	PrecacheScriptSound( ROUND_SETUP_3SECS );
	PrecacheScriptSound( ROUND_SETUP_2SECS );
	PrecacheScriptSound( ROUND_SETUP_1SECS );
	PrecacheScriptSound( ROUND_TIMER_TIME_ADDED );
	PrecacheScriptSound( ROUND_TIMER_TIME_ADDED_LOSER );
	PrecacheScriptSound( ROUND_TIMER_TIME_ADDED_WINNER );
	PrecacheScriptSound( ROUND_START_BELL );

#ifdef TF_CLIENT_DLL
	PrecacheScriptSound( MERASMUS_SETUP_5SECS );
	PrecacheScriptSound( MERASMUS_SETUP_4SECS );
	PrecacheScriptSound( MERASMUS_SETUP_3SECS );
	PrecacheScriptSound( MERASMUS_SETUP_2SECS );
	PrecacheScriptSound( MERASMUS_SETUP_1SECS );
#endif // TF_CLIENT_DLL
#endif // TF_DLL || TF_CLIENT_DLL
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamRoundTimer::Activate( void )
{
	BaseClass::Activate();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamRoundTimer::Spawn( void )
{
	Precache();

#ifdef CLIENT_DLL
	SetNextClientThink( CLIENT_THINK_ALWAYS );
#else

	int nTimerTime = 0;

	// do we have a setup time?
	if ( m_nSetupTimeLength > 0 )
	{
		nTimerTime = m_nSetupTimeLength;
		SetState( RT_STATE_SETUP );
	}
	else
	{
		nTimerTime = m_nTimerInitialLength;
		SetState( RT_STATE_NORMAL );
	}

	m_nTimeToUseAfterSetupFinished = m_nTimerInitialLength;

	if ( IsDisabled() )  // we need to get the data initialized before actually become disabled
	{
		m_bIsDisabled = false;
		PauseTimer(); // start paused
		SetTimeRemaining( nTimerTime );
		m_bIsDisabled = true;
	}
	else
	{
		PauseTimer(); // start paused
		SetTimeRemaining( nTimerTime );
	}

	m_nTimerLength = nTimerTime;

	BaseClass::Spawn();
#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTeamRoundTimer::ShowInHud( void )
{
	return m_bShowInHUD;
}

//-----------------------------------------------------------------------------
// Purpose: Gets the seconds left on the timer, paused or not.
//-----------------------------------------------------------------------------
float CTeamRoundTimer::GetTimeRemaining( void )
{
	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamRoundTimer::SetCaptureWatchState( bool bCaptureWatch )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CTeamRoundTimer::GetTimerMaxLength( void )
{
	return 99;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamRoundTimer::CalculateOutputMessages( void )
{
}

#ifdef CLIENT_DLL

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamRoundTimer::ClientThink()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamRoundTimer::OnPreDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnPreDataChanged( updateType );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamRoundTimer::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
const char *CTeamRoundTimer::GetTimeWarningSound( int nWarning )
{
	return "";
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamRoundTimer::SendTimeWarning( int nWarning )
{
}

#else

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamRoundTimer::SetState( int nState, bool bFireOutput )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamRoundTimer::SetTimerThink( int nType )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamRoundTimer::RoundTimerSetupThink( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamRoundTimer::RoundTimerThink( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamRoundTimer::InputRoundSpawn( inputdata_t &input )
{
}

//-----------------------------------------------------------------------------
// Purpose: To set the initial timer duration
//-----------------------------------------------------------------------------
void CTeamRoundTimer::SetTimeRemaining( int iTimerSeconds )
{
}

//-----------------------------------------------------------------------------
// Purpose: To set the initial timer duration
//-----------------------------------------------------------------------------
void CTeamRoundTimer::SetStopWatchTimeStamp( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: Timer is paused at round end, stops the countdown
//-----------------------------------------------------------------------------
void CTeamRoundTimer::PauseTimer( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: To start or re-start the timer after a pause
//-----------------------------------------------------------------------------
void CTeamRoundTimer::ResumeTimer( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: Add seconds to the timer while it is running or paused
//-----------------------------------------------------------------------------
void CTeamRoundTimer::AddTimerSeconds( int iSecondsToAdd, int iTeamResponsible /* = TEAM_UNASSIGNED*/ )
{
}

//-----------------------------------------------------------------------------
// Purpose: The timer is always transmitted to clients
//-----------------------------------------------------------------------------
int CTeamRoundTimer::UpdateTransmitState()
{
	// ALWAYS transmit to all clients.
	return SetTransmitState( FL_EDICT_ALWAYS );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamRoundTimer::InputPause( inputdata_t &input )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamRoundTimer::InputResume( inputdata_t &input )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamRoundTimer::InputSetTime( inputdata_t &input )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamRoundTimer::InputSetMaxTime( inputdata_t &input )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamRoundTimer::InputAddTime( inputdata_t &input )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamRoundTimer::InputAddTeamTime( inputdata_t &input )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamRoundTimer::InputRestart( inputdata_t &input )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamRoundTimer::InputEnable( inputdata_t &input )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamRoundTimer::InputDisable( inputdata_t &input )
{ 
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamRoundTimer::InputShowInHUD( inputdata_t &input )
{ 
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamRoundTimer::InputAutoCountdown( inputdata_t &input )
{ 
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamRoundTimer::InputSetSetupTime( inputdata_t &input )
{ 
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamRoundTimer::SetActiveTimer( CTeamRoundTimer *pNewlyActive )
{
}

#endif
