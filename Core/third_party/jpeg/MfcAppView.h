// MfcAppView.h : interface of the MfcAppView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MFCAPPVIEW_H__7B4DD98F_945B_11D2_815A_444553540000__INCLUDED_)
#define AFX_MFCAPPVIEW_H__7B4DD98F_945B_11D2_815A_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class MfcAppView : public View
{
protected: // create from serialization only
	MfcAppView();
	DECLARE_DYNCREATE(MfcAppView)

// Attributes
public:
	MfcAppDoc* GetDocument();

// Operations
public:

	// global image params
	BYTE *m_buf;
	UINT m_width;
	UINT m_height;
	UINT m_widthDW;

	// draw what we've loaded
	void DrawBMP();
	
	// jpg load save
	void LoadJPG(String fileName);
	void SaveJPG(String filename, BOOL color);

	void LoadBMP(String fileName);
	void SaveBMP24(String fileName);

	// turn 24-bit to 256-color gray scale
	BYTE * MakeColormappedGrayscale(BYTE *inBuf,
								  UINT inWidth,
								  UINT inHeight,
								  UINT inWidthBytes,
								  UINT colors,
								  RGBQUAD* colormap);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MfcAppView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~MfcAppView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(DumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(MfcAppView)
	afx_msg void OnFileOpen();
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileSavecolormappedbmp();
	afx_msg void OnFileSavegrayas();
	afx_msg void OnFileGetdimensionsjpg();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in MfcAppView.cpp
inline MfcAppDoc* MfcAppView::GetDocument()
   { return (MfcAppDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFCAPPVIEW_H__7B4DD98F_945B_11D2_815A_444553540000__INCLUDED_)
