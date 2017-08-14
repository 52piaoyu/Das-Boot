#pragma once
#include "afxcmn.h"
// FileTree.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFileTree window

#define LOADDIR 2


class CSortString : public CStringArray
{
public:
  void Sort()
  {
    BOOL bRet = TRUE;
    while (bRet)
    {
      bRet = FALSE;
      for (int nPos = 0; nPos < GetUpperBound(); nPos++)
        bRet |= CompareString(nPos);
    }
  }
private:
  BOOL CompareString(int nPos)
  {
    CString csSwap;
    int nFirst = nPos;
    int nNext = nPos + 1;
    if (GetAt(nFirst).CompareNoCase(GetAt(nNext)) > 0)
    {
      csSwap = GetAt(nFirst);
      SetAt(nFirst, GetAt(nNext));
      SetAt(nNext, csSwap);
      return TRUE;
    }
    return FALSE;
  }
};


class CFileTree : public CTreeCtrl
{
  // Construction
public:
  CFileTree();
  UINT m_nFlags;
  HIMAGELIST m_hImage;
  CImageList m_ImageList;		//����ڵ�ͼ��
  CImageList m_StateImages;	//����״̬ͼ���б�
  COLORREF m_clBk;			//������ɫ
  COLORREF m_clLine;			//������ɫ
  COLORREF m_clText;			//�ı���ɫ
                          // Attributes
public:

  // Operations
public:

  // Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CFileTree)
protected:
  virtual void PreSubclassWindow();
  //}}AFX_VIRTUAL

  void OnItemExpanded(NMHDR* pNMHDR, LRESULT* pResult);
  // Implementation
public:
  HTREEITEM FindNode(HTREEITEM hParant, LPCTSTR lpszName);
  void ExpandItem(HTREEITEM hItem, UINT nCode);
  BOOL IsSubDir(LPCTSTR lpszPath);
  CString ExtractPath(LPCTSTR lpszPath);
  HTREEITEM AddItem(HTREEITEM hParent, LPCTSTR lpszPath);
  CString GetFullPath(HTREEITEM hItem);
  void ShowPath(HTREEITEM hParent, LPCTSTR lpszPath);
  void LoadDiskName();
  BOOL LoadSysFileIcon();
  void RansackParentAndChild(HTREEITEM hItem, UINT nState);
  void RansackChild(HTREEITEM hItem, UINT nState);
  BOOL SetItemState(HTREEITEM hItem, UINT nState, UINT nStateMask, BOOL bSearch = TRUE);
  virtual ~CFileTree();

  // Generated message map functions
protected:
  //{{AFX_MSG(CFileTree)
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  //}}AFX_MSG

  DECLARE_MESSAGE_MAP()
};
