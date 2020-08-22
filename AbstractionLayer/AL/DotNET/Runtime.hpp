#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_WINDOWS)
	#error Platform not supported
#endif

#include "AL/FileSystem/File.hpp"

#include "AL/Exceptions/DotNETException.hpp"

#include <CorError.h>
#include <metahost.h>

#pragma comment(lib, "MSCorEE.lib")

namespace AL::DotNET
{
	typedef Function<bool(const String& version)> RuntimeEnumCallback;

	class Runtime
	{
		String version;

		ICLRMetaHost* lpHost;
		ICLRRuntimeHost* lpRuntime;
		ICLRRuntimeInfo* lpRuntimeInfo;

		Runtime(Runtime&&) = delete;
		Runtime(const Runtime&) = delete;

	public:
		Runtime()
			: Runtime(
				""
			)
		{
		}

		explicit Runtime(const String& version)
			: version(
				version
			)
		{
		}

		bool IsRunning() const
		{
			return lpHost != nullptr;
		}

		auto& GetVersion() const
		{
			return version;
		}

		// @throw AL::Exception
		void Start()
		{
			AL_ASSERT(!IsRunning(), "Runtime already started");

			HRESULT hResult = S_OK;

			if (FAILED(hResult = CLRCreateInstance(CLSID_CLRMetaHost, IID_ICLRMetaHost, reinterpret_cast<LPVOID*>(&lpHost))))
			{

				throw Exceptions::DotNETException(
					"CLRCreateInstance",
					hResult
				);
			}

			auto wVersion = WString::Format(
				L"%S",
				GetVersion().GetCString()
			);

			if (!wVersion.GetLength())
			{
#pragma pack(push, 1)
				union Version
				{
					struct
					{
						uint16 Build;
						uint8 Minor;
						uint8 Major;
					};

					uint32 Value;
				};
#pragma pack(pop)

				Regex pattern(
					"v(\\d+).(\\d+).(\\d+)"
				);

				Version version;
				version.Value = 0;

				EnumerateInstalledVersions(
					[&pattern, &version](const String& _version)
					{
						Regex::MatchCollection matches;

						if (pattern.Match(matches, _version))
						{
							Version v;
							v.Major = Integer<uint8>::FromString(
								matches[1]
							);
							v.Minor = Integer<uint8>::FromString(
								matches[2]
							);
							v.Build = Integer<uint16>::FromString(
								matches[3]
							);

							if (v.Value > version.Value)
							{

								version = v;
							}
						}

						return true;
					}
				);

				if (!version.Value)
				{
					lpHost->Release();
					lpHost = nullptr;

					throw Exceptions::Exception(
						"No CLR runtimes installed"
					);
				}

				wVersion = WString::Format(
					L"v%u.%u.%u",
					version.Major,
					version.Minor,
					version.Build
				);
			}

			if (FAILED(hResult = lpHost->GetRuntime(wVersion.GetCString(), IID_PPV_ARGS(&lpRuntimeInfo))))
			{
				lpHost->Release();
				lpHost = nullptr;

				throw Exceptions::DotNETException(
					"ICLRMetaHost::GetRuntime",
					hResult
				);
			}

			if (FAILED(hResult = lpRuntimeInfo->GetInterface(CLSID_CLRRuntimeHost, IID_PPV_ARGS(&lpRuntime))))
			{
				lpRuntimeInfo->Release();
				lpRuntimeInfo = nullptr;

				lpHost->Release();
				lpHost = nullptr;

				throw Exceptions::DotNETException(
					"ICLRRuntimeInfo::GetInterface",
					hResult
				);
			}

			if (FAILED(hResult = lpRuntime->Start()))
			{
				lpRuntime->Release();
				lpRuntime = nullptr;

				lpRuntimeInfo->Release();
				lpRuntimeInfo = nullptr;

				lpHost->Release();
				lpHost = nullptr;

				throw Exceptions::DotNETException(
					"ICLRRuntimeHost::Start",
					hResult
				);
			}
		}

		void Stop()
		{
			if (IsRunning())
			{
				lpRuntime->Stop();

				lpRuntime->Release();
				lpRuntime = nullptr;

				lpRuntimeInfo->Release();
				lpRuntimeInfo = nullptr;

				lpHost->Release();
				lpHost = nullptr;
			}
		}

		// @throw AL::Exception
		int32 Execute(const FileSystem::File& file, const String& type, const String& method)
		{
			return Execute(
				file,
				type,
				method,
				""
			);
		}
		// @throw AL::Exception
		int32 Execute(const FileSystem::File& file, const String& type, const String& method, const String& arg)
		{
			AL_ASSERT(IsRunning(), "Runtime not started");

			auto wFile = WString::Format(
				L"%S",
				file.GetPath().GetString().GetCString()
			);
			auto wType = WString::Format(
				L"%S",
				type.GetCString()
			);
			auto wMethod = WString::Format(
				L"%S",
				method.GetCString()
			);
			auto wArg = WString::Format(
				L"%S",
				arg.GetCString()
			);

			HRESULT hResult;
			DWORD returnValue = 0;

			if (FAILED(hResult = lpRuntime->ExecuteInDefaultAppDomain(wFile.GetCString(), wType.GetCString(), wMethod.GetCString(), wArg.GetCString(), &returnValue)))
			{

				throw Exceptions::DotNETException(
					"ICLRRuntimeHost::ExecuteInDefaultAppDomain",
					hResult
				);
			}

			return static_cast<int32>(
				returnValue
			);
		}

		// @throw AL::Exception
		static void EnumerateInstalledVersions(const RuntimeEnumCallback& callback)
		{
			ICLRMetaHost* lpHost;
			HRESULT hResult = S_OK;

			if (FAILED(hResult = CLRCreateInstance(CLSID_CLRMetaHost, IID_ICLRMetaHost, reinterpret_cast<LPVOID*>(&lpHost))))
			{

				throw Exceptions::DotNETException(
					"CLRCreateInstance",
					hResult
				);
			}

			IEnumUnknown* lpEnumerator;

			if (FAILED(hResult = lpHost->EnumerateInstalledRuntimes(&lpEnumerator)))
			{
				lpHost->Release();

				throw Exceptions::DotNETException(
					"ICLRMetaHost::EnumerateInstalledRuntimes",
					hResult
				);
			}

			ULONG count;
			IUnknown* lpInterface;
			ICLRRuntimeInfo* lpRuntimeInfo;

			while (SUCCEEDED(hResult = lpEnumerator->Next(1, &lpInterface, &count)) && lpInterface) // TOOD: msdn says nothing about lpInterface being null, why?
			{
				if (FAILED(hResult = lpInterface->QueryInterface(IID_PPV_ARGS(&lpRuntimeInfo))))
				{
					lpInterface->Release();

					throw Exceptions::Exception(
						"Error calling IUnknown::QueryInterface"
					);
				}

				DWORD versionLength = 0;

				if (FAILED(hResult = lpRuntimeInfo->GetVersionString(nullptr, &versionLength)) &&
					((hResult != HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)) ||
					(versionLength == 0)))
				{
					lpRuntimeInfo->Release();
					lpInterface->Release();

					throw Exceptions::DotNETException(
						"ICLRRuntimeInfo::GetVersionString",
						hResult
					);
				}

				WString version(
					L'\0',
					versionLength
				);

				if (FAILED(hResult = lpRuntimeInfo->GetVersionString(&version[0], &versionLength)))
				{
					lpRuntimeInfo->Release();
					lpInterface->Release();

					throw Exceptions::DotNETException(
						"ICLRRuntimeInfo::GetVersionString",
						hResult
					);
				}

				try
				{
					if (!callback(String::Format("%S", version.GetCString())))
					{
						lpRuntimeInfo->Release();
						lpInterface->Release();

						break;
					}
				}
				catch (Exceptions::Exception&)
				{
					lpRuntimeInfo->Release();
					lpInterface->Release();

					throw;
				}

				lpRuntimeInfo->Release();
				lpInterface->Release();
			}

			if (FAILED(hResult))
			{
				lpEnumerator->Release();
				lpHost->Release();

				throw Exceptions::DotNETException(
					"IEnumUnknown::Next",
					hResult
				);
			}

			lpEnumerator->Release();
			lpHost->Release();
		}
	};
}
