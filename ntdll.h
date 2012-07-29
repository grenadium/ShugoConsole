#pragma once

#include <QString>
#include <QList>

#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>

#define STATUS_SUCCESS 0x00000000L 

typedef enum _SYSTEM_INFORMATION_CLASS {
	SystemInformationClassMin = 0,
	SystemBasicInformation = 0,
	SystemProcessorInformation = 1,
	SystemPerformanceInformation = 2,
	SystemTimeOfDayInformation = 3,
	SystemPathInformation = 4,
	SystemNotImplemented1 = 4,
	SystemProcessInformation = 5,
	SystemProcessesAndThreadsInformation = 5,
	SystemCallCountInfoInformation = 6,
	SystemCallCounts = 6,
	SystemDeviceInformation = 7,
	SystemConfigurationInformation = 7,
	SystemProcessorPerformanceInformation = 8,
	SystemProcessorTimes = 8,
	SystemFlagsInformation = 9,
	SystemGlobalFlag = 9,
	SystemCallTimeInformation = 10,
	SystemNotImplemented2 = 10,
	SystemModuleInformation = 11,
	SystemLocksInformation = 12,
	SystemLockInformation = 12,
	SystemStackTraceInformation = 13,
	SystemNotImplemented3 = 13,
	SystemPagedPoolInformation = 14,
	SystemNotImplemented4 = 14,
	SystemNonPagedPoolInformation = 15,
	SystemNotImplemented5 = 15,
	SystemHandleInformation = 16,
	SystemObjectInformation = 17,
	SystemPageFileInformation = 18,
	SystemPagefileInformation = 18,
	SystemVdmInstemulInformation = 19,
	SystemInstructionEmulationCounts = 19,
	SystemVdmBopInformation = 20,
	SystemInvalidInfoClass1 = 20,
	SystemFileCacheInformation = 21,
	SystemCacheInformation = 21,
	SystemPoolTagInformation = 22,
	SystemInterruptInformation = 23,
	SystemProcessorStatistics = 23,
	SystemDpcBehaviourInformation = 24,
	SystemDpcInformation = 24,
	SystemFullMemoryInformation = 25,
	SystemNotImplemented6 = 25,
	SystemLoadImage = 26,
	SystemUnloadImage = 27,
	SystemTimeAdjustmentInformation = 28,
	SystemTimeAdjustment = 28,
	SystemSummaryMemoryInformation = 29,
	SystemNotImplemented7 = 29,
	SystemNextEventIdInformation = 30,
	SystemNotImplemented8 = 30,
	SystemEventIdsInformation = 31,
	SystemNotImplemented9 = 31,
	SystemCrashDumpInformation = 32,
	SystemExceptionInformation = 33,
	SystemCrashDumpStateInformation = 34,
	SystemKernelDebuggerInformation = 35,
	SystemContextSwitchInformation = 36,
	SystemRegistryQuotaInformation = 37,
	SystemLoadAndCallImage = 38,
	SystemPrioritySeparation = 39,
	SystemPlugPlayBusInformation = 40,
	SystemNotImplemented10 = 40,
	SystemDockInformation = 41,
	SystemNotImplemented11 = 41,
	/* SystemPowerInformation = 42, Conflicts with POWER_INFORMATION_LEVEL 1 */
	SystemInvalidInfoClass2 = 42,
	SystemProcessorSpeedInformation = 43,
	SystemInvalidInfoClass3 = 43,
	SystemCurrentTimeZoneInformation = 44,
	SystemTimeZoneInformation = 44,
	SystemLookasideInformation = 45,
	SystemSetTimeSlipEvent = 46,
	SystemCreateSession = 47,
	SystemDeleteSession = 48,
	SystemInvalidInfoClass4 = 49,
	SystemRangeStartInformation = 50,
	SystemVerifierInformation = 51,
	SystemAddVerifier = 52,
	SystemSessionProcessesInformation	= 53,
	SystemInformationClassMax
} SYSTEM_INFORMATION_CLASS;

typedef struct _CLIENT_ID {
    HANDLE UniqueProcess;
    HANDLE UniqueThread;
} CLIENT_ID;
typedef CLIENT_ID *PCLIENT_ID;

typedef struct _UNICODE_STRING {
 WORD Length;
 WORD MaximumLength;
 PWSTR Buffer;
 } UNICODE_STRING;

 typedef struct _VM_COUNTERS {
 DWORD PeakVirtualSize;
 DWORD VirtualSize;
 DWORD PageFaultCount;
 DWORD PeakWorkingSetSize;
 DWORD WorkingSetSize;
 DWORD QuotaPeakPagedPoolUsage;
 DWORD QuotaPagedPoolUsage;
 DWORD QuotaPeakNonPagedPoolUsage;
 DWORD QuotaNonPagedPoolUsage;
 DWORD PagefileUsage;
 DWORD PeakPagefileUsage;
 } VM_COUNTERS;

typedef struct _SYSTEM_THREAD_INFORMATION {
    LARGE_INTEGER   KernelTime;
    LARGE_INTEGER   UserTime;
    LARGE_INTEGER   CreateTime;
    ULONG			WaitTime;
    PVOID			StartAddress;
    CLIENT_ID	    ClientId;
    LONG     	    Priority;
    LONG      	    BasePriority;
    ULONG			ContextSwitchCount;
    LONG			State;
    LONG			WaitReason;
} SYSTEM_THREAD_INFORMATION, * PSYSTEM_THREAD_INFORMATION;

typedef ULONG KPRIORITY;

typedef struct _SYSTEM_PROCESS_INFORMATION {
    ULONG			NextEntryDelta;
    ULONG			ThreadCount;
    LARGE_INTEGER   Reserved[3];
    LARGE_INTEGER   CreateTime;
    LARGE_INTEGER   UserTime;
    LARGE_INTEGER   KernelTime;
    UNICODE_STRING  ProcessName;
    KPRIORITY       BasePriority;
    HANDLE			ProcessId;
    HANDLE			InheritedFromProcessId;
    ULONG           HandleCount;
	ULONG           Reserved2[2];
	ULONG           PrivatePageCount;
    VM_COUNTERS	    VmCounters;
    IO_COUNTERS	    IoCounters;
    SYSTEM_THREAD_INFORMATION Threads[0];
} SYSTEM_PROCESS_INFORMATION, * PSYSTEM_PROCESS_INFORMATION;

class NtDll
{
public:
	typedef long (__stdcall *NtQuerySystemInformation_t)( SYSTEM_INFORMATION_CLASS,PVOID,ULONG,PULONG );
	static NtQuerySystemInformation_t NtQuerySystemInformation;

	static bool load();
private:
	NtDll();
	~NtDll();

};

class SystemInformationQuery
{
public:
	SystemInformationQuery();
	~SystemInformationQuery();

	SYSTEM_PROCESS_INFORMATION* query();
	SYSTEM_PROCESS_INFORMATION* next(SYSTEM_PROCESS_INFORMATION*);

	QList<HANDLE> findProcessByImageName(const QString& processname);
private:	
	ULONG buffersize;
	void* buffer;
};