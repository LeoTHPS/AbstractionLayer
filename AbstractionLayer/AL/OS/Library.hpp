#pragma once
#include "AL/Common.hpp"

#include "AL/FileSystem/Path.hpp"

#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
	#include <TlHelp32.h>
#endif

namespace AL::OS
{
	typedef Function<void(const String& name, const FileSystem::Path& path)> LibraryEnumLibrariesCallback;

	class Library
	{
		bool isOpen = false;

#if defined(AL_PLATFORM_LINUX)
		static constexpr int HANDLE_NULL = 0;

#elif defined(AL_PLATFORM_WINDOWS)
		static constexpr HMODULE HANDLE_NULL = NULL;

		HMODULE hModule = HANDLE_NULL;
#endif

		Library(const Library&) = delete;

	public:
#if defined(AL_PLATFORM_LINUX)
		typedef int Handle;
#elif defined(AL_PLATFORM_WINDOWS)
		typedef decltype(hModule) Handle;
#endif

		// @throw AL::Exceptions::Exception
		// @return false if not loaded
		static bool GetLibraryByName(Library& library, const String& name)
		{
			library.Close();

#if defined(AL_PLATFORM_LINUX)
			throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			if ((library.hModule = GetModuleHandleA(name.GetCString())) == NULL)
			{
				auto errorCode = OS::GetLastError();

				// TODO: verify this is the correct error code
				if (errorCode == ERROR_MOD_NOT_FOUND)
				{

					return false;
				}
				
				throw Exceptions::SystemException(
					"GetModuleHandleA",
					errorCode
				);
			}
#endif

			library.isOpen = true;

			return true;
		}

		// @throw AL::Exceptions::Exception
		static void LoadLibrary(Library& library, const FileSystem::Path& path)
		{
			library.Close();

#if defined(AL_PLATFORM_LINUX)
			throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			if ((library.hModule = LoadLibraryA(path.GetString().GetCString())) == NULL)
			{

				throw Exceptions::SystemException(
					"LoadLibraryA"
				);
			}
#endif

			library.isOpen = true;
		}
		// @throw AL::Exceptions::Exception
		static void LoadLibrary(Library& library, const void* lpBuffer, size_t size)
		{

			throw Exceptions::NotImplementedException();
		}

		// @throw AL::Exceptions::Exception
		static void EnumerateLibraries(const LibraryEnumLibrariesCallback& callback)
		{
#if defined(AL_PLATFORM_LINUX)
			throw Exceptions::NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			HANDLE hSnapshot;

			if ((hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId())) == INVALID_HANDLE_VALUE)
			{

				throw Exceptions::SystemException(
					"CreateToolhelp32Snapshot"
				);
			}

			MODULEENTRY32 module;
			ZeroMemory(&module, sizeof(MODULEENTRY32));
			
			module.dwSize = sizeof(MODULEENTRY32);

			if (Module32First(hSnapshot, &module))
			{
				do
				{
					callback(
						String(module.szModule),
						FileSystem::Path(module.szExePath)
					);
				} while (Module32Next(hSnapshot, &module));
			}
			else if (GetLastError() != ERROR_NO_MORE_FILES)
			{
				CloseHandle(hSnapshot);

				throw Exceptions::SystemException(
					"Module32First"
				);
			}

			CloseHandle(hSnapshot);
#endif
		}

		Library()
		{
		}

		Library(Handle handle)
			: isOpen(
				handle != HANDLE_NULL
			)
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
			,
			hModule(
				handle
			)
#endif
		{
		}

		Library(Library&& library)
			: isOpen(
				library.isOpen
			)
#if defined(AL_PLATFORM_LINUX)
			
#elif defined(AL_PLATFORM_WINDOWS)
			,
			hModule(
				library.hModule
			)
#endif
		{
			if (IsOpen())
			{
				library.isOpen = false;

#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
				library.hModule = NULL;
#endif
			}
		}

		virtual ~Library()
		{
			Close();
		}

		bool IsOpen() const
		{
			return isOpen;
		}

		auto GetHandle() const
		{
#if defined(AL_PLATFORM_LINUX)
			
#elif defined(AL_PLATFORM_WINDOWS)
			return hModule;
#endif
		}

		auto GetBaseAddress() const
		{
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
			return reinterpret_cast<void*>(
				hModule
			);
#endif
		}

		// @throw AL::Exceptions::Exception
		template<typename T>
		auto Import(uint16 ordinal) const
		{
			AL_ASSERT(IsOpen(), "Library not open");

#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
			FARPROC lpExport;

			if ((lpExport = GetProcAddress(hModule, Integer<uint16>::ToString(ordinal).GetCString())) == NULL)
			{

				throw Exceptions::SystemException(
					"GetProcAddress"
				);
			}

			return reinterpret_cast<T*>(
				lpExport
			);
#endif
		}
		// @throw AL::Exceptions::Exception
		template<typename T>
		auto Import(const String& name) const
		{
			AL_ASSERT(IsOpen(), "Library not open");

#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
			FARPROC lpExport;

			if ((lpExport = GetProcAddress(hModule, name.GetCString())) == NULL)
			{

				throw Exceptions::SystemException(
					"GetProcAddress"
				);
			}

			return reinterpret_cast<T*>(
				lpExport
			);
#endif
		}

		void Close()
		{
			if (IsOpen())
			{
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
				hModule = NULL;
#endif

				isOpen = false;
			}
		}

		// @throw AL::Exceptions::Exception
		void UnloadAndClose()
		{
			if (IsOpen())
			{
#if defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
				FreeLibrary(hModule);
#endif

				Close();
			}
		}
	};
}
