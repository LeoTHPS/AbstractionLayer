#pragma once
#include "AL/DotNET/Common.hpp"

namespace AL::DotNET::OS
{
	public enum class RtlPrivileges
		: typename Get_Enum_Base<AL::OS::RtlPrivileges>::Type
	{
		SeCreateTokenPrivilege          = static_cast<typename Get_Enum_Base<AL::OS::RtlPrivileges>::Type>(AL::OS::RtlPrivileges::SeCreateTokenPrivilege),
		SeAssignPrimaryTokenPrivilege   = static_cast<typename Get_Enum_Base<AL::OS::RtlPrivileges>::Type>(AL::OS::RtlPrivileges::SeAssignPrimaryTokenPrivilege),
		SeLockMemoryPrivilege           = static_cast<typename Get_Enum_Base<AL::OS::RtlPrivileges>::Type>(AL::OS::RtlPrivileges::SeLockMemoryPrivilege),
		SeIncreaseQuotaPrivilege        = static_cast<typename Get_Enum_Base<AL::OS::RtlPrivileges>::Type>(AL::OS::RtlPrivileges::SeIncreaseQuotaPrivilege),
		SeUnsolicitedInputPrivilege     = static_cast<typename Get_Enum_Base<AL::OS::RtlPrivileges>::Type>(AL::OS::RtlPrivileges::SeUnsolicitedInputPrivilege),
		SeMachineAccountPrivilege       = static_cast<typename Get_Enum_Base<AL::OS::RtlPrivileges>::Type>(AL::OS::RtlPrivileges::SeMachineAccountPrivilege),
		SeTcbPrivilege                  = static_cast<typename Get_Enum_Base<AL::OS::RtlPrivileges>::Type>(AL::OS::RtlPrivileges::SeTcbPrivilege),
		SeSecurityPrivilege             = static_cast<typename Get_Enum_Base<AL::OS::RtlPrivileges>::Type>(AL::OS::RtlPrivileges::SeSecurityPrivilege),
		SeTakeOwnershipPrivilege        = static_cast<typename Get_Enum_Base<AL::OS::RtlPrivileges>::Type>(AL::OS::RtlPrivileges::SeTakeOwnershipPrivilege),
		SeLoadDriverPrivilege           = static_cast<typename Get_Enum_Base<AL::OS::RtlPrivileges>::Type>(AL::OS::RtlPrivileges::SeLoadDriverPrivilege),
		SeSystemProfilePrivilege        = static_cast<typename Get_Enum_Base<AL::OS::RtlPrivileges>::Type>(AL::OS::RtlPrivileges::SeSystemProfilePrivilege),
		SeSystemtimePrivilege           = static_cast<typename Get_Enum_Base<AL::OS::RtlPrivileges>::Type>(AL::OS::RtlPrivileges::SeSystemtimePrivilege),
		SeProfileSingleProcessPrivilege = static_cast<typename Get_Enum_Base<AL::OS::RtlPrivileges>::Type>(AL::OS::RtlPrivileges::SeProfileSingleProcessPrivilege),
		SeIncreaseBasePriorityPrivilege = static_cast<typename Get_Enum_Base<AL::OS::RtlPrivileges>::Type>(AL::OS::RtlPrivileges::SeIncreaseBasePriorityPrivilege),
		SeCreatePagefilePrivilege       = static_cast<typename Get_Enum_Base<AL::OS::RtlPrivileges>::Type>(AL::OS::RtlPrivileges::SeCreatePagefilePrivilege),
		SeCreatePermanentPrivilege      = static_cast<typename Get_Enum_Base<AL::OS::RtlPrivileges>::Type>(AL::OS::RtlPrivileges::SeCreatePermanentPrivilege),
		SeBackupPrivilege               = static_cast<typename Get_Enum_Base<AL::OS::RtlPrivileges>::Type>(AL::OS::RtlPrivileges::SeBackupPrivilege),
		SeRestorePrivilege              = static_cast<typename Get_Enum_Base<AL::OS::RtlPrivileges>::Type>(AL::OS::RtlPrivileges::SeRestorePrivilege),
		SeShutdownPrivilege             = static_cast<typename Get_Enum_Base<AL::OS::RtlPrivileges>::Type>(AL::OS::RtlPrivileges::SeShutdownPrivilege),
		SeDebugPrivilege                = static_cast<typename Get_Enum_Base<AL::OS::RtlPrivileges>::Type>(AL::OS::RtlPrivileges::SeDebugPrivilege),
		SeAuditPrivilege                = static_cast<typename Get_Enum_Base<AL::OS::RtlPrivileges>::Type>(AL::OS::RtlPrivileges::SeAuditPrivilege),
		SeSystemEnvironmentPrivilege    = static_cast<typename Get_Enum_Base<AL::OS::RtlPrivileges>::Type>(AL::OS::RtlPrivileges::SeSystemEnvironmentPrivilege),
		SeChangeNotifyPrivilege         = static_cast<typename Get_Enum_Base<AL::OS::RtlPrivileges>::Type>(AL::OS::RtlPrivileges::SeChangeNotifyPrivilege),
		SeRemoteShutdownPrivilege       = static_cast<typename Get_Enum_Base<AL::OS::RtlPrivileges>::Type>(AL::OS::RtlPrivileges::SeRemoteShutdownPrivilege),
		SeUndockPrivilege               = static_cast<typename Get_Enum_Base<AL::OS::RtlPrivileges>::Type>(AL::OS::RtlPrivileges::SeUndockPrivilege),
		SeSyncAgentPrivilege            = static_cast<typename Get_Enum_Base<AL::OS::RtlPrivileges>::Type>(AL::OS::RtlPrivileges::SeSyncAgentPrivilege),
		SeEnableDelegationPrivilege     = static_cast<typename Get_Enum_Base<AL::OS::RtlPrivileges>::Type>(AL::OS::RtlPrivileges::SeEnableDelegationPrivilege),
		SeManageVolumePrivilege         = static_cast<typename Get_Enum_Base<AL::OS::RtlPrivileges>::Type>(AL::OS::RtlPrivileges::SeManageVolumePrivilege),
		SeImpersonatePrivilege          = static_cast<typename Get_Enum_Base<AL::OS::RtlPrivileges>::Type>(AL::OS::RtlPrivileges::SeImpersonatePrivilege),
		SeCreateGlobalPrivilege         = static_cast<typename Get_Enum_Base<AL::OS::RtlPrivileges>::Type>(AL::OS::RtlPrivileges::SeCreateGlobalPrivilege),
		SeTrustedCredManAccessPrivilege = static_cast<typename Get_Enum_Base<AL::OS::RtlPrivileges>::Type>(AL::OS::RtlPrivileges::SeTrustedCredManAccessPrivilege),
		SeRelabelPrivilege              = static_cast<typename Get_Enum_Base<AL::OS::RtlPrivileges>::Type>(AL::OS::RtlPrivileges::SeRelabelPrivilege),
		SeIncreaseWorkingSetPrivilege   = static_cast<typename Get_Enum_Base<AL::OS::RtlPrivileges>::Type>(AL::OS::RtlPrivileges::SeIncreaseWorkingSetPrivilege),
		SeTimeZonePrivilege             = static_cast<typename Get_Enum_Base<AL::OS::RtlPrivileges>::Type>(AL::OS::RtlPrivileges::SeTimeZonePrivilege),
		SeCreateSymbolicLinkPrivilege   = static_cast<typename Get_Enum_Base<AL::OS::RtlPrivileges>::Type>(AL::OS::RtlPrivileges::SeCreateSymbolicLinkPrivilege)
	};

	public ref class ntdll abstract sealed
	{
	public:
		static NTSTATUS RtlAdjustPrivilege(RtlPrivileges ulPrivilege, bool bEnable, bool bCurrentThread)
		{
			bool bOldValue;

			return ntdll::RtlAdjustPrivilege(
				ulPrivilege,
				bEnable,
				bCurrentThread,
				bOldValue
			);
		}
		static NTSTATUS RtlAdjustPrivilege(RtlPrivileges ulPrivilege, bool bEnable, bool bCurrentThread, [::System::Runtime::InteropServices::OutAttribute] bool% pbOldValue)
		{
			BOOLEAN bOldValue;

			auto status = AL::OS::RtlAdjustPrivilege(
				static_cast<AL::OS::RtlPrivileges>(ulPrivilege),
				bEnable ? 1 : 0,
				bCurrentThread ? 1 : 0,
				&bOldValue
			);

			pbOldValue = bOldValue != 0;

			return status;
		}

		static NTSTATUS NtQueryTimerResolution([::System::Runtime::InteropServices::OutAttribute] ::System::UInt32 pulMaximumResolution, [::System::Runtime::InteropServices::OutAttribute] ::System::UInt32 pulMinimumResolution, [::System::Runtime::InteropServices::OutAttribute] ::System::UInt32 pulCurrentResolution)
		{
			ULONG ulMaximumResolution;
			ULONG ulMinimumResolution;
			ULONG ulCurrentResolution;

			auto status = AL::OS::NtQueryTimerResolution(
				&ulMaximumResolution,
				&ulMinimumResolution,
				&ulCurrentResolution
			);

			pulMaximumResolution = ulMaximumResolution;
			pulMinimumResolution = ulMinimumResolution;
			pulCurrentResolution = ulCurrentResolution;

			return status;
		}

		static NTSTATUS NtSetTimerResolution(::System::UInt32 ulDesiredResolution, bool bSetResolution, [::System::Runtime::InteropServices::OutAttribute] ::System::UInt32% pulCurrentResolution)
		{
			ULONG ulCurrentResolution;

			auto status = AL::OS::NtSetTimerResolution(
				static_cast<LONG>(ulDesiredResolution),
				bSetResolution ? 1 : 0,
				&ulCurrentResolution
			);

			pulCurrentResolution = ulCurrentResolution;

			return status;
		}

//		static NTSTATUS NtRaiseHardError(::System::Int32 lErrorStatus, ::System::UInt32 ulNumberOfParameters, ::System::UInt32 ulUnicodeStringParameterMask, PULONG_PTR pulParameters);
//		static NTSTATUS NtRaiseHardError(::System::Int32 lErrorStatus, ::System::UInt32 ulNumberOfParameters, ::System::UInt32 ulUnicodeStringParameterMask, PULONG_PTR pulParameters, ::System::UInt32 ulValidResponseOptions, [::System::Runtime::InteropServices::OutAttribute] ::System::UInt32 pulResponse);
	};
}
