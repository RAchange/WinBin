// ls.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
/* lsW[options][files]
	List the attributes of one or more files.
	Options:
		-R	recursive
		-l	longList listing(size and time of modification)
			Depending on the ProcessItem function, this will
			also list the owner and permissions(See Chapter 5 - security). */

			/* This program illustrates:
					1.	Search handles and directory traversal
					2.	File attributes, including time
					3.	Using generic string functions to output file information */
					/* THIS PROGRAM IS NOT A FAITHFUL IMPLEMENATION OF THE POSIX ls COMMAND - IT IS INTENDED TO ILLUSRATE THE WINDOWS API */
					/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
					/* BEGIN BOILERPLATE CODE */

#include "Everything.h"

DWORD ScanPath(LPTSTR, DWORD, LPBOOL, DWORD);
DWORD FileType(LPWIN32_FIND_DATA);
BOOL ProcessItem(LPWIN32_FIND_DATA, DWORD, LPBOOL);

int _tmain(int argc, LPTSTR argv[])
{
	BOOL flags[MAX_OPTIONS];
	TCHAR currPath[MAX_PATH_LONG + 1], argPath[MAX_PATH_LONG+1];
	int fileIndex;
	DWORD pathLength, ok;

	fileIndex = Options(argc, (LPCTSTR *)argv, _T("Rlcaw"), &flags[0], &flags[1], &flags[2], &flags[3], &flags[4], NULL);

	pathLength = GetCurrentDirectory(MAX_PATH_LONG, currPath);
	if (pathLength == 0 || pathLength >= MAX_PATH_LONG) { /* pathLength >= MAX_PATH_LONG (32780) should be impossible */
		ReportError(_T("GetCurrentDirectory failed"), 1, TRUE);
	}

	if(argc < fileIndex + 1)
		ok = ScanPath(currPath, MAX_OPTIONS, flags, pathLength+1);
	else if(argv[argc-1][1] == _T(':')){
		ok = ScanPath(argv[argc - 1], MAX_OPTIONS, flags, 0);
	}
	else {
		_stprintf_s(argPath, _T("%s\\%s"), currPath, argv[argc - 1]);
		ok = ScanPath(argPath, MAX_OPTIONS, flags, pathLength + 1);
	}
	return ok?0:1;
}

static DWORD ScanPath(LPTSTR lpPath, DWORD nFlags, LPBOOL lpFlags, DWORD pathLength) {
	WIN32_FIND_DATA ffd;
	TCHAR szDir[MAX_PATH];
	size_t length_of_arg;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError = 0;
	DWORD dwCount = 0;
	DWORD dwFileAttr = GetFileAttributes(lpPath);

	if ((dwFileAttr & FILE_ATTRIBUTE_DIRECTORY) && (lpFlags[0] || _tcslen(lpPath)==pathLength-1)) {
		length_of_arg = _tcslen(lpPath);
	
		if (length_of_arg > (MAX_PATH - 3)) {
			return 0;
		}

		_tcscpy_s(szDir, MAX_PATH, lpPath);
		_tcscat_s(szDir, _T("\\*"));

		if ((hFind = FindFirstFile(szDir, &ffd)) == INVALID_HANDLE_VALUE) {
			ReportError(_T("FindFirstFile error : "), 0, FALSE);
			return 0;
		}

		do {
			if (!_tcscmp(ffd.cFileName, _T(".")) ||
				!_tcscmp(ffd.cFileName, _T(".."))) {
				continue;
			}
			TCHAR szFullPath[MAX_PATH];
			_stprintf_s(szFullPath, MAX_PATH - 1, _T("%s\\%s"), lpPath, ffd.cFileName);
			length_of_arg = _tcslen(szFullPath);
			if (length_of_arg <= (MAX_PATH - 3)) {
				dwCount += ScanPath(szFullPath, nFlags, lpFlags, pathLength);
			}
		} while (FindNextFile(hFind, &ffd) != 0);

		FindClose(hFind);
	}
	else {
		FindFirstFile(lpPath, &ffd);
		ProcessItem(&ffd, nFlags, lpFlags);
		_tprintf(_T("%s\n"), lpPath+(min(pathLength, _tcslen(lpPath))), ffd.cFileName);
		dwCount++;
	}

	return dwCount;
}

static DWORD FileType(LPWIN32_FIND_DATA pFileData)

/* Return file type from the find data structure.
	Types supported:
		TYPE_FILE:	If this is a file
		TYPE_DIR:	If this is a directory other than . or ..
		TYPE_DOT:	If this is . or .. directory */
{
	BOOL isDir;
	DWORD fType;
	fType = TYPE_FILE;
	isDir = (pFileData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
	if (isDir)
		if (lstrcmp(pFileData->cFileName, _T(".")) == 0
			|| lstrcmp(pFileData->cFileName, _T("..")) == 0)
			fType = TYPE_DOT;
		else fType = TYPE_DIR;
	return fType;
}
/*  END OF BOILERPLATE CODE */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

static BOOL ProcessItem(LPWIN32_FIND_DATA pFileData, DWORD numFlags, LPBOOL flags)

/* Function to process(list attributes, in this case)
	the file or directory. This implementation only shows
	the low order part of the file size. */
{
	const TCHAR fileTypeChar[] = { _T(' '), _T('d') };
	DWORD fType = FileType(pFileData);
	BOOL longList = flags[1],
		bCreateTime = flags[2],
		bAccessTime = flags[3],
		bWriteTime = flags[4];
	FILETIME fLastWrite;
	SYSTEMTIME sLastWrite;

	if (fType != TYPE_FILE && fType != TYPE_DIR) return FALSE;

	_tprintf(_T("\n"));
	if (longList) {
		_tprintf(_T("%c"), fileTypeChar[fType - 1]);
		_tprintf(_T("%10d\t"), pFileData->nFileSizeLow);
	}
	if (bCreateTime) {
		FileTimeToLocalFileTime(&(pFileData->ftCreationTime), &fLastWrite);
		FileTimeToSystemTime(&fLastWrite, &sLastWrite);
		_tprintf(_T("Creation: %02d/%02d/%04d %02d:%02d:%02d\t"),
			sLastWrite.wMonth, sLastWrite.wDay,
			sLastWrite.wYear, sLastWrite.wHour,
			sLastWrite.wMinute, sLastWrite.wSecond);
	}
	if (bAccessTime) {
		FileTimeToLocalFileTime(&(pFileData->ftLastAccessTime), &fLastWrite);
		FileTimeToSystemTime(&fLastWrite, &sLastWrite);
		_tprintf(_T("Access: %02d/%02d/%04d %02d:%02d:%02d\t"),
			sLastWrite.wMonth, sLastWrite.wDay,
			sLastWrite.wYear, sLastWrite.wHour,
			sLastWrite.wMinute, sLastWrite.wSecond);
	}
	if (bWriteTime || longList) {
		FileTimeToLocalFileTime(&(pFileData->ftLastWriteTime), &fLastWrite);
		FileTimeToSystemTime(&fLastWrite, &sLastWrite);
		_tprintf(_T("Write: %02d/%02d/%04d %02d:%02d:%02d\t"),
		sLastWrite.wMonth, sLastWrite.wDay,
		sLastWrite.wYear, sLastWrite.wHour,
		sLastWrite.wMinute, sLastWrite.wSecond);
	}
	
	return TRUE;
}
