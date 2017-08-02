#include <string>
#include <list>
#include <unordered_map>
#include <afxtempl.h>
#include "BufPacket.h"
#include "DasBootSocket.h"
#include "DasBootBasicRepl.h"


using namespace std;

#pragma comment(lib, "ws2_32.lib")

typedef int(*pfnDBExportClient)(const char *strParam, int nMsgLength);

typedef int(*pfnPluginInterfaceClient)(unordered_map<string, pfnDBExportClient>& mapFunctions);


DWORD RecvThreadFunc(LPVOID lpParam);
DWORD SendPktThreadProc(LPARAM lparam);

//ȫ�ֱ���
unordered_map<string, pfnDBExportClient> g_mapFunctions;
SOCKET g_hSocketClient = INVALID_SOCKET;
FILE *g_pCurrentFile = NULL;
int g_nFileChunckCount = 0;

CList<SOCKET, SOCKET> g_hSendListSockets;
CList<CBufPacket *, CBufPacket *> g_hSendListPkts;
int g_nSendListElementCount = 0;
CRITICAL_SECTION g_csSendListOperation;


int main()
{
//   printf("helloworld\r\n");
// 
//   HINSTANCE hDll = LoadLibrary("DasBootDllProcess.dll");
// 
//   if (hDll == NULL)
//   {
//     //TRACE(_T("DLL����ʧ��"));
// 
//     return 0;
//   }
// 
//   pfnPluginInterface pfnInterface = (pfnPluginInterface)GetProcAddress(hDll,
//     "dbplugin_load_dll");
// 
//   unordered_map<string, pfnDBExport> mapFunctions;
// 
//   pfnInterface(mapFunctions);
// 
//   pfnDBExport pfnHelloWorld;
// 
//   pfnHelloWorld = mapFunctions["EnumProcess00000"];
// 
//   list <string> lstMsg;
// 
//   string tmp = "���";
// 
//   pfnHelloWorld(tmp, lstMsg);
// 
//   getchar();

  //��ʼ��map
  AddBasicReplToMapFunctions();

  InitializeCriticalSection(&g_csSendListOperation);

  HANDLE hSendThread = CreateThread(NULL, 0,
    (LPTHREAD_START_ROUTINE)SendPktThreadProc, 0, 0, NULL);

  WORD wVersionRequested;
  WSADATA wsaData;
  int err;

  wVersionRequested = MAKEWORD(1, 1);
  err = WSAStartup(wVersionRequested, &wsaData);

  if (err != 0)
  {
    WSACleanup();

    return 0;
  }

  sockaddr_in addr;
  memset(&addr, 0, sizeof(sockaddr_in));

  g_hSocketClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (g_hSocketClient == INVALID_SOCKET)
  {
    return 0;
  }

  addr.sin_family = AF_INET;
  addr.sin_port = htons(8889);
  addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

  int nRet = connect(g_hSocketClient, (sockaddr*)&addr, sizeof(sockaddr_in));

  if (nRet == SOCKET_ERROR)
  {
    return 0;
  }

  HANDLE hRecvThread = CreateThread(NULL, 0, 
    (LPTHREAD_START_ROUTINE)RecvThreadFunc, (LPVOID)g_hSocketClient, 0, NULL);

  

//   closesocket(hSocketClient);
//   WSACleanup();

  WaitForSingleObject(hRecvThread, INFINITE);
  TerminateThread(hSendThread, 0);

  DeleteCriticalSection(&g_csSendListOperation);

  return 0;
}



//������������͵�����
DWORD RecvThreadFunc(LPVOID lpParam)
{
//   if (theApp.m_pMainWnd == NULL)
//   {
//     return 0;
//   }

  SOCKET sRecv = (SOCKET)lpParam;
//  CClientDlg* pDlg = (CClientDlg*)theApp.m_pMainWnd;

  while (TRUE)
  {
    CBufPacket RecvPkt;
    CDasBootSocket recvSkt(sRecv);

    int nRet = recvSkt.RecvPkt(RecvPkt);

    if (nRet == SOCKET_SUCCESS)
    {
      //��ʼ������յ��¼�
      //lock();
      //m_dwTick = GetTickCount();
      //unlock();

      char* pszBuf = RecvPkt.GetBuf();

      //printf(TEXT("%s\r\n"), pszBuf + 4);  //debug
      //printf(TEXT("%s\r\n"), pszBuf + 4 + 16);  //debug
      int nMsgLength = (int &)(*pszBuf);
      char szMsgType[16] = { 0 };
      strcpy(szMsgType, pszBuf + 4);

      pfnDBExportClient pfn;
      pfn = g_mapFunctions[szMsgType];
      pfn(pszBuf + 4 + 16, nMsgLength);
    }
    else
    {
      //�Ͽ�����,�ͷ���Դ

    }

  }

  return 0;
}

DWORD SendPktThreadProc(LPARAM lparam)
{
  while (true)
  {
    if (g_nSendListElementCount != 0)
    {
      if (g_hSendListPkts.GetCount() != 0)
      {
        EnterCriticalSection(&g_csSendListOperation);

        SOCKET hSocket = g_hSendListSockets.GetTail();
        CBufPacket *pTmpPkt = g_hSendListPkts.GetTail();
        CDasBootSocket sendSkt(hSocket);
        sendSkt.SendData((*pTmpPkt).GetBuf(), (*pTmpPkt).GetLength());
        delete pTmpPkt;
        g_hSendListSockets.RemoveTail();
        g_hSendListPkts.RemoveTail();
        g_nSendListElementCount--;

        LeaveCriticalSection(&g_csSendListOperation);
      }
    }

    Sleep(50);  //30���ϱ���
  }

  return 0;
}