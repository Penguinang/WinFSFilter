#pragma once

#define ERROR 0x0
#define WARNING 0X1
#define TRACE 0x2
#define INFO  0x3
#define CUR_LEVEL ERROR
#define DPRINT(LEVEL, ...)\
	DbgPrintEx(DPFLTR_DEFAULT_ID, LEVEL, __VA_ARGS__)


#define FLT_PREOP_FUNC(F)\
	FLT_PREOP_CALLBACK_STATUS\
	F(\
		_Inout_ PFLT_CALLBACK_DATA Data,\
		_In_ PCFLT_RELATED_OBJECTS FltObjects,\
		_Flt_CompletionContext_Outptr_ PVOID *CompletionContext\
	);
#define FLT_POSTOP_FUNC(F)\
	FLT_POSTOP_CALLBACK_STATUS\
	F(\
		_Inout_ PFLT_CALLBACK_DATA Data,\
		_In_ PCFLT_RELATED_OBJECTS FltObjects,\
		_In_opt_ PVOID CompletionContext,\
		_In_ FLT_POST_OPERATION_FLAGS Flags\
	);

#define CONFIG_FILE L"\\Device\\HarddiskVolume2\\user.txt"
#define TARGET_CFG_FILE L"\\Device\\HarddiskVolume2\\WinFSFilter.cfg"

int READ_ACCESS = 1;
int WRITE_ACCESS = 1;
int DELETE_ACCESS = 1;
#define  BUFFER_SIZE 1024
wchar_t TARGET[BUFFER_SIZE] = { 0 };
int IS_TARGET_FILE = 1;

//_declspec(dllexport) void WriteError();
//
//_declspec(dllexport) void ReadError();
//
//_declspec(dllexport) void DeleteError();

//typedef void(*ExternWarningFunc)();
//ExternWarningFunc WriteError;
//ExternWarningFunc ReadError;
//ExternWarningFunc DeleteError;

#define EmptyFunction 1 == 1
#define WriteError() EmptyFunction
#define ReadError() EmptyFunction
#define DeleteError() EmptyFunction