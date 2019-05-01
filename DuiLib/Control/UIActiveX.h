#ifndef __UIACTIVEX_H__
#define __UIACTIVEX_H__

#pragma once

struct IOleObject;


namespace DUILIB {
	/////////////////////////////////////////////////////////////////////////////////////
	//

	class CActiveXCtrl;

	template< class T >
	class CSafeRelease
	{
	public:
		CSafeRelease(T* p) : m_p(p) { };
		~CSafeRelease() { if( m_p != NULL ) m_p->Release(); };
		T* Detach() { T* t = m_p; m_p = NULL; return t; };
		T* m_p;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class DUILIB_API CActiveXUI : public CControlUI, public IMessageFilterUI
	{
		DECLARE_DUICONTROL(CActiveXUI)

		friend class CActiveXCtrl;
	public:
		CActiveXUI();
		virtual ~CActiveXUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		HWND GetHostWindow() const;

		virtual BOOL IsDelayCreate() const;
		virtual void SetDelayCreate(BOOL bDelayCreate = true);
		virtual BOOL IsMFC() const;
		virtual void SetMFC(BOOL bMFC = FALSE);

		BOOL CreateControl(const CLSID clsid);
		BOOL CreateControl(LPCTSTR pstrCLSID);
		HRESULT GetControl(const IID iid, LPVOID* ppRet);
		CLSID GetClisd() const;
		CDuiString GetModuleName() const;
		void SetModuleName(LPCTSTR pstrText);

		void SetVisible(BOOL bVisible = true);
		void SetInternVisible(BOOL bVisible = true);
		void SetPos(RECT rc, BOOL bNeedInvalidate = true);
		void Move(SIZE szOffset, BOOL bNeedInvalidate = true);
		BOOL DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	protected:
		virtual void ReleaseControl();
		virtual BOOL DoCreateControl();

	protected:
		CLSID m_clsid;
		CDuiString m_sModuleName;
		BOOL m_bCreated;
		BOOL m_bDelayCreate;
		BOOL m_bMFC;
		IOleObject* m_pUnk;
		CActiveXCtrl* m_pControl;
		HWND m_hwndHost;
	};

} // namespace DUILIB

#endif // __UIACTIVEX_H__
