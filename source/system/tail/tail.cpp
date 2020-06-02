// tail.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
/* tail file - Print the last 10 lines of the named file.
	All options are ignored. The file must be specified. */

#include "Everything.h"

#define NUM_LINES 11
			 /* One more than number of lines in the tail. */
#define MAX_LINE_SIZE 256
#define MAX_CHAR NUM_LINES*(MAX_LINE_SIZE+1)

int _tmain(int argc, LPTSTR argv[])
{
	HANDLE hInFile;
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	LARGE_INTEGER FileSize, CurPtr, FPos;
	LARGE_INTEGER LinePos[NUM_LINES];
	DWORD LastLine, FirstLine, LineCount, nRead;
	TCHAR buffer[MAX_CHAR + 1], c;
	OVERLAPPED ov = { 0, 0, 0, 0, NULL };

	if (argc != 2)
		ReportError(_T("Usage: tail file"), 1, FALSE);
	hInFile = CreateFile(argv[1], GENERIC_READ,
		0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hInFile == INVALID_HANDLE_VALUE)
		ReportError(_T("tail error: Cannot openfile."), 2, TRUE);

	/* Get the current file size. */
	if (!GetFileSizeEx(hInFile, &FileSize))
		ReportError(_T("tail error: file size"), 3, TRUE);


	CurPtr.QuadPart = (LONGLONG)FileSize.QuadPart
		- NUM_LINES * MAX_LINE_SIZE * sizeof(TCHAR);
	if (CurPtr.QuadPart < 0) CurPtr.QuadPart = 0;
	ov.Offset = CurPtr.LowPart;
	ov.OffsetHigh = CurPtr.HighPart;
	ReadFile(hInFile, NULL, 0, &nRead, &ov);
	/*  Scan the file for the start of new lines and retain their
		position. Assume that a line starts at the current position. */

	LinePos[0].QuadPart = CurPtr.QuadPart;
	LineCount = 1;
	LastLine = 1;
	while (TRUE) {
		while (ReadFile(hInFile, &c, sizeof(TCHAR), &nRead, NULL)
			&& nRead > 0 && c != CR); /* Empty loop body */
		if (!SetFilePointerEx(hInFile, { 0, 0 }, &FPos, FILE_CURRENT))
			ReportError(_T("tail Error: Set Pointer."), 4, TRUE);
		if (nRead < sizeof(TCHAR)) break;
		/* Found a CR. Is LF next? */
		ReadFile(hInFile, &c, sizeof(TCHAR), &nRead, NULL);
		if (nRead < sizeof(TCHAR)) break;
		if (c != LF) continue;
		/* Get the current file position. */
		if (!SetFilePointerEx(hInFile, {0, 0}, &FPos, FILE_CURRENT))
			ReportError(_T("tail Error: Set Pointer to get current position."), 5, TRUE);
		/* Retain the start-of-line position */
		LinePos[LastLine].QuadPart = FPos.QuadPart;
		LineCount++;
		LastLine = LineCount % NUM_LINES;
		ov.Offset = FPos.LowPart;
		ov.OffsetHigh = FPos.HighPart;
	}

	FirstLine = LastLine % NUM_LINES;
	if (LineCount < NUM_LINES) FirstLine = 0;
	CurPtr.QuadPart = LinePos[FirstLine].QuadPart;

	/* The start of each line is now stored in LinePos []
		with the last line having index LastLine.
		Display the last strings. */

	// if (!SetFilePointerEx(hInFile, CurPtr, NULL, FILE_BEGIN))
		// ReportError(_T("tail Error: Set Pointer to new line position."), 6, TRUE);

	ov.Offset = CurPtr.LowPart;
	ov.OffsetHigh = CurPtr.HighPart;

	ReadFile(hInFile, buffer, MAX_CHAR * sizeof(TCHAR), &nRead, &ov);
	buffer[nRead / sizeof(TCHAR)] = _T('\0');
	_tprintf(_T("%s"), buffer);
	CloseHandle(hInFile);
	return 0;
}