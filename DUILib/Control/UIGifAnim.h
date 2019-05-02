#ifndef GifAnimUI_h__
#define GifAnimUI_h__

#pragma once

namespace DUI
{
	class DUILIB_API CGifAnimUI : public CControlUI
	{
		enum
		{ 
			EVENT_TIEM_ID = 100,
		};
		UI_DECLARE_CONTROL(CGifAnimUI)
	public:
		CGifAnimUI(void);
		~CGifAnimUI(void);

		LPCTSTR	GetClass() const;
		LPVOID	GetInterface(LPCTSTR pstrName);
		void	DoInit();
		BOOL	DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
		void	DoEvent(struct TEventUI& event);
		void	SetVisible(BOOL bVisible = true );
		void	SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void	SetBkImage(LPCTSTR pStrImage);
		LPCTSTR GetBkImage();

		void	SetAutoPlay(BOOL bIsAuto = true );
		BOOL	IsAutoPlay() const;
		void	SetAutoSize(BOOL bIsAuto = true );
		BOOL	IsAutoSize() const;
		void	PlayGif();
		void	PauseGif();
		void	StopGif();

	private:
		void	InitGifImage();
		void	DeleteGif();
		void    OnTimer( UINT_PTR idEvent );
		void	DrawFrame( HDC hDC );		// ����GIFÿ֡

	private:
		Gdiplus::Image	*m_pGifImage;
		UINT			m_nFrameCount;				// gifͼƬ��֡��
		UINT			m_nFramePosition;			// ��ǰ�ŵ��ڼ�֡
		Gdiplus::PropertyItem*	m_pPropertyItem;	// ֡��֮֡����ʱ��

		CDuiString		m_sBkImage;
		BOOL			m_bIsAutoPlay;				// �Ƿ��Զ�����gif
		BOOL			m_bIsAutoSize;				// �Ƿ��Զ�����ͼƬ���ô�С
		BOOL			m_bIsPlaying;
	};
}

#endif // GifAnimUI_h__