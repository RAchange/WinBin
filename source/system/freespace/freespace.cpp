// freespace.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//

#include "Everything.h"

void ReportDriverSpace(LPCTSTR lpDriveName);

int _tmain(int argc, LPCTSTR* argv)
{
	DWORD dwDrivers = GetLogicalDrives();
	TCHAR lpDriverName[4] = _T("C:\\");

	if (argc == 1) {
		for (int i = 0; i < 26; i++) {
			if (dwDrivers & (1 << i)) {
				lpDriverName[0] = i + 'A';
				_tprintf(_T("Driver %c: found\n"), lpDriverName[0]);
				ReportDriverSpace((LPCTSTR)lpDriverName);
			}
		}
	}
	else {
		for (int i = 1, driverBit; i < argc; i++) {
			toupper(argv[i][0]);
			driverBit = (1 << (argv[i][0] - 'A'));
			if (dwDrivers & driverBit) {
				lpDriverName[0] = argv[i][0];
				_tprintf(_T("Driver %c: found\n"), argv[i][0]);
				ReportDriverSpace((LPCTSTR)lpDriverName);
			}else{
				_tprintf(_T("Driver %c: not found\n"), argv[i][0]);
			}
		}
	}

	return 0;
}

void ReportDriverSpace(LPCTSTR lpDriveName)
{
	LPCTSTR llpDriveType[7] = {
		_T("Unknown"),
		_T("No root dir"),
		_T("Removeable disk"),
		_T("Fixed disk"),
		_T("Remote drive"),
		_T("CDROM"),
		_T("RAM disk")
	};

	ULARGE_INTEGER FreeBytes, TotalBytes, NumFreeBytes;

	if (!GetDiskFreeSpaceEx(lpDriveName, &FreeBytes, &TotalBytes, &NumFreeBytes))
		ReportError(_T("Cannot get free space"), 1, TRUE);
	/* Note: Windows NT 5.0 and greater (including 2000) - This measures
	space available to the user, accounting for disc quotas */
	_tprintf(_T("Drive %35s.\nType: %16s\nTotal: %16x; Free on disk: %16x; Avail to user: %16x\n"), lpDriveName, llpDriveType[GetDriveType(lpDriveName)],
		TotalBytes.QuadPart, NumFreeBytes.QuadPart, FreeBytes.QuadPart);

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
