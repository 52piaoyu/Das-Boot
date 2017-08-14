#include "stdafx.h"

#include "DllCommon.h"
#include "resource.h"	
#include "DasBootDllFile.h"

#include "ClientOperationSubWindowFile.h"

int DBHelloWorldOperationConfirm(SOCKET hSocket,
  CClientOperationMainWindow *pParentWnd, CClientContext* pClientContext, 
  pfnSendMessageOut pfnSMO);
// int DBDllHelloWorldM(const char *strParam, SOCKET hSocket, int nMsgLength, 
//   CClientContext* pClientContext, pfnSendMessageOut pfnSMO);

extern "C" {
  __declspec(dllexport) int DBLoadDLLServer(std::unordered_map<std::string,
    pfnDBExportServerOperationConfirm>& mapConfirmFunctions,
    std::unordered_map<std::string, pfnDBExportServer>& mapFunctions,
    std::unordered_map<std::string, std::string>& mapDllIntroduction);
}

#define DLL_NAME _T("DasBootDllFile.dll")
#define DLL_INTRODUCTION _T("�ļ���������䡣")

//���е�dll�����У����ҵ����˺���
int DBLoadDLLServer(std::unordered_map<std::string, 
  pfnDBExportServerOperationConfirm>& mapConfirmFunctions, 
  std::unordered_map<std::string, pfnDBExportServer>& mapFunctions,
  std::unordered_map<std::string, std::string>& mapDllIntroduction)
{
  mapConfirmFunctions[DLL_NAME] =
    (pfnDBExportServerOperationConfirm)DBHelloWorldOperationConfirm;
  mapDllIntroduction[DLL_NAME] = DLL_INTRODUCTION;

  //mapFunctions["HelloWorld0000R"] = (pfnDBExportServer)DBDllHelloWorldM;

  return 0;
}

extern CDasBootDllFileApp theApp;

int DBHelloWorldOperationConfirm(SOCKET hSocket, 
  CClientOperationMainWindow *pParentWnd, CClientContext* pClientContext, 
  pfnSendMessageOut pfnSMO)
{
  HINSTANCE hOldInst = AfxGetResourceHandle();
  AfxSetResourceHandle(theApp.m_hInstance);

  CClientOperationSubWindowFile *pClientOperationSubWindow =
    new CClientOperationSubWindowFile;

  if (pClientContext->m_mapClientOperationSubWindows[DLL_NAME] != NULL)
  {
    delete pClientContext->m_mapClientOperationSubWindows[DLL_NAME];
    pClientContext->m_mapClientOperationSubWindows[DLL_NAME] = NULL;
  }

  pClientContext->m_mapClientOperationSubWindows[DLL_NAME] = 
    pClientOperationSubWindow;


  pClientOperationSubWindow->Create(MAKEINTRESOURCE(IDD_DIALOG_DLL_FILE), 
    (CWnd *)pParentWnd);

  pClientOperationSubWindow->ShowWindow(SW_SHOW);
  pClientOperationSubWindow->m_hSocket = hSocket;
  pClientOperationSubWindow->m_pfnSMO = pfnSMO;

  CString strWindowCaption;
  strWindowCaption.Format(_T("%s %d"), _T("�ͻ����Ʒִ��� �ļ� �ͻ�socket��"),
    (int)hSocket);
  pClientOperationSubWindow->SetWindowText(strWindowCaption);

  AfxSetResourceHandle(hOldInst);

  return 0;
}

/*
int DBDllHelloWorldM(const char *strParam, SOCKET hSocket, int nMsgLength, CClientContext* pClientContext, pfnSendMessageOut pfnSMO)
{
  CString strRecvdMsg;
  strRecvdMsg.Format(_T("%s"), strParam);

  CClientOperationSubWindowHelloWorld *pClientOperationSubWindow = 
    (CClientOperationSubWindowHelloWorld *)pClientContext->m_mapClientOperationSubWindows[DLL_NAME];

  //������ڲ����ڣ��ѱ��رգ�����ֱ���˳���������򣨿��ܷ����ģ�����
  if (pClientOperationSubWindow == NULL)
  {
    return 0;
  }

  pClientOperationSubWindow->m_strEdit.Append(strRecvdMsg);

  pClientOperationSubWindow->UpdateData(FALSE);


  return 0;
}*/