#pragma once
#include "AL/Common.hpp"

#include "SSLException.hpp"

namespace AL::OpenSSL
{
	enum class Modes : uint8
	{
		Client,
		Server
	};

	enum class Protocols : uint8
	{
		TLS,
		DTLS
	};

	class SSL
	{
		typedef Get_Function_Arg_Type<decltype(::SSL_set_fd), 1>::Type SSL_FILE_HANDLE;

		Bool            isCreated   = False;
		Bool            isConnected = False;

		SSL_FILE_HANDLE fd;
		::SSL*          ssl;
		Modes           mode;
		::SSL_CTX*      context;
		Protocols       protocol;

		SSL(const SSL&) = delete;

	public:
		SSL(SSL&& ssl)
			: isCreated(
				ssl.isCreated
			),
			isConnected(
				ssl.isConnected
			),
			fd(
				ssl.fd
			),
			ssl(
				ssl.ssl
			),
			mode(
				ssl.mode
			),
			context(
				ssl.context
			),
			protocol(
				ssl.protocol
			)
		{
			ssl.isCreated = False;
			ssl.isConnected = False;
		}

		SSL(Modes mode, Protocols protocol)
			: mode(
				mode
			),
			protocol(
				protocol
			)
		{
		}

		virtual ~SSL()
		{
			if (IsCreated())
			{

				Destroy();
			}
		}

		Bool IsCreated() const
		{
			return isCreated;
		}

		Bool IsConnected() const
		{
			return isConnected;
		}

		auto GetFD() const
		{
			return fd;
		}

		auto GetMode() const
		{
			return mode;
		}

		auto GetHandle() const
		{
			return ssl;
		}

		auto GetContext() const
		{
			return context;
		}

		auto GetProtocol() const
		{
			return protocol;
		}

		// @throw AL::Exception
		Void SetFD(SSL_FILE_HANDLE value)
		{
			AL_ASSERT(
				IsCreated(),
				"SSL not created"
			);

			if (::SSL_set_fd(GetHandle(), value) == 0)
			{

				throw SSLException(
					"SSL_set_fd"
				);
			}

			fd = value;
		}

		// @throw AL::Exception
		Void Create()
		{
			AL_ASSERT(
				!IsCreated(),
				"SSL already created"
			);

			auto method = SSL_GetMethod(
				GetMode(),
				GetProtocol()
			);

			if ((context = ::SSL_CTX_new(method)) == nullptr)
			{

				throw SSLException(
					"SSL_CTX_new"
				);
			}

			if ((ssl = ::SSL_new(GetContext())) == nullptr)
			{
				::SSL_CTX_free(
					GetContext()
				);

				throw SSLException(
					"SSL_new"
				);
			}

			if ((fd = ::SSL_get_fd(GetHandle())) < 0)
			{

				fd = 0;
			}

			isCreated = True;
		}

		Void Destroy()
		{
			if (IsCreated())
			{
				if (IsConnected())
				{

					Shutdown();
				}

				::SSL_free(
					GetHandle()
				);

				::SSL_CTX_free(
					GetContext()
				);

				isConnected = False;
				isCreated   = False;
			}
		}

		// @throw AL::Exception
		Void Clear()
		{
			AL_ASSERT(
				IsCreated(),
				"SSL not created"
			);

			if (::SSL_clear(GetHandle()) == 0)
			{

				throw SSLException(
					"SSL_clear"
				);
			}
		}

		// @throw AL::Exception
		Void Accept()
		{
			AL_ASSERT(
				IsCreated(),
				"SSL not created"
			);

			AL_ASSERT(
				GetMode() == Modes::Server,
				"SSL not in server mode"
			);

			if (::SSL_accept(GetHandle()) <= 0)
			{

				throw SSLException(
					"SSL_accept"
				);
			}

			isConnected = True;
		}

		// @throw AL::Exception
		Void Connect()
		{
			AL_ASSERT(
				IsCreated(),
				"SSL not created"
			);

			AL_ASSERT(
				GetMode() == Modes::Client,
				"SSL not in client mode"
			);

			if (::SSL_connect(GetHandle()) <= 0)
			{

				throw SSLException(
					"SSL_connect"
				);
			}

			isConnected = True;
		}

		// @throw AL::Exception
		Void Shutdown()
		{
			AL_ASSERT(
				IsCreated(),
				"SSL not created"
			);

			if (IsConnected())
			{
				if (::SSL_shutdown(GetHandle()) < 0)
				{

					throw SSLException(
						"SSL_shutdown"
					);
				}

				isConnected = False;
			}
		}

		// @throw AL::Exception
		// @return AL::False on connection closed
		Bool Read(Void* lpBuffer, size_t size, size_t& numberOfBytesRead)
		{
			AL_ASSERT(
				IsCreated(),
				"SSL not created"
			);

			if (size > Integer<::size_t>::Maximum)
			{

				size = Integer<::size_t>::Maximum;
			}

			int      error;
			::size_t _numberOfBytesRead;

			if ((error = ::SSL_read_ex(GetHandle(), lpBuffer, static_cast<::size_t>(size), &_numberOfBytesRead)) <= 0)
			{
				if (error == 0)
				{
					Shutdown();

					return False;
				}

				if (::SSL_get_error(GetHandle(), error) == SSL_ERROR_WANT_READ)
				{
					numberOfBytesRead = 0;

					return True;
				}

				throw SSLException(
					"SSL_read_ex"
				);
			}

			numberOfBytesRead = _numberOfBytesRead;

			return True;
		}

		// @throw AL::Exception
		// @return AL::False on connection closed
		Bool Write(const Void* lpBuffer, size_t size, size_t& numberOfBytesWritten)
		{
			AL_ASSERT(
				IsCreated(),
				"SSL not created"
			);

			if (size > Integer<::size_t>::Maximum)
			{

				size = Integer<::size_t>::Maximum;
			}

			int      error;
			::size_t _numberOfBytesWritten;

			if ((error = ::SSL_write_ex(GetHandle(), lpBuffer, static_cast<::size_t>(size), &_numberOfBytesWritten)) <= 0)
			{
				if (error == 0)
				{
					Shutdown();

					return False;
				}

				if (::SSL_get_error(GetHandle(), error) == SSL_ERROR_WANT_WRITE)
				{
					numberOfBytesWritten = 0;

					return True;
				}

				throw SSLException(
					"SSL_write_ex"
				);
			}

			numberOfBytesWritten = _numberOfBytesWritten;

			return True;
		}

		SSL& operator = (SSL&& ssl)
		{
			if (IsCreated())
			{

				Destroy();
			}

			isCreated = ssl.isCreated;
			ssl.isCreated = False;

			isConnected = ssl.isConnected;
			ssl.isConnected = False;

			fd        = ssl.fd;
			this->ssl = ssl.ssl;
			mode      = ssl.mode;
			context   = ssl.context;
			protocol  = ssl.protocol;

			return *this;
		}

		Bool operator == (const SSL& ssl) const
		{
			if (IsCreated() != ssl.IsCreated())
			{

				return False;
			}

			if (IsCreated() && (GetHandle() != ssl.GetHandle()))
			{

				return False;
			}

			return True;
		}
		Bool operator != (const SSL& ssl) const
		{
			if (operator==(ssl))
			{

				return False;
			}

			return True;
		}

	private:
		static const SSL_METHOD* SSL_GetMethod(Modes mode, Protocols protocol)
		{
			switch (protocol)
			{
				case Protocols::TLS:  return (mode == Modes::Client) ? ::TLS_client_method()  : ::TLS_server_method();
				case Protocols::DTLS: return (mode == Modes::Client) ? ::DTLS_client_method() : ::DTLS_server_method();
			}

			return nullptr;
		}
	};
}
