// touch.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。

#include "Everything.h"

int _tmain(int argc, LPTSTR argv[])
{
	FILETIME newFileTime;
	LPFILETIME pAccessTime = NULL, pModifyTime = NULL;
	HANDLE hFile;
	BOOL setAccessTime, setModTime, notCreateNew;
	DWORD CreateFlag;
	int i, FileIndex;

	/* Determine the options. */

	if (argc < 2) {
		_tprintf(_T("Usage: touch[options] files"));
		return 1;
	}
	FileIndex = Options(argc, (LPCTSTR *)argv, _T("amc"),
		&setAccessTime, &setModTime, &notCreateNew, NULL);

	CreateFlag = notCreateNew ? OPEN_EXISTING : OPEN_ALWAYS;

	for (i = FileIndex; i < argc; i++) {
		hFile = CreateFile(argv[i], GENERIC_READ | GENERIC_WRITE, 0, NULL,
			CreateFlag, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) {
			if (!notCreateNew) ReportError(_T("touch error: Cannot open file."), 0, TRUE);
			continue;
		}
		/* Get current system time and convert to a file time.
			Do not change the create time. */
		GetSystemTimeAsFileTime(&newFileTime);
		if (setAccessTime) pAccessTime = &newFileTime;
		if (setModTime) pModifyTime = &newFileTime;
		if (!SetFileTime(hFile, NULL, pAccessTime, pModifyTime))
			ReportError(_T("Failure setting file times."), 2, TRUE);
		CloseHandle(hFile);
	}
	return 0;
}
