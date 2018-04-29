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

// Simulate Config
#define SP_DIR "C:\\1\\"
#define READ_ACCESS 0
#define WRITE_ACCESS 1
#define DELETE_ACCESS 0
#define TARGET L"\\Device\\HarddiskVolume2\\1\\"