// pwd.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
/* pwd: Print the current directory. */

#include "Everything.h"

#define DIRNAME_LEN (MAX_PATH + 2)

int _tmain(int argc, LPTSTR argv[])
{
	/* Buffer to receive current directory allows for the CR,
		LF at the end of the longest possible path. */

	TCHAR pwdBuffer[DIRNAME_LEN];
	DWORD lenCurDir;

	lenCurDir = GetCurrentDirectory(DIRNAME_LEN, pwdBuffer);
	if (lenCurDir == 0)
		ReportError(_T("Failure getting pathname."), 1, TRUE);
	if (lenCurDir > DIRNAME_LEN)
		ReportError(_T("Pathname is too long."), 2, FALSE);
	// _tcscat_s(pwdBuffer, _T("\n"));
	PrintMsg(GetStdHandle(STD_OUTPUT_HANDLE), pwdBuffer);
	return 0;
}
