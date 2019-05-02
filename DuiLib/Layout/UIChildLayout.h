#ifndef __UICHILDLAYOUT_H__
#define __UICHILDLAYOUT_H__

#pragma once

namespace DUI
{
	class DUILIB_API CChildLayoutUI : public CContainerUI
	{
		UI_DECLARE_CONTROL(CChildLayoutUI)
	public:
		CChildLayoutUI();

		void Init();
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void SetChildLayoutXML(CDuiString pXML);
		CDuiString GetChildLayoutXML();
		virtual LPVOID GetInterface(LPCTSTR pstrName);
		virtual LPCTSTR GetClass() const;

	private:
		CDuiString m_pstrXMLFile;
	};
} // namespace DUI
#endif // __UICHILDLAYOUT_H__
