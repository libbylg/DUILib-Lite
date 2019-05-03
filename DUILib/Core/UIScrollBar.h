#ifndef __UISCROLLBAR_H__
#define __UISCROLLBAR_H__

#include "Core/UIControl.h"

namespace DUI
{
    class CContainerUI;

    class DUILIB_API CScrollBarUI : public CControlUI
    {
        UI_DECLARE_CONTROL(CScrollBarUI)
    public:
        CScrollBarUI();

        LPCTSTR GetClass() const;
        LPVOID GetInterface(LPCTSTR pstrName);

        CContainerUI* GetOwner() const;
        void SetOwner(CContainerUI* pOwner);

        void SetVisible(BOOL bVisible = TRUE);
        void SetEnabled(BOOL bEnable = TRUE);
        void SetFocus();

        BOOL IsHorizontal();
        void SetHorizontal(BOOL bHorizontal = TRUE);
        int GetScrollRange() const;
        void SetScrollRange(int nRange);
        int GetScrollPos() const;
        void SetScrollPos(int nPos);
        int GetLineSize() const;
        void SetLineSize(int nSize);

        BOOL GetShowButton1();
        void SetShowButton1(BOOL bShow);
        LPCTSTR GetButton1NormalImage();
        void SetButton1NormalImage(LPCTSTR pStrImage);
        LPCTSTR GetButton1HotImage();
        void SetButton1HotImage(LPCTSTR pStrImage);
        LPCTSTR GetButton1PushedImage();
        void SetButton1PushedImage(LPCTSTR pStrImage);
        LPCTSTR GetButton1DisabledImage();
        void SetButton1DisabledImage(LPCTSTR pStrImage);

        BOOL GetShowButton2();
        void SetShowButton2(BOOL bShow);
        LPCTSTR GetButton2NormalImage();
        void SetButton2NormalImage(LPCTSTR pStrImage);
        LPCTSTR GetButton2HotImage();
        void SetButton2HotImage(LPCTSTR pStrImage);
        LPCTSTR GetButton2PushedImage();
        void SetButton2PushedImage(LPCTSTR pStrImage);
        LPCTSTR GetButton2DisabledImage();
        void SetButton2DisabledImage(LPCTSTR pStrImage);

        LPCTSTR GetThumbNormalImage();
        void SetThumbNormalImage(LPCTSTR pStrImage);
        LPCTSTR GetThumbHotImage();
        void SetThumbHotImage(LPCTSTR pStrImage);
        LPCTSTR GetThumbPushedImage();
        void SetThumbPushedImage(LPCTSTR pStrImage);
        LPCTSTR GetThumbDisabledImage();
        void SetThumbDisabledImage(LPCTSTR pStrImage);

        LPCTSTR GetRailNormalImage();
        void SetRailNormalImage(LPCTSTR pStrImage);
        LPCTSTR GetRailHotImage();
        void SetRailHotImage(LPCTSTR pStrImage);
        LPCTSTR GetRailPushedImage();
        void SetRailPushedImage(LPCTSTR pStrImage);
        LPCTSTR GetRailDisabledImage();
        void SetRailDisabledImage(LPCTSTR pStrImage);

        LPCTSTR GetBkNormalImage();
        void SetBkNormalImage(LPCTSTR pStrImage);
        LPCTSTR GetBkHotImage();
        void SetBkHotImage(LPCTSTR pStrImage);
        LPCTSTR GetBkPushedImage();
        void SetBkPushedImage(LPCTSTR pStrImage);
        LPCTSTR GetBkDisabledImage();
        void SetBkDisabledImage(LPCTSTR pStrImage);

        void SetPos(RECT rc, BOOL bNeedInvalidate = TRUE);
        void DoEvent(TEVENT_UI& event);
        void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

        BOOL DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);

        void PaintBk(HDC hDC);
        void PaintButton1(HDC hDC);
        void PaintButton2(HDC hDC);
        void PaintThumb(HDC hDC);
        void PaintRail(HDC hDC);

    protected:

        enum
        {
            DEFAULT_SCROLLBAR_SIZE = 16,
            DEFAULT_TIMERID = 10,
        };

        BOOL m_bHorizontal;
        int m_nRange;
        int m_nScrollPos;
        int m_nLineSize;
        CContainerUI* m_pOwner;
        POINT ptLastMouse;
        int m_nLastScrollPos;
        int m_nLastScrollOffset;
        int m_nScrollRepeatDelay;

        CStringUI m_sBkNormalImage;
        CStringUI m_sBkHotImage;
        CStringUI m_sBkPushedImage;
        CStringUI m_sBkDisabledImage;

        BOOL m_bShowButton1;
        RECT m_rcButton1;
        UINT m_uButton1State;
        CStringUI m_sButton1NormalImage;
        CStringUI m_sButton1HotImage;
        CStringUI m_sButton1PushedImage;
        CStringUI m_sButton1DisabledImage;

        BOOL m_bShowButton2;
        RECT m_rcButton2;
        UINT m_uButton2State;
        CStringUI m_sButton2NormalImage;
        CStringUI m_sButton2HotImage;
        CStringUI m_sButton2PushedImage;
        CStringUI m_sButton2DisabledImage;

        RECT m_rcThumb;
        UINT m_uThumbState;
        CStringUI m_sThumbNormalImage;
        CStringUI m_sThumbHotImage;
        CStringUI m_sThumbPushedImage;
        CStringUI m_sThumbDisabledImage;

        CStringUI m_sRailNormalImage;
        CStringUI m_sRailHotImage;
        CStringUI m_sRailPushedImage;
        CStringUI m_sRailDisabledImage;

        CStringUI m_sImageModify;
    };
}

#endif // __UISCROLLBAR_H__

