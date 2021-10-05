#pragma once
#include "AL/Common.hpp"

namespace AL
{
	class Exception
	{
		String message;

		Exception* lpInnerException = nullptr;

		Exception(const Exception&) = delete;

	public:
		Exception(Exception&& exception)
			: message(
				Move(exception.message)
			),
			lpInnerException(
				exception.lpInnerException
			)
		{
			exception.lpInnerException = nullptr;
		}

		explicit Exception(String&& message)
			: message(
				Move(message)
			)
		{
		}

		template<typename ... TArgs>
		Exception(const char* message, TArgs ... args)
			: Exception(
				String::Format(
					message,
					Forward<TArgs>(args) ...
				)
			)
		{
		}
		template<typename ... TArgs>
		Exception(const String& message, TArgs ... args)
			: Exception(
				message.GetCString(),
				Forward<TArgs>(args) ...
			)
		{
		}

		Exception(Exception&& exception, Exception&& innerException)
			: message(
				Move(exception.message)
			)
		{
			if (auto lpInnerMostException = (lpInnerException = exception.lpInnerException))
			{
				while (auto lpInnerException = lpInnerMostException->lpInnerException)
				{
					lpInnerMostException = lpInnerException;
				}

				lpInnerMostException->lpInnerException = new Exception(
					Move(innerException)
				);

				exception.lpInnerException = nullptr;
			}
			else
			{
				lpInnerException = new Exception(
					Move(innerException)
				);
			}
		}

		Exception(Exception&& innerException, String&& message)
			: message(
				Move(message)
			),
			lpInnerException(
				new Exception(
					Move(innerException)
				)
			)
		{
		}

		template<typename ... TArgs>
		Exception(Exception&& innerException, const char* message, TArgs ... args)
			: message(
				String::Format(
					message,
					Forward<TArgs>(args) ...
				)
			),
			lpInnerException(
				new Exception(
					Move(innerException)
				)
			)
		{
		}
		template<typename ... TArgs>
		Exception(Exception&& innerException, const String& message, TArgs ... args)
			: Exception(
				Move(innerException),
				message.GetCString(),
				Forward<TArgs>(args) ...
			)
		{
		}

		virtual ~Exception()
		{
			if (lpInnerException != nullptr)
			{
				delete lpInnerException;
				lpInnerException = nullptr;
			}
		}

		auto& GetMessage() const
		{
			return message;
		}

		auto GetInnerException() const
		{
			return lpInnerException;
		}

		auto GetInnerExceptionCount() const
		{
			size_t count = 0;

			if (auto lpInnerException = GetInnerException())
			{
				do
				{
					++count;
				} while ((lpInnerException = lpInnerException->GetInnerException()));
			}

			return count;
		}

		Exception& operator = (Exception&& exception)
		{
			message = Move(
				exception.message
			);

			lpInnerException = exception.lpInnerException;
			exception.lpInnerException = nullptr;

			return *this;
		}
	};
}
