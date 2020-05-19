// cci_f.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//

#include "Everything.h"

#define ZSYCCIF _T(".enccif")

typedef struct {
	int id;
	LPTSTR pIn;
	LPTSTR pOut;
	SIZE_T len;
	BYTE key;
}ENC_ARGS, *PENC_ARGS;

static DWORD WINAPI do_xor(PENC_ARGS args)
{
	int i;
	for (i = 0; i < args->len; i++)
	{
		if (*(args->pIn) == _T('\0') || *(args->pIn) == EOF) printf("BOF at Thread %d\n", args->id);
		*(args->pOut + i) = *(args->pIn + i) ^ args->key;
	}
	return args->len;
}


int _tmain(int argc, LPTSTR argv[])
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);

	DWORD nCPU = si.dwNumberOfProcessors, ThreadIdx;

	HANDLE hIn, hOut, hInMap, hOutMap, hProcHeap;
	HANDLE* tHandle;
	LPTSTR pIn, pOut, ptr;
	LARGE_INTEGER liFileSize;

	TCHAR lpMsg[MAX_PATH], TempFileName[MAX_PATH];

	hProcHeap = GetProcessHeap();
	HANDLE hArgsHeap = (HANDLE)HeapCreate(HEAP_GENERATE_EXCEPTIONS, 10000, 0);
	PENC_ARGS pArgs = (PENC_ARGS)HeapAlloc(hArgsHeap, HEAP_GENERATE_EXCEPTIONS, sizeof(ENC_ARGS)*(nCPU+1));

	if (argc <= 1) {
		_tprintf(_T("Usage: %s <filename>\n"), argv[0]);
	}

	for (int i = 1; i < argc; i++) {
		if ((hIn = CreateFile(argv[i], GENERIC_READ | GENERIC_WRITE, 0, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) 
			== INVALID_HANDLE_VALUE) {
			ReportError(_T("Failure opening file ."), 0, FALSE);
			continue;
		}

		PTCHAR ph;
		if ((ph = _tcsstr(argv[i], ZSYCCIF))!=NULL) {
			_tcsnccpy_s(TempFileName, argv[i], _tcslen(argv[i])-_tcslen(ZSYCCIF));
		}
		else {
			_stprintf_s(TempFileName, _T("%s")ZSYCCIF, argv[i]);
		}

		
		if ((hOut = CreateFile(TempFileName, GENERIC_READ | GENERIC_WRITE,
			0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL))
			== INVALID_HANDLE_VALUE) {
			ReportError(_T("Failure opening file ."), 0, FALSE);
			CloseHandle(hIn);
			continue;
		}

		if (!GetFileSizeEx(hIn, &liFileSize)) {
			ReportError(_T("Failure getting file size."), 0, FALSE);
			CloseHandle(hIn);
			continue;
		}


		if ((hInMap = CreateFileMapping(hIn, NULL, PAGE_READWRITE, 0, 0, NULL)) == NULL) {
			CloseHandle(hOut); CloseHandle(hIn);
			ReportError(_T("Failure Creating input map."), 0, FALSE);
			continue;
		}

		pIn = (LPTSTR)MapViewOfFile(hInMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);

		if ((hOutMap = CreateFileMapping(hOut, NULL, PAGE_READWRITE, liFileSize.HighPart, liFileSize.LowPart, NULL)) == NULL) {
			CloseHandle(hOut); CloseHandle(hIn); CloseHandle(hInMap);
			ReportError(_T("Failure Creating output map."), 0, FALSE);
			continue;
		}

		pOut = (LPTSTR)MapViewOfFile(hOutMap, FILE_MAP_WRITE, 0, 0, (SIZE_T)liFileSize.QuadPart);

		tHandle = (HANDLE*)HeapAlloc(hProcHeap, HEAP_GENERATE_EXCEPTIONS, (MAXIMUM_WAIT_OBJECTS)*sizeof(HANDLE));

		int Block = (((SIZE_T)liFileSize.QuadPart) % nCPU)? 
			((SIZE_T)liFileSize.QuadPart) / nCPU+1 : ((SIZE_T)liFileSize.QuadPart) / nCPU, tid = 0;
		for (int j = 0; j < liFileSize.QuadPart; j += Block)
		{
			SIZE_T len = min(liFileSize.QuadPart - j, Block);
			pArgs[tid] = {tid, pIn+j, pOut+j, len, 0x87};
			tHandle[tid] = (HANDLE)_beginthreadex(
				NULL, 0, (_beginthreadex_proc_type)do_xor, &pArgs[tid], 0, NULL);
			tid++;
		}

		WaitForMultipleObjects(
				tid, tHandle, TRUE, INFINITE);

		for (int j = 0; j < nCPU; j++) {
			CloseHandle(tHandle[j]);
		}

		HeapFree(hProcHeap, 0, tHandle);
	
		ZeroMemory(pIn, liFileSize.QuadPart);

		UnmapViewOfFile(pOut); UnmapViewOfFile(pIn);
		CloseHandle(hOutMap); CloseHandle(hInMap);
		CloseHandle(hOut);
		CloseHandle(hIn);

		if (_tcsstr(argv[i], ZSYCCIF) == NULL) {
			_tprintf(_T("%s encrypted\n"), argv[i]);
		}
		else {
			_tprintf(_T("%s decrypted\n"), argv[i]);
		}
		
		DeleteFile(argv[i]);
	}

	HeapFree(hArgsHeap, 0, pArgs);
	HeapDestroy(hArgsHeap);

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
