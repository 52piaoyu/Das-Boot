#include <string>
#include <list>
#include <unordered_map>
#include <windows.h>
#include <Tlhelp32.h>

using namespace std;

typedef int(*pfnDBExport)(string &strParam, list<string>& lstMsgToSend);

typedef int(*pfnPluginInterface)(unordered_map<string, pfnDBExport>& mapFunctions);

extern "C" {
  __declspec(dllexport) int dbplugin_load_dll(unordered_map<string, pfnDBExport>& mapFunctions);
}

int db_enum_process(string &strParam, list<string>& lstMsgToSend)
{
  HANDLE hSnapshort = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  PROCESSENTRY32 pe32 = { 0 };
  pe32.dwSize = sizeof(PROCESSENTRY32);

  if (Process32First(hSnapshort, &pe32))
  {
    do
    {
      TCHAR buf[MAXBYTE] = { 0 };

      wsprintf(buf, "���̵�ַ %s ����ID %d ���̸�ID %d", pe32.szExeFile, pe32.th32ProcessID, pe32.th32ParentProcessID);

      printf("%s\r\n", buf);
        
    } while (Process32Next(hSnapshort, &pe32));

  }

  CloseHandle(hSnapshort);

  return 0;
}

//���е�dll�����У����ҵ����˺���
int dbplugin_load_dll(unordered_map<string, pfnDBExport>& mapFunctions)
{
  mapFunctions["EnumProcess00000"] = (pfnDBExport)db_enum_process;

  //TRACE(_T("��dll�ڵ��������к������سɹ�"));
  return 0;
}
