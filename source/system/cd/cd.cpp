// cd.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
/*	cd [directory]: Similar to the UNIX command. */
/*  This program illustrates:
	1.  Win32 SetCurrentDirectory
	2.  How each drive has its own working directory */

#include "Everything.h"

#define DIRNAME_LEN MAX_PATH + 2

int _tmain(int argc, LPCTSTR argv[])
{
	TCHAR lpTargetDir[DIRNAME_LEN]=_T("");

	if (argv[1][1] != _T(':')) {
		if (!GetCurrentDirectory(DIRNAME_LEN - _tcslen(argv[1]) - 1, lpTargetDir)) {
			ReportError(_T("SetCurrentDirectory error."), 1, TRUE);
			return 1;
		}
		_tcscat_s(lpTargetDir, _T("\\"));
	}

	_tcscat_s(lpTargetDir, argv[1]);
	
	if (!SetCurrentDirectory(lpTargetDir))
		ReportError(_T("SetCurrentDirectory error."), 1, TRUE);
	return 0;
}