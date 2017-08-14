// ClientOperationSubWindowFile.cpp : implementation file
//

#include "stdafx.h"
#include "DasBootDllFile.h"
#include "ClientOperationSubWindowFile.h"
#include "afxdialogex.h"


// CClientOperationSubWindowFile dialog

IMPLEMENT_DYNAMIC(CClientOperationSubWindowFile, CDialogEx)

CClientOperationSubWindowFile::CClientOperationSubWindowFile(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_DLL_FILE, pParent)
{

}

CClientOperationSubWindowFile::~CClientOperationSubWindowFile()
{
}

void CClientOperationSubWindowFile::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_TREE_CLIENT_OPERATION_SUB_WINDOW_FILE_TREE, m_Tree);
}


BEGIN_MESSAGE_MAP(CClientOperationSubWindowFile, CDialogEx)
  ON_BN_CLICKED(IDC_BUTTON_CLIENT_OPERATION_SUB_WINDOW_FILE_START, &CClientOperationSubWindowFile::OnBnClickedButtonClientOperationSubWindowFileStart)
  ON_BN_CLICKED(IDC_BUTTON_CLIENT_OPERATION_SUB_WINDOW_FILE_SAVE_SINGLE_FILE, &CClientOperationSubWindowFile::OnBnClickedButtonClientOperationSubWindowFileSaveSingleFile)
  ON_BN_CLICKED(IDC_BUTTON_CLIENT_OPERATION_SUB_WINDOW_FILE_SAVE_MULT_FILES, &CClientOperationSubWindowFile::OnBnClickedButtonClientOperationSubWindowFileSaveMultFiles)
END_MESSAGE_MAP()


// CClientOperationSubWindowFile message handlers


void CClientOperationSubWindowFile::OnBnClickedButtonClientOperationSubWindowFileStart()
{
  // TODO: Add your control notification handler code here
  m_pfnSMO(m_hSocket, _T("FileViewStart0C"), _T("nothing"), -1);
  AfxMessageBox(_T("ʱ�����ޣ�����û��������������ʵ�Ǳ��ص��ļ��������ǿͻ����ļ���"));

}


void CClientOperationSubWindowFile::OnBnClickedButtonClientOperationSubWindowFileSaveSingleFile()
{
  // TODO: Add your control notification handler code here
  CFileDialog dlg(FALSE);

  dlg.m_ofn.lpstrTitle = _T("���浥���ͻ����ļ�������");

  if (IDOK == dlg.DoModal())
  {
    AfxMessageBox(_T("ʱ�����ޣ�����û�����������Ժ�������"));
  }
}


void CClientOperationSubWindowFile::OnBnClickedButtonClientOperationSubWindowFileSaveMultFiles()
{
  // TODO: Add your control notification handler code here
  AfxMessageBox(_T("ʱ�����ޣ�����û�����������Ժ�������"));
}
