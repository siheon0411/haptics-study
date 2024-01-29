#ifndef IMOTION_PLAYBACK_H
#define IMOTION_PLAYBACK_H

#include "IMotion.h"
#include "IMotion_csv.h"

#ifdef __cplusplus
extern "C"{
#endif
	
static int MOTION_AMP_MAX[] = {1, 1, 1, 1, 1, 1};
static int MOTION_FRQ_MAX[] = {1, 1, 1, 1, 1, 1};

typedef struct
{
	IM_FORMAT format;
    uint8 *motion;		/* Pointer to wave data */
    uint32 motionlen;	/* Length of wave data */
    int motionpos;		/* Current play position */
	uint32 mask;		/* Channel Mask of wave data */
} motion_wave;

static struct motion_sample_name
{
	const char* name;
} motion_samples[] = 
{
	"Direct Position", 	
	"Sine Wave",
	"Motion File",
	"Seek Frame",
	"Motion Stream",
};

class IMotion_Playback
{
public:
	IMotion_Playback();
	~IMotion_Playback();
	
private:
	IMotion*		m_pDevice;
	IMotionSource*	m_pSource;
	motion_wave		m_motion_data;
	IMotionDebugCallback m_debug_callback;
	void *m_debug_user_data;
	int m_nPlayCount, m_nPlayState;
	
	// shared motion
	IMotion*		m_pSlave;
public:
	// description
	int m_nDevCount;			// the number of supported device types.
	int m_nDevIndex;			// current device index
	IM_DEVICE_DESC* m_desc;		// supported device specifications
	IM_DEVICE_DESC m_profile;	// currently set device profile
	
	int m_nFltCount;			// the number of supported filter types.
	int m_nFltIndex;			// current filter index
	IM_FILTER_DESC* m_filter;	// supported filter specifications
	
	// diagnostic
	int m_nDevAxisCount;		// the number of supported device axes.
	IM_DIAGNOSTIC_AXIS_INFO m_info[IM_DOF_COUNT];
	
	// If the frequency is 0, it is a manual control, otherwise it is an automatic control(sine wave).
	double m_dPosition[IM_DOF_COUNT];	// current position
	double m_dFrequency[IM_DOF_COUNT];	// sine wave frequency
	double m_dAmplitude[IM_DOF_COUNT];	// sine wave amplitude

	// operation
	int m_nSampleRate;	// samples per sec
	int m_nTest;
	int m_nSamplePos, m_nLoopCount;	
	int m_nPlayTime, m_nDuration;	// motion data
	bool m_bLoadData, m_bPlayData;

	int GetProfile(int nDevId);
	int SetProfile(int nDevId, int nDevNo);	// change device

public:
	// motion device
	int Open(int nDevId=0, int nSlaveId=0);		// connect (zero position)
	int Stop(unsigned int nFlags=IM_DEVICE_MOVE_SETTLE);	// settle (init position)
	int Start(unsigned int nFlags=IM_DEVICE_MOVE_NEUTRAL);	// neutral (center position)
	int Close(unsigned int nFlags=0);	// disconnect (init position)
	
	int SetServo(int nAxisNo);		// servo on/off
	int AlarmReset(int nAxisNo);	// alarm reset
	int SetLogger(IMotionDebugCallback callback, void *userdata);
	int Notify(unsigned int nState);// completition callback
	bool IsBusy();		// for device sync
	int GetDefaultMask(int channels);

	// motion source
	int Load(const char* url, const char* key);
	int LoadData(const double* pam, int samples, IM_FORMAT* format);
	int Unload();
	int PlayMotion();
	int StopMotion();
	int SendSample(int nSampleNo);
	int SendBuffer();
	int Update(unsigned int time);
};

#ifdef __cplusplus
}
#endif

#endif // IMOTION_PLAYBACK_H