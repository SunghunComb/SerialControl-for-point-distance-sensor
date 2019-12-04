
// SerialControlDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "SerialControl.h"
#include "SerialControlDlg.h"
#include "afxdialogex.h"

#include <cmath>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define ID_COM_TIMER 100

// CSerialControlDlg 대화 상자

#define MAX_MEAS_DATA 20000
static double data_raw[MAX_MEAS_DATA];
static double data_proc[MAX_MEAS_DATA];


CSerialControlDlg::CSerialControlDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSerialControlDlg::IDD, pParent)
	, m_str_comport(_T(""))
	, m_str_baudrate(_T(""))
	, m_str_parity(_T(""))
	, m_str_databit(_T(""))
	, m_str_stopbit(_T(""))
	, m_str_meas_intv(_T(""))
	, m_str_meas_thrs(_T(""))
	, m_bMeasRunning(false)
	, m_Data(NULL)
	, m_nData(0)
	, m_nThreshold(0)
	, m_str_meas_maxd(_T(""))
	, m_nMaxData(0)
	, m_nFilter(0)
	, m_nMeasWorking(0)
	, m_str_meas_thrw(_T(""))
	, m_nThWidth(0)
	, m_str_save_id(_T(""))
	, m_str_save_filter(_T(""))
	, m_str_offset(_T(""))
	, m_bool_autostart(FALSE)
	, m_nNaN(0)
	, m_str_save_cut(_T(""))
	, m_nCut(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSerialControlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_COMPORT, m_combo_comport_list);
	DDX_CBString(pDX, IDC_COMBO_COMPORT, m_str_comport);
	DDX_Control(pDX, IDC_COMBO_BAUDRATE, m_combo_baudrate_list);
	DDX_CBString(pDX, IDC_COMBO_BAUDRATE, m_str_baudrate);
	DDX_Control(pDX, IDC_EDIT_RCV_VIEW, m_edit_rcv_view);
	DDX_Control(pDX, IDC_EDIT_SEND_DATA, m_edit_send_data);
	DDX_Control(pDX, IDC_COMBO_PARITY, m_combo_parity_list);
	DDX_Control(pDX, IDC_COMBO_DATABIT, m_combo_databit_list);
	DDX_Control(pDX, IDC_COMBO_STOPBIT, m_combo_stopbit_list);
	DDX_CBString(pDX, IDC_COMBO_PARITY, m_str_parity);
	DDX_CBString(pDX, IDC_COMBO_DATABIT, m_str_databit);
	DDX_CBString(pDX, IDC_COMBO_STOPBIT, m_str_stopbit);
	DDX_Control(pDX, IDC_EDIT_MEAS_INTV, m_edit_meas_intv);
	DDX_Control(pDX, IDC_EDIT_MEAS_THRS, m_edit_meas_thrs);
	DDX_Text(pDX, IDC_EDIT_MEAS_INTV, m_str_meas_intv);
	DDX_Text(pDX, IDC_EDIT_MEAS_THRS, m_str_meas_thrs);
	DDX_Control(pDX, IDC_EDIT_MEAS_MAXD, m_edit_meas_maxd);
	DDX_Text(pDX, IDC_EDIT_MEAS_MAXD, m_str_meas_maxd);
	DDX_Control(pDX, IDC_EDIT_MEAS_THRW, m_edit_meas_thrw);
	DDX_Text(pDX, IDC_EDIT_MEAS_THRW, m_str_meas_thrw);
	DDX_Control(pDX, IDC_EDIT_SAVE_ID, m_edit_save_id);
	DDX_Text(pDX, IDC_EDIT_SAVE_ID, m_str_save_id);
	DDX_Control(pDX, IDC_EDIT_SAVE_FILTER, m_edit_save_filter);
	DDX_Text(pDX, IDC_EDIT_SAVE_FILTER, m_str_save_filter);
	DDX_Control(pDX, IDC_COMBO_OFFSET, m_combo_offset_list);
	DDX_CBString(pDX, IDC_COMBO_OFFSET, m_str_offset);
	DDX_Control(pDX, IDC_CHECK_AUTOSTART, m_check_autostart);
	DDX_Check(pDX, IDC_CHECK_AUTOSTART, m_bool_autostart);
	DDX_Control(pDX, IDC_EDIT_SAVE_CUT, m_edit_save_cut);
	DDX_Text(pDX, IDC_EDIT_SAVE_CUT, m_str_save_cut);
}

BEGIN_MESSAGE_MAP(CSerialControlDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

	ON_MESSAGE(WM_MYCLOSE, &CSerialControlDlg::OnThreadClosed)
	ON_MESSAGE(WM_MYRECEIVE, &CSerialControlDlg::OnReceive)

	ON_BN_CLICKED(IDC_BT_CONNECT, &CSerialControlDlg::OnBnClickedBtConnect)
	ON_CBN_SELCHANGE(IDC_COMBO_COMPORT, &CSerialControlDlg::OnCbnSelchangeComboComport)
	ON_CBN_SELCHANGE(IDC_COMBO_BAUDRATE, &CSerialControlDlg::OnCbnSelchangeComboBaudrate)
	ON_BN_CLICKED(IDC_BT_CLEAR, &CSerialControlDlg::OnBnClickedBtClear)
	ON_BN_CLICKED(IDC_BT_SEND, &CSerialControlDlg::OnBnClickedBtSend)
	ON_CBN_SELCHANGE(IDC_COMBO_PARITY, &CSerialControlDlg::OnCbnSelchangeComboParity)
	ON_CBN_SELCHANGE(IDC_COMBO_DATABIT, &CSerialControlDlg::OnCbnSelchangeComboDatabit)
	ON_CBN_SELCHANGE(IDC_COMBO_STOPBIT, &CSerialControlDlg::OnCbnSelchangeComboStopbit)
	ON_BN_CLICKED(IDC_BT_MEAS, &CSerialControlDlg::OnBnClickedBtMeas)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BT_SAVE, &CSerialControlDlg::OnBnClickedBtSave)
	ON_CBN_SELCHANGE(IDC_COMBO_OFFSET, &CSerialControlDlg::OnCbnSelchangeComboOffset)
	ON_BN_CLICKED(IDC_Lazer_Control, &CSerialControlDlg::OnBnClickedLazerControl)
	ON_BN_CLICKED(IDC_Lazer_control2, &CSerialControlDlg::OnBnClickedLazercontrol2)
END_MESSAGE_MAP()


// CSerialControlDlg 메시지 처리기

BOOL CSerialControlDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	m_combo_comport_list.AddString(_T("COM1"));
	m_combo_comport_list.AddString(_T("COM2"));
	m_combo_comport_list.AddString(_T("COM3"));
	m_combo_comport_list.AddString(_T("COM4"));
	m_combo_comport_list.AddString(_T("COM5"));
	m_combo_comport_list.AddString(_T("COM6"));
	m_combo_comport_list.AddString(_T("COM7"));
	m_combo_comport_list.AddString(_T("COM8"));

	m_combo_baudrate_list.AddString(_T("9600"));
	m_combo_baudrate_list.AddString(_T("19200"));
	m_combo_baudrate_list.AddString(_T("57600"));
	m_combo_baudrate_list.AddString(_T("115200"));
	m_combo_baudrate_list.AddString(_T("128000"));
	m_combo_baudrate_list.AddString(_T("256000"));

	m_combo_parity_list.AddString(_T("None"));
	m_combo_parity_list.AddString(_T("Even"));
	m_combo_parity_list.AddString(_T("Odd"));

	m_combo_databit_list.AddString(_T("7 Bit"));
	m_combo_databit_list.AddString(_T("8 Bit"));

	m_combo_stopbit_list.AddString(_T("1 Bit"));
	m_combo_stopbit_list.AddString(_T("1.5 Bit"));
	m_combo_stopbit_list.AddString(_T("2 Bit"));

	m_combo_offset_list.AddString(_T("None"));
	m_combo_offset_list.AddString(_T("Left"));
	m_combo_offset_list.AddString(_T("Mid"));
	m_combo_offset_list.AddString(_T("Right"));
	m_combo_offset_list.AddString(_T("Tilt"));
	m_combo_offset_list.AddString(_T("Rotation"));

	comport_state = false;
	GetDlgItem(IDC_BT_CONNECT)->SetWindowText(_T("OPEN"));
	m_str_comport = _T("COM5");
	m_str_baudrate = _T("57600");
	m_str_parity = _T("Even");
	m_str_databit = _T("8 Bit");
	m_str_stopbit = _T("1 Bit");

	m_str_com_received = _T("");

	m_str_meas_intv = _T("100");
	m_str_meas_maxd = _T("10000");
	m_str_meas_thrs = _T("2");
	m_str_meas_thrw = _T("5");
	m_str_save_filter = _T("5");
	m_str_save_id = _T("Device ID");
	m_bool_autostart = true;
	m_str_offset = _T("Tilt");
	m_str_save_cut = _T("10");

	UpdateData(FALSE);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CSerialControlDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CSerialControlDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CSerialControlDlg::OnThreadClosed(WPARAM length, LPARAM lpara)
{
	//overlapped i/o 핸들을닫는다.
	((CMycomm*)lpara)->HandleClose();
	delete ((CMycomm*)lpara);
	return 0;
}
LRESULT CSerialControlDlg::OnReceive(WPARAM length, LPARAM lpara)
{
	CString str;
	char com_data[20000];
	if (m_comm)
	{
		m_comm->Receive(com_data, length); //length 길이만큼데이터를받는다.
		com_data[length] = _T('\0');
		for (unsigned int i = 0; i<length; i++)
		{
			str += com_data[i];
		}
//		m_edit_rcv_view.ReplaceSel(str); //에디트박스에표시하기위함
		m_str_com_received += str; // 버퍼에 저장
		str = "";
	}
	return 0;
}

void CSerialControlDlg::OnBnClickedBtConnect()
{
	// TODO: Add your control notification handler code here
	CString str;

	if (comport_state)
	{
		if (m_comm) //컴포트가존재하면
		{
			str = ":01W010;1;79C2\r\n";
			m_comm->Send(str, str.GetLength());
			Sleep(100);

			m_comm->Close();
			m_comm = NULL;
			m_edit_rcv_view.ReplaceSel(_T("COM Prot Closed\r\n")); 

			comport_state = false;
			GetDlgItem(IDC_BT_CONNECT)->SetWindowText(_T("OPEN"));

			GetDlgItem(IDC_COMBO_COMPORT)->EnableWindow(true);
			GetDlgItem(IDC_COMBO_BAUDRATE)->EnableWindow(true);
			GetDlgItem(IDC_COMBO_PARITY)->EnableWindow(true);
			GetDlgItem(IDC_COMBO_DATABIT)->EnableWindow(true);
			GetDlgItem(IDC_COMBO_STOPBIT)->EnableWindow(true);		}

			GetDlgItem(IDC_BT_SEND)->EnableWindow(false);
			GetDlgItem(IDC_BT_MEAS)->EnableWindow(false);
	}
	else
	{
		m_comm = new CMycomm(_T("\\\\.\\") + m_str_comport, 
			m_str_baudrate, m_str_parity, m_str_databit, m_str_stopbit);
		if (m_comm->Create(GetSafeHwnd()) != 0) //통신포트를열고윈도우의핸들을넘긴다.
		{
			m_edit_rcv_view.ReplaceSel(_T("COM Prot Opened Successfully\r\n")); 
			// AfxMessageBox(_T("COM 포트열림"));
			comport_state = true;
			GetDlgItem(IDC_BT_CONNECT)->SetWindowText(_T("CLOSE"));
			GetDlgItem(IDC_COMBO_COMPORT)->EnableWindow(false);
			GetDlgItem(IDC_COMBO_BAUDRATE)->EnableWindow(false);
			GetDlgItem(IDC_COMBO_PARITY)->EnableWindow(false);
			GetDlgItem(IDC_COMBO_DATABIT)->EnableWindow(false);
			GetDlgItem(IDC_COMBO_STOPBIT)->EnableWindow(false);

			GetDlgItem(IDC_BT_SEND)->EnableWindow(true);
			GetDlgItem(IDC_BT_MEAS)->EnableWindow(true);

			str = ":01W010;0;E9C3\r\n";
			m_comm->Send(str, str.GetLength());
			Sleep(100);

			m_str_com_received = _T("");
		}
		else
		{
			AfxMessageBox(_T("ERROR!"));
		}
	}
}


void CSerialControlDlg::OnCbnSelchangeComboComport()
{
	// TODO: Add your control notification handler code here
	UpdateData();
}


void CSerialControlDlg::OnCbnSelchangeComboBaudrate()
{
	// TODO: Add your control notification handler code here
	UpdateData();
}

void CSerialControlDlg::OnCbnSelchangeComboParity()
{
	// TODO: Add your control notification handler code here
	UpdateData();
}


void CSerialControlDlg::OnCbnSelchangeComboDatabit()
{
	// TODO: Add your control notification handler code here
	UpdateData();
}


void CSerialControlDlg::OnCbnSelchangeComboStopbit()
{
	// TODO: Add your control notification handler code here
	UpdateData();
}

void CSerialControlDlg::OnCbnSelchangeComboOffset()
{
	// TODO: Add your control notification handler code here
	UpdateData();
}

void CSerialControlDlg::OnBnClickedBtClear()
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_EDIT_RCV_VIEW)->SetWindowTextA(_T(" "));
}


void CSerialControlDlg::OnBnClickedBtSend()
{
	// TODO: Add your control notification handler code here
	CString str;
	GetDlgItem(IDC_EDIT_SEND_DATA)->GetWindowText(str);
	str += "\r\n";
	m_comm->Send(str, str.GetLength());
}



void CSerialControlDlg::OnBnClickedBtMeas()
{
	// TODO: Add your control notification handler code here

	UpdateData();

	if (!m_bMeasRunning) {
		GetDlgItem(IDC_BT_SEND)->EnableWindow(false);
		GetDlgItem(IDC_EDIT_MEAS_INTV)->EnableWindow(false);
		GetDlgItem(IDC_EDIT_MEAS_MAXD)->EnableWindow(false);
		GetDlgItem(IDC_EDIT_MEAS_THRS)->EnableWindow(false);
		GetDlgItem(IDC_EDIT_MEAS_THRW)->EnableWindow(false);
		GetDlgItem(IDC_EDIT_SAVE_FILTER)->EnableWindow(false);
		GetDlgItem(IDC_EDIT_SAVE_ID)->EnableWindow(false);
		GetDlgItem(IDC_CHECK_AUTOSTART)->EnableWindow(false);
		GetDlgItem(IDC_BT_SAVE)->EnableWindow(false);

		GetDlgItem(IDC_BT_MEAS)->SetWindowText(_T("STOP"));


		CString str_receive;
		TCHAR str_out[128];
		int nInterval;

		GetDlgItem(IDC_EDIT_MEAS_INTV)->GetWindowText(str_receive);
		sscanf_s( str_receive, "%d", &nInterval);

		GetDlgItem(IDC_EDIT_MEAS_MAXD)->GetWindowText(str_receive);
		sscanf_s( str_receive, "%d", &m_nMaxData);
		if (m_nMaxData > MAX_MEAS_DATA-1) {
			m_nMaxData = MAX_MEAS_DATA;
			sprintf_s( str_out, "%d", m_nMaxData);
			GetDlgItem(IDC_EDIT_MEAS_MAXD)->SetWindowText(str_out);
		}
		
		GetDlgItem(IDC_EDIT_MEAS_THRS)->GetWindowText(str_receive);
		sscanf_s( str_receive, "%d", &m_nThreshold);

		GetDlgItem(IDC_EDIT_MEAS_THRW)->GetWindowText(str_receive);
		sscanf_s( str_receive, "%d", &m_nThWidth);

		m_str_com_received = _T("");
		GetDlgItem(IDC_EDIT_RCV_VIEW)->SetWindowTextA(_T(" "));

		UpdateData();

		SetTimer(ID_COM_TIMER, nInterval, NULL);
		m_nData = 0;
		m_nNaN = 0;
		if (m_bool_autostart)
			m_nMeasWorking = 0;
		else
			m_nMeasWorking = 2;

		m_bMeasRunning = true;

		m_edit_rcv_view.ReplaceSel(_T("Measure Started\r\n")); 

	}

	else { /// STOP botton 누를 때 모션
		GetDlgItem(IDC_BT_SEND)->EnableWindow(true);
		GetDlgItem(IDC_EDIT_MEAS_INTV)->EnableWindow(true);
		GetDlgItem(IDC_EDIT_MEAS_MAXD)->EnableWindow(true);
		GetDlgItem(IDC_EDIT_MEAS_THRS)->EnableWindow(true);
		GetDlgItem(IDC_EDIT_MEAS_THRW)->EnableWindow(true);
		GetDlgItem(IDC_EDIT_SAVE_FILTER)->EnableWindow(true);
		GetDlgItem(IDC_EDIT_SAVE_ID)->EnableWindow(true);
		GetDlgItem(IDC_CHECK_AUTOSTART)->EnableWindow(true);
		GetDlgItem(IDC_BT_SAVE)->EnableWindow(true);

		GetDlgItem(IDC_BT_MEAS)->SetWindowText(_T("START"));

		KillTimer(ID_COM_TIMER);
		m_bMeasRunning = false;

		TCHAR str_out[200];
		sprintf_s(str_out, "Total (%d) data Measured with (%d) NaN\r\n", m_nData, m_nNaN);
		m_edit_rcv_view.ReplaceSel(str_out); 
	}
}


void CSerialControlDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	//CDialogEx::OnTimer(nIDEvent);

	CString str_receive;
	CString str_message = ":01R021;09F4\r\n"; // Measurement Value
	TCHAR str_out[200];

	double data_now;

	m_comm->Send(str_message, str_message.GetLength());
	Sleep(1);
	str_receive = m_str_com_received;

	if ( str_receive.GetLength() < 17)
		return;

	m_str_com_received = _T("");

	if ( ( str_receive.Left(5) == _T(":01A;")) 
		                                && ( str_receive.Mid(5,3) != _T("NaN"))   ) 
		sscanf_s(str_receive.Mid(5), "%lf", &data_now);
	else { 
		m_nNaN++;
		return;
	}

	data_now *= 1000.f;

	sprintf_s(str_out, "%d : [%d] %.0f\r\n", m_nMeasWorking, m_nData, data_now);
	m_edit_rcv_view.ReplaceSel(str_out); 
	m_edit_rcv_view.LineScroll(m_edit_rcv_view.GetLineCount());
		
	if (m_nMeasWorking == 0) {  // Start
		data_raw[m_nData++] = data_now;
		if (m_nData >= m_nThWidth) {
			m_nMeasWorking = 1;
			m_nData = 0;
		}
	}
	else if (m_nMeasWorking == 1) {  // Ready

		if ( (data_max(data_raw,0,m_nThWidth) - data_min(data_raw,0,m_nThWidth)) >= double(m_nThreshold) ) {
			data_raw[0] = data_now;
			m_nMeasWorking = 2;
			m_nData = 1;
			m_edit_rcv_view.ReplaceSel(_T("Moving Started\r\n")); 
		}
		else {
			data_raw[m_nData++] = data_now;
			if (m_nData >= m_nThWidth) m_nData = 0;
		}
	}
	else if (m_nMeasWorking == 2) {  // Running
		data_raw[m_nData++] = data_now;
		/////////////////////////////////////////////////////////////////////////////////Auto stop 제거(max data 대체)
		if (m_nData > m_nMaxData) {
			m_edit_rcv_view.ReplaceSel(_T("Max Data Reached\r\n")); 
			OnBnClickedBtMeas();
		}
	}
}


double CSerialControlDlg::data_min(double *data, int nStart, int nSize)
{
	double value = data[nStart];
	
	for (int i=nStart+1; i<nStart+nSize; i++)
		if ( data[i] < value ) value = data[i];
	return value;
}


double CSerialControlDlg::data_max(double *data, int nStart, int nSize)
{
	double value = data[nStart];
	
	for (int i=nStart+1; i<nStart+nSize; i++)
		if ( value < data[i]  ) value = data[i];
	return value;
}


//기존 median filtering
/*
for (i = m_nCut; i< (m_nData-m_nFilter-m_nCut); i+=m_nFilter ) {
			data_proc[nDataProc++] = data_median(data_raw, i, m_nFilter);
		}
*/
double CSerialControlDlg::data_median(double *data, int nStart, int nSize)
{
	int nHalf;
	int loop, i;
	double hold;

	nHalf = nSize/2; //2.xx > 2
	
	for (loop=0; loop<nSize; loop++) {
		for ( i=nStart; i<nSize-1; i++) { 
			if ( data[i] > data[i+1] ) {
				hold = data[i];
				data[i] = data[i+1];
				data[i+1] = hold;
			}
		}
	}

	if (nSize % 2 == 1) 
		return data[nStart+nHalf];
	else
		return (data[nStart+nHalf-1]+data[nStart+nHalf])/2.0f;
}


void CSerialControlDlg::OnBnClickedBtSave()
{

	CString str_receive;
	TCHAR str_out[200];

	GetDlgItem(IDC_EDIT_SAVE_FILTER)->GetWindowText(str_receive);
	sscanf_s( str_receive, "%d", &m_nFilter);

	GetDlgItem(IDC_EDIT_SAVE_CUT)->GetWindowText(str_receive);
	sscanf_s( str_receive, "%d", &m_nCut);

	if (m_nData < 2*m_nCut+m_nFilter) {
		m_edit_rcv_view.ReplaceSel(_T("No Data Saved\r\n")); 
		return;
	}

	int nDataProc;
	int i;

	///////////////////////////////////////////////////////////////////////////////// Filtering
	/////////////////////////////////////////////////////////////////////////////////i+=m_nFilter >> i++ 변경
	//data_proc(++) > data_proc() 변경
	if (m_nFilter>2) {
		nDataProc = 9;
		for (i = 9; i< (m_nData-m_nFilter-m_nCut+1-9); i++ ) {
			data_proc[i] = data_median(data_raw, i, m_nFilter);
		}
	}
	else {
		nDataProc = 0;
		for (i = m_nCut; i< (m_nData-m_nCut); i++ ) {
			data_proc[nDataProc++] = data_raw[i];        
		}
	}

	sprintf_s(str_out, "%d Raw Data -> %d Filtered Data by %d Width\r\n", m_nData, nDataProc, m_nFilter);
	m_edit_rcv_view.ReplaceSel(str_out); 
	m_edit_rcv_view.LineScroll(m_edit_rcv_view.GetLineCount());

	// Offset Correction
	
	double fOffset=0.0f, fSlope=0.0f, fGain=1.0f;

	if( m_str_offset == "Left" )
		fOffset = data_proc[0];
	else if ( m_str_offset == "Mid" )
		fOffset = (data_proc[0]+data_proc[nDataProc-1]) / 2.0f;
	else if ( m_str_offset == "Right" )
		fOffset = data_proc[nDataProc-1];
	else if ( m_str_offset == "Tilt" ) {

		double mean_left_braket;
		double mean_right_braket;

		mean_left_braket=(data_proc[2]+data_proc[3]+data_proc[4]+data_proc[5]+data_proc[6]+data_proc[7])/6;
		mean_right_braket=(data_proc[nDataProc-1]+data_proc[nDataProc-2]+data_proc[nDataProc-3]+data_proc[nDataProc-4]+data_proc[nDataProc-5]+data_proc[nDataProc-6])/6;


		fOffset = 0; //data_proc[19];/////////////////////////////////////////////////////////////////////////////////////////////////offset 정하는 곳
		fSlope = (mean_right_braket-mean_left_braket)/120000;/////////////////////////////////////////////////////slope 정하는 곳
	}
	else if ( m_str_offset == "Rotation" ) {
		fOffset = data_proc[0];
		fSlope = (data_proc[nDataProc-1]-data_proc[0])/double(nDataProc-1);
		fGain = 1/sqrt(1.0+fSlope*fSlope);
	}

	for (i = 0; i< nDataProc; i++ ) {
		data_proc[i] -= (fSlope*double(i) + fOffset);
		data_proc[i] *= fGain;
	}

	sprintf_s(str_out, "Correction by Offset=%.0f Slope=%f Gain=%f\r\n", fOffset, fSlope, fGain);
	m_edit_rcv_view.ReplaceSel(str_out); 
	m_edit_rcv_view.LineScroll(m_edit_rcv_view.GetLineCount());


	// Saving
	TCHAR SaveFileName[256]=_T(".\\FlatnessResult.csv");

	errno_t err_r, err_a;
	FILE *OutFile;
	err_r = fopen_s( &OutFile, SaveFileName,"r");
	_fcloseall();
	if (err_r != 0 ) { // New File
		err_a = fopen_s( &OutFile, SaveFileName, "w");
		if (err_a != 0 ) {
			AfxMessageBox(_T("File Write Error"));
			return;
		}
		fprintf_s( OutFile, 
			"ID,No of Data,Offset,Slope,Gain,Min,Max,Dev,,Data...\n" );
	}
	else {
		err_a = fopen_s( &OutFile, SaveFileName, "a");
		if (err_a != 0 ) {
			AfxMessageBox(_T("File Write Error"));
			return;
		}
	}

	CString str_id;
	GetDlgItem(IDC_EDIT_SAVE_ID)->GetWindowText(str_id);

	double fMin = data_min(data_proc,0,nDataProc);
	double fMax = data_max(data_proc,0,nDataProc);
	double fDev = fMax-fMin;

	fprintf_s( OutFile, "%s,%d,%.0f,%f,%.0f,%.0f,%.0f,", 
		str_id,nDataProc, fOffset, fSlope, fGain, fMin, fMax, fDev);

	for (int i=1; i<(m_nMaxData); i++)/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		fprintf_s( OutFile, ",%.0f", data_proc[i] );
	fprintf_s( OutFile, "\n");
	fclose( OutFile );

	sprintf_s(str_out, "Total (%d) prcessed data Saved\r\n", nDataProc);
	m_edit_rcv_view.ReplaceSel(str_out); 
	m_edit_rcv_view.LineScroll(m_edit_rcv_view.GetLineCount());

}

void CSerialControlDlg::OnBnClickedLazerControl()
{
	// TODO: Add your control notification handler code here

	CString str = ":01W034;0;19BB\r\n";
			m_comm->Send(str, str.GetLength());
			Sleep(100);

			m_str_com_received = _T("");
}


void CSerialControlDlg::OnBnClickedLazercontrol2()
{
	// TODO: Add your control notification handler code here

	CString str = ":01W034;1;89BA\r\n";
			m_comm->Send(str, str.GetLength());
			Sleep(100);

			m_str_com_received = _T("");
}


void CSerialControlDlg::data_regression(double * data, int nStart, int nSize, int nDeg, double * a)
{
	int i,j,k;
 
	double X[20];                        //Array that will store the values of sigma(xi),sigma(xi^2),sigma(xi^3)....sigma(xi^2n)
    for (i=0;i<2*nDeg+1;i++)
    {
        X[i]=0;
        for (j=nStart;j<(nStart+nSize);j++)
            X[i]=X[i]+pow(double(j),i);        //consecutive positions of the array will store N,sigma(xi),sigma(xi^2),sigma(xi^3)....sigma(xi^2n)
    }
    double B[10][10];            //B is the Normal matrix(augmented) that will store the equations 
    for (i=0;i<=nDeg;i++)
        for (j=0;j<=nDeg;j++)
            B[i][j]=X[i+j];            //Build the Normal matrix by storing the corresponding coefficients at the right positions except the last column of the matrix
    double Y[10];                    //Array to store the values of sigma(yi),sigma(xi*yi),sigma(xi^2*yi)...sigma(xi^n*yi)
    for (i=0;i<nDeg+1;i++)
    {    
        Y[i]=0;
        for (j=nStart;j<(nStart+nSize);j++)
        Y[i]=Y[i]+pow(double(j),i)*data[j];        //consecutive positions will store sigma(yi),sigma(xi*yi),sigma(xi^2*yi)...sigma(xi^n*yi)
    }
    for (i=0;i<=nDeg;i++)
        B[i][nDeg+1]=Y[i];                //load the values of Y as the last column of B(Normal Matrix but augmented)
    nDeg=nDeg+1;                //n is made n+1 because the Gaussian Elimination part below was for n equations, but here n is the degree of polynomial and for n degree we get n+1 equations

	double temp;
    for (i=0;i<nDeg;i++)                    //From now Gaussian Elimination starts(can be ignored) to solve the set of linear equations (Pivotisation)
        for (k=i+1;k<nDeg;k++)
            if (B[i][i]<B[k][i])
                for (j=0;j<=nDeg;j++)
                {
                    temp=B[i][j];
                    B[i][j]=B[k][j];
                    B[k][j]=temp;
                }
    
    for (i=0;i<nDeg-1;i++)            //loop to perform the gauss elimination
        for (k=i+1;k<nDeg;k++)
            {
                double t=B[k][i]/B[i][i];
                for (j=0;j<=nDeg;j++)
                    B[k][j]=B[k][j]-t*B[i][j];    //make the elements below the pivot elements equal to zero or elimnate the variables
            }
    for (i=nDeg-1;i>=0;i--)                //back-substitution
    {                        //x is an array whose values correspond to the values of x,y,z..
        a[i]=B[i][nDeg];                //make the variable to be calculated equal to the rhs of the last equation
        for (j=0;j<nDeg;j++)
            if (j!=i)            //then subtract all the lhs values except the coefficient of the variable whose value                                   is being calculated
                a[i]=a[i]-B[i][j]*a[j];
        a[i]=a[i]/B[i][i];            //now finally divide the rhs by the coefficient of the variable to be calculated
    }

    return; // a[0] x^0 + a[1] x^1 + a[2] x^2 + ...
}
