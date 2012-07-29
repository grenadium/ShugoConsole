#include "ntdll.h"

NtDll::NtQuerySystemInformation_t NtDll::NtQuerySystemInformation = (NtDll::NtQuerySystemInformation_t)0;

bool NtDll::load()
{
	HMODULE ntdll = LoadLibraryA("ntdll.dll");
	NtQuerySystemInformation = (NtQuerySystemInformation_t) GetProcAddress(ntdll,"NtQuerySystemInformation");

	return NtQuerySystemInformation != (NtQuerySystemInformation_t)0;
}

QList<HANDLE> SystemInformationQuery::findProcessByImageName(const QString& processname)
{
	QList<HANDLE> lst;
	SYSTEM_PROCESS_INFORMATION *pi = query();

	while(pi != NULL)
	{
		if(processname == QString::fromUtf16((const ushort*)pi->ProcessName.Buffer))
		{
			lst.append(pi->ProcessId);
		}
		pi = next(pi);
	}

	return lst;
}

SystemInformationQuery::SystemInformationQuery() : buffersize(131072), buffer(NULL)
{
	buffer = malloc(buffersize);
}

SYSTEM_PROCESS_INFORMATION* SystemInformationQuery::query()
{
	LONG result;
	ULONG newsize;

	do
	{
		result = NtDll::NtQuerySystemInformation(SystemProcessInformation, buffer, buffersize, &newsize);
		if(newsize > buffersize)
		{
			buffer = realloc(buffer, newsize);
		}
	}
	while(result != STATUS_SUCCESS);

	return (SYSTEM_PROCESS_INFORMATION*) buffer;
}

SYSTEM_PROCESS_INFORMATION* SystemInformationQuery::next(SYSTEM_PROCESS_INFORMATION* pi)
{
	return  pi->NextEntryDelta != 0 ? (SYSTEM_PROCESS_INFORMATION*)((BYTE*)pi + pi->NextEntryDelta) : NULL;
}

SystemInformationQuery::~SystemInformationQuery()
{
	if(buffer)
	{
		free(buffer);
		buffer = NULL;
	}
}