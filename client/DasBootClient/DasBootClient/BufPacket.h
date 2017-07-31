#pragma once
#include <windows.h>

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

