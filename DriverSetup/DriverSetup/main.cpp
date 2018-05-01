#include<stdio.h>
#include<windows.h>

#define FORMAT_DESCP \
	"Format:\n\
	\tDriverSetup Install TargetPath\n\
	\tDriverSetup Uninstall\n\
And be sure directory ends with '\\'\n"

void Install(char *TargetPath) {
	// Use cmd command to install driver file
	system("rundll32 setupapi,InstallHinfSection DefaultInstall 128 .\\WinFSFilter.inf");

	// Transfer DOS device name to NT device name
	char szNtDeviceName[MAX_PATH];
	char DriveLetter[4] = { 0 };
	memcpy(DriveLetter, TargetPath, 2 * sizeof(char));

	if (QueryDosDevice(DriveLetter, szNtDeviceName, MAX_PATH))
	{
		strcat_s(szNtDeviceName, MAX_PATH, TargetPath + 2);
		printf("%s is linked to %s\n", TargetPath, szNtDeviceName);
	}

	// Write NT device name of target path to C:\WinFSFilter.cfg
	HANDLE hFile = CreateFile("C:\\WinFSFilter.cfg", GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		printf(TEXT("Open File Failed %d\r\n"), GetLastError());
	}
	else if (hFile) {
		char *p = szNtDeviceName;
		int l = 0;
		while (*p)
			l++, p++;
		DWORD w;
		wchar_t WriteBuffer[MAX_PATH] = { 0 };
		wchar_t *wp = WriteBuffer;
		p = szNtDeviceName;
		while (*p) {
			*wp = *p;
			wp++;
			p++;
		}
		WriteFile(hFile, WriteBuffer, l * sizeof(wchar_t), &w, NULL);
	}
	CloseHandle(hFile);

	system("sc start winfsfilter");
}

void Uninstall() {
	system("sc stop winfsfilter");
	system("rundll32 setupapi,InstallHinfSection DefaultUninstall 128 .\\WinFSFilter.inf");
	system("del C:\\WinFSFilter.cfg");
}

int main(int argc, char **argv) {
	if (argc < 2) {
		printf(FORMAT_DESCP);
		return -1;
	}

	if (*(argv[1]) == 'I' || *(argv[1]) == 'i') {
		if (argc < 3) {
			printf(FORMAT_DESCP);
			return -1;
		}
		Install(argv[2]);
	}
	else if (*(argv[1]) == 'U' || *(argv[1]) == 'u') {
		Uninstall();
	}
	else {
		printf(FORMAT_DESCP);
		return -1;
	}
	return 0;
}