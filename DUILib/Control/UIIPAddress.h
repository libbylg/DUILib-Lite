#ifndef __UIIPADDRESS_H__
#define __UIIPADDRESS_H__

#include "Control/UILabel.h"

//  ���ÿؼ����һ������dtstyle

namespace DUI
{
    class CIPAddressWnd;

    /// ʱ��ѡ��ؼ�
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