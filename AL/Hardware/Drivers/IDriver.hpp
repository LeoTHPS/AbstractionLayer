#pragma once
#include "AL/Common.hpp"

namespace AL::Hardware::Drivers
{
	template<typename T_CHANNEL, typename T_DATA_READ, typename T_DATA_WRITE>
	class IDriver;

	template<typename T_DATA_READ, typename T_DATA_WRITE>
	class IDriver<Void, T_DATA_READ, T_DATA_WRITE>
	{
		IDriver(const IDriver&) = delete;

	public:
		typedef T_DATA_READ  ReadData;
		typedef T_DATA_WRITE WriteData;

		IDriver()
		{
		}

		virtual ~IDriver()
		{
		}

		virtual Bool IsOpen() const = 0;

		// @throw AL::Exception
		virtual Void Open() = 0;

		virtual Void Close() = 0;

		// @throw AL::Exception
		virtual Void Read(ReadData& data) = 0;

		// @throw AL::Exception
		virtual Void Write(const WriteData& data) = 0;
	};
	template<typename T_CHANNEL, typename T_DATA_READ, typename T_DATA_WRITE>
	class IDriver
	{
		IDriver(const IDriver&) = delete;

	public:
		typedef T_CHANNEL    Channel;
		typedef T_DATA_READ  ReadData;
		typedef T_DATA_WRITE WriteData;

		IDriver()
		{
		}

		virtual ~IDriver()
		{
		}

		virtual Bool IsOpen() const = 0;

		// @throw AL::Exception
		virtual Void Open() = 0;

		virtual Void Close() = 0;

		// @throw AL::Exception
		virtual Void Read(Channel channel, ReadData& data) = 0;

		// @throw AL::Exception
		virtual Void Write(Channel channel, const WriteData& data) = 0;
	};

	template<typename T_DATA_READ>
	class IDriver<Void, T_DATA_READ, Void>
	{
		IDriver(const IDriver&) = delete;

	public:
		typedef T_DATA_READ ReadData;

		IDriver()
		{
		}

		virtual ~IDriver()
		{
		}

		virtual Bool IsOpen() const = 0;

		// @throw AL::Exception
		virtual Void Open() = 0;

		virtual Void Close() = 0;

		// @throw AL::Exception
		virtual Void Read(ReadData& data) = 0;
	};
	template<typename T_CHANNEL, typename T_DATA_READ>
	class IDriver<T_CHANNEL, T_DATA_READ, Void>
	{
		IDriver(const IDriver&) = delete;

	public:
		typedef T_CHANNEL   Channel;
		typedef T_DATA_READ ReadData;

		IDriver()
		{
		}

		virtual ~IDriver()
		{
		}

		virtual Bool IsOpen() const = 0;

		// @throw AL::Exception
		virtual Void Open() = 0;

		virtual Void Close() = 0;

		// @throw AL::Exception
		virtual Void Read(Channel channel, ReadData& data) = 0;
	};
}
