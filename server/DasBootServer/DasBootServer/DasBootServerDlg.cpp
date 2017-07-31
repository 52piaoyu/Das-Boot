
// DasBootServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DasBootServer.h"
#include "DasBootServerDlg.h"
#include "afxdialogex.h"

#include "ClientManager.h"
#include "BufPacket.h"
#include "DasBootSocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CDasBootServerDlg dialog



CDasBootServerDlg::CDasBootServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DASBOOTSERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDasBootServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDasBootServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
  ON_BN_CLICKED(IDC_BUTTON_MAIN_START_LISTENING, &CDasBootServerDlg::OnBnClickedButtonMainStartListening)
  ON_MESSAGE(WM_PKTHANDLEMSG, OnPktHandleMsg)
END_MESSAGE_MAP()


// CDasBootServerDlg message handlers

BOOL CDasBootServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDasBootServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDasBootServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDasBootServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

extern CDasBootServerApp theApp;

//EventSelectʱ��ѡ��ģ��
DWORD ThreadProc(LPARAM lparam)
{
  CDasBootServerDlg* pDlg = (CDasBootServerDlg*)lparam;
  if (pDlg == NULL)
  {
    TRACE(_T("ThreadProc()�У���ȡCDasBootServerDlg��thisָ��ʧ��"));

    return 0;
  }

  DWORD dwRet = 0;

  //����accept��ʱ���ȡ��ַ��Ϣ
  sockaddr_in tagRecvAddr;
  int nRecvAddrSize = sizeof(sockaddr_in);

  while (true)
  {
    //��ʼ�ȴ�socketevent������¼�
    dwRet = WSAWaitForMultipleEvents(theApp.GetSocketEventNum(), 
      theApp.GetEventAry(), FALSE, INFINITE,FALSE/*��completion routine �й�*/);

    if (dwRet == WSA_WAIT_FAILED)
    {
      TRACE(_T("WSAWaitForMultipleEvents()�ȴ�ʧ�ܣ�ThreadProc�̱߳���ֹ"));

      return 0;
    }
    else if (dwRet == WSA_WAIT_TIMEOUT)
    {
      //��ʾ��ʱ
      continue;
    }
    else if (dwRet >= WSA_WAIT_EVENT_0 &&
      dwRet <= (WSA_WAIT_EVENT_0 + WSA_MAXIMUM_WAIT_EVENTS))
    {
      //��ʾ�ȵ��˺Ϸ��¼���Ϣ
      int nFirstEventIndex = dwRet - WSA_WAIT_EVENT_0;  //��ǰ�Ϸ���Ϣ��λ��
      int nPotentialCount = theApp.GetSocketEventNum(); //���У�Ǳ�ںϷ�����Ϣ������

      //�ȵ��˺Ϸ��¼���Ϣ�󣬴Ӵ��������ο����滹��û�кϷ���Ϣ
      for (int i = nFirstEventIndex; i < nPotentialCount; i++)
      {
        dwRet = WSAWaitForMultipleEvents(1, &(theApp.GetEventAry()[i]), FALSE,
          0, FALSE);

        if (dwRet == WSA_WAIT_FAILED)
        {
          TRACE(_T("WSAWaitForMultipleEvents()�ȴ�ʧ�ܣ�ThreadProc�̱߳���ֹ"));

          return 0;
        }
        else if (dwRet == WSA_WAIT_TIMEOUT)
        {
          //��ʾ��ʱ
          continue;
        }

        int nIndex = i;
        //��ʾ��socket����Ϣ��,��Ҫ����
        SOCKET hSocket = theApp.GetSocketFromSocketevent(nIndex);
        WSAEVENT hNotifyEvent = theApp.GetEventFromSocketevent(nIndex);
        WSANETWORKEVENTS tagNetworkEvent = { 0 };

        int nRet = WSAEnumNetworkEvents(hSocket, hNotifyEvent,
          &tagNetworkEvent);  //���ڻ�ȡ��ǰ�¼�����ϸ��Ϣ

        if (nRet == SOCKET_ERROR)
        {
          //�Ѹ�socket���¼���������ɾ��,���ͷ���Դ
          /*ȷ����˵��Ӧ����WSAGetLastError()���ΪWSAENOTSOCK����ɾ����������������
          ���ԭ�򣬾�������������socket���Ϸ�*/
          theApp.DestorySocketEvent(nIndex);

          continue;
        }

        //һ�кϷ�֮�󣬽��д������������accept��read
        if (tagNetworkEvent.iErrorCode[FD_ACCEPT_BIT] == 0 &&
          tagNetworkEvent.lNetworkEvents == FD_ACCEPT)
        {
          //��ʾ���Խ�������������
          memset(&tagRecvAddr, 0, sizeof(sockaddr_in));
          tagRecvAddr.sin_family = AF_INET;
          SOCKET hSocketConnection = accept(hSocket, (sockaddr*)&tagRecvAddr,
            &nRecvAddrSize);

          if (hSocketConnection == SOCKET_ERROR)
          {
            TRACE(_T("���Խ��������ӣ�����accept()ʧ��"));
            continue;
          }

          //���յ��ͻ��˵�����,���뵽���������
          //�����ӵĶ�����뵽m_ClientManager��
          CClientContext* pContext = new CClientContext(hSocketConnection,
            tagRecvAddr);
          theApp.m_objClientManager.Add(hSocketConnection, pContext);

          theApp.AddSocketEvent(hSocketConnection, WSACreateEvent());

          nRet = WSAEventSelect(
            theApp.GetSocketFromSocketevent(theApp.GetSocketEventNum() - 1),
            theApp.GetEventFromSocketevent(theApp.GetSocketEventNum() - 1),
            FD_READ | FD_WRITE | FD_CLOSE);

          if (nRet == SOCKET_ERROR)
          {
            TRACE("WSAAsyncSelect�� socket ʧ��");
            theApp.DecreaseSocketEventNum();
          }
        }
        else if (tagNetworkEvent.iErrorCode[FD_READ_BIT] == 0 &&
          tagNetworkEvent.lNetworkEvents == FD_READ)
        {
          //1�յ�����, ���ݶ�Ӧ��socket�ҵ���Ӧ��ClientContext
          CClientContext* pContext = NULL;
          BOOL bRet = theApp.m_objClientManager.FindAt(hSocket, pContext);
          if (!bRet || pContext == NULL)
          {
            return 0;
          }

          int nRecvedBytes = recv(hSocket, pContext->m_szRecvBuf, RECV_BYTES,
            0);

          //2���յ������ݼ��뵽��������
          if (nRecvedBytes > 0)
          {
            pContext->m_Packet.Append(pContext->m_szRecvBuf, nRecvedBytes);
            //3��黺�����Ĵ�С,������ȴﵽĿ��,��ô��ʼ���ô�����
            pContext->m_Packet.Handle(hSocket);
          }

        }

      }//for (int i = nFirstEventIndex; i < nPotentialCount; i++)
    
    }//else if (dwRet >= WSA_WAIT_EVENT_0 && dwRet <= (WSA_WAIT_EVENT_0 + WSA_MAXIMUM_WAIT_EVENTS))
  
  }//while (true)

  return 0;
}

void CDasBootServerDlg::OnBnClickedButtonMainStartListening()
{
  // TODO: Add your control notification handler code here
  //1. ����һ��tcp��socket
  SOCKET hListeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (hListeningSocket == INVALID_SOCKET)
  {
    TRACE(_T("��������socketʧ��"));
    AfxMessageBox(_T("��������socketʧ��"));

    return;
  }

  //2. ��ip�Ͷ˿�
  sockaddr_in tagAddr;
  memset(&tagAddr, 0, sizeof(sockaddr_in));

  tagAddr.sin_family = AF_INET;
  tagAddr.sin_port = htons(8889);
  tagAddr.sin_addr.S_un.S_addr = INADDR_ANY;  //inet_addr("127.0.0.1");

  int nRet = bind(hListeningSocket, (sockaddr*)&tagAddr, sizeof(sockaddr_in));

  if (nRet == SOCKET_ERROR)
  {
    TRACE(_T("�󶨼���socket��IP�Ͷ˿�ʱʧ��"));
    AfxMessageBox(_T("�󶨼���socket��IP�Ͷ˿�ʱʧ��"));

    return;
  }

  //3. ��ʼ����
  nRet = listen(hListeningSocket, SOMAXCONN);

  if (nRet == SOCKET_ERROR)
  {
    TRACE(_T("��ʼ����ʧ��"));
    AfxMessageBox(_T("��ʼ����ʧ��"));

    return;
  }

  //4. �����¼����ʹ�socket�󶨣�����event����ģ�ͣ�
  WSAEVENT hListeningEvent = WSACreateEvent();

  if (hListeningEvent == WSA_INVALID_EVENT)
  {
    TRACE(_T("�����¼������ڼ���socket��ʧ��"));
    AfxMessageBox(_T("�����¼������ڼ���socket��ʧ��"));

    return;
  }

  //5. ���Ӵ˼���socket��event��theApp���б���
  BOOL bRet = theApp.AddSocketEvent(hListeningSocket, hListeningEvent);

  if (bRet == FALSE)
  {
    TRACE("��Ӽ���socket��theApp��socketevent����ʧ��");
    AfxMessageBox(_T("��Ӽ���socket��theApp��socketevent����ʧ��"));

    return;
  }

  //6. �󶨼���socketevent��һ������£�����ɹ�����
  nRet = WSAEventSelect(hListeningSocket, hListeningEvent, 
    FD_ACCEPT | FD_CLOSE);

  if (nRet == SOCKET_ERROR)
  {
    TRACE("WSAEventSelect�󶨣����ڼ���socketevent��ʧ��");
    AfxMessageBox(_T("WSAEventSelect�󶨣����ڼ���socketevent��ʧ��"));

    return;
  }

  //7. ��һ�гɹ��������̴߳���socketevent����Ҳ�Ǳ��������ڴ�������socket��Ψһ�̣߳�
  //����������event����ģ�ͣ��õ��̼߳�������socket��ͬһʱ��֧��socket�������ֵ��64
  AfxBeginThread((AFX_THREADPROC)ThreadProc, this);

}

HRESULT CDasBootServerDlg::OnPktHandleMsg(WPARAM wParam, LPARAM lParam)
{

  SOCKET sRecv = (SOCKET)wParam;
  char* pszBuf = (char*)lParam;

  if (pszBuf == NULL)
  {
    return 0;
  }

  switch (pszBuf[4])
  {
//   case CLIENT_SCREEN:
//   {
//     OnScreen(sRecv, pszBuf + 5, *(DWORD*)(pszBuf));
//   }
  break;
  }

  CDasBootSocket sendSkt(sRecv);
  CBufPacket pkt;
  char szHead[5] = { 0 };
  DWORD dwSize = sizeof(DWORD) + sizeof(BYTE) + 6/*sizeof(DWORD) + sizeof(BYTE) + sizeof(DWORD) + sizeof(DWORD)
                                                 + 88*/;
  (DWORD&)*szHead = dwSize;
  szHead[4] = 'a';
  pkt.Append(szHead, sizeof(szHead));
  //   pkt.Append((char*)&nWidth, sizeof(DWORD));
  //   pkt.Append((char*)&nHeigth, sizeof(DWORD));
  //   pkt.Append(szBit, dwCount);

  pkt.Append("world", sizeof("world"));

  sendSkt.SendPkt(pkt);

  return 0;
}