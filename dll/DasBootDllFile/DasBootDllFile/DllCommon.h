#pragma once

#include "stdafx.h"
#include <string>
#include <unordered_map>

class CClientOperationMainWindow;
class CClientContext;

#define MSG_ELEMENT_DELIM '\1'

typedef int(*pfnSendMessageOut)(SOCKET hSocket, const char * szType, 
  const char * szContent, DWORD dwContentLength);

typedef int(*pfnDBExportClient)(const char *strParam, int nMsgLength, 
  SOCKET hSocketClient, pfnSendMessageOut pfnSMO);
typedef int(*pfnPluginInterfaceClient)(std::unordered_map<std::string, 
  pfnDBExportClient>& mapFunctions);

typedef int(*pfnDBExportServerOperationConfirm)(SOCKET hSocket, 
  CClientOperationMainWindow *pParentWnd, CClientContext* pClientContext, 
  pfnSendMessageOut pfnSMO);
typedef int(*pfnDBExportServer)(const char *strParam, SOCKET hSocket, 
  int nMsgLength, CClientContext* pClientContext, pfnSendMessageOut pfnSMO);
typedef int(*pfnPluginInterfaceServer)(std::unordered_map<std::string, 
  pfnDBExportServerOperationConfirm>& mapConfirmFunctions, 
  std::unordered_map<std::string, pfnDBExportServer>& mapFunctions,
  std::unordered_map<std::string, std::string>& mapDllIntroduction);

//��������������Ҫ���͵����ݣ�����������������ʱ���
class CBufPacket
{
public:
#define BUF_SIZE 256
#define BUF_FACTOR 2
#define WM_PKTHANDLEMSG WM_USER + 1

public:
  CBufPacket();
  virtual ~CBufPacket();

  CBufPacket(CBufPacket& pkt);
  void operator=(CBufPacket& pkt);

  BOOL InitBuf();
  BOOL ReleaseBuf();

  BOOL SetBuf(char* pszBuf, int nBufSize, int nLength);

  char* GetBuf(); //��ȡ������
  int GetLength(); //��ȡ����������

  BOOL Append(char* pBuf, int nLength);//��������ӵ���������

  BOOL Handle(SOCKET s);  //�Դ���

private:
  char* m_pchBuf;    //������
  int   m_nSize;   //��������С
  int   m_nLength; //�����������ݵ�ʵ�ʳ���
};

typedef BOOL(*PFNTransferMsg)(SOCKET s, CString& str);

#define RECV_BYTES 256 * 1024

//��ʾ�ͻ���������
struct CClientContext
{
  CClientContext(SOCKET s, sockaddr_in& addr)
  {
    m_sClient = s;
    memcpy(&m_addr, &addr, sizeof(sockaddr_in));

    m_pMyClientOperationMainWindow = NULL;
  }

  BOOL DoSomething(PFNTransferMsg pfn, CString& str)
  {
    if (pfn != NULL)
    {
      return pfn(m_sClient, str);
    }

    return FALSE;
  }

  SOCKET  m_sClient; //��ʾ�ͻ��˵�socket
  sockaddr_in m_addr;//��ʾ�ͻ��˵�ip�Ͷ˿�
  DWORD     m_dwTick;  //��ʾ�ͻ������һ�θ���ʱ��
  CBufPacket m_Packet; //������ɾ�Ĳ飨ÿ��clientӵ���Լ���BufPacket�����ڴ洢���յ����ݣ�
                       //���͵�ʱ������������С������û��Ҫ��ÿ��client����������pkt
  char      m_szRecvBuf[RECV_BYTES]; //����ÿ�ν���(recv()����)����ʱ������

  FILE *m_pCurrentFile = NULL;
  int m_nFileChunckCount = 0;

  CClientOperationMainWindow *m_pMyClientOperationMainWindow;
  std::unordered_map<std::string, CDialog*> m_mapClientOperationSubWindows;
};

/*
class CClientManager
{
public:
  CClientManager();
  virtual ~CClientManager();

  //��
  BOOL Add(SOCKET s, CClientContext* pClient);

  //ɾ
  BOOL Delete(SOCKET s);

  //��
  BOOL FindAt(SOCKET s, CClientContext*& pClient);

  BOOL IterateAndDoSomething(PFNTransferMsg pfn, CString& str);

private:
  CMap<SOCKET, SOCKET&, CClientContext*, CClientContext*> m_Map;
};*/

