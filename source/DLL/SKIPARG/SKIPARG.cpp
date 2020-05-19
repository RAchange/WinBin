#include "pch.h"
#include "SKIPARG.h"

#include "Everything.h"

/*
	argc, argv[] are the normal argument count and value array taken from a command line
	argn: The argument number that you want to position to
	cLine: The current command line.

	Returns: A pointer within cLine, pointing to the first, non-white space, character in the argument
			 NULL in case of error.
*/
__declspec(dllexport)
LPTSTR SkipArg(LPTSTR cLine, int argn, int argc, LPTSTR argv[])
{
	LPTSTR pArg = cLine, cEnd = pArg + strlen((LPCSTR)cLine);
	int iArg;

	if (argn >= argc) return NULL;

	for (iArg = 0; iArg < argn && pArg < cEnd; iArg++)
	{
		if ('"' == *pArg)
		{
			pArg += strlen((LPCSTR)argv[iArg]) + 2; /* Skip over the argument and the enclosing quotes */
		}
		else
		{
			pArg += strlen((LPCSTR)argv[iArg]); /* Skip over the argument */
		}

		while ((pArg < cEnd) && ((' ' == *pArg) || ('\t') == *pArg)) pArg++;
	}

	if (pArg >= cEnd) return NULL;
	return pArg;
}