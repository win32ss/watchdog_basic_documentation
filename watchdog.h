// win32 - July 4 2022
// Custom header for watchdog.sys, based on an amd64 version of the driver from around NT 6.2.
// This driver was introduced with Windows XP/Server 2003 and is used frequently by the DirectX Graphics Kernel.

#pragma once

#include <ntddk.h>

#include <ntddvdeo.h>

#define WDAPI NTAPI

typedef enum _WD_DEBUG_FLAG{
  WdDebugFlagBreakOnAssertion,
  WdDebugFlagBreakOnError
} WD_DEBUG_FLAG, *PWD_DEBUG_FLAG;

typedef enum _WD_EVENT_TYPE{
  WdNoEvent = 0x1,
  WdTimeoutEvent = 0x2,
  WdRecoveryEvent = 0x3
} WD_EVENT_TYPE, *PWD_EVENT_TYPE;

typedef enum _WD_TIME_TYPE{
  WdKernelTime = 0x1,
  WdUserTime = 0x2,
  WdFullTime = 0x3
} WD_TIME_TYPE, *PWD_TIME_TYPE;

typedef enum _WD_OBJECT_TYPE{
  WdStandardWatchdog = 0x57536457,
  WdDeferredWatchdog = 0x57446457
} WD_OBJECT_TYPE, *PWD_OBJECT_TYPE;

typedef enum _WD_LOGEVENT_LEVEL {
  WdCriticalError,
  WdAssertion,
  WdError,
  WdWarning,
  WdEvent,
  WdTrace,
  WdLowResource,
  WdDmmEvent,
  WdLogLevelAll,
  WdLogLevelFirst = 0
} WD_LOGEVENT_LEVEL, *PWD_LOGEVENT_LEVEL;

typedef enum _SESSION_CALLBACK_REASON{
  SESSION_CALLBACK_REASON_CHECK_SESSION,
  SESSION_CALLBACK_REASON_SESSION_DONE,
  SESSION_CALLBACK_REASON_CLEANUP_CONTEXT
} SESSION_CALLBACK_REASON, *PSESSION_CALLBACK_REASON;

typedef struct _WATCHDOG_OBJECT{
  WD_OBJECT_TYPE ObjectType;
  LONG ReferenceCount;
  ULONG OwnerTag;
  PDEVICE_OBJECT DeviceObject;
  WD_TIME_TYPE TimeType;
  WD_EVENT_TYPE LastEvent;
  PKTHREAD LastQueuedThread;
  ULONG64 SpinLock;
  PVOID Context;
} WATCHDOG_OBJECT, *PWATCHDOG_OBJECT;

typedef struct _WATCHDOG{
  WATCHDOG_OBJECT Header;
  ULONG StartCount;
  ULONG SuspendCount;
  ULONG LastKernelTime;
  ULONG LastUserTime;
  ULONG TimeIncrement;
  LARGE_INTEGER DueTime;
  LARGE_INTEGER InitialDueTime;
  PKTHREAD Thread;
  KTIMER Timer;
  KDPC TimerDpc;
  KDPC *ClientDpc;
} WATCHDOG, *PWATCHDOG;

typedef struct _DEFERRED_WATCHDOG{
  WATCHDOG_OBJECT Header;
  ULONG Period;
  ULONG SuspendCount;
  volatile ULONG InCount;
  volatile ULONG InCountComplete;
  ULONG OutCount;
  ULONG LastInCount;
  ULONG LastOutCount;
  ULONG LastKernelTime;
  ULONG LastUserTime;
  ULONG TimeIncrement;
  LONG Trigger;
  ULONG State;
  PKTHREAD Thread;
  KTIMER Timer;
  KDPC TimerDpc;
  KDPC *ClientDpc;
  BOOLEAN Reentrant;
} DEFERRED_WATCHDOG, *PDEFERRED_WATCHDOG;

typedef struct _WD_LOGENTRY{
  ULONG EventOrder;
  PKTHREAD Thread;
  ULONG64 ReturnAddress;
  ULONG64 Parameter[5]; // seems to hold a variety of error codes; index 1 seemingly for NTSTATUS, index 0 for something more application-specific.
                        // In the case of the logging of a critical error; Parameter represents the bug check code and parameters in respective order.
} WD_LOGENTRY, *PWD_LOGENTRY;

typedef struct _WD_WER_CONTEXT{
  PVOID hReport;
  WCHAR wszBasePath[260];
  WCHAR wszReportType[16];
  WCHAR wszReportId[32];
  WCHAR wszFileName[260];
} WD_WER_CONTEXT, *PWD_WER_CONTEXT;

typedef struct _WD_DEBUG_REPORT{
  PDEVICE_OBJECT pDeviceObject;
  ULONG ulCode;
  ULONG64 ulpArg[4];
  PVOID pvDump;
  ULONG ulDumpSize;
  ULONG ulReportCount;
  WCHAR wszShortDriverName[32];
  WCHAR wszFullDriverName[260];
  WD_WER_CONTEXT WerContext;
} WD_DEBUG_REPORT, *PWD_DEBUG_REPORT;

typedef struct _WD_DEBUG_REPORT_INFO{
  ULONG Size;
  ULONG Code;
  ULONG Count;
  ULONG DumpSize;
} WD_DEBUG_REPORT_INFO, *PWD_DEBUG_REPORT_INFO;

typedef struct _WD_DIAG_CONSISTENCY_CHECK{
  const PGUID pControlGuid;
  const PEVENT_DESCRIPTOR pNotifyUserEvent;
} WD_DIAG_CONSISTENCY_CHECK, *PWD_DIAG_CONSISTENCY_CHECK;

typedef struct _GTF_WORKSET
{
  long double M;
  long double C;
  long double K;
  long double BotMarginLines;
  long double HBlankPixels;
  long double HFreq;
  long double HPeriod;
  long double HPeriodEst;
  long double HPixelRnd;
  long double IdealDutyCycle;
  long double IdealHPeriod;
  long double Interlace;
  long double LeftMarginPixels;
  long double PixelFreq;
  long double RightMarginPixels;
  long double TopMarginLines;
  long double TotalActivePixels;
  long double TotalPixels;
  long double TotalVLines;
  long double VBlankLines;
  long double VBackPorch;
  long double VFieldRate;
  long double VFieldRateEst;
  long double VFieldRateRqd;
  long double VFrameRate;
  long double VFrameRateEst;
  long double VFrameRateRqd;
  long double VLinesRnd;
  long double VSyncBp;
} GTF_WORKSET, *PGTF_WORKSET;

NTSTATUS WDAPI DMgrAcquireGdiViewId(
PULONG UniqueGdiViewId
);

NTSTATUS WDAPI DMgrGetSmbiosInfo(
PVOID Data, 
ULONG DataSize
);

BOOLEAN WDAPI DMgrIsSetupRunning();

void WDAPI DMgrReleaseGdiViewId(
ULONG UniqueGdiViewId, 
BOOLEAN AllowReuse
);

NTSTATUS WDAPI DMgrWriteDeviceCountToRegistry();

int WDAPI GTF_FloatToInteger(
const long double *pFloat, 
int *pInt
);

int WDAPI GTF_FloatToRational(
const long double *pFloat, 
int *pNumerator, 
int *pDenominator
);

int WDAPI GTF_GetGCD(
int a, 
int b
);

void WDAPI GTF_InitWorkset(
PGTF_WORKSET pWs
);

void WDAPI GTF_Normalize(
int *pNumerator, 
int *pDenominator
);

long double WDAPI GTF_Round(
long double x, 
int y
);

long double WDAPI GTF_Sqrt(
long double x
);

long double WDAPI GTF_Square(
long double x
);

void WDAPI GTF_UsingPixelClockFrequency_Stage1(
int IntRqd, 
int HPixels, 
int VLines, 
int IPFreqRqd, 
PGTF_WORKSET pWs
);

void WDAPI GTF_UsingVerticalRefreshFrequency_Stage1(
int IntRqd, 
int HPixels, 
int VLines, 
int IPFreqRqd_Numerator, 
int IPFreqRqd_Denominator, 
PGTF_WORKSET pWs
);

NTSTATUS WDAPI SMgrGdiCallout(
PVIDEO_WIN32K_CALLBACKS_PARAMS CalloutParameters, 
BOOLEAN CallAllSessions, 
BOOLEAN SynchronousCall, 
BOOLEAN ( *pfnSessionCallback)(PVOID, SESSION_CALLBACK_REASON, ULONG), 
PVOID pCallbackContext
);

PEPROCESS WDAPI SMgrGetActiveSessionProcess();

ULONG WDAPI SMgrGetNumberOfSessions();

NTSTATUS WDAPI SMgrNotifySessionChange(
BOOLEAN Created
);

NTSTATUS WDAPI SMgrRegisterGdiCallout(
void ( *Win32kCalloutToRegister)(PVOID)
);

NTSTATUS WDAPI SMgrRegisterSessionChangeCallout(
NTSTATUS (*SessionChangeCalloutToRegister)(BOOLEAN)
);

NTSTATUS WDAPI SMgrUnregisterSessionChangeCallout(
NTSTATUS (*SessionChangeCalloutToRegister)(BOOLEAN)
);

NTSTATUS VpInitialize(); // same as WdInitialize

PDEFERRED_WATCHDOG WDAPI WdAllocateDeferredWatchdog(
PDEVICE_OBJECT pDeviceObject, 
WD_TIME_TYPE timeType, 
ULONG ulTag
);

PWATCHDOG WDAPI WdAllocateWatchdog(
PDEVICE_OBJECT pDeviceObject, 
WD_TIME_TYPE timeType, 
ULONG ulTag
);

/*
A note about every function that takes a "PVOID pWatch" paramter.

They can accept either pointers to WATCHDOG or DEFERRED_WATCHDOG structs as they usually manipulate something in the
header, which is a WATCHDOG_OBJECT struct.

*/

PVOID WDAPI WdAttachContext(
PVOID pWatch, 
ULONG ulSize
);

void WDAPI WdCompleteEvent(
PVOID pWatch,
PKTHREAD pThread
);

NTSTATUS WDAPI WdDbgCreateSnapshot(
const PGUID pSecondaryDataGuid, 
const PVOID pvSecondaryData, 
ULONG ulSecondaryDataSize, 
PVOID *ppvDump, 
PULONG pulDumpSize
);

void WDAPI WdDbgDestroySnapshot(
PVOID pvDump
);

void WDAPI WdDbgReportCancel(
PWD_DEBUG_REPORT pReport
);

void WDAPI WdDbgReportComplete(
PWD_DEBUG_REPORT pReport
);

PWD_DEBUG_REPORT WDAPI WdDbgReportCreate(
PDEVICE_OBJECT pDeviceObject, 
ULONG ulCode, 
ULONGLONG ulpArg1, 
ULONGLONG ulpArg2, 
ULONGLONG ulpArg3, 
ULONGLONG ulpArg4
);

BOOLEAN WDAPI WdDbgReportQueryInfo(
PWD_DEBUG_REPORT pReport, 
PWD_DEBUG_REPORT_INFO pInfo
);

PWD_DEBUG_REPORT WDAPI WdDbgReportRecreate(
PDEVICE_OBJECT pDeviceObject, 
ULONG ulCode, 
ULONGLONG ulpArg1, 
ULONGLONG ulpArg2, 
ULONGLONG ulpArg3, 
ULONGLONG ulpArg4, 
PWD_DEBUG_REPORT pReport
);

BOOLEAN WDAPI WdDbgReportSecondaryData(
PWD_DEBUG_REPORT pReport, 
PVOID pvData, 
ULONG ulDataSize
);

void WDAPI WdDereferenceObject(
PVOID pWatch
);

void WDAPI WdDetachContext(
PVOID pWatch
);

REGHANDLE WDAPI WdDiagGetEtwHandle();

NTSTATUS WDAPI WdDiagInit(
PWD_DIAG_CONSISTENCY_CHECK pConsistencyCheck
);

BOOLEAN WDAPI WdDiagIsTracingEnabled(
const PEVENT_DESCRIPTOR pEvent
);

NTSTATUS WDAPI WdDiagNotifyUser(
ULONG Flags, 
ULONG MessageId, 
USHORT ArgumentsCount, 
WCHAR **Arguments
);

NTSTATUS WDAPI WdDiagShutdown();

void WDAPI WdEnterMonitoredSection(
PDEFERRED_WATCHDOG pWatch
);

void WDAPI WdExitMonitoredSection(
PDEFERRED_WATCHDOG pWatch
);

void WDAPI WdFreeDeferredWatchdog(
PDEFERRED_WATCHDOG pWatch
);

void WDAPI WdFreeWatchdog(
PWATCHDOG pWatch
);

PDEVICE_OBJECT WDAPI WdGetDeviceObject(
PVOID pWatch
);

WD_EVENT_TYPE WDAPI WdGetLastEvent(
PVOID pWatch
);

PDEVICE_OBJECT WDAPI WdGetLowestDeviceObject(
PVOID pWatch
);

NTSTATUS WDAPI WdInitialize();

BOOLEAN WDAPI WdIsDebuggerPresent(
BOOLEAN bRefresh
);

ULONG WDAPI WdLogGetEventOrder();

ULONG64 WDAPI WdLogGetRecentEvents(
WD_LOGEVENT_LEVEL Level, 
ULONG EventsNumber, 
PVOID pBuffer, 
ULONG BufferSize
); // The return value appears to be the size of the returned buffer. It will return 0 for Level == WdLogLevelAll.

// The following set of functions prefixed with WdLogNewEntry5_Wd* initialize WD_LOGENTRY structs

PWD_LOGENTRY WDAPI WdLogNewEntry5_WdAssertion();

PWD_LOGENTRY WDAPI WdLogNewEntry5_WdCriticalError();

PWD_LOGENTRY WDAPI WdLogNewEntry5_WdDmmEvent();

PWD_LOGENTRY WDAPI WdLogNewEntry5_WdError();

PWD_LOGENTRY WDAPI WdLogNewEntry5_WdEvent();

PWD_LOGENTRY WDAPI WdLogNewEntry5_WdLowResource();

PWD_LOGENTRY WDAPI WdLogNewEntry5_WdTrace();

PWD_LOGENTRY WDAPI WdLogNewEntry5_WdWarning();

// The following set of functions prefixed with WdLogEntry5_Wd* dump the struct contents and invoke the debugger if available.

void WDAPI WdLogEntry5_WdAssertion(
PWD_LOGENTRY pEntry
);

void WDAPI WdLogEntry5_WdCriticalError(
PWD_LOGENTRY pEntry
);

void WDAPI WdLogEntry5_WdDmmEvent(
PWD_LOGENTRY pEntry
);

void WDAPI WdLogEntry5_WdError(
PWD_LOGENTRY pEntry
);

void WDAPI WdLogEntry5_WdEvent(
PWD_LOGENTRY pEntry
);

void WDAPI WdLogEntry5_WdLowResource(
PWD_LOGENTRY pEntry
);

void WDAPI WdLogEntry5_WdTrace(
PWD_LOGENTRY pEntry
);

void WDAPI WdLogEntry5_WdWarning(
PWD_LOGENTRY pEntry
);

BOOLEAN WDAPI WdMadeAnyProgress(
PDEFERRED_WATCHDOG pWatch
);

BOOLEAN WDAPI WdQueryDebugFlag(
WD_DEBUG_FLAG Flag
);

void WDAPI WdReferenceObject(
PVOID pWatch
);

void WDAPI WdDbgDestroySnapshot(
PVOID pvDump
);

void WDAPI WdRegFreeInfo(
PVOID hRegKey
);

NTSTATUS WDAPI WdRegOpenSubkey(PVOID *phSubkey, 
ULONG DesiredAccess, 
PVOID hRegKey, 
const PUNICODE_STRING pSubkeyName
); 

NTSTATUS WDAPI WdRegRetrieveSubkeyInfo(
PVOID hRegKey, 
ULONG SubkeyIndex, 
PKEY_BASIC_INFORMATION *ppSubkeyInfo
);

NTSTATUS WDAPI WdRegRetrieveValueInfo(
PVOID hRegKey, 
const PUNICODE_STRING pRegValName, 
PKEY_VALUE_PARTIAL_INFORMATION *ppRegValInfo
);

void WDAPI WdResetDeferredWatch(
PDEFERRED_WATCHDOG pWatch
);

void WDAPI WdResetWatch(
PWATCHDOG pWatch
);

void WDAPI WdResumeDeferredWatch(
PDEFERRED_WATCHDOG pWatch, 
BOOLEAN bIncremental
);

void WDAPI WdResumeWatch(
PWATCHDOG pWatch, 
BOOLEAN bIncremental
);

void WDAPI WdStartDeferredWatch(
PDEFERRED_WATCHDOG pWatch, 
PKDPC pDpc, 
LONG lPeriod
); // initializes a DEFERRED_WATCHDOG instance

void WDAPI WdStartWatch(
PWATCHDOG pWatch, 
LARGE_INTEGER liDueTime, 
PKDPC pDpc
); // initializes a WATCHDOG instance

void WDAPI WdStopDeferredWatch(
PDEFERRED_WATCHDOG pWatch 
); // stops a DEFERRED_WATCHDOG instance

void WDAPI WdStopWatch(
PWATCHDOG pWatch, 
BOOLEAN bIncremental
); // stops a WATCHDOG instance; if bIncremental is FALSE or pWatch->StartCount is equal to 1 beforehand, then all values in pWatch are zeroed and its timer is cancelled

void WDAPI WdSuspendDeferredWatch(
PDEFERRED_WATCHDOG pWatch
); // suspends a DEFERRED_WATCHDOG instance.

void WDAPI WdSuspendWatch(
 PWATCHDOG pWatch
); // Suspends a WATCHDOG instance.

PWD_DEBUG_REPORT WDAPI WdpDbgReportCreateFromDump(
PDEVICE_OBJECT pDeviceObject, 
PVOID pvDump, 
ULONG ulDumpSize
); // Returns a pointer to a WD_DEBUG_REPORT initialized using the function parameters.

void WDAPI WdpInterfaceReferenceNop(
PVOID pContext
); // This function does nothing, as its name suggests.

