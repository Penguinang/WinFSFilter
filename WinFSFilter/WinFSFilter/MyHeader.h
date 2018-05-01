#pragma once


//#define FORCE_DBG_PRINT(_string)					\
//	DbgPrintEx(DPFLTR_DEFAULT_ID, DPFLTR_ERROR_LEVEL, _string)

// Use error level print to be sure this wont be filtered
#define FORCE_DBG_PRINT(_string)			\
	DbgPrintEx(DPFLTR_DEFAULT_ID, DPFLTR_ERROR_LEVEL, _string)

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
//wchar_t *TARGET = L"\\Device\\HarddiskVolume2\\1\\";
#define  BUFFER_SIZE 1024
wchar_t TARGET[BUFFER_SIZE] = { 0 };
int IS_TARGET_FILE = 1;