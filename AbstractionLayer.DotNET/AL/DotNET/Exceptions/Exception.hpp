#pragma once
#include "AL/DotNET/Common.hpp"

namespace AL::DotNET::Exceptions
{
	public ref class Exception
		: public System::Exception
	{
	public:
		explicit Exception(System::String^ message)
			: System::Exception(
				message
			)
		{
		}

		Exception(const AL::Exceptions::Exception& exception)
			: Exception(
				Marshal::ToString(
					exception.GetMessage()
				),
				GetInnerException(
					exception
				)
			)
		{
		}

		Exception(Exception^ exception, Exception^ innerException)
			: System::Exception(
				exception->Message,
				innerException
			)
		{
		}

		Exception(System::String^ message, Exception^ innerException)
			: System::Exception(
				message,
				static_cast<System::Exception^>(innerException)
			)
		{
		}
		
	private:
		static Exception^ GetInnerException(const AL::Exceptions::Exception& exception)
		{
			AL::Exceptions::Exception* lpInnerException;

			if ((lpInnerException = exception.GetInnerException()) != nullptr)
			{
				auto innerException = gcnew Exception(
					*lpInnerException
				);

				while ((lpInnerException = lpInnerException->GetInnerException()) != nullptr)
				{
					innerException = gcnew Exception(
						innerException,
						gcnew Exception(
							*lpInnerException
						)
					);
				}

				return innerException;
			}

			return nullptr;
		}
	};
}
