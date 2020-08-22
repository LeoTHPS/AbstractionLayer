#pragma once
#include "AL/Common.hpp"

#include <CorError.h>

namespace AL::Exceptions
{
	class DotNETException
		: public Exception
	{
	public:
		explicit DotNETException(HRESULT hResult)
			: Exception(
				GetErrorString(hResult)
			)
		{
		}

		template<size_t S>
		explicit DotNETException(const char(&function)[S])
			: Exception(
				"Error calling '%s'",
				&function[0]
			)
		{
		}

		template<size_t S>
		DotNETException(const char(&function)[S], HRESULT errorCode)
			: Exception(
				"Error calling '%s': %s",
				&function[0],
				GetErrorString(errorCode).GetCString()
			)
		{
		}

	private:
		static String GetErrorString(HRESULT hResult)
		{
			switch (hResult)
			{
				case S_FALSE:
					// undefined message
					break;

				case E_FAIL:
					return "An unknown catastrophic failure occurred.";

				case E_POINTER:
					return "ppInterface is null.";

				case E_OUTOFMEMORY:
					return "Not enough memory is available to handle the request.";

				case HOST_E_TIMEOUT:
					return "The call timed out.";

				case HOST_E_ABANDONED:
					return "An event was canceled while a blocked thread or fiber was waiting on it.";

				case HOST_E_NOT_OWNER:
					return "The caller does not own the lock.";

				case HOST_E_CLRNOTAVAILABLE:
					return "The common language runtime (CLR) has not been loaded into a process, or the CLR is in a state in which it cannot run managed code or process the call successfully.";

				case CLR_E_SHIM_LEGACYRUNTIMEALREADYBOUND:
					return "A different runtime was already bound to the legacy CLR version 2 activation policy.";
			}

			return "Undefined error";
		}
	};
}
