// mv.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//

#include "Everything.h"

int _tmain(int argc, LPCTSTR* argv)
{
	if (argc < 3) {
		_tprintf(_T("Mv Error: file does not initialized.\n"));
		return 0;
	}
	TCHAR lpSrc[MAX_PATH]=_T(""), lpDst[MAX_PATH]= _T("");
	TCHAR lpLog[250];
	PTCHAR lpFileName;
	DWORD dwDstAttr;
	if (!GetFullPathName(argv[1], MAX_PATH - 1, lpSrc, NULL)) {
		ReportError(_T("Mv Error: GetFullPathName error\n"), 0, TRUE);
		return 0;
	}
	if (!GetFullPathName(argv[2], MAX_PATH - 1, lpDst, NULL)) {
		ReportError(_T("Mv Error: GetFullPathName error\n"), 0, TRUE);
		return 0;
	}
	
	lpFileName = _tcsrchr(lpSrc, _T('\\'));
	dwDstAttr = GetFileAttributes(lpDst);
	if (dwDstAttr & FILE_ATTRIBUTE_DIRECTORY) {
		_tcscat_s(lpDst, lpFileName);
	}

	if (lpSrc[0] != lpDst[0]) {
		if (!CopyFile(lpSrc, lpDst, FALSE) ||
			!DeleteFile(lpSrc)) {
			_stprintf_s(lpLog, _T("Mv Error: Error occurred while moving file from  Drive %c to Drive %c\n"), lpSrc[0], lpDst[0]);
			ReportError(lpLog, 0, TRUE);
			return 0;
		}
		_tprintf(_T("Move Succeeds.\n"));
	}
	else {
		if (!MoveFile(lpSrc, lpDst)) {
			ReportError(_T("Mv Error: MoveFile error\n"), 0, TRUE);
			return 0;
		}
		_tprintf(_T("Move Succeeds.\n"));
	}
	return 0;
}

// 執行程式: Ctrl + F5 或 [偵錯] > [啟動但不偵錯] 功能表
// 偵錯程式: F5 或 [偵錯] > [啟動偵錯] 功能表

// 開始使用的提示: 
//   1. 使用 [方案總管] 視窗，新增/管理檔案
//   2. 使用 [Team Explorer] 視窗，連線到原始檔控制
//   3. 使用 [輸出] 視窗，參閱組建輸出與其他訊息
//   4. 使用 [錯誤清單] 視窗，檢視錯誤
//   5. 前往 [專案] > [新增項目]，建立新的程式碼檔案，或是前往 [專案] > [新增現有項目]，將現有程式碼檔案新增至專案
//   6. 之後要再次開啟此專案時，請前往 [檔案] > [開啟] > [專案]，然後選取 .sln 檔案
