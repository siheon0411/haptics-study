
// IMotion_TestbedDlg.h : header file
//

#pragma once

typedef enum
{
	MOTION_CLOSE	= 0,
	MOTION_OPEN		= 1,	// connect
	MOTION_STOP		= 2,	// settle
	MOTION_START	= 4,	// neutral
	MOTION_RUNNING	= 8,	// operation
} motion_state;


// CIMotion_TestbedDlg dialog
class CIMotion_TestbedDlg : public CDialogEx
{
// Construction
public:
	CIMotion_TestbedDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_IMOTION_TESTBED_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	
private:
	HANDLE m_hMutex;
	SYSTEMTIME m_startTime;
	int m_nDevID;
	int m_nSlaveID;
	bool m_bConnected;
	bool m_bBusy;
	int m_nError;
	motion_state m_nState;
	COLORREF m_color[3]; // normal, error, busy
	HBRUSH m_hbr[3];
	CEdit m_editLog;
	CString m_staticStatus;
	CString m_staticUrl;
	CString m_staticCurrentTime;
	CString m_staticTotalTime;
	CString m_strLogTemp;

	void Initialize();
	void Update();
	void Cleanup();
	void LogOutput(unsigned int devid, const char* msg, void* param);	
	int SetCheck(int dof);
	int SetMaxFrequency(int dof);
	void UpdateAxisMap();

public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonOpen();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonOperation();
	afx_msg void OnBnClickedButtonServo1();
	afx_msg void OnBnClickedButtonServo2();
	afx_msg void OnBnClickedButtonServo3();
	afx_msg void OnBnClickedButtonServo4();
	afx_msg void OnBnClickedButtonServo5();
	afx_msg void OnBnClickedButtonServo6();
	afx_msg void OnBnClickedButtonAlarm1();
	afx_msg void OnBnClickedButtonAlarm2();
	afx_msg void OnBnClickedButtonAlarm3();
	afx_msg void OnBnClickedButtonAlarm4();
	afx_msg void OnBnClickedButtonAlarm5();
	afx_msg void OnBnClickedButtonAlarm6();
	afx_msg void OnBnClickedButtonEncoderReset();
	afx_msg void OnBnClickedButtonLoadMotionData();
	afx_msg void OnSelchangeComboName();
	afx_msg void OnSelchangeComboTest();
	afx_msg void OnBnClickedCheckLoop();
	afx_msg void OnBnClickedCheckSurge();
	afx_msg void OnBnClickedCheckSway();
	afx_msg void OnBnClickedCheckHeave();
	afx_msg void OnBnClickedCheckRoll();
	afx_msg void OnBnClickedCheckPitch();
	afx_msg void OnBnClickedCheckYaw();
	afx_msg void OnSetfocusEditFreqSurge();
	afx_msg void OnSetfocusEditFreqSway();
	afx_msg void OnSetfocusEditFreqHeave();
	afx_msg void OnSetfocusEditFreqRoll();
	afx_msg void OnSetfocusEditFreqPitch();
	afx_msg void OnSetfocusEditFreqYaw();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnEnChangeEditEquip();
};
