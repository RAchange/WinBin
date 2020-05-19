// rm.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//

#include "Everything.h"
#include "ezfile.h"

#define BUF_SIZE 0x200

int _tmain(int argc, LPTSTR argv[])
{
	HANDLE hInFile, hStdIn = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	BOOL dashS, dashD, bDeleted;
	int iArg, iFirstFile;

	/*	dashS will be set only if "-s" is on the command line. */
	/*	iFirstFile is the argv [] index of the first input file. */
	iFirstFile = Options(argc, (LPCTSTR*)argv, _T("sd"), &dashS, &dashD, NULL);

	if (iFirstFile == argc) { /* No files in arg list. */
		_tprintf(_T("Rm Error: File is not instailaized.\n"));
		return 0;
	}

	/* Process the input files. */
	for (iArg = iFirstFile; iArg < argc; iArg++) {
		if (dashD) {
			bDeleted = DeleteFileZero(argv[iArg]);
		}
		else {
			bDeleted = DeleteFile(argv[iArg]);
		}
		if (!bDeleted) {
			ReportError(_T("Rm Error: DeleteFile Error\n"), 0, TRUE);
		}
		else {
			_tprintf(_T("%32s\tdeleted\n"), argv[iArg]);
		}
	}
	return 0;
}
