#include "stdafx.h"
#include "DllCommon.h"

#include "Resource.h"
#include "DasBootDllRemoteCmd.h"

#include "ClientOperationSubWindowRemoteCmd.h"

extern "C" {
  __declspec(dllexport) int DBLoadDLLServer(std::unordered_map<std::string,
    pfnDBExportServerOperationConfirm>& mapConfirmFunctions,
    std::unordered_map<std::string, pfnDBExportServer>& mapFunctions,
    std::unordered_map<std::string, std::string>& mapDllIntroduction);
}

#define DLL_NAME _T("DasBootDllRemoteCmd.dll")
#define DLL_INTRODUCTION _T("Զ�̿���̨��")

int DBRemoteCmdOperationConfirm(SOCKET hSocket,
  CClientOperationMainWindow *pParentWnd, CClientContext* pClientContext,
  pfnSendMessageOut pfnSMO);

static int DBDllStartRemoteCmdM(const char *strParam, SOCKET hSocket, 
  int nMsgLength, CClientContext* pClientContext, pfnSendMessageOut pfnSMO);
static int DBDllRemoteCmdMsgM(const char *strParam, SOCKET hSocket, 
  int nMsgLength, CClientContext* pClientContext, pfnSendMessageOut pfnSMO);
static int DBDllRemoteCmdSendM(const char *strParam, SOCKET hSocket,
  int nMsgLength, CClientContext* pClientContext, pfnSendMessageOut pfnSMO);
static int DBDllCloseRemoteCmdM(const char *strParam, SOCKET hSocket, 
  int nMsgLength, CClientContext* pClientContext, pfnSendMessageOut pfnSMO);

//���е�dll�����У����ҵ����˺���
int DBLoadDLLServer(std::unordered_map<std::string,
  pfnDBExportServerOperationConfirm>& mapConfirmFunctions,
  std::unordered_map<std::string, pfnDBExportServer>& mapFunctions,
  std::unordered_map<std::string, std::string>& mapDllIntroduction)
{
  mapDllIntroduction[DLL_NAME] = DLL_INTRODUCTION;
  mapConfirmFunctions[DLL_NAME] = (pfnDBExportServerOperationConfirm)DBRemoteCmdOperationConfirm;

  mapFunctions["StartRemoteCmdR"] = (pfnDBExportServer)DBDllStartRemoteCmdM;
  mapFunctions["RemoteCmdMsg00N"] = (pfnDBExportServer)DBDllRemoteCmdMsgM;
  mapFunctions["RmotCmdSendCmdR"] = (pfnDBExportServer)DBDllRemoteCmdSendM;
  mapFunctions["CloseRemoteCmdR"] = (pfnDBExportServer)DBDllCloseRemoteCmdM;

  return 0;
}

extern CDasBootDllRemoteCmdApp theApp;

int DBRemoteCmdOperationConfirm(SOCKET hSocket,
  CClientOperationMainWindow *pParentWnd, CClientContext* pClientContext,
  pfnSendMessageOut pfnSMO)
{
  HINSTANCE hOldInst = AfxGetResourceHandle();
  AfxSetResourceHandle(theApp.m_hInstance);

  CClientOperationSubWindowRemoteCmd *pClientOperationSubWindow =
    new CClientOperationSubWindowRemoteCmd;

  if (pClientContext->m_mapClientOperationSubWindows[DLL_NAME] != NULL)
  {
    delete pClientContext->m_mapClientOperationSubWindows[DLL_NAME];
    pClientContext->m_mapClientOperationSubWindows[DLL_NAME] = NULL;
  }

  pClientContext->m_mapClientOperationSubWindows[DLL_NAME] = 
    pClientOperationSubWindow;


  pClientOperationSubWindow->Create(MAKEINTRESOURCE(IDD_DIALOG_DLL_REMOTE_CMD),
    (CWnd *)pParentWnd);

  pClientOperationSubWindow->ShowWindow(SW_SHOW);
  pClientOperationSubWindow->m_hSocket = hSocket;
  pClientOperationSubWindow->m_pfnSMO = pfnSMO;

  CString strWindowCaption;
  strWindowCaption.Format(_T("%s %d"), _T("�ͻ����Ʒִ��� Զ��cmd �ͻ�socket��"),
    (int)hSocket);
  pClientOperationSubWindow->SetWindowText(strWindowCaption);

  AfxSetResourceHandle(hOldInst);

  return 0;
}

int DBDllStartRemoteCmdM(const char *strParam, SOCKET hSocket, int nMsgLength, 
  CClientContext* pClientContext, pfnSendMessageOut pfnSMO)
{
  //��ʱʲô������
  //done ���Կ��Ǽ��ؽ���󣬡����������ťĬ�ϲ�����յ�����Ϣ���ټ�����������ť

  CClientOperationSubWindowRemoteCmd *pClientOperationSubWindow =
    (CClientOperationSubWindowRemoteCmd *)pClientContext->
    m_mapClientOperationSubWindows[DLL_NAME];

  CWnd *pWndTmp = pClientOperationSubWindow->
    GetDlgItem(IDC_BUTTON_CLIENT_OPERATION_SUB_WINDOW_SEND_COMMAND);
  pWndTmp->EnableWindow(TRUE);

  pWndTmp = pClientOperationSubWindow->
    GetDlgItem(IDC_BUTTON_CLIENT_OPERATION_SUB_WINDOW_INIT_REMOTE_CMD);
  pWndTmp->EnableWindow(FALSE);

  AfxMessageBox(_T("�ͻ���cmd�Ѽ���"));

  return 0;
}

int DBDllCloseRemoteCmdM(const char *strParam, SOCKET hSocket, int nMsgLength,
  CClientContext* pClientContext, pfnSendMessageOut pfnSMO)
{
  //��ʱʲô������

  return 0;
}

int DBDllRemoteCmdSendM(const char *strParam, SOCKET hSocket, int nMsgLength,
  CClientContext* pClientContext, pfnSendMessageOut pfnSMO)
{
  //��ʱʲô������
  
  return 0;
}

//�ͻ���pipe�������ݣ����Ҵ��˹���
int DBDllRemoteCmdMsgM(const char *strParam, SOCKET hSocket, int nMsgLength,
  CClientContext* pClientContext, pfnSendMessageOut pfnSMO)
{
  CClientOperationSubWindowRemoteCmd *pClientOperationSubWindow = 
    (CClientOperationSubWindowRemoteCmd *)pClientContext->
    m_mapClientOperationSubWindows[DLL_NAME];

  CString strCmdContent;
  strCmdContent.Format("%s", strParam);

  pClientOperationSubWindow->m_edtctlStrConsoleContent += strCmdContent;

  pClientOperationSubWindow->UpdateData(FALSE);

  return 0;
}