#pragma once

class CPrediction
{
    // Construction
public:

    virtual ~CPrediction(void) = 0;//

    virtual void Init(void) = 0;//
    virtual void Shutdown(void) = 0;//

                                    // Implement IPrediction
public:

    virtual void Update
    (
        int startframe, // World update ( un-modded ) most recently received
        bool validframe, // Is frame data valid
        int incoming_acknowledged, // Last command acknowledged to have been run by server (un-modded)
        int outgoing_command // Last command (most recent) sent to server (un-modded)
    );//

    virtual void PreEntityPacketReceived(int commands_acknowledged, int current_world_update_packet);//
    virtual void PostEntityPacketReceived(void);//5
    virtual void PostNetworkDataReceived(int commands_acknowledged);//

    virtual void OnReceivedUncompressedPacket(void);//

                                                    // The engine needs to be able to access a few predicted values
    virtual void GetViewOrigin(Vec3D& org);//
    virtual void SetViewOrigin( Vec3D& org);//
    virtual void GetViewAngles( Vec3D& ang);//10
    virtual void SetViewAngles( Vec3D& ang);//

    virtual void GetLocalViewAngles( Vec3D& ang);//
    virtual void SetLocalViewAngles( Vec3D& ang);//

    bool InPrediction;//14
    virtual bool IsFirstTimePredicted(void) const;//

    virtual int GetLastAcknowledgedCommandNumber(void) const;//i

#if !defined( NO_ENTITY_PREDICTION )
    virtual int GetIncomingPacketNumber(void) const;//
#endif
    
    virtual void CheckMovingGround(CBaseEntity* player, double frametime);//
    virtual void RunCommand( CBaseEntity* player, CUserCmd* cmd, IMoveHelper* moveHelper);//

    virtual void SetupMove( CBaseEntity* player, CUserCmd* cmd, IMoveHelper* pHelper, CMoveData* move);//20
    virtual void FinishMove( CBaseEntity* player, CUserCmd* cmd, CMoveData* move);//
    virtual void SetIdealPitch(int nSlot, CBaseEntity* player, const Vec3D& origin, const Vec3D& angles, const Vec3D& viewheight);//

    virtual void CheckError(int nSlot, CBaseEntity* player, int commands_acknowledged);//
    
public:
    virtual void _Update
    (
        int nSlot,
        bool received_new_world_update,
        bool validframe,            // Is frame data valid
        int incoming_acknowledged,  // Last command acknowledged to have been run by server (un-modded)
        int outgoing_command        // Last command (most recent) sent to server (un-modded)
    );

    // Actually does the prediction work, returns false if an error occurred
    bool PerformPrediction(int nSlot, IClientEntity* localPlayer, bool received_new_world_update, int incoming_acknowledged, int outgoing_command);

    void ShiftIntermediateDataForward(int nSlot, int slots_to_remove, int previous_last_slot);

    void RestoreEntityToPredictedFrame(int nSlot, int predicted_frame);

    int ComputeFirstCommandToExecute(int nSlot, bool received_new_world_update, int incoming_acknowledged, int outgoing_command);

    void DumpEntity(IClientEntity* ent, int commands_acknowledged);

    void ShutdownPredictables(void);

    void ReinitPredictables(void);

    void RemoveStalePredictedEntities(int nSlot, int last_command_packet);

    void RestoreOriginalEntityState(int nSlot);

    void RunSimulation(int current_command, float curtime, CUserCmd* cmd, CBaseEntity* localPlayer);

    void Untouch(int nSlot);

    void StorePredictionResults(int nSlot, int predicted_frame);

    bool ShouldDumpEntity( CBaseEntity* ent);

    void SmoothViewOnMovingPlatform( CBaseEntity* pPlayer, Vec3D& offset);

    void ResetSimulationTick();

    void ShowPredictionListEntry(int listRow, int showlist, CBaseEntity* ent, int& totalsize, int& totalsize_intermediate);

    void FinishPredictionList(int listRow, int showlist, int totalsize, int totalsize_intermediate);

    void CheckPredictConvar();

#if !defined( NO_ENTITY_PREDICTION )

#endif
};