#include "DasBootBasicRepl.h"
#include "BufPacket.h"
#include "DasBootSocket.h"
#include <string>
#include <unordered_map>
#include <windows.h>
#include <wininet.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>

#define IP_STRING_LENGTH 16
#define USER_NAME_LENGTH 200
#define RUNNING_TIME_LENGTH 200
#define OS_INFO_LENGTH 256

//using namespace std;
#pragma comment(lib, "Advapi32.lib") //for GetUserName()
#pragma comment(lib, "wininet.lib")  //for InternetOpen()
#pragma comment(lib, "User32.lib")  //for GetOSDisplayString()

static int DBBRClientListInfoQueryR(const char *strParam);
static DWORD GetExternalIPThreadFunc(LPVOID lpParam);
static BOOL GetOSDisplayString(LPTSTR pszOS);

typedef int(*pfnDBExportClient)(const char *strParam);

extern std::unordered_map<std::string, pfnDBExportClient> g_mapFunctions;
extern SOCKET g_hSocketClient;

int SendMessageOut(SOCKET hSocket, const char * szType, const char * szContent)
{
  CDasBootSocket sendSkt(hSocket);
  CBufPacket pkt;

  char szHead[4] = { 0 };
  DWORD dwSize = sizeof(DWORD) + 16 + strlen(szContent) + 1;
  (DWORD&)*szHead = dwSize;
  pkt.Append(szHead, sizeof(szHead));
  pkt.Append((char *)szType, 16);
  pkt.Append((char *)szContent, strlen(szContent) + 1);

  sendSkt.SendPkt(pkt);

  return 0;
}

int AddBasicReplToMapFunctions()
{
  g_mapFunctions["ClientListInfoC"] = (pfnDBExportClient)DBBRClientListInfoQueryR;

  return 0;
}

//   LPTSTR lpszSystemInfo;
//   DWORD cchBuff = 256;       
//   TCHAR tchBuffer2[256];
// 
//   lpszSystemInfo = tchBuffer2;
// 
//   int i = GetUserName(lpszSystemInfo, &cchBuff);

int DBBRClientListInfoQueryR(const char *strParam)
{
  //��ȡ�������ⲿIP������server����ȷ�����ĵ���λ��
  char szIPBuf[IP_STRING_LENGTH];
  HANDLE hGetExternalIPThread = CreateThread(NULL, 0,
    (LPTHREAD_START_ROUTINE)GetExternalIPThreadFunc, (LPVOID)szIPBuf, 0, NULL);

  //��ȡ��ǰ����ϵͳ
  char szOSInfo[OS_INFO_LENGTH];
  GetOSDisplayString(szOSInfo);

  //��ȡ��ǰ�û���
  char szUserNameBuf[USER_NAME_LENGTH];
  DWORD size = USER_NAME_LENGTH;
  int i = GetUserName((LPSTR)szUserNameBuf, &size);

  //��ȡ����������ʱ��
  DWORD time = GetTickCount();
  char szRunningTime[RUNNING_TIME_LENGTH];
  int n, hour, minute, second;

  n = time / 1000;
  second = n % 60;
  n = n / 60;
  minute = n % 60;
  hour = n / 60;

  sprintf(szRunningTime, "%d:%02d:%02d", hour, minute, second);

  //��10��ʱ����ⲿIP���鲻���Ļ���������ϣ�ֱ�Ӹ�һ��unkown-timeout
  DWORD nResult = WaitForSingleObject(hGetExternalIPThread, 10000); 
  if (nResult != WAIT_OBJECT_0)
  {
    TerminateThread(hGetExternalIPThread, 0);
    strcpy(szIPBuf, "unkown-timeout");
  }

  //�ܽ�
  char chSep = '\1';
  char szClientListInfoSum[IP_STRING_LENGTH + OS_INFO_LENGTH + USER_NAME_LENGTH
    + RUNNING_TIME_LENGTH];

  sprintf(szClientListInfoSum, "%s%c%s%c%s%c%s", szIPBuf, chSep, szOSInfo,
    chSep, szUserNameBuf, chSep, szRunningTime);

  SendMessageOut(g_hSocketClient, TEXT("ClientListInfoR"), szClientListInfoSum);

  return 0;
}

DWORD GetExternalIPThreadFunc(LPVOID lpParam)
{
  char *buffer = (char *)lpParam;

  HINTERNET net = InternetOpen("IP retriever",
    INTERNET_OPEN_TYPE_PRECONFIG,
    NULL,
    NULL,
    0);

  HINTERNET conn = InternetOpenUrl(net,
    "http://myexternalip.com/raw",  //"http://api.ipify.org", backup  
    NULL,
    0,
    INTERNET_FLAG_RELOAD,
    0);

  DWORD read;

  InternetReadFile(conn, buffer, IP_STRING_LENGTH, &read);
  buffer[read - 1] = '\0';
  InternetCloseHandle(net);

  return 0;
}



typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
typedef BOOL(WINAPI *PGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);

BOOL GetOSDisplayString(LPTSTR pszOS)
{
  OSVERSIONINFOEX osvi;
  SYSTEM_INFO si;
  PGNSI pGNSI;
  PGPI pGPI;
  BOOL bOsVersionInfoEx;
  DWORD dwType;

  ZeroMemory(&si, sizeof(SYSTEM_INFO));
  ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));

  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
  bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*)&osvi);

  if (!bOsVersionInfoEx) return 1;

  // Call GetNativeSystemInfo if supported or GetSystemInfo otherwise.

  pGNSI = (PGNSI)GetProcAddress(
    GetModuleHandle(TEXT("kernel32.dll")),
    "GetNativeSystemInfo");
  if (NULL != pGNSI)
    pGNSI(&si);
  else GetSystemInfo(&si);

  if (VER_PLATFORM_WIN32_NT == osvi.dwPlatformId &&
    osvi.dwMajorVersion > 4)
  {
    StringCchCopy(pszOS, OS_INFO_LENGTH, TEXT("Microsoft "));

    // Test for the specific product.

    if (osvi.dwMajorVersion == 6)
    {
      if (osvi.dwMinorVersion == 0)
      {
        if (osvi.wProductType == VER_NT_WORKSTATION)
          StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Windows Vista "));
        else StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Windows Server 2008 "));
      }

      if (osvi.dwMinorVersion == 1 || osvi.dwMinorVersion == 2)
      {
        if (osvi.wProductType == VER_NT_WORKSTATION && osvi.dwMinorVersion == 1)
          StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Windows 7 "));
        else
          if (osvi.wProductType == VER_NT_WORKSTATION && osvi.dwMinorVersion == 2)
            StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Windows 8 "));
          else
            StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Windows Server 2008 R2 "));
      }

      pGPI = (PGPI)GetProcAddress(
        GetModuleHandle(TEXT("kernel32.dll")),
        "GetProductInfo");

      pGPI(osvi.dwMajorVersion, osvi.dwMinorVersion, 0, 0, &dwType);

      switch (dwType)
      {
      case PRODUCT_ULTIMATE:
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Ultimate Edition"));
        break;
      case PRODUCT_PROFESSIONAL:
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Professional"));
        break;
      case PRODUCT_HOME_PREMIUM:
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Home Premium Edition"));
        break;
      case PRODUCT_HOME_BASIC:
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Home Basic Edition"));
        break;
      case PRODUCT_ENTERPRISE:
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Enterprise Edition"));
        break;
      case PRODUCT_BUSINESS:
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Business Edition"));
        break;
      case PRODUCT_STARTER:
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Starter Edition"));
        break;
      case PRODUCT_CLUSTER_SERVER:
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Cluster Server Edition"));
        break;
      case PRODUCT_DATACENTER_SERVER:
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Datacenter Edition"));
        break;
      case PRODUCT_DATACENTER_SERVER_CORE:
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Datacenter Edition (core installation)"));
        break;
      case PRODUCT_ENTERPRISE_SERVER:
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Enterprise Edition"));
        break;
      case PRODUCT_ENTERPRISE_SERVER_CORE:
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Enterprise Edition (core installation)"));
        break;
      case PRODUCT_ENTERPRISE_SERVER_IA64:
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Enterprise Edition for Itanium-based Systems"));
        break;
      case PRODUCT_SMALLBUSINESS_SERVER:
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Small Business Server"));
        break;
      case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Small Business Server Premium Edition"));
        break;
      case PRODUCT_STANDARD_SERVER:
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Standard Edition"));
        break;
      case PRODUCT_STANDARD_SERVER_CORE:
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Standard Edition (core installation)"));
        break;
      case PRODUCT_WEB_SERVER:
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Web Server Edition"));
        break;
      }
    }

    if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2)
    {
      if (GetSystemMetrics(SM_SERVERR2))
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Windows Server 2003 R2, "));
      else if (osvi.wSuiteMask & VER_SUITE_STORAGE_SERVER)
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Windows Storage Server 2003"));
      else if (osvi.wSuiteMask & VER_SUITE_WH_SERVER)
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Windows Home Server"));
      else if (osvi.wProductType == VER_NT_WORKSTATION &&
        si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
      {
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Windows XP Professional x64 Edition"));
      }
      else StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Windows Server 2003, "));

      // Test for the server type.
      if (osvi.wProductType != VER_NT_WORKSTATION)
      {
        if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
        {
          if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
            StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Datacenter Edition for Itanium-based Systems"));
          else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
            StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Enterprise Edition for Itanium-based Systems"));
        }

        else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
        {
          if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
            StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Datacenter x64 Edition"));
          else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
            StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Enterprise x64 Edition"));
          else StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Standard x64 Edition"));
        }

        else
        {
          if (osvi.wSuiteMask & VER_SUITE_COMPUTE_SERVER)
            StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Compute Cluster Edition"));
          else if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
            StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Datacenter Edition"));
          else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
            StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Enterprise Edition"));
          else if (osvi.wSuiteMask & VER_SUITE_BLADE)
            StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Web Edition"));
          else StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Standard Edition"));
        }
      }
    }

    if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1)
    {
      StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Windows XP "));
      if (osvi.wSuiteMask & VER_SUITE_PERSONAL)
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Home Edition"));
      else StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Professional"));
    }

    if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)
    {
      StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Windows 2000 "));

      if (osvi.wProductType == VER_NT_WORKSTATION)
      {
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Professional"));
      }
      else
      {
        if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
          StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Datacenter Server"));
        else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
          StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Advanced Server"));
        else StringCchCat(pszOS, OS_INFO_LENGTH, TEXT("Server"));
      }
    }

    // Include service pack (if any) and build number.

    if (_tcslen(osvi.szCSDVersion) > 0)
    {
      StringCchCat(pszOS, OS_INFO_LENGTH, TEXT(" "));
      StringCchCat(pszOS, OS_INFO_LENGTH, osvi.szCSDVersion);
    }

    TCHAR buf[80];

    StringCchPrintf(buf, 80, TEXT(" (build %d)"), osvi.dwBuildNumber);
    StringCchCat(pszOS, OS_INFO_LENGTH, buf);

    if (osvi.dwMajorVersion >= 6)
    {
      if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT(", 64-bit"));
      else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
        StringCchCat(pszOS, OS_INFO_LENGTH, TEXT(", 32-bit"));
    }

    return TRUE;
  }

  else
  {
    printf("This sample does not support this version of Windows.\n");
    return FALSE;
  }
}