#include "stdafx.h"

#include "DllCommon.h"

extern "C" {
  __declspec(dllexport) int DBLoadDLLClient(
    std::unordered_map<std::string, pfnDBExportClient>& mapFunctions);
}

static int DBDllFileViewStartR(const char *strParam, int nMsgLength, 
  SOCKET hSocketClient, pfnSendMessageOut pfnSMO);

//���е�dll�����У����ҵ����˺���
int DBLoadDLLClient(std::unordered_map<std::string, pfnDBExportClient>& mapFunctions)
{
  mapFunctions["FileViewStart0C"] = (pfnDBExportClient)DBDllFileViewStartR;

  return 0;
}

int DBDllFileViewStartR(const char *strParam, int nMsgLength, 
  SOCKET hSocketClient, pfnSendMessageOut pfnSMO)
{
  //undone


  //pfnSMO(hSocketClient, TEXT("FileViewStart0R"), TEXT("HelloWorld reply from client's dll. "), -1);

  return 0;
}