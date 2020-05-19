
#include "hexdump.h"

ULONG hexdump(PUCHAR data, ULONG size)
{
	ULONG nResult = 0;
    
	for (ULONG i = 0; i < size; i += 16) {
		nResult += _tprintf(_T("%08X |"), i);
		for (ULONG j = 0; j < 16; j++) {
			if (i + j < size) {
				nResult += _tprintf(_T(" %02X"), data[i + j]);
			}
			else {
				nResult += _tprintf(_T("   "));
			}
			if ((j + 1) % 8 == 0) {
				nResult += _tprintf(_T(" "));
			}
		}
		nResult += _tprintf(_T("|"));
		for (ULONG j = 0; j < 16; j++) {
			if (i + j < size) {
				UCHAR k = data[i + j];
				UCHAR c = k < 32 || k > 127 ? '.' : k;
				nResult += _tprintf(_T("%c"), c);
			}
			else {
				nResult += _tprintf(_T(" "));
			}
		}
		nResult += _tprintf(_T("\n"));
	}
	return nResult;
}

ULONG hexdump(PTCHAR data, ULONG size)
{
	ULONG nResult = 0;

	for (ULONG i = 0; i < size; i += 16) {
		nResult += _tprintf(_T("%08X |"), i);
		for (ULONG j = 0; j < 16; j++) {
			if (i + j < size) {
				nResult += _tprintf(_T(" %02X"), data[i + j]);
			}
			else {
				nResult += _tprintf(_T("   "));
			}
			if ((j + 1) % 8 == 0) {
				nResult += _tprintf(_T(" "));
			}
		}
		nResult += _tprintf(_T("|"));
		for (ULONG j = 0; j < 16; j++) {
			if (i + j < size) {
				UCHAR k = data[i + j];
				UCHAR c = k < 32 || k > 127 ? '.' : k;
				nResult += _tprintf(_T("%c"), c);
			}
			else {
				nResult += _tprintf(_T(" "));
			}
		}
		nResult += _tprintf(_T("\n"));
	}
	return nResult;
}