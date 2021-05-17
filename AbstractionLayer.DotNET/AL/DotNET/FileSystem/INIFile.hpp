#pragma once
#include "AL/DotNET/Common.hpp"

#include <AL/FileSystem/INIFile.hpp>

namespace AL::DotNET::FileSystem
{
	[::System::FlagsAttribute]
	public enum class INIFileFieldAttributes
		: typename Get_Enum_Base<AL::FileSystem::INIFileFieldAttributes>::Type
	{
		Struct   = static_cast<typename Get_Enum_Base<AL::FileSystem::INIFileFieldAttributes>::Type>(AL::FileSystem::INIFileFieldAttributes::Struct),
		String   = static_cast<typename Get_Enum_Base<AL::FileSystem::INIFileFieldAttributes>::Type>(AL::FileSystem::INIFileFieldAttributes::String),
		Integer  = static_cast<typename Get_Enum_Base<AL::FileSystem::INIFileFieldAttributes>::Type>(AL::FileSystem::INIFileFieldAttributes::Integer),
		Decimal  = static_cast<typename Get_Enum_Base<AL::FileSystem::INIFileFieldAttributes>::Type>(AL::FileSystem::INIFileFieldAttributes::Decimal),
		Unsigned = static_cast<typename Get_Enum_Base<AL::FileSystem::INIFileFieldAttributes>::Type>(AL::FileSystem::INIFileFieldAttributes::Unsigned)
	};

	public ref class INIFileField
	{
		bool isFieldAllocated = false;

		AL::FileSystem::INIFileField* const lpField;

	public:
		explicit INIFileField(AL::FileSystem::INIFileField& field)
			: isFieldAllocated(
				true
			),
			lpField(
				&field
			)
		{
		}

		INIFileField(::System::String^ name, ::System::String^ value)
			: lpField(
				new AL::FileSystem::INIFileField(
					Marshal::ToNativeString(
						name
					),
					Marshal::ToNativeString(
						value
					)
				)
			)
		{
		}

		virtual ~INIFileField()
		{
			if (isFieldAllocated)
			{
				delete lpField;
			}
		}

		bool IsStruct()
		{
			return lpField->IsStruct();
		}

		bool IsString()
		{
			return lpField->IsString();
		}

		bool IsInteger()
		{
			return lpField->IsInteger();
		}

		bool IsDecimal()
		{
			return lpField->IsDecimal();
		}

		bool IsUnsigned()
		{
			return lpField->IsUnsigned();
		}

		auto GetName()
		{
			return Marshal::ToString(
				lpField->GetName()
			);
		}

		auto GetValue()
		{
			return Marshal::ToString(
				lpField->GetValue()
			);
		}

		auto GetAttributes()
		{
			return static_cast<INIFileFieldAttributes>(
				lpField->GetAttributes().Value
			);
		}

		/// <exception cref="AL::DotNET::Exceptions::Exception" />
		generic<typename T>
		auto GetStruct()
		{
			if (!IsStruct())
			{

				throw gcnew ::System::InvalidOperationException(
					"INIFileField type is not struct"
				);
			}

			auto nativeBuffer = lpField->GetStructAsBuffer();
			
			auto buffer = gcnew array<::System::Byte>(
				static_cast<::System::UInt32>(
					nativeBuffer.GetSize()
				)
			);

			Marshal::Copy(
				&nativeBuffer[0],
				buffer,
				0,
				buffer->Length
			);

			return Marshal::FromArray<T>(
				buffer,
				0,
				buffer->Length
			);
		}

		/// <exception cref="AL::DotNET::Exceptions::Exception" />
		auto GetStructAsBuffer()
		{
			if (!IsStruct())
			{

				throw gcnew ::System::InvalidOperationException(
					"INIFileField type is not struct"
				);
			}

			auto nativeBuffer = lpField->GetStructAsBuffer();
			
			auto buffer = gcnew array<::System::Byte>(
				static_cast<::System::UInt32>(
					nativeBuffer.GetSize()
				)
			);

			Marshal::Copy(
				&nativeBuffer[0],
				buffer,
				0,
				buffer->Length
			);

			return buffer;
		}

		/// <exception cref="AL::DotNET::Exceptions::Exception" />
		auto GetString()
		{
			if (!IsString())
			{

				throw gcnew ::System::InvalidOperationException(
					"INIFileField type is not string"
				);
			}

			auto nativeString = lpField->GetString();

			return Marshal::ToString(
				nativeString
			);
		}

		/// <exception cref="AL::DotNET::Exceptions::Exception" />
		generic<typename T>
		T GetInteger()
		{
			if (!IsInteger())
			{

				throw gcnew ::System::InvalidOperationException(
					"INIFileField type is not integer"
				);
			}

			auto type = T::typeid;

			if (type->UnderlyingSystemType == uint8::typeid->UnderlyingSystemType)
			{
				return T(lpField->GetInteger<uint8>());
			}
			else if (type->UnderlyingSystemType == uint16::typeid->UnderlyingSystemType)
			{
				return T(lpField->GetInteger<uint16>());
			}
			else if (type->UnderlyingSystemType == uint32::typeid->UnderlyingSystemType)
			{
				return T(lpField->GetInteger<uint32>());
			}
			else if (type->UnderlyingSystemType == uint64::typeid->UnderlyingSystemType)
			{
				return T(lpField->GetInteger<uint64>());
			}
			else if (type->UnderlyingSystemType == int8::typeid->UnderlyingSystemType)
			{
				return T(lpField->GetInteger<int8>());
			}
			else if (type->UnderlyingSystemType == int16::typeid->UnderlyingSystemType)
			{
				return T(lpField->GetInteger<int16>());
			}
			else if (type->UnderlyingSystemType == int32::typeid->UnderlyingSystemType)
			{
				return T(lpField->GetInteger<int32>());
			}
			else if (type->UnderlyingSystemType == int64::typeid->UnderlyingSystemType)
			{
				return T(lpField->GetInteger<int64>());
			}

			throw gcnew ::System::ArgumentException(
				"T is an unsupported type"
			);
		}

		/// <exception cref="AL::DotNET::Exceptions::Exception" />
		generic<typename T>
		auto GetDecimal()
		{
			if (!IsDecimal())
			{

				throw gcnew ::System::InvalidOperationException(
					"INIFileField type is not decimal"
				);
			}

			auto type = T::typeid;

			if (type->UnderlyingSystemType == float::typeid->UnderlyingSystemType)
			{
				return T(lpField->GetDecimal<float>());
			}
			else if (type->UnderlyingSystemType == double::typeid->UnderlyingSystemType)
			{
				return T(lpField->GetDecimal<double>());
			}

			throw gcnew ::System::ArgumentException(
				"T is an unsupported type"
			);
		}

		generic<typename T>
		void SetStruct(T value)
		{
			auto buffer = Marshal::ToArray(
				value
			);

			SetStructAsBuffer(
				buffer,
				0,
				buffer->Length
			);
		}

		void SetStructAsBuffer(array<::System::Byte>^ buffer, ::System::UInt32 offset, ::System::UInt32 count)
		{
			AL::Collections::Array<uint8> nativeBuffer(
				count
			);

			Marshal::Copy(
				buffer,
				&nativeBuffer[0],
				offset,
				count
			);

			lpField->SetStructAsBuffer(
				&nativeBuffer[0],
				nativeBuffer.GetSize()
			);
		}

		void SetString(::System::String^ value)
		{
			auto nativeString = Marshal::ToNativeString(
				value
			);

			lpField->SetString(
				nativeString
			);
		}

		void SetInteger(::System::Byte value)
		{
			lpField->SetInteger<uint8>(
				value
			);
		}
		void SetInteger(::System::UInt16 value)
		{
			lpField->SetInteger<uint16>(
				value
			);
		}
		void SetInteger(::System::UInt32 value)
		{
			lpField->SetInteger<uint32>(
				value
			);
		}
		void SetInteger(::System::UInt64 value)
		{
			lpField->SetInteger<uint64>(
				value
			);
		}
		void SetInteger(::System::SByte value)
		{
			lpField->SetInteger<int8>(
				value
			);
		}
		void SetInteger(::System::Int16 value)
		{
			lpField->SetInteger<int16>(
				value
			);
		}
		void SetInteger(::System::Int32 value)
		{
			lpField->SetInteger<int32>(
				value
			);
		}
		void SetInteger(::System::Int64 value)
		{
			lpField->SetInteger<int64>(
				value
			);
		}

		void SetDecimal(::System::Single value)
		{
			lpField->SetDecimal<float>(
				value
			);
		}
		void SetDecimal(::System::Double value)
		{
			lpField->SetDecimal<double>(
				value
			);
		}

	internal:
		AL::FileSystem::INIFileField& GetNativeField()
		{
			return *lpField;
		}
	};

	public delegate void INIFileFieldEnumCallback(INIFileField^ field);

	public ref class INIFileSection
	{
		bool isSectionAllocated = false;

		AL::FileSystem::INIFileSection* const lpSection;

	public:
		explicit INIFileSection(::System::String^ name)
			: isSectionAllocated(
				true
			),
			lpSection(
				new AL::FileSystem::INIFileSection(
					Marshal::ToNativeString(
						name
					)
				)
			)
		{
		}

		explicit INIFileSection(AL::FileSystem::INIFileSection& section)
			: lpSection(
				&section
			)
		{
		}

		virtual ~INIFileSection()
		{
			if (isSectionAllocated)
			{
				delete lpSection;
			}
		}

		auto GetName()
		{
			return Marshal::ToString(
				lpSection->GetName()
			);
		}

		auto GetFieldCount()
		{
			return lpSection->GetFieldCount();
		}

		void Clear()
		{
			lpSection->Clear();
		}

		void EnumerateFields(INIFileFieldEnumCallback^ callback)
		{
			for (auto& section : *lpSection)
			{
				callback(
					gcnew INIFileField(section)
				);
			}
		}

		property INIFileField^ default[::System::String^]
		{
			INIFileField^ get(::System::String^ fieldName)
			{
				auto& field = lpSection->operator[](
					Marshal::ToNativeString(
						fieldName
					)
				);

				return gcnew INIFileField(
					field
				);
			}
			void set(::System::String^ fieldName, INIFileField^ field)
			{
				auto nativeFieldName = Marshal::ToNativeString(
					fieldName
				);

				lpSection->operator[](Move(nativeFieldName)) = Move(
					field->GetNativeField()
				);
			}
		}

	internal:
		AL::FileSystem::INIFileSection& GetNativeSection()
		{
			return *lpSection;
		}
	};

	public delegate void INIFileSectionEnumCallback(INIFileSection^ section);

	public ref class INIFile
	{
		::System::String^ const path;

		AL::FileSystem::INIFile* const lpINIFile;

	public:
		explicit INIFile(::System::String^ path)
			: path(
				path
			),
			lpINIFile(
				new AL::FileSystem::INIFile(
					Marshal::ToNativeString(
						path->ToString()
					)
				)
			)
		{
		}

		virtual ~INIFile()
		{
			delete lpINIFile;
		}

		auto GetPath()
		{
			return path;
		}

		auto GetSectionCount()
		{
			return lpINIFile->GetSectionCount();
		}

		/// <exception cref="AL::DotNET::Exceptions::Exception" />
		bool Exists()
		{
			try
			{
				return lpINIFile->Exists();
			}
			catch (const AL::Exceptions::Exception& exception)
			{

				throw gcnew Exceptions::Exception(
					exception
				);
			}
		}

		/// <summary>
		/// Returns false if not found
		/// </summary>
		/// <exception cref="AL::DotNET::Exceptions::Exception" />
		bool Read()
		{
			try
			{
				return lpINIFile->Read();
			}
			catch (const AL::Exceptions::Exception& exception)
			{

				throw gcnew Exceptions::Exception(
					exception
				);
			}
		}

		/// <exception cref="AL::DotNET::Exceptions::Exception" />
		void Write()
		{
			try
			{
				lpINIFile->Write();
			}
			catch (const AL::Exceptions::Exception& exception)
			{

				throw gcnew Exceptions::Exception(
					exception
				);
			}
		}

		void Clear()
		{
			lpINIFile->Clear();
		}

		/// <exception cref="AL::DotNET::Exceptions::Exception" />
		void Delete()
		{
			try
			{
				lpINIFile->Delete();
			}
			catch (const AL::Exceptions::Exception& exception)
			{

				throw gcnew Exceptions::Exception(
					exception
				);
			}
		}

		void EnumerateSections(INIFileSectionEnumCallback^ callback)
		{
			for (auto& section : *lpINIFile)
			{
				callback(
					gcnew INIFileSection(section)
				);
			}
		}

		property INIFileSection^ default[::System::String^]
		{
			INIFileSection^ get(::System::String^ sectionName)
			{
				auto& section = lpINIFile->operator[](
					Marshal::ToNativeString(
						sectionName
					)
				);

				return gcnew INIFileSection(
					section
				);
			}
			void set(::System::String^ sectionName, INIFileSection^ section)
			{
				auto sectionNameNative = Marshal::ToNativeString(
					sectionName
				);

				lpINIFile->operator[](Move(sectionNameNative)) = Move(
					section->GetNativeSection()
				);
			}
		}
	};
}
