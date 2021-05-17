#pragma once
#include "AL/DotNET/Common.hpp"

#include <AL/FileSystem/LogFile.hpp>

namespace AL::DotNET::FileSystem
{
	public ref class LogFile
	{
		System::String^ const path;

		AL::FileSystem::LogFile* const lpLogFile;

	public:
		explicit LogFile(System::String^ path)
			: path(
				path
			),
			lpLogFile(
				new AL::FileSystem::LogFile(
					Marshal::ToNativeString(path->ToString())
				)
			)
		{
		}

		virtual ~LogFile()
		{
			delete lpLogFile;
		}

		bool IsOpen()
		{
			return lpLogFile->IsOpen();
		}

		auto GetPath()
		{
			return path;
		}

		void Open()
		{
			try
			{
				lpLogFile->Open();
			}
			catch (const AL::Exceptions::Exception& exception)
			{

				throw gcnew Exceptions::Exception(
					exception
				);
			}
		}

		void Close()
		{
			lpLogFile->Close();
		}

		/// <exception cref="AL::DotNET::Exceptions::Exception" />
		void WriteLine(System::String^ format, ... array<System::Object^>^ args)
		{
			auto line = System::String::Format(
				format,
				args
			);

			auto nativeLine = Marshal::ToNativeString(
				line
			);

			try
			{
				lpLogFile->WriteLine(
					nativeLine
				);
			}
			catch (const AL::Exceptions::Exception& exception)
			{
				
				throw gcnew Exceptions::Exception(
					exception
				);
			}
		}
	};
}
