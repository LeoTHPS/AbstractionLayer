#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_WINDOWS)
	#error Platform not supported
#endif

#include "AL/OS/SystemException.hpp"

namespace AL::OS::Windows
{
	enum class VirtualMemoryPageTypes : uint32
	{
		// Indicates that the memory pages within the region are mapped into the view of an image section.
		Image   = MEM_IMAGE,
		// Indicates that the memory pages within the region are mapped into the view of a section.
		Mapped  = MEM_MAPPED,
		// Indicates that the memory pages within the region are private (that is, not shared by other processes).
		Private = MEM_PRIVATE
	};

	AL_DEFINE_ENUM_FLAG_OPERATORS(VirtualMemoryPageTypes);

	enum class VirtualMemoryPageStates : uint32
	{
		Free    = MEM_FREE,
		Commit  = MEM_COMMIT,
		Reserve = MEM_RESERVE
	};

	AL_DEFINE_ENUM_FLAG_OPERATORS(VirtualMemoryPageStates);

	enum class VirtualMemoryProtections : uint32
	{
		// Pages in the region become guard pages.
		// Any attempt to access a guard page causes the system to raise a STATUS_GUARD_PAGE_VIOLATION exception and turn off the guard page status.
		// Guard pages thus act as a one-time access alarm.
		Guard              = PAGE_GUARD,
		// Sets all pages to be non-cachable.
		// Applications should not use this attribute except when explicitly required for a device.
		// Using the interlocked functions with memory that is mapped with SEC_NOCACHE can result in an EXCEPTION_ILLEGAL_INSTRUCTION exception.
		// The NoCache flag cannot be used with the Guard, NoAccess, or Write_Combine flags.
		NoCache            = PAGE_NOCACHE,
		// Disables all access to the committed region of pages.
		// An attempt to read from, write to, or execute the committed region results in an access violation.
		NoAccess           = PAGE_NOACCESS,
		// Enables read-only access to the committed region of pages.
		// An attempt to write to the committed region results in an access violation.
		Read               = PAGE_READONLY,
		// Enables read-only or read/write access to the committed region of pages.
		Read_Write         = PAGE_READWRITE,
		// Enables read-only or copy-on-write access to a mapped view of a file mapping object.
		// An attempt to write to a committed copy-on-write page results in a private copy of the page being made for the process.
		// The private page is marked as Read_Write, and the change is written to the new page.
		Write_Copy         = PAGE_WRITECOPY,
		// Sets all pages to be write-combined.
		// Applications should not use this attribute except when explicitly required for a device.
		// Using the interlocked functions with memory that is mapped as write-combined can result in an EXCEPTION_ILLEGAL_INSTRUCTION exception.
		// The Write_Combine flag cannot be specified with the NoAccess, Guard, and NoCache flags.
		Write_Combine      = PAGE_WRITECOMBINE,
		// Enables execute access to the committed region of pages.
		// An attempt to write to the committed region results in an access violation.
		Execute            = PAGE_EXECUTE,
		// Enables execute or read-only access to the committed region of pages.
		// An attempt to write to the committed region results in an access violation.
		Execute_Read       = PAGE_EXECUTE_READ,
		// Enables execute, read-only, or read/write access to the committed region of pages.
		Execute_Read_Write = PAGE_EXECUTE_READWRITE,
		// Enables execute, read-only, or copy-on-write access to a mapped view of a file mapping object.
		// An attempt to write to a committed copy-on-write page results in a private copy of the page being made for the process.
		// The private page is marked as Execute_Read_Write, and the change is written to the new page.
		Execute_Write_Copy = PAGE_EXECUTE_WRITECOPY,
		// Sets all locations in the pages as invalid targets for CFG.
		// Used along with any execute page protection like Execute, Execute_Read, Execute_Read_Write and Execute_Write_Copy.
		// Any indirect call to locations in those pages will fail CFG checks and the process will be terminated.
		// The default behavior for executable pages allocated is to be marked valid call targets for CFG.
		Targets_Invalid    = PAGE_TARGETS_INVALID,
		// Pages in the region will not have their CFG information updated while the protection changes for VirtualProtect.
		// For example, if the pages in the region was allocated using Targets_Invalid, then the invalid information will be maintained while the page protection changes.
		// This flag is only valid when the protection changes to an executable type like Execute, Execute_Read, Execute_Read_Write and Execute_Write_Copy.
		// The default behavior for VirtualProtect protection change to executable is to mark all locations as valid call targets for CFG.
		Targets_NoUpdate   = PAGE_TARGETS_NO_UPDATE
	};

	AL_DEFINE_ENUM_FLAG_OPERATORS(VirtualMemoryProtections);

	enum class VirtualMemoryAllocationTypes : uint32
	{
		// Indicates that data in the memory range specified by lpAddress and dwSize is no longer of interest.
		// The pages should not be read from or written to the paging file.
		// However, the memory block will be used again later, so it should not be decommitted.
		// This value cannot be used with any other value.
		Reset      = MEM_RESET,
		// Allocates memory charges (from the overall size of memory and the paging files on disk) for the specified reserved memory pages.
		// The function also guarantees that when the caller later initially accesses the memory, the contents will be zero.
		// Actual physical pages are not allocated unless/until the virtual addresses are actually accessed.
		Commit     = MEM_COMMIT,
		// Reserves a range of the process's virtual address space without allocating any actual physical storage in memory or in the paging file on disk.
		Reserve    = MEM_RESERVE,
		// ResetUndo should only be called on an address range to which Reset was successfully applied earlier.
		// It indicates that the data in the specified memory range specified by lpAddress and dwSize is of interest to the caller and attempts to reverse the effects of MEM_RESET.
		// If the function succeeds, that means all data in the specified address range is intact.
		// If the function fails, at least some of the data in the address range has been replaced with zeroes.
		// This value cannot be used with any other value.
		// If ResetUndo is called on an address range which was not Reset earlier, the behavior is undefined.
		ResetUndo  = MEM_RESET_UNDO,

		// Allocates memory using large page support.
		// The size and alignment must be a multiple of the large-page minimum.
		// If you specify this value, you must also specify Reserve and Commit.
		LargePages = MEM_LARGE_PAGES,
		// Reserves an address range that can be used to map Address Windowing Extensions (AWE) pages.
		// This value must be used with Reserve and no other values.
		Physical   = MEM_PHYSICAL,
		// Allocates memory at the highest possible address.
		// This can be slower than regular allocations, especially when there are many allocations.
		TopDown    = MEM_TOP_DOWN,
		// Causes the system to track pages that are written to in the allocated region.
		// If you specify this value, you must also specify Reserve.
		WriteWatch = MEM_WRITE_WATCH
	};

	AL_DEFINE_ENUM_FLAG_OPERATORS(VirtualMemoryAllocationTypes);

	struct VirtualMemoryInformation
	{
		Void*                             Base;
		size_t                            Size;
		BitMask<VirtualMemoryPageTypes>   PageTypes;
		BitMask<VirtualMemoryPageStates>  PageStates;
		BitMask<VirtualMemoryProtections> Protection;
	};

	// @throw AL::Exception
	// @return AL::False to stop enumeration
	typedef Function<Bool(const VirtualMemoryInformation& information)> VirtualMemoryEnumPagesCallback;

	class VirtualMemory
	{
		VirtualMemory() = delete;

	public:
		typedef VirtualMemoryPageTypes         PageTypes;
		typedef VirtualMemoryPageStates        PageStates;
		typedef VirtualMemoryInformation       Information;
		typedef VirtualMemoryProtections       Protections;
		typedef VirtualMemoryAllocationTypes   AllocationTypes;

		// @throw AL::Exception
		// @return AL::False to stop enumeration
		typedef VirtualMemoryEnumPagesCallback EnumPagesCallback;

		// @throw AL::Exception
		static Void* Allocate(Void* lpBuffer, size_t size, AllocationTypes type, Protections protection)
		{
			return Allocate(
				::GetCurrentProcess(),
				lpBuffer,
				size,
				type,
				protection
			);
		}
		// @throw AL::Exception
		static Void* Allocate(::HANDLE hProcess, Void* lpBuffer, size_t size, AllocationTypes type, Protections protection)
		{
			::LPVOID value;

			if ((value = ::VirtualAllocEx(hProcess, lpBuffer, static_cast<::SIZE_T>(size), static_cast<::DWORD>(type), static_cast<::DWORD>(protection))) == NULL)
			{

				throw SystemException(
					"VirtualAllocEx"
				);
			}

			return value;
		}

		static Void Release(Void* lpBuffer)
		{
			Release(
				::GetCurrentProcess(),
				lpBuffer
			);
		}
		static Void Release(::HANDLE hProcess, Void* lpBuffer)
		{
			if (!::VirtualFreeEx(hProcess, lpBuffer, 0, MEM_RELEASE))
			{
			}
		}

		// @throw AL::Exception
		static Void Lock(Void* lpBuffer, size_t size)
		{
			if (!::VirtualLock(lpBuffer, static_cast<::SIZE_T>(size)))
			{

				throw SystemException(
					"VirtualLock"
				);
			}
		}

		// @throw AL::Exception
		static Void Unlock(Void* lpBuffer, size_t size)
		{
			if (!::VirtualUnlock(lpBuffer, static_cast<::SIZE_T>(size)))
			{

				throw SystemException(
					"VirtualUnlock"
				);
			}
		}

		// @throw AL::Exception
		// @return previous Protections
		static Protections SetProtection(Void* lpBuffer, size_t size, Protections value)
		{
			return SetProtection(
				::GetCurrentProcess(),
				lpBuffer,
				size,
				value
			);
		}
		// @throw AL::Exception
		// @return previous Protections
		static Protections SetProtection(::HANDLE hProcess, Void* lpBuffer, size_t size, Protections value)
		{
			if (!::VirtualProtectEx(hProcess, lpBuffer, static_cast<::SIZE_T>(size), static_cast<::DWORD>(value), reinterpret_cast<::PDWORD>(&value)))
			{

				throw SystemException(
					"VirtualProtectEx"
				);
			}

			return value;
		}

		// @throw AL::Exception
		static Information Query(Void* lpBuffer)
		{
			return Query(
				::GetCurrentProcess(),
				lpBuffer
			);
		}
		// @throw AL::Exception
		static Information Query(::HANDLE hProcess, Void* lpBuffer)
		{
			::MEMORY_BASIC_INFORMATION memoryInformation;

			if (::VirtualQueryEx(hProcess, lpBuffer, &memoryInformation, sizeof(::MEMORY_BASIC_INFORMATION)) == 0)
			{

				throw SystemException(
					"VirtualQueryEx"
				);
			}

			Information information =
			{
				.Base       = memoryInformation.BaseAddress,
				.Size       = memoryInformation.RegionSize,
				.PageTypes  = static_cast<PageTypes>(memoryInformation.Type),
				.PageStates = static_cast<PageStates>(memoryInformation.State),
				.Protection = static_cast<Protections>(memoryInformation.Protect)
			};

			return information;
		}

		// @throw AL::Exception
		static Void EnumeratePages(const EnumPagesCallback& callback)
		{
			return EnumeratePages(
				::GetCurrentProcess(),
				callback
			);
		}
		// @throw AL::Exception
		static Void EnumeratePages(::HANDLE hProcess, const EnumPagesCallback& callback)
		{
			::SYSTEM_INFO systemInfo;
			::GetSystemInfo(&systemInfo);

			for (Void* lpAddress = systemInfo.lpMinimumApplicationAddress; lpAddress < systemInfo.lpMaximumApplicationAddress; )
			{
				auto info = Query(
					hProcess,
					lpAddress
				);

				if (info.PageStates.IsSet(PageStates::Commit) && !callback(info))
				{

					break;
				}

				lpAddress = reinterpret_cast<Void*>(
					reinterpret_cast<size_t>(lpAddress) + info.Size
				);
			}
		}
	};
}
