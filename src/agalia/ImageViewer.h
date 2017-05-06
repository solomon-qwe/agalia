#pragma once

// CImageViewer

class CImageViewer : public CWnd
{
	DECLARE_DYNAMIC(CImageViewer)

public:
	CImageViewer();
	virtual ~CImageViewer();

	static void CreateViewer(LPCTSTR pszFilePath, LPARAM lParam);

	ULONG_PTR gdiplusToken;
	void* gdi_image;

protected:
	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
public:
	afx_msg void OnPaint();
};


