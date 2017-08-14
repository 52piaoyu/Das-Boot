#include "stdafx.h"

#include "FileTree.h"
#include "resource.h"


CFileTree::CFileTree()
{
  m_clText = RGB(58, 79, 107);
  m_clBk = RGB(234, 242, 255);
  m_clLine = RGB(58, 79, 107);

  m_nFlags = 0;

}

CFileTree::~CFileTree()
{

}


BEGIN_MESSAGE_MAP(CFileTree, CTreeCtrl)
  //{{AFX_MSG_MAP(CFileTree)
  ON_WM_LBUTTONDOWN()
  //}}AFX_MSG_MAP
  ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemExpanded)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileTree message handlers

void CFileTree::PreSubclassWindow()
{
  CTreeCtrl::PreSubclassWindow();
  m_StateImages.Create(12, 14, ILC_COLOR24 | ILC_MASK, 3, 1);

  CBitmap bmp;
  bmp.LoadBitmap(IDB_NOMAL1);
  m_StateImages.Add(&bmp, RGB(255, 255, 255));
  bmp.DeleteObject();

  bmp.LoadBitmap(IDB_NOMAL2);
  m_StateImages.Add(&bmp, RGB(255, 255, 255));
  bmp.DeleteObject();

  bmp.LoadBitmap(IDB_CHECKED);
  m_StateImages.Add(&bmp, RGB(255, 255, 255));
  bmp.DeleteObject();

  bmp.LoadBitmap(IDB_FULLCHECKED);
  m_StateImages.Add(&bmp, RGB(255, 255, 255));
  bmp.DeleteObject();

  //�����ı���ɫ
  SetTextColor(m_clText);
  //���ñ�����ɫ
  SetBkColor(m_clBk);
  //����������ɫ
  //SendMessage(TV_FIRST + 40, 0, (LPARAM)m_clLine);
  SetLineColor(m_clLine);
  LoadDiskName();
}

BOOL CFileTree::SetItemState(HTREEITEM hItem, UINT nState, UINT nStateMask, BOOL bSearch)
{
  BOOL bRet = CTreeCtrl::SetItemState(hItem, nState, nStateMask);
  UINT nStatus = nState >> 12;
  if (nStatus != 0)
  {
    if (bSearch)
      RansackChild(hItem, nStatus);
    RansackParentAndChild(hItem, nStatus);
  }
  return bRet;
}

void CFileTree::RansackChild(HTREEITEM hItem, UINT nState)
{
  HTREEITEM hChildItem, hBrotherItem;
  //�����ӽڵ�
  hChildItem = GetChildItem(hItem);
  if (hChildItem != NULL)
  {
    //�������ӽڵ��״̬�����븸�ڵ���ͬ
    CTreeCtrl::SetItemState(hChildItem, INDEXTOSTATEIMAGEMASK(nState), TVIS_STATEIMAGEMASK);
    //�ٵݹ鴦���ӽڵ���ӽڵ�
    RansackChild(hChildItem, nState);
    //�����ӽڵ���ֵܽڵ�
    hBrotherItem = GetNextSiblingItem(hChildItem);
    while (hBrotherItem)
    {
      //�����ӽڵ���ֵܽڵ�״̬�뵱ǰ�ڵ��״̬һ��
      int nStatus = GetItemState(hBrotherItem, TVIS_STATEIMAGEMASK) >> 12;
      if (nStatus != 0)
      {
        CTreeCtrl::SetItemState(hBrotherItem, INDEXTOSTATEIMAGEMASK(nState), TVIS_STATEIMAGEMASK);
      }
      //�ٵݹ鴦���ֵܽڵ�
      RansackChild(hBrotherItem, nState);
      hBrotherItem = GetNextSiblingItem(hBrotherItem);
    }
  }
}

void CFileTree::RansackParentAndChild(HTREEITEM hItem, UINT nState)
{
  HTREEITEM hNextItem, hPrevItem, hParentItem;
  //���Ҹ��ڵ㣬û�оͽ���
  hParentItem = GetParentItem(hItem);
  if (hParentItem != NULL)
  {
    UINT nStatus = nState;	//���ʼֵ����ֹû���ֵܽڵ�ʱ����
                            //���ҵ�ǰ�ڵ�������ֵܽڵ�,��������ֵܽڵ�״̬����ͬ,
                            //���ø��ڵ��״̬
                            //���ҵ�ǰ�ڵ�������ֵܽڵ��״̬
    hNextItem = GetNextSiblingItem(hItem);
    while (hNextItem != NULL)
    {
      nStatus = GetItemState(hNextItem, TVIS_STATEIMAGEMASK) >> 12;
      if (nStatus != nState && nStatus != 0)
        break;
      else
        hNextItem = GetNextSiblingItem(hNextItem);
    }
    if (nStatus == nState)
    {
      //���ҵ�ǰ�ڵ�������ֵܽڵ��״̬
      hPrevItem = GetPrevSiblingItem(hItem);
      while (hPrevItem != NULL)
      {
        nStatus = GetItemState(hPrevItem, TVIS_STATEIMAGEMASK) >> 12;
        if (nStatus != nState && nStatus != 0)
          break;
        else
          hPrevItem = GetPrevSiblingItem(hPrevItem);
      }
    }
    if (nStatus == nState || nStatus == 0)
    {
      nStatus = GetItemState(hParentItem, TVIS_STATEIMAGEMASK) >> 12;
      if (nStatus != 0)
      {
        //���״̬һ�£��򸸽ڵ��״̬�뵱ǰ�ڵ��״̬һ��
        CTreeCtrl::SetItemState(hParentItem, INDEXTOSTATEIMAGEMASK(nState), TVIS_STATEIMAGEMASK);
      }
      //�ٵݹ鴦���ڵ���ֵܽڵ���丸�ڵ�
      RansackParentAndChild(hParentItem, nState);
    }
    else
    {
      //״̬��һ�£���ǰ�ڵ�ĸ��ڵ㡢���ڵ�ĸ��ڵ㡭��״̬��Ϊ����̬
      hParentItem = GetParentItem(hItem);
      while (hParentItem != NULL)
      {
        nStatus = GetItemState(hParentItem, TVIS_STATEIMAGEMASK) >> 12;
        if (nStatus != 0)
        {
          CTreeCtrl::SetItemState(hParentItem, INDEXTOSTATEIMAGEMASK(2), TVIS_STATEIMAGEMASK);
        }
        hParentItem = GetParentItem(hParentItem);
      }
    }
  }
}

void CFileTree::OnLButtonDown(UINT nFlags, CPoint point)
{
  HTREEITEM hItemInfo = HitTest(point, &m_nFlags);
  nFlags = m_nFlags;
  //TVHT_ONITEMSTATEICON��ʾ�û��������ͼ���ͼ��״̬
  if (m_nFlags & TVHT_ONITEMSTATEICON)
  {
    //State: 0��״̬ 1û��ѡ�� 2����ѡ�� 3ȫ��ѡ��
    //12��15λ��ʾ��ͼ���ͼ��״̬����
    UINT nState = GetItemState(hItemInfo, TVIS_STATEIMAGEMASK) >> 12;
    nState = (nState == 3) ? 1 : 3;
    SetItemState(hItemInfo, INDEXTOSTATEIMAGEMASK(nState), TVIS_STATEIMAGEMASK);
  }
  else
    CTreeCtrl::OnLButtonDown(nFlags, point);
}

BOOL CFileTree::LoadSysFileIcon()
{
  SHFILEINFO shInfo;							//�����ļ���Ϣ
  memset(&shInfo, 0, sizeof(SHFILEINFO));		//��ʼ���ļ���Ϣ
  m_hImage = (HIMAGELIST)SHGetFileInfo("c:\\", 0, &shInfo, sizeof(SHFILEINFO),
    SHGFI_SYSICONINDEX | SHGFI_SMALLICON);	//��ȡϵͳ�ļ�ͼ��
  if (m_hImage != NULL)
  {
    BOOL bRet = m_ImageList.Attach(m_hImage);	//����ͼ���б��� 
    if (bRet)
    {
      HICON hIcon = NULL;
      hIcon = ExtractIcon(NULL, "explorer.exe", 0);
      //����ҵĵ���ͼ��
      int nIcon = m_ImageList.Add(hIcon);
      m_ImageList.SetBkColor(m_clBk);
      SetImageList(&m_ImageList, TVSIL_NORMAL);
      //����״̬ͼ���б�
      SetImageList(&m_StateImages, TVSIL_STATE);
    }
    return	bRet;
  }
  return FALSE;
}

void CFileTree::LoadDiskName()
{
  if (LoadSysFileIcon() == TRUE)	//���ȼ���ϵͳ�ļ�ͼ���б�
  {
    //��Ӹ��ڵ�
    HTREEITEM hRoot = AddItem(TVI_ROOT, "�ҵĵ���");
    char  pchDrives[128] = { 0 };
    char* pchDrive;
    GetLogicalDriveStrings(sizeof(pchDrives), pchDrives);
    pchDrive = pchDrives;
    while (*pchDrive)
    {
      HTREEITEM hParent = AddItem(hRoot, pchDrive);
      if (IsSubDir(pchDrive))
        InsertItem("", 0, 0, hParent);
      pchDrive += strlen(pchDrive) + 1;
    }
  }
}


void CFileTree::ShowPath(HTREEITEM hParent, LPCTSTR lpszPath)
{
  CFileFind flFind;
  CString   csPath = lpszPath;
  BOOL      bFind;
  CSortString strDirArray;
  CSortString strFileArray;

  if (csPath.Right(1) != "\\")
    csPath += "\\";
  csPath += "*.*";

  bFind = flFind.FindFile(csPath);
  while (bFind)
  {
    bFind = flFind.FindNextFile();
    if (flFind.IsDirectory() && !flFind.IsDots())
    {
      strDirArray.Add(flFind.GetFilePath());
    }
    if (!flFind.IsDirectory())
      strFileArray.Add(flFind.GetFilePath());
  }
  strDirArray.Sort();
  SetRedraw(FALSE);
  CWaitCursor wait;
  for (int i = 0; i < strDirArray.GetSize(); i++)
  {
    HTREEITEM hItem = AddItem(hParent, strDirArray.GetAt(i));
    UINT nState = GetItemState(hParent, TVIS_STATEIMAGEMASK) >> 12;
    SetItemState(hItem, INDEXTOSTATEIMAGEMASK(nState), TVIS_STATEIMAGEMASK);
    if (IsSubDir(strDirArray.GetAt(i)))
    {
      InsertItem("", 0, 0, hItem);
    }
  }
  strFileArray.Sort();
  for (int i = 0; i < strFileArray.GetSize(); i++)
  {
    HTREEITEM hItem = AddItem(hParent, strFileArray.GetAt(i));
    UINT nState = GetItemState(hParent, TVIS_STATEIMAGEMASK) >> 12;
    SetItemState(hItem, INDEXTOSTATEIMAGEMASK(nState), TVIS_STATEIMAGEMASK);
  }
  SetRedraw(TRUE);
}

CString CFileTree::GetFullPath(HTREEITEM hItem)
{
  CString csRet;	//��¼Ŀ¼
  CString csCurDir;
  HTREEITEM hParent = hItem;
  csRet = "";
  //�������ڵ�
  HTREEITEM hRoot = GetRootItem();
  while (hParent != hRoot)
  {
    csCurDir = GetItemText(hParent);
    csCurDir += "\\";
    csRet = csCurDir + csRet;
    hParent = GetParentItem(hParent);
  }
  //��ȥ��β��"\"����
  if (csRet.Right(1) == '\\')
    csRet.SetAt(csRet.GetLength() - 1, '\0');
  return csRet;
}

HTREEITEM CFileTree::AddItem(HTREEITEM hParent, LPCTSTR lpszPath)
{
  SHFILEINFO shFinfo;
  int nIcon, nIconSel;
  CString    csPath = lpszPath;
  if (csPath.Right(1) != '\\')
    csPath += "\\";
  if (hParent == TVI_ROOT)
  {
    nIcon = nIconSel = m_ImageList.GetImageCount() - 1;
  }
  else
  {
    //��ȡĿ¼���ļ���ϵͳͼ������
    if (!SHGetFileInfo(csPath, 0, &shFinfo, sizeof(shFinfo), SHGFI_ICON | SHGFI_SMALLICON))
    {
      return NULL;
    }
    nIcon = shFinfo.iIcon;
    //��ȡĿ¼���ļ�ѡ��ʱ��ϵͳͼ������
    if (!SHGetFileInfo(csPath, 0, &shFinfo, sizeof(shFinfo),
      SHGFI_ICON | SHGFI_OPENICON | SHGFI_SMALLICON))
    {
      return NULL;
    }
    nIconSel = shFinfo.iIcon;
  }
  if (csPath.Right(1) == "\\")
    csPath.SetAt(csPath.GetLength() - 1, '\0');
  return InsertItem(ExtractPath(csPath), nIcon, nIcon, hParent);
}

CString CFileTree::ExtractPath(LPCTSTR lpszPath)
{
  CString csPath = "";
  int nPos;
  csPath = lpszPath;
  //��ȥ��β��"\"
  if (csPath.Right(1) == '\\')
  {
    csPath.SetAt(csPath.GetLength() - 1, '\0');
  }
  nPos = csPath.ReverseFind('\\');
  if (nPos != -1)
    csPath = csPath.Mid(nPos + 1, csPath.GetLength());
  return (LPCTSTR)csPath;
}

BOOL CFileTree::IsSubDir(LPCTSTR lpszPath)
{
  CFileFind flFind;
  CString csPath = lpszPath;
  BOOL bFind = FALSE;
  //��֤·����\*.*��β
  if (_tcslen(lpszPath) == 3)
  {
    if (lpszPath[1] == ':' && lpszPath[2] == '\\')
      csPath += "*.*";
    else
      csPath += "\\*.*";
  }
  else
  {
    csPath += "\\*.*";
  }
  bFind = flFind.FindFile(csPath);
  while (bFind)
  {
    bFind = flFind.FindNextFile();
    if (!flFind.IsDots())
    {
      return TRUE;
    }
  }
  return FALSE;
}
void CFileTree::OnItemExpanded(NMHDR *pNMHDR, LRESULT *pResult)
{
  NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
  CString strPath;

  if (pNMTreeView->itemNew.state & TVIS_EXPANDED)
  {
    ExpandItem(pNMTreeView->itemNew.hItem, TVE_EXPAND);
  }
  else
  {
    if (pNMTreeView->itemNew.hItem != GetRootItem())
    {
      int nData = GetItemData(pNMTreeView->itemNew.hItem);
      if (nData != LOADDIR)
      {
        HTREEITEM hChild = GetChildItem(pNMTreeView->itemNew.hItem);
        while (hChild)
        {
          DeleteItem(hChild);
          hChild = GetChildItem(pNMTreeView->itemNew.hItem);
        }
        InsertItem("", pNMTreeView->itemNew.hItem);
        SetItemData(pNMTreeView->itemNew.hItem, LOADDIR);
      }
    }
  }
  *pResult = 0;
}


void CFileTree::ExpandItem(HTREEITEM hItem, UINT nCode)
{
  CString strPath;
  if (nCode == TVE_EXPAND)
  {
    if (hItem != GetRootItem())
    {
      int nData = GetItemData(hItem);
      if (nData != LOADDIR)				//δ������Ŀ¼����ʼ������Ŀ¼
      {
        HTREEITEM hChild = GetChildItem(hItem);
        DeleteItem(hChild);
        strPath = GetFullPath(hItem);
        ShowPath(hItem, strPath);
        SetItemData(hItem, LOADDIR);	//�Ѽ���Ŀ¼
      }
      else								//�Ѽ���Ŀ¼���ж��Ƿ����µ�Ŀ¼���ļ���Ҫ����
      {
        CString csPath = "";
        csPath = GetFullPath(hItem);
        char chDir[MAX_PATH] = { 0 };
        strcpy(chDir, csPath);
        //��ȡ�µ�Ŀ¼��Ϣ
        CFileFind flFind;
        BOOL      bFind;
        CSortString strDirArray;
        strcat(chDir, "\\*.*");

        bFind = flFind.FindFile(chDir);
        while (bFind)
        {
          bFind = flFind.FindNextFile();
          if (!flFind.IsDots())
          {
            strDirArray.Add(flFind.GetFilePath());
          }
        }
        strDirArray.Sort();
        //��ȡ��ǰ�ڵ��µ���Ŀ¼
        CSortString csNodeArray;
        HTREEITEM hChild = GetChildItem(hItem);
        while (hChild)
        {
          csNodeArray.Add(GetItemText(hChild));
          hChild = GetNextItem(hChild, TVGN_NEXT);
        }
        csNodeArray.Sort();
        int nSize = csNodeArray.GetSize();
        int *pArray = new int[nSize];
        memset(pArray, -1, nSize);
        int nCount = 0;
        //�����̵�Ŀ¼��Ϣ��ڵ���Ϣ����ƥ�䣬���Ҳ���Ŀ¼�����ڵ���û����ʾ���µĴ���Ŀ¼��ӵ��ڵ���
        //���ڵ����д���Ŀ¼�����Ľڵ�Ŀ¼ɾ��
        for (int i = 0; i < strDirArray.GetSize(); i++)
        {
          BOOL bExist = FALSE;
          for (int j = 0; j < nSize; j++)
          {
            CString csDisk = ExtractPath(strDirArray.GetAt(i));
            CString csNode = csNodeArray.GetAt(j);
            if (csDisk == csNode)	//�ڵ���Ŀ¼����ƥ��
            {
              bExist = TRUE;
              pArray[nCount] = j;	//��¼ƥ���Ԫ��
              nCount++;
              break;
            }
          }
          if (!bExist)				//Ŀ¼������,�ڽ�β���Ŀ¼
          {

            HTREEITEM hNode = AddItem(hItem, strDirArray.GetAt(i));
            //��ȡ���ڵ��״̬
            UINT nState = GetItemState(hItem, TVIS_STATEIMAGEMASK) >> 12;
            nState = (nState == 2) ? 3 : nState;
            SetItemState(hNode, INDEXTOSTATEIMAGEMASK(nState), TVIS_STATEIMAGEMASK);
          }
        }
        //��csNodeArray�в��Ҳ����ڵ�Ŀ¼
        int *pNoMatch = new int[nSize];
        memset(pNoMatch, -1, nSize);
        int nMatchCounter = 0;
        for (int nIndex = 0; nIndex < nSize; nIndex++)
        {
          BOOL bMatched = FALSE;
          for (int nID = 0; nID < nCount; nID++)
          {
            if (nIndex == pArray[nID])
            {
              bMatched = TRUE;
              break;
            }
          }
          if (!bMatched)
          {
            pNoMatch[nMatchCounter] = nIndex;
            nMatchCounter++;
          }
        }
        if (nMatchCounter > 0)	//ɾ���ڵ��в����ڵ�Ŀ¼��Ϣ
        {
          for (int i = 0; i < nMatchCounter; i++)
          {
            HTREEITEM hChildItem = FindNode(hItem, csNodeArray.GetAt(pNoMatch[i]));
            if (hChildItem)
            {

              UINT nState = GetItemState(hChildItem, TVIS_STATEIMAGEMASK) >> 12;
              nState = (nState == 3) ? 1 : 3;
              DeleteItem(hChildItem);
              SetItemState(hItem, INDEXTOSTATEIMAGEMASK(nState), TVIS_STATEIMAGEMASK);
            }
          }
        }
        delete[] pArray;
        delete[] pNoMatch;
      }
    }
  }
}

HTREEITEM CFileTree::FindNode(HTREEITEM hParant, LPCTSTR lpszName)
{
  HTREEITEM hChild = GetChildItem(hParant);
  CString csText = GetItemText(hChild);
  if (lpszName == csText)
  {
    return hChild;
  }
  while (hChild)
  {
    hChild = GetNextItem(hChild, TVGN_NEXT);
    csText = GetItemText(hChild);
    if (lpszName == csText)
    {
      return hChild;
    }
  }
  return NULL;
}


