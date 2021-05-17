#pragma once
#include "AL/DotNET/Common.hpp"

namespace AL::DotNET::OS
{
	/// <exception cref="AL::DotNET::Exceptions::Exception" />
	public delegate void LibraryEnumLibrariesCallback(::System::String^ name, ::System::String^ path);

	class LibraryEnumLibrariesNativeCallback
	{
		typedef void(NativeCallback)(const char*, const char*);

		NativeCallback* const lpCallback;

	public:
		explicit LibraryEnumLibrariesNativeCallback(LibraryEnumLibrariesCallback^ callback)
			: lpCallback(
				reinterpret_cast<NativeCallback*>(
					::System::Runtime::InteropServices::Marshal::GetFunctionPointerForDelegate(callback).ToPointer()
				)
			)
		{
		}

		virtual ~LibraryEnumLibrariesNativeCallback()
		{
		}

		// @throw AL::Exceptions::Exception
		void Execute(const String& name, const AL::FileSystem::Path& path) const
		{
			try
			{
				lpCallback(
					name.GetCString(),
					path.GetString().GetCString()
				);
			}
			catch (::System::Exception^ exception)
			{

				throw AL::Exceptions::Exception(
					Marshal::ToNativeString(exception->Message)
				);
			}
		}
	};

	public ref class Library
	{
		AL::OS::Library* const lpLibrary;

		explicit Library(AL::OS::Library&& library)
			: lpLibrary(
				new AL::OS::Library(
					Move(library)
				)
			)
		{
		}

	public:
		/// <summary>
		/// Returns false if not loaded
		/// </summary>
		/// <exception cref="AL::DotNET::Exceptions::Exception" />
		static bool GetLibraryByName([::System::Runtime::InteropServices::OutAttribute] Library^% library, ::System::String^ name)
		{
			AL::OS::Library _library;

			try
			{
				if (!AL::OS::Library::GetLibraryByName(_library, Marshal::ToNativeString(name)))
				{

					return false;
				}
			}
			catch (const AL::Exceptions::Exception& exception)
			{

				throw gcnew Exceptions::Exception(
					exception
				);
			}

			library = gcnew Library(
				Move(_library)
			);

			return true;
		}

		/// <exception cref="AL::DotNET::Exceptions::Exception" />
		static void LoadLibrary([::System::Runtime::InteropServices::OutAttribute] Library^% library, ::System::String^ path)
		{
			AL::OS::Library _library;

			try
			{
				AL::OS::Library::LoadLibrary(
					_library,
					Marshal::ToNativeString(path)
				);
			}
			catch (const AL::Exceptions::Exception& exception)
			{

				throw gcnew Exceptions::Exception(
					exception
				);
			}

			library = gcnew Library(
				Move(_library)
			);
		}
		/// <exception cref="AL::DotNET::Exceptions::Exception" />
		static void LoadLibrary([::System::Runtime::InteropServices::OutAttribute] Library^% library, array<::System::Byte>^ buffer)
		{
			auto lpBuffer = new uint8[
				static_cast<size_t>(buffer->Length)
			];

			Marshal::Copy(
				buffer,
				lpBuffer,
				0,
				static_cast<size_t>(buffer->Length)
			);

			AL::OS::Library _library;

			try
			{
				AL::OS::Library::LoadLibrary(
					_library,
					lpBuffer,
					static_cast<size_t>(buffer->Length)
				);
			}
			catch (const AL::Exceptions::Exception& exception)
			{
				delete[] lpBuffer;

				throw gcnew Exceptions::Exception(
					exception
				);
			}

			delete[] lpBuffer;

			library = gcnew Library(
				Move(_library)
			);
		}

		/// <exception cref="AL::DotNET::Exceptions::Exception" />
		static void EnumerateLibraries(LibraryEnumLibrariesCallback^ callback)
		{
			LibraryEnumLibrariesNativeCallback nativeCallback(
				callback
			);

			try
			{
				AL::OS::Library::EnumerateLibraries(
					AL::OS::LibraryEnumLibrariesCallback(
						&LibraryEnumLibrariesNativeCallback::Execute,
						nativeCallback
					)
				);
			}
			catch (const AL::Exceptions::Exception& exception)
			{

				throw gcnew Exceptions::Exception(
					exception
				);
			}
		}

		Library()
			: lpLibrary(
				new AL::OS::Library()
			)
		{
		}

		virtual ~Library()
		{
			delete lpLibrary;
		}

		bool IsOpen()
		{
			return lpLibrary->IsOpen();
		}

		auto GetHandle()
		{
			return ::System::IntPtr(
				lpLibrary->GetHandle()
			);
		}

		auto GetBaseAddress()
		{
			return ::System::IntPtr(
				lpLibrary->GetBaseAddress()
			);
		}

		/// <exception cref="AL::DotNET::Exceptions::Exception" />
		auto Import(::System::String^ name)
		{
			void* address;

			try
			{
				address = lpLibrary->Import<void>(
					Marshal::ToNativeString(name)
				);
			}
			catch (const AL::Exceptions::Exception& exception)
			{

				throw gcnew Exceptions::Exception(
					exception
				);
			}

			return ::System::IntPtr(
				address
			);
		}
		/// <exception cref="AL::DotNET::Exceptions::Exception" />
		auto Import(::System::UInt16 ordinal)
		{
			void* address;

			try
			{
				address = lpLibrary->Import<void>(
					ordinal
				);
			}
			catch (const AL::Exceptions::Exception& exception)
			{

				throw gcnew Exceptions::Exception(
					exception
				);
			}

			return ::System::IntPtr(
				address
			);
		}
		
		void Close()
		{
			lpLibrary->Close();
		}

		void UnloadAndClose()
		{
			lpLibrary->UnloadAndClose();
		}
	};
}
