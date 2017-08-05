#pragma once
#include "BufPacket.h"
#include "ClientOperationMainWindow.h"
#include <string>
#include <unordered_map>

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
};

