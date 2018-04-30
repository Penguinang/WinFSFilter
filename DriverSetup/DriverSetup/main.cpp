#include<stdio.h>
#include<windows.h>

int main(int argc, char **argv) {
	if (argc < 2) {
		printf("Format:\n\
			\tDriverSetup /Install\n\
			\tDriverSetup /Uninstall\n");
		return -1;
	}

	if(*(argv[1]) == 'I' || *(argv[1]) == 'i')
		system("rundll32 setupapi,InstallHinfSection DefaultInstall 128 .\\WinFSFilter.inf");
	else if (*(argv[1]) == 'U' || *(argv[1]) == 'u')
		system("rundll32 setupapi,InstallHinfSection DefaultUninstall 128 .\\WinFSFilter.inf");
	else {
		printf("Format:\n\
			\tDriverSetup /Install\n\
			\tDriverSetup /Uninstall\n");
		return -1;
	}
	return 0;
}