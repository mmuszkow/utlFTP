#ifndef __UTL_FTP_API_H__
#define __UTL_FTP_API_H__

namespace utlFTP 
{
	struct wtwFtpFile 
	{
		/** struct size */
		int				structSize;
		/** path to file */
		wchar_t			filePath[260];
		/** Contact to which the file will be sent, only contactID, 
		  * netClass and netId (sometimes called sID) are needed
		  */
		wtwContactDef	contact;
		/** bit field */
		DWORD			flags;
	};

	namespace flag
	{
		/* if flag set - filePath has format "directory<NUL>file1<NUL>file2<NUL>...lastfile<NUL><NUL>" */
		static const DWORD WTW_FTP_MULTIFILE	= 0x80000000;
		/* if flag set - dialog with progress will not be shown */
		static const DWORD WTW_FTP_NODIALOG		= 0x40000000;
		/* if flag set - links won't be sent neither to chat window nor to input window */
		static const DWORD WTW_FTP_NOLINK		= 0x20000000;
	};

	/** wParam = wtwFtpFile*, lParam = NULL */
	static const wchar_t WTW_FTP_SEND_FILE[] = L"utlFTP/sendFile";
};

#endif // __UTL_FTP_API_H__
