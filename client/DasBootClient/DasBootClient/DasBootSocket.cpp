#include "DasBootSocket.h"


CDasBootSocket::CDasBootSocket(SOCKET hSocket)
{
  m_Socket = hSocket;
}


CDasBootSocket::~CDasBootSocket()
{
}

int CDasBootSocket::SendPkt(CBufPacket& pkt)
{

  return SendData(pkt.GetBuf(), pkt.GetLength());
}

//undone ������������pkt����Ϣ���Ƚϱ�������ʱ����ô�á������Ĵ������������ݣ�������
//CBufPacket�����Append()��Handle()����������˵��������Ҫ��һ������������ʱ����ô
//���š�
//�˺�����ֻ��������С�������ݵĽ��ܣ�����Client�����Server�ظ���Ϣ�������ܴ������
//�������ݡ�
//���仰˵�����������ֻ������û����������ģ�͵Ŀͻ��˳������ڽ��м򵥵ؽ�����Ϣ��
int CDasBootSocket::RecvPkt(CBufPacket& pkt)
{
  char szHead[5] = { 0 };

  //���Ƚ���4���ֽڵ�ͷ��,��ʾ��ǰ���ݰ��Ĵ�С
  int nRet = RecvData(szHead, 4);
  if (nRet != SOCKET_SUCCESS)
  {
    return nRet;
  }
   
  //��ʾ�Ѿ��ɹ��Ľ�����5���ֽڵ�ͷ��.
  int nRequreLength = *(DWORD*)(szHead);


  char* pszBuf = new char[nRequreLength];
  if (pszBuf == NULL)
  {
    return SOCKET_ERROR;
  }

  (DWORD&)*pszBuf = nRequreLength;

  nRet = RecvData(pszBuf + 4, nRequreLength - 4);

  if (nRet == 1)
  {
    pkt.SetBuf(pszBuf, nRequreLength, nRequreLength);
    return SOCKET_SUCCESS;
  }
  else
  {
    delete pszBuf;
  }

  return SOCKET_ERROR;
}

//����ָ����С������
int CDasBootSocket::SendData(const char* pszData, int nLength)
{
  if (pszData == NULL)
  {
    return 0;
  }

  int nTotalSentBytes = 0;
  while (nTotalSentBytes < nLength)
  {
    int nSendedBytes = send(m_Socket, pszData + nTotalSentBytes,
      nLength - nTotalSentBytes, 0);

    if (nSendedBytes == SOCKET_ERROR)
    {
      return nSendedBytes;
    }

    nTotalSentBytes += nSendedBytes;
  }

  return SOCKET_SUCCESS;
}

//����ָ����С������
int CDasBootSocket::RecvData(char* pszData, int nLength)
{
  if (pszData == NULL)
  {
    return SOCKET_ERROR;
  }

  int nTotalRecvedBytes = 0;
  while (nTotalRecvedBytes < nLength)
  {
    int nRecvedBytes = recv(m_Socket, pszData + nTotalRecvedBytes,
      nLength - nTotalRecvedBytes, 0);

    if (nRecvedBytes == SOCKET_ERROR)
    {
      return nRecvedBytes;
    }
    else if (nRecvedBytes == 0) //��ʾ�Է�"����"�ر���socket
    {
      return 0;
    }

    nTotalRecvedBytes += nRecvedBytes;
  }

  return SOCKET_SUCCESS;
}