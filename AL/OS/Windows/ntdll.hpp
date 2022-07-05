#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_WINDOWS)
	#warning Platform not supported
#endif

#include <winternl.h>

#if defined(AL_COMPILER_MSVC)
	#pragma comment(lib, "ntdll.lib")
#endif

extern "C"
{
	NTSTATUS NTAPI RtlAdjustPrivilege(ULONG ulPrivilege, BOOLEAN bEnable, BOOLEAN bCurrentThread, PBOOLEAN pbOldValue);

	NTSTATUS NTAPI NtQueryTimerResolution(PULONG pulMaximumResolution, PULONG pulMinimumResolution, PULONG pulCurrentResolution);
	
	NTSTATUS NTAPI NtSetTimerResolution(ULONG ulDesiredResolution, BOOLEAN bSetResolution, PULONG pulCurrentResolution);

	NTSTATUS NTAPI NtRaiseHardError(LONG lErrorStatus, ULONG ulNumberOfParameters, ULONG ulUnicodeStringParameterMask, PULONG_PTR pulParameters, ULONG ulValidResponseOptions, PULONG pulResponse);
}

namespace AL::OS::Windows
{
	enum class RtlPrivileges : ULONG
	{
		SeCreateTokenPrivilege          = 1,
		SeAssignPrimaryTokenPrivilege   = 2,
		SeLockMemoryPrivilege           = 3,
		SeIncreaseQuotaPrivilege        = 4,
		SeUnsolicitedInputPrivilege     = 5,
		SeMachineAccountPrivilege       = 6,
		SeTcbPrivilege                  = 7,
		SeSecurityPrivilege             = 8,
		SeTakeOwnershipPrivilege        = 9,
		SeLoadDriverPrivilege           = 10,
		SeSystemProfilePrivilege        = 11,
		SeSystemtimePrivilege           = 12,
		SeProfileSingleProcessPrivilege = 13,
		SeIncreaseBasePriorityPrivilege = 14,
		SeCreatePagefilePrivilege       = 15,
		SeCreatePermanentPrivilege      = 16,
		SeBackupPrivilege               = 17,
		SeRestorePrivilege              = 18,
		SeShutdownPrivilege             = 19,
		SeDebugPrivilege                = 20,
		SeAuditPrivilege                = 21,
		SeSystemEnvironmentPrivilege    = 22,
		SeChangeNotifyPrivilege         = 23,
		SeRemoteShutdownPrivilege       = 24,
		SeUndockPrivilege               = 25,
		SeSyncAgentPrivilege            = 26,
		SeEnableDelegationPrivilege     = 27,
		SeManageVolumePrivilege         = 28,
		SeImpersonatePrivilege          = 29,
		SeCreateGlobalPrivilege         = 30,
		SeTrustedCredManAccessPrivilege = 31,
		SeRelabelPrivilege              = 32,
		SeIncreaseWorkingSetPrivilege   = 33,
		SeTimeZonePrivilege             = 34,
		SeCreateSymbolicLinkPrivilege   = 35
	};

	inline ::NTSTATUS RtlAdjustPrivilege(RtlPrivileges ulPrivilege, ::BOOLEAN bEnable, ::BOOLEAN bCurrentThread, ::PBOOLEAN pbOldValue)
	{
		return ::RtlAdjustPrivilege(
			static_cast<LONG>(ulPrivilege),
			bEnable,
			bCurrentThread,
			pbOldValue
		);
	}
	inline ::NTSTATUS RtlAdjustPrivilege(RtlPrivileges ulPrivilege, ::BOOLEAN bEnable, ::BOOLEAN bCurrentThread)
	{
		BOOLEAN bOldValue;

		return RtlAdjustPrivilege(
			ulPrivilege,
			bEnable,
			bCurrentThread,
			&bOldValue
		);
	}

	inline ::NTSTATUS NtQueryTimerResolution(::PULONG pulMaximumResolution, ::PULONG pulMinimumResolution, ::PULONG pulCurrentResolution)
	{
		return ::NtQueryTimerResolution(
			pulMaximumResolution,
			pulMinimumResolution,
			pulCurrentResolution
		);
	}

	inline ::NTSTATUS NtSetTimerResolution(::ULONG ulDesiredResolution, ::BOOLEAN bSetResolution, ::PULONG pulCurrentResolution)
	{
		return ::NtSetTimerResolution(
			ulDesiredResolution,
			bSetResolution,
			pulCurrentResolution
		);
	}

	inline ::NTSTATUS NtRaiseHardError(::LONG lErrorStatus, ::ULONG ulNumberOfParameters, ::ULONG ulUnicodeStringParameterMask, ::PULONG_PTR pulParameters, ::ULONG ulValidResponseOptions, ::PULONG pulResponse)
	{
		return ::NtRaiseHardError(
			lErrorStatus,
			ulNumberOfParameters,
			ulUnicodeStringParameterMask,
			pulParameters,
			ulValidResponseOptions,
			pulResponse
		);
	}
	inline ::NTSTATUS NtRaiseHardError(::LONG lErrorStatus, ::ULONG ulNumberOfParameters, ::ULONG ulUnicodeStringParameterMask, ::PULONG_PTR pulParameters)
	{
		ULONG ulResponse;

		return NtRaiseHardError(
			lErrorStatus,
			ulNumberOfParameters,
			ulUnicodeStringParameterMask,
			pulParameters,
			6,
			&ulResponse
		);
	}
}
