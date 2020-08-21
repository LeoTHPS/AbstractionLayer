#pragma once
#include "AL/Common.hpp"

namespace AL::Exceptions
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

		template<size_t S, typename ... TArgs>
		explicit Exception(const char(&message)[S], TArgs ... args)
			: Exception(
				String::Format(
					message,
					Forward<TArgs>(args) ...
				)
			)
		{
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

		template<size_t S, typename ... TArgs>
		Exception(Exception&& innerException, const char(&message)[S], TArgs ... args)
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

		virtual ~Exception()
		{
			if (lpInnerException)
			{
				delete lpInnerException;
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

		auto& operator = (Exception&& exception)
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
