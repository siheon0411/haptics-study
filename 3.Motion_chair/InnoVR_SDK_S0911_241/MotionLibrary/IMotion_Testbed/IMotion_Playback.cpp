/********************************************************************************//**
\file      IMotion_Playback.cpp
\brief     IMotion Playback implementation.
\copyright Copyright (C) 2016-2019 InnoSimulation Co., Ltd. All rights reserved.
************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "IMotion_Playback.h"

int pcm2pam(const short* pcm, double* pam, unsigned int mask) {
	if(pcm == 0 || pam == 0)
		return 0;
	
	int channels = 0;
	for(int i=0; i<IM_DOF_COUNT; i++) {
		if(mask & (1<<i)) {
			pam[i] = (double)pcm[channels] * MOTION_AMP_MAX[i] / MOTION_MAX_16;
			channels++;
		}
	}
	return channels;
}

int pam2pcm(const double* pam, short* pcm, unsigned int mask) {
	if(pcm == 0 || pam == 0)
		return 0;
	
	int channels = 0;
	for(int i=0; i<IM_DOF_COUNT; i++) {
		if(mask & (1<<i)) {
			pcm[channels] = pam[i] * MOTION_MAX_16 / MOTION_AMP_MAX[i];
			channels++;
		}
	}
	return channels;
}
	
IMotion_Playback::IMotion_Playback()
	: m_pDevice(0), m_pSlave(0), m_pSource(0), m_desc(0), m_debug_callback(0), m_debug_user_data(0)
{	
	// init
	memset(&m_motion_data, 0, sizeof(motion_wave));
	memset(&m_profile, 0, sizeof(IM_DEVICE_DESC));	
	m_nDevCount = 0;
	m_nDevIndex = 0;
	m_nSamplePos = 0;
	m_nPlayState = 0;
	m_nFltCount = 0;
	m_nFltIndex = 0;

	Close();
	IMotion_Startup();
	
	// description (obtain specifications of supported devices)
	m_nDevCount = IMotion_GetDeviceCount();
	m_desc = (IM_DEVICE_DESC*)malloc(sizeof(IM_DEVICE_DESC)*m_nDevCount);
	for(int i=0; i<m_nDevCount; i++) {
		IMotion_GetDeviceDescription(i, &m_desc[i]);
	}

	m_nFltCount = IMotion_GetFilterCount();
	m_filter = (IM_FILTER_DESC*)malloc(sizeof(IM_FILTER_DESC)*m_nFltCount);
	for(int i=0; i<m_nFltCount; i++) {
		IMotion_GetFilterDescription(i, &m_filter[i]);
	}

	// profile (get the profile of the current device)
	GetProfile(0);
}

int IMotion_Playback::GetProfile(int nDevId)
{
	// get profile from id
	IMotion_GetDeviceProfile(nDevId, &m_profile);

	// find device index
	for(int nDevNo=0; nDevNo<m_nDevCount; nDevNo++) {
		if(!strcmp(m_desc[nDevNo].szName, m_profile.szName)) {
			m_nDevIndex = nDevNo;
			break;
		}
	}
	// find filter index
	for(int nFltNo=0; nFltNo<m_nFltCount; nFltNo++) {
		if(!strcmp(m_filter[nFltNo].szName, m_desc[m_nDevIndex].szFilter)) {
			m_nFltIndex = nFltNo;
			break;
		}
	}	
	// get device axes from mask info in device description
	m_nDevAxisCount = MOTION_BITCOUNT(m_desc[m_nDevIndex].nMask);
	return m_nDevIndex;
}

int IMotion_Playback::SetProfile(int nDevId, int nDevNo)
{
	if(nDevNo >= m_nDevCount) 
		return 0;

	uint32 old_options = m_profile.nOptions;
	uint32 old_axismap = m_profile.nAxisMap;
	m_profile = m_desc[nDevNo];
	m_profile.nAxisMap = old_axismap;
	m_profile.nOptions = old_options;
	
	// find filter index
	for(int nFltNo=0; nFltNo<m_nFltCount; nFltNo++) {
		if(!strcmp(m_filter[nFltNo].szName, m_desc[m_nDevIndex].szFilter)) {
			m_nFltIndex = nFltNo;
			break;
		}
	}

	if(m_debug_callback) {
		char msg[1024];
			sprintf(msg, "\r\n#### profile (%d) #### \r\ntype = 0x%x \r\nmask = 0x%x \r\naxismap = 0x%x \r\nversion = %d \r\noptions = 0x%x", 
				m_nDevIndex, m_profile.nType, m_profile.nMask, m_profile.nAxisMap, m_profile.nVersion, m_profile.nOptions);
		(*m_debug_callback)(0, msg, m_debug_user_data);
		sprintf(msg, "name = \"%s\"", m_profile.szName);
		(*m_debug_callback)(0, msg, m_debug_user_data);
		sprintf(msg, "detail = \"%s\"", m_profile.szDetail);
		(*m_debug_callback)(0, msg, m_debug_user_data);
	}
	// get device axes from mask info in device description
	m_nDevAxisCount = MOTION_BITCOUNT(m_desc[m_nDevIndex].nMask);
	return 1;
}

IMotion_Playback::~IMotion_Playback()
{
	Close();

	if(m_desc) {
		free(m_desc);
		m_desc = NULL;
	}
	if(m_filter) {
		free(m_filter);
		m_filter = NULL;
	}
	IMotion_Shutdown();	
}

int IMotion_Playback::Open(int nDevId, int nSlaveId)
{
	Close();
	
	m_pDevice = IMotion_Create(nDevId, &m_profile);
	m_pDevice->GetProfile(&m_profile);
	int nRet = m_pDevice->GetAxesInfo(m_info, m_nDevAxisCount);
	
	if(nSlaveId) {
		m_pSlave = IMotion_Create(nSlaveId);
	}
	return nRet;
}

int IMotion_Playback::Close(unsigned int nFlags)
{
	Unload();
	Stop();
		
	memset(&m_motion_data, 0, sizeof(motion_wave));		
	memset(m_info, 0, sizeof(m_info));
	memset(m_dPosition, 0, sizeof(m_dPosition));
	memset(m_dFrequency, 0, sizeof(m_dFrequency));
	memset(m_dAmplitude, 0, sizeof(m_dAmplitude));

	m_nTest = 0;
	m_nLoopCount = IM_LOOP_INFINITE;
	m_nSampleRate = IM_FORMAT_SAMPLE_RATE_DEFAULT;
	
	if(m_pSlave) {
		IMotion_Destroy(m_pSlave, nFlags);
		m_pSlave = NULL;
	}
	if(m_pDevice) {
		IMotion_Destroy(m_pDevice, nFlags);
		m_pDevice = NULL;
	}

	return IM_DISCONNECTED;
}

int IMotion_Playback::Start(unsigned int nFlags)
{	
	Stop(IM_DEVICE_MOVE_NONE);

	int ret = 0;
	if(m_pDevice) {
		ret = m_pDevice->Start(NULL, nFlags); 
	}
	if(m_pSlave) {
		m_pSlave->Start(NULL, nFlags, m_pDevice); 
	}
	return ret; 
}

int IMotion_Playback::Stop(unsigned int nFlags) 
{	
	// restore device mask
	if(m_pDevice)
		m_pDevice->GetProfile(&m_profile);		
	// restore motion_data mask
	if(m_motion_data.motion && (m_nTest == 2 || m_nTest == 4))
		m_profile.nMask = m_motion_data.mask;
	
	int ret = 0;
	if(m_pDevice) {
		ret = m_pDevice->Stop(nFlags); 
	}
	if(m_pSlave) {
		m_pSlave->Stop(nFlags); 
	}
	return ret; 
}

bool IMotion_Playback::IsBusy()
{
	for(int i=0; i<m_nDevAxisCount; i++) {
		if(m_info[i].bBusy)
			return true;
	}
	return false;
}

int IMotion_Playback::GetDefaultMask(int channels) 
{
	int mask;
	switch(channels) {
	case 1: mask = (IM_BIT_DOF_HEAVE); break;
	case 2: mask = (IM_BIT_DOF_ROLL | IM_BIT_DOF_PITCH); break;
	case 3: mask = (IM_BIT_DOF_HEAVE | IM_BIT_DOF_ROLL | IM_BIT_DOF_PITCH);	break;
	case 4: mask = (IM_BIT_DOF_HEAVE | IM_BIT_DOF_ROLL | IM_BIT_DOF_PITCH | IM_BIT_DOF_YAW); break;
	default: mask = ((1<<channels)-1);	break;
	}
	return mask;
}

// diagnostic
int IMotion_Playback::SetServo(int nAxisNo)
{
	m_info[nAxisNo].bServoOn = !m_info[nAxisNo].bServoOn;		
	int nRet = m_pDevice->SetAxesInfo(m_info, m_nDevAxisCount);
	return nRet;
}

int IMotion_Playback::AlarmReset(int nAxisNo)
{
	m_info[nAxisNo].bAlarmResetOn = 1;
	int nRet = m_pDevice->SetAxesInfo(m_info, m_nDevAxisCount);
	m_info[nAxisNo].bAlarmResetOn = 0;
	return nRet;
}

// source callback
void MotionCallback(void* context, unsigned int state)
{
	IMotion_Playback* playback = (IMotion_Playback*)context;	
	playback->Notify(state);
}

// log callback
int IMotion_Playback::SetLogger(IMotionDebugCallback callback, void *userdata)
{	
	m_debug_callback = callback;
	m_debug_user_data = userdata;
	return IMotion_SetLogFunction(callback, userdata);	
}

int IMotion_Playback::Load(const char* url, const char* key)
{
	Unload();

	if(IMotion_LoadCSV(url, &m_motion_data.format, (uint8 **)&m_motion_data.motion, (uint32*)&m_motion_data.motionlen, key) == 0) {	
		return 0;
	}
	m_nDuration = (m_motion_data.motionlen / m_motion_data.format.nBlockAlign) * (1000 / m_motion_data.format.nSampleRate);
	m_nSampleRate = m_motion_data.format.nSampleRate;	
	m_bLoadData = true;
	m_motion_data.mask = GetDefaultMask(m_motion_data.format.nChannels);	
	// set source profile
	m_profile.nMask = m_motion_data.mask;
	return 1;
}

int IMotion_Playback::LoadData(const double* pam, int nSamples, IM_FORMAT* format)
{
	if(pam == NULL || format == NULL)
		return 0;

	Unload();
	
	/* load wave */
	int size = nSamples * format->nBlockAlign;
	int16* buffer = (int16*)malloc(size);
	memset(buffer, 0, size);
	
	m_motion_data.format = (*format);
	m_motion_data.motion = (uint8*)buffer;
	m_motion_data.motionlen = size;
	m_motion_data.motionpos = 0;	
	m_motion_data.mask = GetDefaultMask(m_motion_data.format.nChannels);	
	// set source profile
	m_profile.nMask = m_motion_data.mask;

	// get motion source axes from mask info in profile
	for(int s = 0 ; s < nSamples ; ++s)
		pam2pcm(&pam[s * IM_DOF_COUNT], &buffer[s * format->nChannels], m_profile.nMask);
	return 1;
}

int IMotion_Playback::Unload()
{
	StopMotion();

	if(m_motion_data.motion) {
		IMotion_FreeCSV(m_motion_data.motion);
		memset(&m_motion_data, 0, sizeof(motion_wave));
	}	
	
	m_nPlayTime = 0;
	m_nDuration = 0;
	m_bLoadData = false;
	return 1;
}

// motion sample streaming
int IMotion_Playback::SendSample(int nSampleNo)
{
	int motionpos = nSampleNo * m_motion_data.format.nBlockAlign;
	if(motionpos == m_motion_data.motionpos)
		return 1;

	m_motion_data.motionpos = motionpos;

	if(m_motion_data.motionpos >= m_motion_data.motionlen) {
		m_motion_data.motionpos = 0;
		return 0;
	}

	uint8* stream = &m_motion_data.motion[m_motion_data.motionpos];		
	// send sample stream
	m_pDevice->SendStream(stream, m_motion_data.format.nBlockAlign);	
	// get motion source axes from mask info in profile
	pcm2pam((short*)stream, m_dPosition, m_profile.nMask);
	return 1;
}

// motion buffer streaming
int IMotion_Playback::SendBuffer()
{
	if(m_pSource == NULL)
		return 0;

	int samples = 4; // Number of samples per buffer
	int size = m_motion_data.format.nBlockAlign * samples;
	int len = m_motion_data.motionlen;
	int pos = m_motion_data.motionpos;
	if(pos >= len) return 0;
	
	IM_BUFFER buffer;	
	memset(&buffer, 0, sizeof(IM_BUFFER));	
	buffer.nMotionBytes = size;
	buffer.pMotionData = m_motion_data.motion + pos;
	buffer.nLoopCount = 0;	
	buffer.pContext = this;
		
	if(pos + size >= len) {
		buffer.nMotionBytes = len - pos;
		buffer.nFlags = IM_END_OF_STREAM;
		m_motion_data.motionpos = 0;
	}
	else
		m_motion_data.motionpos += size;
	
	// send buffer stream
	m_pSource->SubmitBuffer(&buffer);	
}

int IMotion_Playback::PlayMotion()
{
	if(m_bPlayData == true) 
		return 1;
	
	m_nPlayTime = 0;
	m_nPlayCount = m_nLoopCount + 1;

	if(m_nTest == 0) { // direct positioning (needs amplitude)
		short pcm[IM_DOF_COUNT];
		// get motion source axes from mask info in profile
		int channels = pam2pcm(m_dPosition, pcm, m_profile.nMask);
		m_pDevice->SendStream((unsigned char*)pcm, sizeof(short)*channels);
	}
	else if(m_nTest == 1) { // sine wave positioning (needs amplitude & frequency)
		if(m_nDuration > 0)
			m_bPlayData = true;
	}
	else if(m_nTest == 2) { // simple motion source play (needs motion file)
		IM_BUFFER buffer;	
		memset(&buffer, 0, sizeof(IM_BUFFER));
		buffer.nMotionBytes = m_motion_data.motionlen;
		buffer.pMotionData = m_motion_data.motion;
		buffer.nLoopCount = m_nLoopCount;
		buffer.pContext = this;
		
		m_pSource = m_pDevice->CreateSource(&m_motion_data.format, MotionCallback, 
			IM_END_OF_STREAM | IM_END_OF_BUFFER | IM_END_OF_LOOP, m_profile.nMask);
		m_pSource->SubmitBuffer(&buffer);					

		if(m_bLoadData) {
			m_pSource->Start();
			m_bPlayData = true;
		}
	}
	else if(m_nTest == 3) { // direct sample streaming (needs motion file)
		if(m_nDuration > 0)
			m_bPlayData = true;
		m_motion_data.motionpos = -1;
	}
	else if(m_nTest == 4) {	 // buffer streaming (needs motion file)
		m_pSource = m_pDevice->CreateSource(&m_motion_data.format, MotionCallback, 
			IM_END_OF_STREAM | IM_END_OF_BUFFER, m_profile.nMask, 4, 2); // 4 samples double buffer (for buffer streaming)
				
		if(m_bLoadData) {
			m_pSource->Start();
			m_bPlayData = true;
		}	

		// fillup double buffer
		m_motion_data.motionpos = 0;
		SendBuffer();		
		SendBuffer();
	}
	
	// save motion_data mask
	if(m_motion_data.motion && (m_nTest == 2 || m_nTest == 4))
		m_motion_data.mask = m_profile.nMask;
	return 1;
}

int IMotion_Playback::StopMotion()
{
	if(m_pSource) {
		m_pSource->Stop();
		m_pDevice->DestroySource(m_pSource);
		m_pSource = NULL;
	}

	// restore user amplitude
	memcpy(m_dPosition, m_dAmplitude, sizeof(m_dAmplitude));
	m_nPlayCount = m_nLoopCount + 1;
	m_bPlayData = false;	

	return 1;
}

int IMotion_Playback::Notify(unsigned int nState)
{	
	m_nPlayState = nState;
	if(m_nTest == 4) {
		// fillup buffer (buffer streaming)
		SendBuffer();
		return 1;
	}

	if(nState & IM_END_OF_BUFFER) {
		m_nPlayCount = 0;
	}
	if(nState & IM_END_OF_STREAM) {			
		m_nPlayCount = 0;
	}	
	if(nState & IM_END_OF_LOOP) {
		IM_BUFFER buffer;
		m_pSource->GetInfo(&buffer);
		m_nPlayCount = buffer.nLoopCount + 1;
	}
	return 1;
}

int IMotion_Playback::Update(unsigned int time)
{
	if(m_pDevice == NULL)
		return -1;
	
	if(m_nPlayCount == 0) {
		StopMotion();
	}
	
	// 1. send stream
	if(m_bPlayData) {	
		// direct positioning (sine wave)
		if(m_nTest == 1) {
			// complete check
			if(m_nLoopCount != IM_LOOP_INFINITE) {
				m_nLoopCount = (m_nDuration * m_nPlayCount - time)/m_nDuration;	
				if(time > (m_nDuration * m_nPlayCount)) {	
					time = m_nDuration * m_nPlayCount; // save timeout
					m_nLoopCount = m_nPlayCount-1; // restore count
					if((m_profile.nOptions & IM_CFG_ASYNC_MODE) || !IsBusy()) // complete sync
						m_nPlayCount = 0;
				}
			}
			// send stream
			bool enable = false;
			for(int i=0; i<IM_DOF_COUNT; i++) {
				if(m_profile.nMask & (1<<i) && m_dFrequency[i] > 0 && m_dAmplitude[i]) {
					m_dPosition[i] = m_dAmplitude[i] * sin(2 * IM_PI * m_dFrequency[i] * time * 0.001f);
					enable = true;
				}
			}
			if(enable) {
				short pcm[IM_DOF_COUNT] = {0,};
				// get motion source axes from mask info in profile
				int channels = pam2pcm(m_dPosition, pcm, m_profile.nMask);
				m_pDevice->SendStream((unsigned char*)pcm, sizeof(short)*channels);	
			}
			else
				enable = enable;
		}	
		// motion effect (update current info)
		else if(m_nTest == 2) {
			if(m_pSource) {
				IM_FORMAT format;
				m_pSource->GetFormat(&format);
				IM_BUFFER buffer;
				m_pSource->GetInfo(&buffer);
				// complete check
				m_nLoopCount = buffer.nLoopCount;
				if(buffer.pMotionData) {
					short* pcm = (short*)(buffer.pMotionData + buffer.nPlayBegin);
					// get motion source axes from mask info in profile
					pcm2pam(pcm, m_dPosition, m_profile.nMask);
				}
			}
		}
		// frame control (motion file)
		else if(m_nTest == 3) {
			// send stream
			// complete check
			if(!SendSample(m_nSamplePos++)) {
				m_nSamplePos = 0;
				if(m_nLoopCount != IM_LOOP_INFINITE) {
					--m_nPlayCount;
				}
			}
			m_nLoopCount = m_nPlayCount-1;
		}
		// motion stream (motion file)
		else if(m_nTest == 4) {
			// complete check
			if(m_nPlayState & IM_END_OF_STREAM) {
				m_nPlayState = 0;
				if(m_nLoopCount != IM_LOOP_INFINITE) {
					--m_nPlayCount;
				}	
			}
			m_nLoopCount = m_nPlayCount-1;
			if(m_pSource) {
				IM_BUFFER buffer;
				m_pSource->GetInfo(&buffer);

				if(buffer.pMotionData) {
					short* pcm = (short*)(buffer.pMotionData + buffer.nPlayBegin);
					// get motion source axes from mask info in profile
					pcm2pam(pcm, m_dPosition, m_profile.nMask);
				}
			}
		}
	}
	// update playtime (duration)
	else {		
		if(m_nTest == 1) {	
			// from user amplitude & frquency
			double frequency = 0;	
			for(int i=0; i<IM_DOF_COUNT; i++) {
				if(/*m_dFrequency[i] > 0 && */m_dPosition[i]) {
					MOTION_FRQ_MAX[i] = 1/m_dPosition[i];
					m_dFrequency[i] = MOTION_CLAMP(m_dFrequency[i], 0, MOTION_FRQ_MAX[i]);
					if(m_dFrequency[i])
						frequency = frequency ? MOTION_MIN(frequency, m_dFrequency[i]) : m_dFrequency[i];
				}
			}
			if(frequency) {
				memcpy(m_dAmplitude, m_dPosition, sizeof(m_dAmplitude));
				m_nDuration = 1000/frequency;
			}
			else
				m_nDuration = 0;
		}
		if(m_nTest >= 2) {	
			// from motion data
			if(m_bLoadData && m_nDuration == 0) {
				m_nDuration = (m_motion_data.motionlen / m_motion_data.format.nBlockAlign) * (1000 / m_motion_data.format.nSampleRate);
			}
		}
	}
	
	if(m_nLoopCount < 0)
		m_nLoopCount = 0;
	// 2. get diagnostics
	return m_pDevice->GetAxesInfo(m_info, m_nDevAxisCount);
}
