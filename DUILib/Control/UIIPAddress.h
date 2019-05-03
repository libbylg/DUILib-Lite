#ifndef __UIIPADDRESS_H__
#define __UIIPADDRESS_H__

#include "Control/UILabel.h"

//  给该控件添加一个属性dtstyle

namespace DUI
{
    class CIPAddressWnd;

    /// 时间选择控件
    class DUILIB_API CIPAddressUI : public CLabelUI
    {
        UI_DECLARE_CONTROL(CIPAddressUI)

            friend class CIPAddressWnd;
    public:
        CIPAddressUI();
        LPCTSTR GetClass() const;
        LPVOID GetInterface(LPCTSTR pstrName);

        DWORD GetIP();
        void SetIP(DWORD dwIP);

        void SetReadOnly(BOOL bReadOnly);
        BOOL IsReadOnly() const;

        void UpdateText();

        void DoEvent(struct TEVENT_UI& event);

        void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    protected:
        DWORD	m_dwIP;
        BOOL       m_bReadOnly;
        int		m_nIPUpdateFlag;

        CIPAddressWnd* m_pWindow;
    };
}
#endif // __UIIPADDRESS_H__