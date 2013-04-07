#pragma once
#include "ntlpcapi.h"

NTSYSAPI
VOID
NTAPI
RtlInitUnicodeString(
	_Out_ PUNICODE_STRING DestinationString,
	_In_opt_z_ __drv_aliasesMem PCWSTR SourceString
	);

NTSYSAPI
NTSTATUS
NTAPI
ZwCreateSection (
	_Out_ PHANDLE SectionHandle,
	_In_ ACCESS_MASK DesiredAccess,
	_In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
	_In_opt_ PLARGE_INTEGER MaximumSize,
	_In_ ULONG SectionPageProtection,
	_In_ ULONG AllocationAttributes,
	_In_opt_ HANDLE FileHandle
	);