
// SerialControlDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"
#include "Mycomm.h"

// CSerialControlDlg 대화 상자
class CSerialControlDlg : public CDialogEx
{
// 생성입니다.
public:
	CSerialControlDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

	CMycomm* m_comm;
	LRESULT OnThreadClosed(WPARAM length, LPARAM lpara);
	LRESULT OnReceive(WPARAM length, LPARAM lpara);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SERIALCONTROL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	BOOL comport_state;
	CComboBox m_combo_comport_list;
	CString m_str_comport;
	CComboBox m_combo_baudrate_list;
	CString m_str_baudrate;
	CEdit m_edit_rcv_view;
	CEdit m_edit_send_data;
	afx_msg void OnBnClickedBtConnect();
	afx_msg void OnCbnSelchangeComboComport();
	afx_msg void OnCbnSelchangeComboBaudrate();
	afx_msg void OnBnClickedBtClear();
	afx_msg void OnBnClickedBtSend();
	CComboBox m_combo_parity_list;
	CComboBox m_combo_databit_list;
	CComboBox m_combo_stopbit_list;
	CString m_str_parity;
	CString m_str_databit;
	CString m_str_stopbit;
	CString m_str_com_received;
	afx_msg void OnCbnSelchangeComboParity();
	afx_msg void OnCbnSelchangeComboDatabit();
	afx_msg void OnCbnSelchangeComboStopbit();
	afx_msg void OnBnClickedBtMeas();
	CEdit m_edit_meas_intv;
	CEdit m_edit_meas_thrs;
	CString m_str_meas_intv;
	CString m_str_meas_thrs;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	bool m_bMeasRunning;
	double *m_Data;
	int m_nData;
	int m_nThreshold;
	CEdit m_edit_meas_maxd;
	CString m_str_meas_maxd;
	int m_nMaxData;
	int m_nFilter;
	int m_nMeasWorking;
	double data_min(double *data, int nStart, int nSize);
	double data_max(double *data, int nStart, int nSize);
	double data_median(double *data, int nStart, int nSize);
	CEdit m_edit_meas_thrw;
	CString m_str_meas_thrw;
	int m_nThWidth;
	afx_msg void OnBnClickedBtSave();
	CEdit m_edit_save_id;
	CString m_str_save_id;
	CEdit m_edit_save_filter;
	CString m_str_save_filter;
	CComboBox m_combo_offset_list;
	CString m_str_offset;
	afx_msg void OnCbnSelchangeComboOffset();
	CButton m_check_autostart;
	BOOL m_bool_autostart;
	int m_nNaN;
	CEdit m_edit_save_cut;
	CString m_str_save_cut;
	int m_nCut;
	afx_msg void OnBnClickedLazerControl();
	afx_msg void OnBnClickedLazercontrol2();
	void data_regression(double * data, int nStart, int nSize, int nDegree, double * a);
};
