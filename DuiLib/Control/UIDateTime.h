#ifndef __UIDATETIME_H__
#define __UIDATETIME_H__

#pragma once

namespace DUI
{
	class CDateTimeWnd;

	/// 时间选择控件
	class DUILIB_API CDateTimeUI : public CLabelUI
	{
		UI_DECLARE_CONTROL(CDateTimeUI)
		friend class CDateTimeWnd;
	public:
		CDateTimeUI();
		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		SYSTEMTIME& GetTime();
		void SetTime(SYSTEMTIME* pst);

		void SetReadOnly(BOOL bReadOnly);
		BOOL IsReadOnly() const;

		void UpdateText();

		void DoEvent(struct TEventUI& event);

	protected:
		SYSTEMTIME m_sysTime;
		int        m_nDTUpdateFlag;
		BOOL       m_bReadOnly;

		CDateTimeWnd* m_pWindow;
	};
}
#endif // __UIDATETIME_H__