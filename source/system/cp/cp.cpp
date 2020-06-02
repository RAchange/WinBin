// cp.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//
#include "Everything.h"
#include <stdio.h>
#define BUF_SIZE 16384  /* Optimal in several experiments. Small values such as 256 give very bad performance */

int _tmain(int argc, LPTSTR argv[])
{
	HANDLE hIn, hOut;
	DWORD nIn, nOut;
	CHAR buffer[BUF_SIZE];
	LPTSTR pfilename;
	TCHAR lpOutFile[MAX_PATH];

	if (argc != 3) {
		fprintf(stderr, "Usage: cpW file1 file2\n");
		return 1;
	}
	hIn = CreateFile(argv[1], GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hIn == INVALID_HANDLE_VALUE) {
		fprintf(stderr, "Cannot open input file. Error: %x\n", GetLastError());
		return 2;
	}

	if (( pfilename = _tcsrchr(argv[1], _T('\\'))) == NULL) {
		pfilename = argv[1];
	}

	DWORD dwAttr = GetFileAttributes(argv[2]);

	if (dwAttr & FILE_ATTRIBUTE_DIRECTORY) {
		_stprintf_s(lpOutFile, _T("%s\\%s"), argv[2], pfilename);
	}
	else {
		CopyMemory(lpOutFile, argv[2], _tcslen(argv[2]));
	}

	hOut = CreateFile(lpOutFile, GENERIC_WRITE, 0, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if (hOut == INVALID_HANDLE_VALUE) {
		ReportError(_T("Cannot open output file."), 0, TRUE);
		//fprintf(stderr, "Cannot open output file. Error: %x\n", GetLastError());
		CloseHandle(hIn);
		return 3;
	}
	while (ReadFile(hIn, buffer, BUF_SIZE, &nIn, NULL) && nIn > 0) {
		WriteFile(hOut, buffer, nIn, &nOut, NULL);
		if (nIn != nOut) {
			fprintf(stderr, "Fatal write error: %x\n", GetLastError());
			CloseHandle(hIn); CloseHandle(hOut);
			return 4;
		}
	}
	CloseHandle(hIn);


	CloseHandle(hOut);
	return 0;
}

