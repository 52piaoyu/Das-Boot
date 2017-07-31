#include "stdafx.h"
#include "ClientManager.h"


CClientManager::CClientManager()
{
}


CClientManager::~CClientManager()
{
  SOCKET s = INVALID_SOCKET;
  CClientContext* pClient = NULL;

  POSITION pos = m_Map.GetStartPosition();

  while (pos)
  {
    m_Map.GetNextAssoc(pos, s, pClient);
    if (pClient != NULL)
    {
      delete pClient;
    }
  }

  m_Map.RemoveAll();
}

//��
BOOL CClientManager::Add(SOCKET s, CClientContext* pClient)
{

  m_Map.SetAt(s, pClient);

  return TRUE;
}

//ɾ
BOOL CClientManager::Delete(SOCKET s)
{
  CClientContext* pClient;

  m_Map.Lookup(s, pClient);

  if (pClient != NULL)
  {
    delete pClient;
  }

  m_Map.RemoveKey(s);

  return TRUE;
}

//��
BOOL CClientManager::FindAt(SOCKET s, CClientContext*& pClient)
{
  m_Map.Lookup(s, pClient);

  return TRUE;
}

//�������ɻ�
BOOL CClientManager::IterateAndDoSomething(PFNTransferMsg pfn, CString& str)
{
  SOCKET s = INVALID_SOCKET;
  CClientContext* pClient = NULL;

  POSITION pos = m_Map.GetStartPosition();

  while (pos)
  {
    m_Map.GetNextAssoc(pos, s, pClient);
    if (pClient != NULL)
    {
      //��ʼ�ɻ�
      pClient->DoSomething(pfn, str);
    }
  }

  return TRUE;
}