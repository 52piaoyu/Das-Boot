#include "stdafx.h"
#include "BufPacket.h"

#include "DasBootServer.h"

CBufPacket::CBufPacket()
{
  m_pchBuf = NULL;
  m_nSize = 0;
  m_nLength = 0;

  InitBuf();
}


CBufPacket::~CBufPacket()
{
  ReleaseBuf();
}

CBufPacket::CBufPacket(CBufPacket& pkt)
{
  char* pNewBuf = NULL;
  m_nSize = pkt.m_nSize;
  m_nLength = pkt.m_nLength;

  //���
  if (m_nSize > 0)
  {
    pNewBuf = new char[m_nSize];
    memcpy(pNewBuf, pkt.m_pchBuf, m_nSize);
  }

  if (m_pchBuf != NULL)
  {
    delete[] m_pchBuf;
    m_pchBuf = NULL;
  }

  m_pchBuf = pNewBuf;
}

BOOL CBufPacket::InitBuf()
{
  if (m_pchBuf != NULL)
  {
    delete[] m_pchBuf;
    m_pchBuf = NULL;
  }

  m_pchBuf = new char[BUF_SIZE];

  if (m_pchBuf == NULL)
  {
    return FALSE;
  }

  m_nSize = BUF_SIZE;
  m_nLength = 0;

  return TRUE;
}

BOOL CBufPacket::ReleaseBuf()
{
  if (m_pchBuf != NULL)
  {
    delete[] m_pchBuf;
    m_pchBuf = NULL;
  }

  m_nSize = 0;
  m_nLength = 0;

  return TRUE;
}


BOOL CBufPacket::SetBuf(char* pszBuf, int nBufSize, int nLength)
{
  if (m_pchBuf != NULL)
  {
    delete[] m_pchBuf;
  }

  m_pchBuf = pszBuf;
  m_nSize = nBufSize;
  m_nLength = nLength;

  return TRUE;
}


//��ȡ������
char* CBufPacket::GetBuf() 
{
  return m_pchBuf;
}

//��ȡ����������
int CBufPacket::GetLength() 
{
  return m_nLength;
}

//��������ӵ���������
BOOL CBufPacket::Append(char* pBuf, int nLength)
{
  if (m_pchBuf == NULL)
  {
    return FALSE;
  }

  //���������Ȳ���
  if (m_nSize < m_nLength + nLength)
  {
    //���·���ռ�
    int nNewSize = (m_nLength + nLength) * BUF_FACTOR;
    char* pNewBuf = new char[nNewSize];

    //����ԭ�е�����
    memcpy(pNewBuf, m_pchBuf, m_nLength);

    delete[] m_pchBuf;
    m_nSize = nNewSize;
    m_pchBuf = pNewBuf;
  }

  //���������ȹ���
  memcpy(m_pchBuf + m_nLength, pBuf, nLength);
  m_nLength += nLength;

  return TRUE;
}

extern CDasBootServerApp theApp;

//�Դ���
BOOL CBufPacket::Handle(SOCKET s)
{
  //��黺�������ȹ�����
  if (m_nLength < 4)
  {
    return FALSE;
  }

  int nRequiredSize = *(int*)m_pchBuf;

  //������û�н�������,��Ȼ������
  if (m_nLength < nRequiredSize)
  {
    return FALSE;
  }

  //���봦��Ĵ���, ��������
  SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_PKTHANDLEMSG, (WPARAM)s,
    (LPARAM)m_pchBuf);

  //���ݴ������,��������ж��������,����ǰŲ��
  memmove(m_pchBuf, m_pchBuf + nRequiredSize, m_nLength - nRequiredSize);

  m_nLength -= nRequiredSize;

  return TRUE;
}