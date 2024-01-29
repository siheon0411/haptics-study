/********************************************************************************//**
\file      IMotion_TestbedDlg.cpp
\brief     IMotion Testbed implementation.
\copyright Copyright (C) 2016-2019 InnoSimulation Co., Ltd. All rights reserved.
************************************************************************************/

#include "stdafx.h"
#include "IMotion_Testbed.h"
#include "IMotion_TestbedDlg.h"
#include "afxdialogex.h"
#include "IMotion_Playback.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TIMER_MOTION_UPDATE				1002

static IMotion_Playback* m_pMotion = NULL;

int getTime2Str( char* strTime, long nTime )
{
	static long _msec, _sec, _min, _hour;
	_msec = _sec = _min = _hour = 0;
	
	_msec	 = nTime / 10;
	_sec	+= _msec / 100;
	_msec	%= 100;
	_min	+= _sec / 60;
	_sec	%= 60;
	_hour	+= _min / 60;
	_min	%= 60;

	sprintf(strTime, "%02d-%02d-%02d-%02d", _hour, _min, _sec, _msec );
	return 1;
}

long getStr2Time(const char* strTime )
{
	long nTime = 0;
	static long _msec, _sec, _min, _hour;
	sscanf(strTime, "%02d-%02d-%02d-%02d", &_hour, &_min, &_sec, &_msec );
	
	nTime = _hour;
	nTime = (nTime * 60) + _min;
	nTime = (nTime * 60) + _sec;
	nTime = (nTime * 1000) + (_msec*10);
	
	return nTime;
}

// Convert SYSTEMTIME to FILETIME
ULONGLONG getFileTime(SYSTEMTIME time)
{
	FILETIME filetime;
	ULARGE_INTEGER uliTime;
	ULONGLONG ullTime;

	SystemTimeToFileTime(&time, &filetime);
	uliTime.LowPart = filetime.dwLowDateTime;
	uliTime.HighPart = filetime.dwHighDateTime;
	ullTime = uliTime.QuadPart;	
	return ullTime;
}

ULONGLONG getDiffTime(SYSTEMTIME time1, SYSTEMTIME time2)
{	
	ULONGLONG ullTime1 = getFileTime(time1);
	ULONGLONG ullTime2 = getFileTime(time2);	
	return (ullTime2 - ullTime1) / 10000;
}

CString getCurrentPath()
{
	TCHAR path[_MAX_PATH];
	GetModuleFileName(NULL, path, sizeof path);
	CString strPath = path;
	int i = strPath.ReverseFind('\\');
	strPath = strPath.Left(i+1);
	return strPath;
}

int findLatestProfileID(CString strPath)
{
	unsigned int profile_id = IM_DEVICE_ID_DEFAULT;
	__time64_t LastWriteTime = 0;

	CFileFind finder;
	BOOL bWorking = finder.FindFile(strPath + _T("IMotion*.ini"));
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		CString profile_name = finder.GetFileName();
		CTime fileLastWriteTime; 
		finder.GetLastWriteTime(fileLastWriteTime);
				
		__time64_t WriteTime = fileLastWriteTime.GetTime();
		if(LastWriteTime == 0 || LastWriteTime < WriteTime) {
			LastWriteTime = WriteTime;
			sscanf((CStringA)profile_name.GetBuffer(0),"IMotion_%d.ini", &profile_id);
		}
	}
	return profile_id;
}

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};


CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	CString ver;
	ver = IM_VERSION_STRING;
	GetDlgItem(IDC_VERSION)->SetWindowText(ver);
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CIMotion_TestbedDlg dialog




CIMotion_TestbedDlg::CIMotion_TestbedDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CIMotion_TestbedDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);	
}

void CIMotion_TestbedDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}  
	else if((nID & 0xFFF0) == SC_CLOSE)
	{	
		EndDialog(IDCANCEL);
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CIMotion_TestbedDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CIMotion_TestbedDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL DDX_IsFocusDlgItem(CDataExchange* pDX, int nIDC)
{
	CWnd* focus = pDX->m_pDlgWnd->GetFocus();
	int nFocusID = 0;	
	if(focus)
		nFocusID = focus->GetDlgCtrlID();
	return (nFocusID == nIDC);
}

void DDX_Check_Bit(CDataExchange* pDX, int nIDC, unsigned int& value, int bits)
{
	BOOL enable;
	if (pDX->m_bSaveAndValidate == FALSE)
		enable = (value&bits)?TRUE:FALSE;
	
	DDX_Check(pDX, nIDC, enable);
	
	if (pDX->m_bSaveAndValidate)
	{
		if(enable)
			value |= bits;
		else
			value &= ~bits;
	}
}

void DDX_Enable(CDataExchange* pDX, int nIDC, BOOL enable)
{	
	if (pDX->m_bSaveAndValidate)
		return;
	pDX->m_pDlgWnd->GetDlgItem(nIDC)->EnableWindow(enable);
}

void DDX_Text_Time(CDataExchange* pDX, int nIDC, int& value)
{		
	CString temp;

	if (! pDX->m_bSaveAndValidate) {
		char strTime[1024];
		getTime2Str( strTime, value );
		temp = (LPSTR)strTime;
	}
			
	DDX_Text(pDX,nIDC,temp);

	if (pDX->m_bSaveAndValidate) {
		char strTime[1024];
		strcpy(strTime, (CStringA)temp.GetBuffer(0));
		value = getStr2Time(strTime);
	}
}

void DDX_Text_Double(CDataExchange* pDX, int nIDC, double& value)
{	
	if(DDX_IsFocusDlgItem(pDX, nIDC))
		return;

	DDX_Text(pDX,nIDC,value);
}	

void DDV_Clamp_Short(CDataExchange* pDX, int nIDC, short& value, short min, short max)
{
	if(DDX_IsFocusDlgItem(pDX, nIDC))
		return;

	if (pDX->m_bSaveAndValidate) 
		value = MOTION_CLAMP(value, min, max);	
}

void DDV_Clamp_Int(CDataExchange* pDX, int nIDC, int& value, int min, int max)
{
	if(DDX_IsFocusDlgItem(pDX, nIDC))
		return;

	if (pDX->m_bSaveAndValidate) 
		value = MOTION_CLAMP(value, min, max);	
}

void DDV_Clamp_Double(CDataExchange* pDX, int nIDC, double& value, double min, double max)
{
	if(DDX_IsFocusDlgItem(pDX, nIDC))
		return;

	if (pDX->m_bSaveAndValidate) 
		value = MOTION_CLAMP(value, min, max);	
}

static int IDC_CHECK_DOF[] = {IDC_CHECK_SURGE, IDC_CHECK_SWAY, IDC_CHECK_HEAVE, IDC_CHECK_ROLL, IDC_CHECK_PITCH, IDC_CHECK_YAW};
static int IDC_EDIT_AMP[] = {IDC_EDIT_AMP_SURGE, IDC_EDIT_AMP_SWAY, IDC_EDIT_AMP_HEAVE, IDC_EDIT_AMP_ROLL, IDC_EDIT_AMP_PITCH, IDC_EDIT_AMP_YAW};
static int IDC_EDIT_FREQ[] = {IDC_EDIT_FREQ_SURGE, IDC_EDIT_FREQ_SWAY, IDC_EDIT_FREQ_HEAVE, IDC_EDIT_FREQ_ROLL, IDC_EDIT_FREQ_PITCH, IDC_EDIT_FREQ_YAW};
static int IDC_BUTTON_SERVO[] = {IDC_BUTTON_SERVO1, IDC_BUTTON_SERVO2, IDC_BUTTON_SERVO3, IDC_BUTTON_SERVO4, IDC_BUTTON_SERVO5, IDC_BUTTON_SERVO6};
static int IDC_BUTTON_ALARM[] = {IDC_BUTTON_ALARM1, IDC_BUTTON_ALARM2, IDC_BUTTON_ALARM3, IDC_BUTTON_ALARM4, IDC_BUTTON_ALARM5, IDC_BUTTON_ALARM6};
static int IDC_STATIC_BUSY[] = {IDC_STATIC_BUSY1, IDC_STATIC_BUSY2, IDC_STATIC_BUSY3, IDC_STATIC_BUSY4, IDC_STATIC_BUSY5, IDC_STATIC_BUSY6};
static int IDC_STATIC_HOME[] = {IDC_STATIC_HOME1, IDC_STATIC_HOME2, IDC_STATIC_HOME3, IDC_STATIC_HOME4, IDC_STATIC_HOME5, IDC_STATIC_HOME6};
static int IDC_STATIC_INPOS[] = {IDC_STATIC_INPOS1, IDC_STATIC_INPOS2, IDC_STATIC_INPOS3, IDC_STATIC_INPOS4, IDC_STATIC_INPOS5, IDC_STATIC_INPOS6};
static int IDC_STATIC_CMD[] = {IDC_STATIC_CMD1, IDC_STATIC_CMD2, IDC_STATIC_CMD3, IDC_STATIC_CMD4, IDC_STATIC_CMD5, IDC_STATIC_CMD6};
static int IDC_STATIC_ENC[] = {IDC_STATIC_ENC1, IDC_STATIC_ENC2, IDC_STATIC_ENC3, IDC_STATIC_ENC4, IDC_STATIC_ENC5, IDC_STATIC_ENC6};
static int IDC_STATIC_EMR[] = {IDC_STATIC_EMR1, IDC_STATIC_EMR2, IDC_STATIC_EMR3, IDC_STATIC_EMR4, IDC_STATIC_EMR5, IDC_STATIC_EMR6};
static int IDC_COMBO_AXIS[] = {IDC_COMBO_AXIS1, IDC_COMBO_AXIS2, IDC_COMBO_AXIS3, IDC_COMBO_AXIS4, IDC_COMBO_AXIS5, IDC_COMBO_AXIS6};

void CIMotion_TestbedDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	
	/* profile */
	// enable
	DDX_Enable(pDX, IDC_EDIT_EQUIP, m_nState == MOTION_CLOSE);
	DDX_Enable(pDX, IDC_EDIT_IPADDRESS, m_nState == MOTION_CLOSE);
	DDX_Enable(pDX, IDC_EDIT_SLAVE, m_nState == MOTION_CLOSE);	
	DDX_Enable(pDX, IDC_COMBO_NAME, m_nState == MOTION_CLOSE);
	DDX_Enable(pDX, IDC_COMBO_FILTER, m_nState == MOTION_CLOSE);
	DDX_Enable(pDX, IDC_EDIT_VERSION, m_nState == MOTION_CLOSE);
	DDX_Enable(pDX, IDC_CHECK_DEBUG, m_nState == MOTION_CLOSE);
	DDX_Enable(pDX, IDC_CHECK_FILE_LOG, m_nState == MOTION_CLOSE);
	DDX_Enable(pDX, IDC_CHECK_EMUL, m_nState == MOTION_CLOSE);
	DDX_Enable(pDX, IDC_CHECK_ASYNC, m_nState == MOTION_CLOSE);
	DDX_Enable(pDX, IDC_EDIT_MASK, m_nState == MOTION_CLOSE);	
	DDX_Enable(pDX, IDC_BUTTON_STOP, m_nState != MOTION_CLOSE && m_nState != MOTION_STOP);
	DDX_Enable(pDX, IDC_BUTTON_START, m_nState != MOTION_CLOSE && m_nState != MOTION_START && m_nState != MOTION_OPEN);	
	DDX_Enable(pDX, IDC_BUTTON_OPERATION, m_nState >= MOTION_START && (m_pMotion->m_nTest == 0 || m_pMotion->m_nDuration > 0));
	DDX_Enable(pDX, IDC_EDIT_RATE_LIMIT, m_nState == MOTION_CLOSE);
	// update
	DDX_Text(pDX, IDC_EDIT_EQUIP, m_nDevID);
	DDX_Text(pDX, IDC_EDIT_SLAVE, m_nSlaveID);	
	DDX_CBIndex(pDX, IDC_COMBO_NAME, m_pMotion->m_nDevIndex);
	DDX_CBIndex(pDX, IDC_COMBO_FILTER, m_pMotion->m_nFltIndex);
	DDX_Text(pDX, IDC_EDIT_VERSION, m_pMotion->m_profile.nVersion);
	DDX_Check_Bit(pDX, IDC_CHECK_DEBUG, m_pMotion->m_profile.nOptions, IM_CFG_DEBUG_MODE);
	DDX_Check_Bit(pDX, IDC_CHECK_FILE_LOG, m_pMotion->m_profile.nOptions, IM_CFG_FILE_LOG);
	DDX_Check_Bit(pDX, IDC_CHECK_EMUL, m_pMotion->m_profile.nOptions, IM_CFG_EMUL_MODE);
	DDX_Check_Bit(pDX, IDC_CHECK_ASYNC, m_pMotion->m_profile.nOptions, IM_CFG_ASYNC_MODE);	
	DDX_Text(pDX, IDC_EDIT_MASK, m_pMotion->m_profile.nMask);
	DDX_Text(pDX, IDC_EDIT_RATE_LIMIT, (short&)m_pMotion->m_profile.nRateLimit);
	
	// clamp
	DDV_Clamp_Int(pDX, IDC_EDIT_EQUIP, m_nDevID, 0, 255);
	DDV_Clamp_Int(pDX, IDC_EDIT_SLAVE, m_nSlaveID, 0, 255);	
	DDV_Clamp_Int(pDX, IDC_EDIT_VERSION, (int&)m_pMotion->m_profile.nVersion, 0, m_pMotion->m_desc[m_pMotion->m_nDevIndex].nVersion);
	DDV_Clamp_Int(pDX, IDC_EDIT_MASK, (int&)m_pMotion->m_profile.nMask, 0, 0xFF);
	DDV_Clamp_Short(pDX, IDC_EDIT_RATE_LIMIT, (short&)m_pMotion->m_profile.nRateLimit, 0, 1024);
		
	/* operation */
	// enable
	DDX_Enable(pDX, IDC_COMBO_TEST, m_nState >= MOTION_START && !m_pMotion->m_bPlayData);	
	DDX_Enable(pDX, IDC_EDIT_SAMPLE_RATE, m_nState != MOTION_CLOSE && (!m_pMotion->m_bPlayData && m_pMotion->m_nTest && m_pMotion->m_nDuration > 0));
	DDX_Enable(pDX, IDC_CHECK_LOOP, m_nState != MOTION_CLOSE && (!m_pMotion->m_bPlayData && m_pMotion->m_nTest && m_pMotion->m_nDuration > 0));
	DDX_Enable(pDX, IDC_EDIT_LOOP_COUNT, m_nState != MOTION_CLOSE && (!m_pMotion->m_bPlayData && m_pMotion->m_nTest && m_pMotion->m_nDuration > 0));	
	DDX_Enable(pDX, IDC_BUTTON_LOAD_MOTION_DATA, m_nState >= MOTION_START && m_pMotion->m_nTest >= 2 && !m_pMotion->m_bPlayData);
	DDX_Enable(pDX, IDC_EDIT_MOTION_DATA_TITLE, m_nState != MOTION_CLOSE && m_pMotion->m_nTest >= 2 && m_pMotion->m_bLoadData);
	DDX_Enable(pDX, IDC_STATIC_TIME_CURRENT, m_nState >= MOTION_START);
	DDX_Enable(pDX, IDC_STATIC_TIME_TOTAL, m_nState >= MOTION_START);
	// update
	DDX_CBIndex(pDX, IDC_COMBO_TEST, m_pMotion->m_nTest);	
	DDX_Text(pDX, IDC_EDIT_SAMPLE_RATE, m_pMotion->m_nSampleRate);	
	DDX_Text(pDX, IDC_EDIT_LOOP_COUNT, m_pMotion->m_nLoopCount);
	DDX_Text_Time(pDX, IDC_STATIC_TIME_CURRENT, m_pMotion->m_nPlayTime);
	DDX_Text_Time(pDX, IDC_STATIC_TIME_TOTAL, m_pMotion->m_nDuration);
	DDX_Text(pDX, IDC_EDIT_MOTION_DATA_TITLE, m_staticUrl);
	DDX_Control(pDX, IDC_EDIT_LOG, m_editLog);	
	// clamp
	DDV_Clamp_Int(pDX, IDC_EDIT_SAMPLE_RATE, m_pMotion->m_nSampleRate, 25, 200);
	DDV_Clamp_Int(pDX, IDC_EDIT_LOOP_COUNT, m_pMotion->m_nLoopCount, 0, 255);
			
	// amplitude & frequency
	for(int i=0; i<IM_DOF_COUNT; i++) {
		int dof_enable = m_pMotion->m_profile.nMask & IM_BIT_DOF(i) ? 1 : 0;
		DDX_Enable(pDX, IDC_CHECK_DOF[i], m_nState == MOTION_CLOSE || (m_nState >= MOTION_START && (m_pMotion->m_nTest == 2 || m_pMotion->m_nTest == 4) && !m_pMotion->m_bPlayData));
		DDX_Check_Bit(pDX, IDC_CHECK_DOF[i], m_pMotion->m_profile.nMask, IM_BIT_DOF(i));
						
		DDX_Enable(pDX, IDC_EDIT_AMP[i], dof_enable && m_nState >= MOTION_START && !m_pMotion->m_bPlayData && (m_pMotion->m_nTest == 0 || m_pMotion->m_nTest == 1));
		DDX_Enable(pDX, IDC_EDIT_FREQ[i], dof_enable && m_nState >= MOTION_START && !m_pMotion->m_bPlayData && m_pMotion->m_nTest == 1);	

		DDX_Text_Double(pDX, IDC_EDIT_AMP[i], m_pMotion->m_dPosition[i]);
		DDX_Text_Double(pDX, IDC_EDIT_FREQ[i], m_pMotion->m_dFrequency[i]);
		DDV_Clamp_Double(pDX, IDC_EDIT_AMP[i], m_pMotion->m_dPosition[i], -MOTION_AMP_MAX[i], MOTION_AMP_MAX[i]);
	}
		
	// axis map
	if (pDX->m_bSaveAndValidate == FALSE) {
		int infinite_loop = (m_pMotion->m_nLoopCount == IM_LOOP_INFINITE);
		DDX_Check(pDX, IDC_CHECK_LOOP, infinite_loop);
		
		for(int i=0; i<IM_DOF_COUNT; i++) {
			CComboBox* cbAxis = (CComboBox*)GetDlgItem(IDC_COMBO_AXIS[i]);					
			int axis = ((m_pMotion->m_profile.nAxisMap >> 4*i) & 0xF);
			axis = axis ? MOTION_INT_U2S(axis,4) : i;
			axis += m_pMotion->m_nDevAxisCount;
			cbAxis->SetCurSel(axis);
		}
	}
	else {
		m_pMotion->m_profile.nAxisMap = 0;
		for(int i=0; i<IM_DOF_COUNT; i++) {
			CComboBox* cbAxis = (CComboBox*)GetDlgItem(IDC_COMBO_AXIS[i]);
			int axis = cbAxis->GetCurSel();
			if(axis >= 0) {
				axis -= m_pMotion->m_nDevAxisCount;
				axis = MOTION_INT_S2U(axis,4);
			}
			else
				axis = i;
			m_pMotion->m_profile.nAxisMap |= (axis << 4*i);
		}
	}

	/* diagnostics */
	for(int i=0; i<IM_DOF_COUNT; i++) {		
		bool axis_enable = (i < m_pMotion->m_nDevAxisCount);
		bool settled = (m_nState == MOTION_OPEN) || (m_nState == MOTION_STOP && !m_pMotion->IsBusy());	

		DDX_Enable(pDX, IDC_COMBO_AXIS[i], axis_enable && (m_nState == MOTION_CLOSE));		
		DDX_Enable(pDX, IDC_BUTTON_SERVO[i], axis_enable && ((m_nState == MOTION_OPEN) || (m_nState == MOTION_STOP && !m_pMotion->IsBusy())));
		DDX_Enable(pDX, IDC_BUTTON_ALARM[i], axis_enable && m_pMotion->m_info[i].bAlarm);
		
		// update
		if(axis_enable) {
			DDX_Text(pDX, IDC_BUTTON_SERVO[i],	m_pMotion->m_info[i].bServoOn);
			DDX_Text(pDX, IDC_BUTTON_ALARM[i],	m_pMotion->m_info[i].bAlarm);
			DDX_Text(pDX, IDC_STATIC_BUSY[i],	m_pMotion->m_info[i].bBusy);	
			DDX_Text(pDX, IDC_STATIC_HOME[i],	m_pMotion->m_info[i].bHome);
			DDX_Text(pDX, IDC_STATIC_INPOS[i],	m_pMotion->m_info[i].bInpos);
			DDX_Text(pDX, IDC_STATIC_CMD[i],	m_pMotion->m_info[i].dCmd);
			DDX_Text(pDX, IDC_STATIC_ENC[i],	m_pMotion->m_info[i].dEnc);
			DDX_Text(pDX, IDC_STATIC_EMR[i],	m_pMotion->m_info[i].bEmer);
		}
		else {
			CString null_str;
			DDX_Text(pDX, IDC_BUTTON_SERVO[i],	null_str);
			DDX_Text(pDX, IDC_BUTTON_ALARM[i],	null_str);
			DDX_Text(pDX, IDC_STATIC_BUSY[i],	null_str);
			DDX_Text(pDX, IDC_STATIC_HOME[i],	null_str);
			DDX_Text(pDX, IDC_STATIC_INPOS[i],	null_str);
			DDX_Text(pDX, IDC_STATIC_CMD[i],	null_str);
			DDX_Text(pDX, IDC_STATIC_ENC[i],	null_str);
			DDX_Text(pDX, IDC_STATIC_EMR[i],	null_str);
		}
	}	

	DDX_Enable(pDX, IDC_BUTTON_ENCODER_RESET, m_nState != MOTION_CLOSE);
	DDX_Text(pDX, IDC_STATIC_STATUS, m_staticStatus);
}

BEGIN_MESSAGE_MAP(CIMotion_TestbedDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CIMotion_TestbedDlg::OnBnClickedButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CIMotion_TestbedDlg::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_START, &CIMotion_TestbedDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_OPERATION, &CIMotion_TestbedDlg::OnBnClickedButtonOperation)
	ON_BN_CLICKED(IDC_BUTTON_SERVO1, &CIMotion_TestbedDlg::OnBnClickedButtonServo1)
	ON_BN_CLICKED(IDC_BUTTON_SERVO2, &CIMotion_TestbedDlg::OnBnClickedButtonServo2)
	ON_BN_CLICKED(IDC_BUTTON_SERVO3, &CIMotion_TestbedDlg::OnBnClickedButtonServo3)
	ON_BN_CLICKED(IDC_BUTTON_SERVO4, &CIMotion_TestbedDlg::OnBnClickedButtonServo4)
	ON_BN_CLICKED(IDC_BUTTON_SERVO5, &CIMotion_TestbedDlg::OnBnClickedButtonServo5)
	ON_BN_CLICKED(IDC_BUTTON_SERVO6, &CIMotion_TestbedDlg::OnBnClickedButtonServo6)
	ON_BN_CLICKED(IDC_BUTTON_ALARM1, &CIMotion_TestbedDlg::OnBnClickedButtonAlarm1)
	ON_BN_CLICKED(IDC_BUTTON_ALARM2, &CIMotion_TestbedDlg::OnBnClickedButtonAlarm2)
	ON_BN_CLICKED(IDC_BUTTON_ALARM3, &CIMotion_TestbedDlg::OnBnClickedButtonAlarm3)
	ON_BN_CLICKED(IDC_BUTTON_ALARM4, &CIMotion_TestbedDlg::OnBnClickedButtonAlarm4)
	ON_BN_CLICKED(IDC_BUTTON_ALARM5, &CIMotion_TestbedDlg::OnBnClickedButtonAlarm5)
	ON_BN_CLICKED(IDC_BUTTON_ALARM6, &CIMotion_TestbedDlg::OnBnClickedButtonAlarm6)
	ON_BN_CLICKED(IDC_BUTTON_ENCODER_RESET, &CIMotion_TestbedDlg::OnBnClickedButtonEncoderReset)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_MOTION_DATA, &CIMotion_TestbedDlg::OnBnClickedButtonLoadMotionData)
	ON_CBN_SELCHANGE(IDC_COMBO_NAME, &CIMotion_TestbedDlg::OnSelchangeComboName)
	ON_CBN_SELCHANGE(IDC_COMBO_TEST, &CIMotion_TestbedDlg::OnSelchangeComboTest)
	ON_BN_CLICKED(IDC_CHECK_LOOP, &CIMotion_TestbedDlg::OnBnClickedCheckLoop)
	ON_BN_CLICKED(IDC_CHECK_SURGE, &CIMotion_TestbedDlg::OnBnClickedCheckSurge)
	ON_BN_CLICKED(IDC_CHECK_SWAY, &CIMotion_TestbedDlg::OnBnClickedCheckSway)
	ON_BN_CLICKED(IDC_CHECK_HEAVE, &CIMotion_TestbedDlg::OnBnClickedCheckHeave)
	ON_BN_CLICKED(IDC_CHECK_ROLL, &CIMotion_TestbedDlg::OnBnClickedCheckRoll)
	ON_BN_CLICKED(IDC_CHECK_PITCH, &CIMotion_TestbedDlg::OnBnClickedCheckPitch)
	ON_BN_CLICKED(IDC_CHECK_YAW, &CIMotion_TestbedDlg::OnBnClickedCheckYaw)
	ON_EN_SETFOCUS(IDC_EDIT_FREQ_SURGE, &CIMotion_TestbedDlg::OnSetfocusEditFreqSurge)
	ON_EN_SETFOCUS(IDC_EDIT_FREQ_SWAY, &CIMotion_TestbedDlg::OnSetfocusEditFreqSway)
	ON_EN_SETFOCUS(IDC_EDIT_FREQ_HEAVE, &CIMotion_TestbedDlg::OnSetfocusEditFreqHeave)
	ON_EN_SETFOCUS(IDC_EDIT_FREQ_ROLL, &CIMotion_TestbedDlg::OnSetfocusEditFreqRoll)
	ON_EN_SETFOCUS(IDC_EDIT_FREQ_PITCH, &CIMotion_TestbedDlg::OnSetfocusEditFreqPitch)
	ON_EN_SETFOCUS(IDC_EDIT_FREQ_YAW, &CIMotion_TestbedDlg::OnSetfocusEditFreqYaw)
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(IDC_EDIT_EQUIP, &CIMotion_TestbedDlg::OnEnChangeEditEquip)
END_MESSAGE_MAP()

BOOL CIMotion_TestbedDlg::OnInitDialog()
{
	Initialize();

	// dialog init
	CDialogEx::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);	
	
	return TRUE;
}

void CIMotion_TestbedDlg::OnDestroy()
{
	CDialogEx::OnDestroy();		
	Cleanup();
}

void CIMotion_TestbedDlg::OnBnClickedButtonOpen()
{	
	UpdateData(TRUE);
	
	if(m_nState == MOTION_CLOSE)
	{		
		m_nError = m_pMotion->Open(m_nDevID, m_nSlaveID);
		m_nState = MOTION_OPEN;
		if(m_nError == IM_DISCONNECTED) {
			m_bConnected = FALSE;
			if(IDYES != AfxMessageBox(L"Connection failed... \n\nContinue in emulation mode ?", MB_YESNO)) {
				m_nError = m_pMotion->Close();
				m_nState = MOTION_CLOSE;
			}
		}
		else {
			m_bConnected = TRUE;
		}
		
		if(m_nState == MOTION_OPEN)
			SetTimer(TIMER_MOTION_UPDATE, 1000/m_pMotion->m_nSampleRate, NULL);	
	}	
	else
	{			
		m_nError = m_pMotion->Close();
		m_nState = MOTION_CLOSE;
		m_bConnected = FALSE;
		m_editLog.SetWindowText(_T(""));
		m_staticUrl = _T("");

		KillTimer(TIMER_MOTION_UPDATE);
		
	}
	Update();	
	UpdateData(FALSE);
}

void CIMotion_TestbedDlg::OnBnClickedButtonStop()
{
	UpdateData(TRUE);

	if(m_nState == MOTION_CLOSE)
		return;	

	m_pMotion->StopMotion();
	m_pMotion->Unload();	
	m_pMotion->Stop();
	m_nState = MOTION_STOP;
	m_staticUrl = _T("");

	UpdateData(FALSE);
}

void CIMotion_TestbedDlg::OnBnClickedButtonStart()
{
	UpdateData(TRUE);
  
	if(m_nState == MOTION_CLOSE)
		return;
	
	m_pMotion->StopMotion();	
	m_pMotion->Start();
	m_nState = MOTION_START;
	
	UpdateData(FALSE);
}

void CIMotion_TestbedDlg::OnBnClickedButtonOperation()
{
	UpdateData(TRUE);
	 
	if(m_nState == MOTION_CLOSE)
		return;
	
	if(m_pMotion->m_bPlayData) {
		m_pMotion->StopMotion();
	}
	else {
		m_pMotion->PlayMotion();
		
		if(m_pMotion->m_nTest) {		
			GetLocalTime(&m_startTime);
		}	
	}

	m_nState = MOTION_RUNNING;
	UpdateData(FALSE);
}

void CIMotion_TestbedDlg::OnBnClickedButtonLoadMotionData()
{	
	CString filter("Image (*.CSV, *.WAV) | *.CSV;*.WAV | All Files(*.*)|*.*||");
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, filter);
	if(IDOK == dlg.DoModal())
	{
		m_staticUrl = dlg.GetPathName();	
		m_pMotion->Load((CStringA)m_staticUrl, NULL);
		UpdateData(FALSE);
	}
}

int CIMotion_TestbedDlg::SetCheck(int dof)
{
	UpdateData(TRUE);	
	if(IsDlgButtonChecked(IDC_CHECK_DOF[dof]))
		m_pMotion->m_profile.nMask |= IM_BIT_DOF(dof);
	else
		m_pMotion->m_profile.nMask &= ~IM_BIT_DOF(dof);
	UpdateData(FALSE);
	return 1;
}

int CIMotion_TestbedDlg::SetMaxFrequency(int dof)
{
	UpdateData(TRUE); 
	double amplitude = m_pMotion->m_dPosition[dof];
	if(amplitude) {
		m_pMotion->m_dFrequency[dof] = 1/amplitude;
		Update();

		CString strFreq;
		strFreq.Format(_T("%d"), (int)m_pMotion->m_dFrequency[dof]);
		SetDlgItemText(IDC_EDIT_FREQ[dof], strFreq);
	}
	UpdateData(FALSE); 
	return 1;
}

void CIMotion_TestbedDlg::OnBnClickedButtonServo1() { m_pMotion->SetServo(0); }
void CIMotion_TestbedDlg::OnBnClickedButtonServo2() { m_pMotion->SetServo(1); }
void CIMotion_TestbedDlg::OnBnClickedButtonServo3() { m_pMotion->SetServo(2); }
void CIMotion_TestbedDlg::OnBnClickedButtonServo4() { m_pMotion->SetServo(3); }
void CIMotion_TestbedDlg::OnBnClickedButtonServo5() { m_pMotion->SetServo(4); }
void CIMotion_TestbedDlg::OnBnClickedButtonServo6() { m_pMotion->SetServo(5); }
void CIMotion_TestbedDlg::OnBnClickedButtonAlarm1() { m_pMotion->AlarmReset(0); }
void CIMotion_TestbedDlg::OnBnClickedButtonAlarm2() { m_pMotion->AlarmReset(1); }
void CIMotion_TestbedDlg::OnBnClickedButtonAlarm3() { m_pMotion->AlarmReset(2); }
void CIMotion_TestbedDlg::OnBnClickedButtonAlarm4() { m_pMotion->AlarmReset(3); }
void CIMotion_TestbedDlg::OnBnClickedButtonAlarm5() { m_pMotion->AlarmReset(4); }
void CIMotion_TestbedDlg::OnBnClickedButtonAlarm6() { m_pMotion->AlarmReset(5); }
void CIMotion_TestbedDlg::OnBnClickedCheckSurge()	{ SetCheck(IM_DOF_SURGE); }
void CIMotion_TestbedDlg::OnBnClickedCheckSway()	{ SetCheck(IM_DOF_SWAY); }
void CIMotion_TestbedDlg::OnBnClickedCheckHeave()	{ SetCheck(IM_DOF_HEAVE); }
void CIMotion_TestbedDlg::OnBnClickedCheckRoll()	{ SetCheck(IM_DOF_ROLL); }
void CIMotion_TestbedDlg::OnBnClickedCheckPitch()	{ SetCheck(IM_DOF_PITCH); }
void CIMotion_TestbedDlg::OnBnClickedCheckYaw()		{ SetCheck(IM_DOF_YAW); }
void CIMotion_TestbedDlg::OnSetfocusEditFreqSurge() { SetMaxFrequency(IM_DOF_SURGE); }
void CIMotion_TestbedDlg::OnSetfocusEditFreqSway()	{ SetMaxFrequency(IM_DOF_SWAY); }
void CIMotion_TestbedDlg::OnSetfocusEditFreqHeave() { SetMaxFrequency(IM_DOF_HEAVE); }
void CIMotion_TestbedDlg::OnSetfocusEditFreqRoll()	{ SetMaxFrequency(IM_DOF_ROLL); }
void CIMotion_TestbedDlg::OnSetfocusEditFreqPitch() { SetMaxFrequency(IM_DOF_PITCH); }
void CIMotion_TestbedDlg::OnSetfocusEditFreqYaw()	{ SetMaxFrequency(IM_DOF_YAW); }

void CIMotion_TestbedDlg::OnEnChangeEditEquip()
{
	UpdateData(TRUE);
	m_pMotion->GetProfile(m_nDevID);
	m_nSlaveID = 0;
	Update();
	UpdateAxisMap();
	UpdateData(FALSE);
}

void CIMotion_TestbedDlg::OnSelchangeComboName()
{
	UpdateData(TRUE);
	m_pMotion->SetProfile(m_nDevID, m_pMotion->m_nDevIndex);
	// host url
	CEdit* ipaddress = (CEdit*)GetDlgItem(IDC_EDIT_IPADDRESS);
	ipaddress->SetWindowText((CString)m_pMotion->m_profile.szIPAddress);
	
	Update();
	UpdateAxisMap();
	UpdateData(FALSE);
}

void CIMotion_TestbedDlg::OnSelchangeComboTest()
{
	UpdateData(TRUE);
	m_pMotion->StopMotion();
	// move neutral
	m_pMotion->Start();
	m_nState = MOTION_START;

	UpdateData(FALSE);
}

void CIMotion_TestbedDlg::OnBnClickedCheckLoop()
{
	UpdateData(TRUE);	
	if(IsDlgButtonChecked(IDC_CHECK_LOOP))
		m_pMotion->m_nLoopCount = IM_LOOP_INFINITE;
	else
		m_pMotion->m_nLoopCount = 0;
	UpdateData(FALSE);
}

void CIMotion_TestbedDlg::OnBnClickedButtonEncoderReset()
{	
	UpdateData(TRUE);
   
	if(m_nState == MOTION_CLOSE)
		return;	
		
	m_pMotion->Close(IM_DEVICE_MOTOR_POWER_OFF);
	m_nState = MOTION_CLOSE;
	m_bConnected = FALSE;
	m_editLog.SetWindowText(_T(""));
		
	UpdateData(FALSE);
}

void CIMotion_TestbedDlg::LogOutput(unsigned int devid, const char* msg, void* param)
{	
	WaitForSingleObject(m_hMutex, INFINITE);
	CString Line;
	Line = (LPSTR)msg;
	m_strLogTemp += Line;
	m_strLogTemp += "\r\n";
	ReleaseMutex(m_hMutex);
}

void CIMotion_TestbedDlg::Initialize()
{ 
	m_pMotion = new IMotion_Playback;
		
	CString strPath = getCurrentPath();
	m_nDevID = findLatestProfileID(strPath);
	m_nSlaveID = 0;
	m_pMotion->GetProfile(m_nDevID);
	
	UpdateAxisMap();

	m_color[0] = RGB(0, 255, 0);	// normal
	m_color[1] = RGB(255, 0, 0);	// error
	m_color[2] = RGB(255, 255, 0);	// busy
	for(int i=0; i<3; i++)
		m_hbr[i] = ::CreateSolidBrush(m_color[i]);
		
	// description
	CComboBox* cbName = (CComboBox*)GetDlgItem(IDC_COMBO_NAME);
	for(int i=0; i<m_pMotion->m_nDevCount; i++) {
		cbName->AddString((CString)m_pMotion->m_desc[i].szName);
	}
	cbName->SetCurSel(m_pMotion->m_nDevIndex);
		
	// filter
	CComboBox* cbFilter = (CComboBox*)GetDlgItem(IDC_COMBO_FILTER);
	for(int i=0; i<m_pMotion->m_nFltCount; i++) {
		cbFilter->AddString((CString)m_pMotion->m_filter[i].szName);
	}
	cbFilter->SetCurSel(m_pMotion->m_nFltIndex);

	// test
	CComboBox* cbTest = (CComboBox*)GetDlgItem(IDC_COMBO_TEST);
	for(int i=0, n=sizeof(motion_samples)/sizeof(motion_sample_name); i<n; i++) {
		cbTest->AddString((CString)motion_samples[i].name);
	}
	cbTest->SetCurSel(m_pMotion->m_nTest);
	
	// logger
	struct wrapper {
		static void logger_wrapper(unsigned int devid, const char* msg, void* worker) {
			return ((CIMotion_TestbedDlg*)worker)->LogOutput(devid, msg, worker);
		}
	};
	m_pMotion->SetLogger(wrapper::logger_wrapper, (void*)this);
	m_hMutex = CreateMutex (NULL, FALSE, NULL);

	CEdit* ipaddress = (CEdit*)GetDlgItem(IDC_EDIT_IPADDRESS);
	ipaddress->SetWindowText((CString)m_pMotion->m_profile.szIPAddress);
	ipaddress->SetReadOnly(TRUE);

	m_nState = MOTION_CLOSE;
	m_bBusy = FALSE;
	m_nError = IM_OK;
	m_bConnected = FALSE;
	m_strLogTemp = _T("");
	m_staticStatus = _T("DISCONNECTED");
}

void CIMotion_TestbedDlg::Cleanup()
{ 
	KillTimer(TIMER_MOTION_UPDATE);
	if(m_hMutex) {
		CloseHandle(m_hMutex);
		m_hMutex = 0;
	}	
	if(m_pMotion) {
		m_pMotion->Close();
		delete m_pMotion;
		m_pMotion = NULL;
	}
	for(int i=0; i<3; i++)
		::DeleteObject(m_hbr[i]);
}

void CIMotion_TestbedDlg::Update()
{
	// timer
	if(m_pMotion->m_bPlayData) 
	{
		SYSTEMTIME currentTime;
		GetLocalTime(&currentTime);	
		m_pMotion->m_nPlayTime = getDiffTime(m_startTime, currentTime);
	}

	// motion
	m_nError = m_pMotion->Update(m_pMotion->m_nPlayTime);
	m_bConnected = (m_nError != IM_DISCONNECTED);

	// logger
	WaitForSingleObject(m_hMutex, INFINITE);
	int count = m_strLogTemp.GetLength();
	if(count > 0) {
		int len = m_editLog.GetWindowTextLength();
		int max = m_editLog.GetLimitText();
		if(len + count >= max) {
			CString str;
			m_editLog.GetWindowText(str);
			while(len + count >= max) {
				int pos = str.Find(_T("\r\n"));
				str.Delete(0, pos+2);
				len -= (pos+2);
			}
			m_editLog.SetWindowText(str);
		}
		m_editLog.SetSel(len, len);
		m_editLog.ReplaceSel(m_strLogTemp); // append
		m_strLogTemp = _T("");
	}
	ReleaseMutex(m_hMutex);	
	
	// status
	if(m_pMotion->m_nTest) {
		if(m_pMotion->m_bPlayData)
			SetDlgItemText(IDC_BUTTON_OPERATION, _T("Stop"));
		else
			SetDlgItemText(IDC_BUTTON_OPERATION, _T("Play"));
	}
	else
		SetDlgItemText(IDC_BUTTON_OPERATION, _T("Positioning"));
	
	m_bBusy = m_pMotion->IsBusy();
	if(!m_bBusy)
	{		
		if(m_nState == MOTION_OPEN) { // 1. open -> stop
			SetDlgItemText(IDC_BUTTON_OPEN, _T("Close"));	
			SetDlgItemText(IDC_BUTTON_STOP, _T("Stop"));
			SetDlgItemText(IDC_BUTTON_START, _T("Start"));
			m_staticStatus = _T("CONNECTED");
		}
		else if(m_nState == MOTION_CLOSE) {
			SetDlgItemText(IDC_BUTTON_OPEN, _T("Open"));
			SetDlgItemText(IDC_BUTTON_STOP, _T("Stop"));
			SetDlgItemText(IDC_BUTTON_START, _T("Start"));
			m_staticStatus = _T("DISCONNECTED");
		}
		else if(m_nState == MOTION_STOP) { // 2. stop -> settle	
			SetDlgItemText(IDC_BUTTON_STOP, _T("Stopped"));
			SetDlgItemText(IDC_BUTTON_START, _T("Start"));
			m_staticStatus = _T("SETTLED");
		}
		else if(m_nState == MOTION_START) { // 3. start -> neutral
			SetDlgItemText(IDC_BUTTON_STOP, _T("Stop"));
			SetDlgItemText(IDC_BUTTON_START, _T("Running"));
			m_staticStatus = _T("NEUTRAL");
		}
		else if(m_nState == MOTION_RUNNING) { // 4. neutral -> running
			SetDlgItemText(IDC_BUTTON_START, _T("Start"));
			m_staticStatus = _T("OPERATING...");
		}
	}
	else
		m_staticStatus = _T("WAITING...");

	if(m_bConnected == FALSE)
		m_staticStatus = _T("DISCONNECTED");
	else if(m_nError) {	
		m_staticStatus.Format(_T("Error %d :"), m_nError);
		if(m_nError == IM_ERROR_EMER)
			m_staticStatus += _T(" \"Emergency\"");
		if(m_nError == IM_ERROR_ALARM)
			m_staticStatus += _T(" \"Alarm\"");
		if(m_nError == IM_ERROR_SERVO)
			m_staticStatus += _T(" \"Servo\"");
	}
}

void CIMotion_TestbedDlg::UpdateAxisMap()
{	
	struct motion_axis_name {
		const char* name;
	} motion_axes[] = {
		"-8", "-7", "-6", "-5", "-4", "-3",	"-2", "-1",	// inverse axis
		"1", "2", "3", "4", "5", "6", "7", "8"
	};

	int axes = (sizeof(motion_axes) / sizeof(motion_axes[0])) >> 1;
	for(int i=0; i<IM_DOF_COUNT; i++) {
		CComboBox* cbAxis = (CComboBox*)GetDlgItem(IDC_COMBO_AXIS[i]);
		cbAxis->ResetContent();
		cbAxis->EnableWindow(i < m_pMotion->m_nDevAxisCount);

		if(i < m_pMotion->m_nDevAxisCount) {
			for(int j=-m_pMotion->m_nDevAxisCount; j<m_pMotion->m_nDevAxisCount; j++) {
				cbAxis->AddString((CString)motion_axes[axes+j].name);
			}
		}
	}
}

void CIMotion_TestbedDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch(nIDEvent)
	{
	case TIMER_MOTION_UPDATE:
		{
			if(GetWindow(GW_ENABLEDPOPUP) == NULL) 
			{
				UpdateData(TRUE);
				Update();
				UpdateData(FALSE);
			}
			break;
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}

HBRUSH CIMotion_TestbedDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	UINT nID = pWnd->GetDlgCtrlID();
	switch (nID)
	{
	case IDC_STATIC_STATUS:
		if(m_bConnected) {
			if(m_nError) {
				pDC->SetBkColor(m_color[1]);
				hbr = m_hbr[1];
			}
			else if(m_bBusy) {
				pDC->SetBkColor(m_color[2]);
				hbr = m_hbr[2];
			}
			else {
				pDC->SetBkColor(m_color[0]);
				hbr = m_hbr[0];
			}
		}
		break;
	}
	
	if(m_bConnected && m_nError) {
		for(int i=0; i<IM_DOF_COUNT; i++) {
			bool error = false;	
			if(nID == IDC_BUTTON_SERVO[i] && (m_nError == IM_ERROR_SERVO) && m_pMotion->m_info[i].bServoOn == false)
				error = true;
			else if(nID == IDC_BUTTON_ALARM[i] && (m_nError == IM_ERROR_ALARM) && m_pMotion->m_info[i].bAlarm == true)
				error = true;	
			else if(nID == IDC_STATIC_EMR[i] && (m_nError == IM_ERROR_EMER) && m_pMotion->m_info[i].bEmer == true)
				error = true;			
			if(error) {				
				pDC->SetBkColor(m_color[1]);
				hbr = m_hbr[1];
			}
		}
	}
	return hbr;
}
