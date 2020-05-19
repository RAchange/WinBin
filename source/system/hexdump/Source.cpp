#include "Everything.h"
#include "hexdump.h"
#include "ezfile.h"

#define HEXDUMP_BUFSIZE 1024

static VOID DumpFile(HANDLE hInFile, HANDLE hOutFile)
{
	DWORD nIn, nOut;
	BYTE buffer[HEXDUMP_BUFSIZE];

	while (ReadFile(hInFile, buffer, HEXDUMP_BUFSIZE, &nIn, NULL) && (nIn != 0)
		&& hexdump(buffer, nIn));

	return;
}

INT _tmain(INT argc, LPCTSTR argv[])
{
	HANDLE hInFile, hStdIn = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	BOOL dashS;
	int iArg, iFirstFile;
	
	iFirstFile = Options(argc, (LPCTSTR*)argv, _T("s"), &dashS, NULL);

	if (iFirstFile == argc) { /* No files in arg list. */
		DumpFile(hStdIn, hStdOut);
		return 0;
	}

	for (iArg = iFirstFile; iArg < argc; iArg++) {
		hInFile = CreateFile(argv[iArg], GENERIC_READ,
			0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hInFile == INVALID_HANDLE_VALUE) {
			if (!dashS) ReportError(_T("Hexdump Error: File does not exist."), 0, TRUE);
			else {
				hexdump((PTCHAR)argv[iArg], _tcslen(argv[iArg]));
			}
		}
		else {
			DumpFile(hInFile, hStdOut);
			if (GetLastError() != 0 && !dashS) {
				ReportError(_T("Hexdump Error: Could not process file completely."), 0, TRUE);
			}
			CloseHandle(hInFile);
		}
	}

	return 0;
}
