// time.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
/* timeprint: Execute a command line and display
	the time (elapsed, kernel, user) required. */

#include "Everything.h"

int _tmain(int argc, LPTSTR argv[])
{
	STARTUPINFO startUp;
	PROCESS_INFORMATION procInfo;
	union {		/* Structure required for file time arithmetic. */
		LONGLONG li;
		FILETIME ft;
	} createTime, exitTime, elapsedTime;

	FILETIME kernelTime, userTime;
	SYSTEMTIME elTiSys, keTiSys, usTiSys;
	LPTSTR targv, cLine = GetCommandLine();
	OSVERSIONINFO windowsVersion;
	HANDLE hProc;

	targv = SkipArg(cLine, 1, argc, argv);
	/*  Skip past the first blank-space delimited token on the command line */
	if (argc <= 1 || NULL == targv)
		ReportError(_T("Usage: timep command ..."), 1, FALSE);

	/* Determine is this is Windows 2000 or NT.  */
	windowsVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (!GetVersionEx(&windowsVersion))
		ReportError(_T("Can not get OS Version info. %d"), 2, TRUE);

	if (windowsVersion.dwPlatformId != VER_PLATFORM_WIN32_NT)
		ReportError(_T("This program only runs on Windows NT kernels"), 2, FALSE);

	GetStartupInfo(&startUp);

	/* Execute the command line and wait for the process to complete. */
	if (!CreateProcess(NULL, targv, NULL, NULL, TRUE,
		NORMAL_PRIORITY_CLASS, NULL, NULL, &startUp, &procInfo))
		ReportError(_T("\nError starting process. %d"), 3, TRUE);
	hProc = procInfo.hProcess;
	if (WaitForSingleObject(hProc, INFINITE) != WAIT_OBJECT_0)
		ReportError(_T("Failed waiting for process termination. %d"), 5, TRUE);;

	if (!GetProcessTimes(hProc, &createTime.ft,
		&exitTime.ft, &kernelTime, &userTime))
		ReportError(_T("Can not get process times. %d"), 6, TRUE);

	elapsedTime.li = exitTime.li - createTime.li;
	FileTimeToSystemTime(&elapsedTime.ft, &elTiSys);
	FileTimeToSystemTime(&kernelTime, &keTiSys);
	FileTimeToSystemTime(&userTime, &usTiSys);
	_tprintf(_T("\nReal Time: %02d:%02d:%02d.%03d\n"),
		elTiSys.wHour, elTiSys.wMinute, elTiSys.wSecond,
		elTiSys.wMilliseconds);
	_tprintf(_T("User Time: %02d:%02d:%02d.%03d\n"),
		usTiSys.wHour, usTiSys.wMinute, usTiSys.wSecond,
		usTiSys.wMilliseconds);
	_tprintf(_T("Sys Time:  %02d:%02d:%02d.%03d\n"),
		keTiSys.wHour, keTiSys.wMinute, keTiSys.wSecond,
		keTiSys.wMilliseconds);

	CloseHandle(procInfo.hThread); CloseHandle(procInfo.hProcess);
	return 0;
}
