#include "Everything.h"
#include "JobManagement.h"

#define MILLION 1000000
static int Jobbg(int, LPTSTR*, LPTSTR);
static int Jobs(int, LPTSTR*, LPTSTR);
static int Kill(int, LPTSTR*, LPTSTR);
static int PipeChain(int, LPTSTR*, LPTSTR);
static BOOL EventHandler(DWORD);
HANDLE hJobObject = NULL;

JOBOBJECT_BASIC_LIMIT_INFORMATION basicLimits = { 0, 0, JOB_OBJECT_LIMIT_PROCESS_TIME };

LPPROCESS_INFORMATION lpCur = NULL;

int _tmain(int argc, LPTSTR argv[])
{
	LARGE_INTEGER processTimeLimit;
	BOOL exitFlag = FALSE;
	TCHAR command[MAX_COMMAND_LINE], originCmd[MAX_COMMAND_LINE], * pc;
	DWORD i, localArgc;
	TCHAR argstr[MAX_ARG][MAX_COMMAND_LINE];
	LPTSTR pArgs[MAX_ARG];

	SetConsoleCtrlHandler(EventHandler, TRUE);

	/* NT Only - due to file locking */
	if (!WindowsVersionOK(3, 1))
		ReportError(_T("This program requires Windows NT 3.1 or greater"), 1, FALSE);
	hJobObject = NULL;
	processTimeLimit.QuadPart = 0;
	if (argc >= 2) processTimeLimit.QuadPart = _ttoi(argv[1]);
	basicLimits.PerProcessUserTimeLimit.QuadPart = processTimeLimit.QuadPart * MILLION;

	hJobObject = CreateJobObject(NULL, NULL);
	if (NULL == hJobObject)
		ReportError(_T("Error creating job object."), 1, TRUE);
	if (!SetInformationJobObject(hJobObject, JobObjectBasicLimitInformation, &basicLimits, sizeof(JOBOBJECT_BASIC_LIMIT_INFORMATION)))
		ReportError(_T("Error setting job object information."), 2, TRUE);

	for (i = 0; i < MAX_ARG; i++)
		pArgs[i] = argstr[i];

	// system("chcp 65001");

	_tprintf(_T("\x1B[37m""Windows Job Mangement with Windows Job Object.\n"));
	while (!exitFlag) {
		_tprintf(_T("%s"), _T("\x1b[;31m""astsu$""\x1B[37m"));
		_fgetts(command, MAX_COMMAND_LINE, stdin);
		pc = _tcschr(command, _T('\n'));
		*pc = _T('\0');
		GetArgs(command, (int*)&localArgc, pArgs);
		if (!localArgc || !_tcslen(pArgs[0])) continue;
		CharLower(argstr[0]);

		if (_tcscmp(argstr[0], _T("jobbg")) == 0) {
			Jobbg(localArgc, pArgs, command);
		}
		else if (_tcscmp(argstr[0], _T("jobs")) == 0) {
			Jobs(localArgc, pArgs, command);
		}
		else if (_tcscmp(argstr[0], _T("kill")) == 0) {
			Kill(localArgc, pArgs, command);
		}
		else if (_tcscmp(argstr[0], _T("quit")) == 0) {
			exitFlag = TRUE;
		}
		else if (_tcsrchr(command, _T('|'))!=NULL) {
			PipeChain(localArgc, pArgs, command);
		}
		else if (_tcscmp(argstr[0], _T("cd")) == 0) {
			if (!SetCurrentDirectory(pArgs[1]))
				ReportError(_T("SetCurrentDirectory error."), 1, TRUE);
		}
		else if (!_tcscmp(argstr[0], _T("charset"))) {
			if (!_tcscmp(argstr[1], _T("utf-8")) || !_tcscmp(argstr[1], _T("utf8")))
				system("chcp 65001");
			else if (!_tcscmp(argstr[1], _T("big-5")) || !_tcscmp(argstr[1], _T("big5")))
				system("chcp 950");
			else
				_tprintf(_T("Charset Error: please select a charset of UTF-8 or BIG-5\n"));
		}
		else {
			LPSTARTUPINFO lpStartUp = new STARTUPINFO;
			LPPROCESS_INFORMATION lpProcessInfo = new PROCESS_INFORMATION;
			GetStartupInfo(lpStartUp);
			if (!CreateProcess(NULL, command, NULL, NULL, TRUE,
				CREATE_NEW_PROCESS_GROUP,
				NULL, NULL, lpStartUp, lpProcessInfo)) {
				// ReportError(_T("Error starting process."), 0, TRUE);
				// _ftprintf(stderr, _T("%s : Command Not Found\n"), argstr[0]);
				_tsystem((LPCTSTR)command);
			}
			lpCur = lpProcessInfo;
			if(lpProcessInfo->hProcess!=INVALID_HANDLE_VALUE) 
				WaitForSingleObject(lpProcessInfo->hProcess, INFINITE);
			delete lpStartUp;
			delete lpProcessInfo;
			lpCur = NULL;
		}
	}

	CloseHandle(hJobObject);

	SetConsoleCtrlHandler(EventHandler, FALSE);
	return 0;
}

/* Jobbg: Execute a command line in the background, put
	the job identity in the user's job file, and exit.
	Related commands (jobs, fg, kill, and suspend) can be used to manage the jobs. */

	/* jobbg [options] command-line
			-c: Give the new process a console.
			-d: The new process is detached, with no console.
		These two options are mutually exclusive.
		If neither is set, the background process shares the console with jobbg. */

		/* This new features this program illustrates:
				1. Creating detached and with separate consoles.
				2. Maintaining a Jobs/Process list in a shared file.
				3. Determining a process status from a process ID.*/

				/* Standard include files. */
				/* - - - - - - - - - - - - - - - - - - - - - - - - - */

int Jobbg(int argc, LPTSTR argv[], LPTSTR command)
{
	/*	Similar to timep.c to process command line. */
	/*	- - - - - - - - - - - - - - - - - - - - - - */
	/*	Execute the command line (targv) and store the job id,
		the process id, and the handle in the jobs file. */

	DWORD fCreate;
	LONG jobNumber;
	BOOL flags[2];

	STARTUPINFO startUp;
	PROCESS_INFORMATION processInfo;
	LPTSTR targv;
	targv = SkipArg(command, 1, argc, argv);
	

	GetStartupInfo(&startUp);

	/* Determine the options. */
	Options(argc, (LPCTSTR*)argv, _T("cd"), &flags[0], &flags[1], NULL);

	/* Skip over the option field as well, if it exists. */
	/* Simplifying assumptions: There's only one of -d, -c (they are mutually exclusive.
	   Also, commands can't start with -. etc. You may want to fix this. */
	if (argv[1][0] == _T('-'))
		targv = SkipArg(command, 2, argc, argv);

	// targv = _tcsstr(, _T("jobbg")) + 6;

	fCreate = flags[0] ? CREATE_NEW_CONSOLE : flags[1] ? DETACHED_PROCESS : 0;

	_tprintf(_T("%s\n"), targv);
	/* Create the job/thread suspended.
		Resume it once the job is entered properly. */
	if (!CreateProcess(NULL, targv, NULL, NULL, TRUE,
		fCreate | CREATE_SUSPENDED | CREATE_NEW_PROCESS_GROUP,
		NULL, NULL, &startUp, &processInfo)) {
		ReportError(_T("Error starting process."), 0, TRUE);
	}
	if (hJobObject != NULL)
	{
		if (!AssignProcessToJobObject(hJobObject, processInfo.hProcess)) {
			ReportError(_T("Could not add process to job object. The process will be terminated."), 0, TRUE);
			TerminateProcess(processInfo.hProcess, 4);
			// CloseHandle(processInfo.hThread);
			// CloseHandle(processInfo.hProcess);
			return 4;
		}
	}

	/* Create a job number and enter the process Id and handle
		into the Job "data base" maintained by the
		GetJobNumber function (part of the job management library). */

	jobNumber = GetJobNumber(&processInfo, targv);
	if (jobNumber >= 0)
		ResumeThread(processInfo.hThread);
	else {
		TerminateProcess(processInfo.hProcess, 3);
		CloseHandle(processInfo.hThread);
		CloseHandle(processInfo.hProcess);
		ReportError(_T("Error: No room in job control list."), 0, FALSE);
		return 5;
	}

	CloseHandle(processInfo.hThread);
	CloseHandle(processInfo.hProcess);
	_tprintf(_T(" [%d] %d\n"), jobNumber, processInfo.dwProcessId);
	return 0;
}

int Jobs(int argc, LPTSTR argv[], LPTSTR command)
{
	JOBOBJECT_BASIC_ACCOUNTING_INFORMATION basicInfo;

	if (!DisplayJobs()) return 1;
	/* Dispaly the job information */
	if (!QueryInformationJobObject(hJobObject, JobObjectBasicAccountingInformation, &basicInfo, sizeof(JOBOBJECT_BASIC_ACCOUNTING_INFORMATION), NULL)) {
		ReportError(_T("Failed QueryInformationJobObject"), 0, TRUE);
		return 0;
	}
	_tprintf(_T("Total Processes: %d, Active: %d, Terminated: %d.\n"),
		basicInfo.TotalProcesses, basicInfo.ActiveProcesses, basicInfo.TotalTerminatedProcesses);
	_tprintf(_T("User time all processes: %d.%03d\n"),
		basicInfo.TotalUserTime.QuadPart / MILLION, (basicInfo.TotalUserTime.QuadPart % MILLION) / 10000);

	return 0;
}

int Kill(int argc, LPTSTR argv[], LPTSTR command)
{
	DWORD processId, jobNumber, iJobNo;
	HANDLE hProcess;
	BOOL cntrlC, cntrlB, killed;

	iJobNo = Options(argc, (LPCTSTR*)argv, _T("bc"), &cntrlB, &cntrlC, NULL);

	/* Find the process ID associated with this job. */

	jobNumber = _ttoi(argv[iJobNo]);
	processId = FindProcessId(jobNumber);
	if (processId == 0) {
		ReportError(_T("Job number not found.\n"), 0, FALSE);
		return 1;
	}
	hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, processId);
	if (hProcess == NULL) {
		ReportError(_T("Process already terminated.\n"), 0, FALSE);
		return 2;
	}

	if (cntrlB)
		killed = GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, 0);
	else if (cntrlC)
		killed = GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0);
	else
		killed = TerminateProcess(hProcess, JM_EXIT_CODE);

	if (!killed) {
		ReportError(_T("Process termination failed."), 0, TRUE);
		return 3;
	}

	WaitForSingleObject(hProcess, 5000);
	CloseHandle(hProcess);

	_tprintf(_T("Job [%d] terminated or timed out\n"), jobNumber);
	return 0;
}

static int PipeChain(int argc, LPTSTR argv[], LPTSTR cLine)
{
	DWORD i;
	HANDLE hReadPipe, hWritePipe;
	TCHAR command1[MAX_PATH];
	SECURITY_ATTRIBUTES pipeSA = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
	/* Initialize for inheritable handles. */

	PROCESS_INFORMATION procInfo1, procInfo2;
	STARTUPINFO startInfoCh1, startInfoCh2;
	LPTSTR targv;

	/* Startup info for the two child processes. */

	GetStartupInfo(&startInfoCh1);
	GetStartupInfo(&startInfoCh2);

	if (cLine == NULL)
		ReportError(_T("\nCannot read command line."), 1, TRUE);
	SkipArg(cLine, 1, argc, argv);

	targv = cLine;

	i = 0;		/* Get the two commands. */
	while (*targv != _T('|') && *targv != _T('\0')) {
		command1[i] = *targv;
		targv++; i++;
	}
	command1[i] = _T('\0');
	if (*targv == _T('\0'))
		ReportError(_T("No command separator found."), 2, FALSE);

	/* Skip past the = and white space to the start of the second command */
	targv++;
	while (*targv != '\0' && (*targv == ' ' || *targv == '\t')) targv++;
	if (*targv == _T('\0'))
		ReportError(_T("Second command not found."), 2, FALSE);

	/* Create an anonymous pipe with default size.
		The handles are inheritable. */

	if (!CreatePipe(&hReadPipe, &hWritePipe, &pipeSA, 0))
		ReportError(_T("Anon pipe create failed."), 3, TRUE);

	/* Set the output handle to the inheritable pipe handle,
		and create the first processes. */

	startInfoCh1.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	startInfoCh1.hStdError = GetStdHandle(STD_ERROR_HANDLE);
	startInfoCh1.hStdOutput = hWritePipe;
	startInfoCh1.dwFlags = STARTF_USESTDHANDLES;

	if (!CreateProcess(NULL, command1, NULL, NULL,
		TRUE,			/* Inherit handles. */
		0, NULL, NULL, &startInfoCh1, &procInfo1)) {
		ReportError(_T("CreateProc1 failed."), 4, TRUE);
	}
	lpCur = &procInfo1;
	CloseHandle(procInfo1.hThread);
	CloseHandle(hWritePipe);

	/* Repeat (symmetrically) for the second process. */

	startInfoCh2.hStdInput = hReadPipe;
	startInfoCh2.hStdError = GetStdHandle(STD_ERROR_HANDLE);
	startInfoCh2.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	startInfoCh2.dwFlags = STARTF_USESTDHANDLES;

	if (!CreateProcess(NULL, targv, NULL, NULL,
		TRUE,			/* Inherit handles. */
		0, NULL, NULL, &startInfoCh2, &procInfo2))
		ReportError(_T("CreateProc2 failed."), 5, TRUE);
	CloseHandle(procInfo2.hThread);
	CloseHandle(hReadPipe);

	/* Wait for both processes to complete.
		The first one should finish first, although it really does not matter. */

	WaitForSingleObject(procInfo1.hProcess, INFINITE);
	WaitForSingleObject(procInfo2.hProcess, INFINITE);
	CloseHandle(procInfo1.hProcess);
	CloseHandle(procInfo2.hProcess);
	lpCur = NULL;
	return 0;
}

static BOOL EventHandler(DWORD dwCtrlType)
{
	switch (dwCtrlType)
	{
	case CTRL_C_EVENT:
		if (lpCur == NULL) break;
		TerminateProcess(lpCur->hProcess, JM_EXIT_CODE);
		_tprintf(_T("\nKeyInterrupt : the process has been terminated.\n"));
		lpCur = NULL;
		break;
	case CTRL_CLOSE_EVENT:
		if (lpCur != NULL) TerminateProcess(lpCur->hProcess, JM_EXIT_CODE);
		TerminateProcess(GetCurrentProcess(), JM_EXIT_CODE);
		break;
	case CTRL_BREAK_EVENT:
		break;
	case CTRL_LOGOFF_EVENT:
		break;
	case CTRL_SHUTDOWN_EVENT:
		if (lpCur != NULL) TerminateProcess(lpCur->hProcess, JM_EXIT_CODE);
		TerminateProcess(GetCurrentProcess(), JM_EXIT_CODE);
		break;
	}
	return TRUE;
}