//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//===========================================================================//

#include "cbase.h"
#include "team_train_watcher.h"
#include "team_control_point.h"
#include "trains.h"
#include "team_objectiveresource.h"
#include "teamplayroundbased_gamerules.h"
#include "team_control_point.h"
#include "team_control_point_master.h"
#include "engine/IEngineSound.h"
#include "soundenvelope.h"
#include "mp_shareddefs.h"
#include "props.h"
#include "physconstraint.h"

#ifdef TF_DLL
#include "tf_shareddefs.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define MAX_ALARM_TIME_NO_RECEDE 18 // max amount of time to play the alarm if the train isn't going to recede

BEGIN_DATADESC( CTeamTrainWatcher )

	// Inputs.
	DEFINE_INPUTFUNC( FIELD_VOID, "RoundActivate", InputRoundActivate ),
	DEFINE_INPUTFUNC( FIELD_INTEGER, "SetNumTrainCappers", InputSetNumTrainCappers ),
	DEFINE_INPUTFUNC( FIELD_VOID, "OnStartOvertime", InputOnStartOvertime ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Enable", InputEnable ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Disable", InputDisable ),
	DEFINE_INPUTFUNC( FIELD_FLOAT, "SetSpeedForwardModifier", InputSetSpeedForwardModifier ),
	DEFINE_INPUTFUNC( FIELD_INTEGER, "SetTrainRecedeTime", InputSetTrainRecedeTime ),
	DEFINE_INPUTFUNC( FIELD_BOOLEAN, "SetTrainCanRecede", InputSetTrainCanRecede ),
	DEFINE_INPUTFUNC( FIELD_INTEGER, "SetTrainRecedeTimeAndUpdate", InputSetTrainRecedeTimeAndUpdate ),

	// Outputs
	DEFINE_OUTPUT( m_OnTrainStartRecede, "OnTrainStartRecede" ),

	// key
	DEFINE_KEYFIELD( m_iszTrain, FIELD_STRING, "train" ),
	DEFINE_KEYFIELD( m_iszStartNode, FIELD_STRING, "start_node" ),
	DEFINE_KEYFIELD( m_iszGoalNode, FIELD_STRING, "goal_node" ),

	DEFINE_KEYFIELD( m_iszLinkedPathTracks[0], FIELD_STRING, "linked_pathtrack_1" ),
	DEFINE_KEYFIELD( m_iszLinkedCPs[0], FIELD_STRING, "linked_cp_1" ),

	DEFINE_KEYFIELD( m_iszLinkedPathTracks[1], FIELD_STRING, "linked_pathtrack_2" ),
	DEFINE_KEYFIELD( m_iszLinkedCPs[1], FIELD_STRING, "linked_cp_2" ),

	DEFINE_KEYFIELD( m_iszLinkedPathTracks[2], FIELD_STRING, "linked_pathtrack_3" ),
	DEFINE_KEYFIELD( m_iszLinkedCPs[2], FIELD_STRING, "linked_cp_3" ),

	DEFINE_KEYFIELD( m_iszLinkedPathTracks[3], FIELD_STRING, "linked_pathtrack_4" ),
	DEFINE_KEYFIELD( m_iszLinkedCPs[3], FIELD_STRING, "linked_cp_4" ),

	DEFINE_KEYFIELD( m_iszLinkedPathTracks[4], FIELD_STRING, "linked_pathtrack_5" ),
	DEFINE_KEYFIELD( m_iszLinkedCPs[4], FIELD_STRING, "linked_cp_5" ),

	DEFINE_KEYFIELD( m_iszLinkedPathTracks[5], FIELD_STRING, "linked_pathtrack_6" ),
	DEFINE_KEYFIELD( m_iszLinkedCPs[5], FIELD_STRING, "linked_cp_6" ),

	DEFINE_KEYFIELD( m_iszLinkedPathTracks[6], FIELD_STRING, "linked_pathtrack_7" ),
	DEFINE_KEYFIELD( m_iszLinkedCPs[6], FIELD_STRING, "linked_cp_7" ),

	DEFINE_KEYFIELD( m_iszLinkedPathTracks[7], FIELD_STRING, "linked_pathtrack_8" ),
	DEFINE_KEYFIELD( m_iszLinkedCPs[7], FIELD_STRING, "linked_cp_8" ),

	DEFINE_KEYFIELD( m_bTrainCanRecede, FIELD_BOOLEAN, "train_can_recede" ),

	DEFINE_KEYFIELD( m_bHandleTrainMovement, FIELD_BOOLEAN, "handle_train_movement" ),

	// can be up to 8 links

	// min speed for train hud speed levels
	DEFINE_KEYFIELD( m_flSpeedLevels[0], FIELD_FLOAT, "hud_min_speed_level_1" ),
	DEFINE_KEYFIELD( m_flSpeedLevels[1], FIELD_FLOAT, "hud_min_speed_level_2" ),
	DEFINE_KEYFIELD( m_flSpeedLevels[2], FIELD_FLOAT, "hud_min_speed_level_3" ),

	DEFINE_KEYFIELD( m_bDisabled,	FIELD_BOOLEAN,	"StartDisabled" ),

	DEFINE_KEYFIELD( m_iszSparkName, FIELD_STRING, "env_spark_name" ),

	DEFINE_KEYFIELD( m_flSpeedForwardModifier, FIELD_FLOAT, "speed_forward_modifier" ),

	DEFINE_KEYFIELD( m_nTrainRecedeTime, FIELD_INTEGER, "train_recede_time" ),

END_DATADESC()


IMPLEMENT_SERVERCLASS_ST(CTeamTrainWatcher, DT_TeamTrainWatcher)

	SendPropFloat( SENDINFO( m_flTotalProgress ), 11, 0, 0.0f, 1.0f ),
	SendPropInt( SENDINFO( m_iTrainSpeedLevel ), 4 ),
	SendPropTime( SENDINFO( m_flRecedeTime ) ),
	SendPropInt( SENDINFO( m_nNumCappers ) ),
#ifdef GLOWS_ENABLE
	SendPropEHandle( SENDINFO( m_hGlowEnt ) ),
#endif // GLOWS_ENABLE

END_SEND_TABLE()


LINK_ENTITY_TO_CLASS( team_train_watcher, CTeamTrainWatcher );

IMPLEMENT_AUTO_LIST( ITFTeamTrainWatcher );

CTeamTrainWatcher::CTeamTrainWatcher()
{
	m_bDisabled = false;
	m_flRecedeTime = 0;
	m_bWaitingToRecede = false;
	m_bCapBlocked = false;

	m_flNextSpeakForwardConceptTime = 0;
	m_hAreaCap = NULL;

	m_bTrainCanRecede = true;
	m_bAlarmPlayed = false;
	m_pAlarm = NULL;
	m_flAlarmEndTime = -1;

	m_bHandleTrainMovement = false;
	m_flSpeedForwardModifier = 1.0f;
	m_iCurrentHillType = HILL_TYPE_NONE;
	m_flCurrentSpeed = 0.0f;
	m_bReceding = false;

	m_flTrainDistanceFromStart = 0.0f;

	m_nTrainRecedeTime = 0;

#ifdef GLOWS_ENABLE
	m_hGlowEnt.Set( NULL );
#endif // GLOWS_ENABLE

	ChangeTeam( TEAM_UNASSIGNED );
}

CTeamTrainWatcher::~CTeamTrainWatcher()
{
}

void CTeamTrainWatcher::UpdateOnRemove( void )
{
	BaseClass::UpdateOnRemove();
}

int CTeamTrainWatcher::UpdateTransmitState()
{
	return SetTransmitState( FL_EDICT_ALWAYS );
}

void CTeamTrainWatcher::InputRoundActivate( inputdata_t &inputdata )
{
}

void CTeamTrainWatcher::InputEnable( inputdata_t &inputdata )
{
}

void CTeamTrainWatcher::InputDisable( inputdata_t &inputdata )
{
}

ConVar tf_escort_recede_time( "tf_escort_recede_time", "30", 0, "", true, 0, false, 0 );
ConVar tf_escort_recede_time_overtime( "tf_escort_recede_time_overtime", "5", 0, "", true, 0, false, 0 );

void CTeamTrainWatcher::FireGameEvent( IGameEvent *event )
{
}

void CTeamTrainWatcher::HandleSparks( bool bSparks )
{
}

void CTeamTrainWatcher::HandleTrainMovement( bool bStartReceding /* = false */ )
{
}

void CTeamTrainWatcher::InputSetSpeedForwardModifier( inputdata_t &inputdata )
{
}

void CTeamTrainWatcher::InternalSetSpeedForwardModifier( float flModifier )
{
}

void CTeamTrainWatcher::InternalSetNumTrainCappers( int iNumCappers, CBaseEntity *pTrigger )
{
}

// only used for train watchers that control the train movement
void CTeamTrainWatcher::SetNumTrainCappers( int iNumCappers, CBaseEntity *pTrigger )
{
}

void CTeamTrainWatcher::InputSetNumTrainCappers( inputdata_t &inputdata )
{
}

void CTeamTrainWatcher::InputSetTrainRecedeTime( inputdata_t &inputdata )
{
}

void CTeamTrainWatcher::InputSetTrainRecedeTimeAndUpdate(inputdata_t &inputdata)
{
}

void CTeamTrainWatcher::InputSetTrainCanRecede( inputdata_t &inputdata )
{
}

void CTeamTrainWatcher::InputOnStartOvertime( inputdata_t &inputdata )
{
}

#ifdef GLOWS_ENABLE
void CTeamTrainWatcher::FindGlowEntity( void )
{
}
#endif // GLOWS_ENABLE

// ==========================================================
// given a start node and a list of goal nodes
// calculate the distance between each
// ==========================================================
void CTeamTrainWatcher::WatcherActivate( void )
{
}

void CTeamTrainWatcher::StopCaptureAlarm( void )
{
}

void CTeamTrainWatcher::StartCaptureAlarm( CTeamControlPoint *pPoint )
{
}

void CTeamTrainWatcher::PlayCaptureAlert( CTeamControlPoint *pPoint, bool bFinalPointInMap )
{
}


ConVar tf_show_train_path( "tf_show_train_path", "0", FCVAR_CHEAT );

void CTeamTrainWatcher::WatcherThink( void )
{
}

void CTeamTrainWatcher::WatcherAlarmThink( void )
{
}

CBaseEntity *CTeamTrainWatcher::GetTrainEntity( void )
{
	return m_hTrain.Get();
}

bool CTeamTrainWatcher::TimerMayExpire( void )
{
	return true;
}


// Project the given position onto the track and return the point and how far along that projected position is
void CTeamTrainWatcher::ProjectPointOntoPath( const Vector &pos, Vector *posOnPathResult, float *distanceAlongPathResult ) const
{
}


// Return true if the given position is farther down the track than the train is
bool CTeamTrainWatcher::IsAheadOfTrain( const Vector &pos ) const
{
	return false;
}


// return true if the train is almost at the next checkpoint
bool CTeamTrainWatcher::IsTrainNearCheckpoint( void ) const
{
	return false;
}


// return true if the train hasn't left its starting position yet
bool CTeamTrainWatcher::IsTrainAtStart( void ) const
{
	return true;
}


// return world space location of next checkpoint along the path
Vector CTeamTrainWatcher::GetNextCheckpointPosition( void ) const
{
	return vec3_origin;
}
