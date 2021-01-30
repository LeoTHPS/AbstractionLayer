#pragma once
#include "AL/Common.hpp"

namespace AL::GPIO::Devices
{
	template<typename T_CHANNEL, typename T_DATA>
	class Device
	{
		bool isOpen = false;

		Device(const Device&) = delete;

	public:
		typedef T_DATA Data;
		typedef T_CHANNEL Channel;

		Device()
		{
		}

		Device(Device&& device)
			: isOpen(
				device.isOpen
			)
		{
			device.isOpen = false;
		}

		virtual ~Device()
		{
			Close();
		}

		bool IsOpen() const
		{
			return isOpen;
		}

		// @throw AL::Exceptions::Exception
		void Open()
		{
			AL_ASSERT(!IsOpen(), "Device already open");

			try
			{
				OnOpen();
			}
			catch (Exceptions::Exception&)
			{

				throw;
			}

			isOpen = true;
		}

		void Close()
		{
			if (IsOpen())
			{
				OnClose();

				isOpen = false;
			}
		}

		// @throw AL::Exceptions::Exception
		void Read(Channel channel, Data& value)
		{
			AL_ASSERT(IsOpen(), "Device not open");

			OnRead(
				channel,
				value
			);
		}

		auto& operator = (Device&& device)
		{
			Close();

			isOpen = device.isOpen;
			device.isOpen = false;

			return *this;
		}

	protected:
		// @throw AL::Exceptions::Exception
		virtual void OnOpen() = 0;

		virtual void OnClose() = 0;

		// @throw AL::Exceptions::Exception
		virtual void OnRead(Channel channel, Data& value) = 0;
	};
	template<typename T_DATA>
	class Device<void, T_DATA>
	{
		bool isOpen = false;

		Device(const Device&) = delete;

	public:
		typedef T_DATA Data;

		Device()
		{
		}

		Device(Device&& device)
			: isOpen(
				device.isOpen
			)
		{
			device.isOpen = false;
		}

		virtual ~Device()
		{
			Close();
		}

		bool IsOpen() const
		{
			return isOpen;
		}

		// @throw AL::Exceptions::Exception
		void Open()
		{
			AL_ASSERT(!IsOpen(), "Device already open");

			try
			{
				OnOpen();
			}
			catch (Exceptions::Exception&)
			{

				throw;
			}

			isOpen = true;
		}

		void Close()
		{
			if (IsOpen())
			{
				OnClose();

				isOpen = false;
			}
		}

		// @throw AL::Exceptions::Exception
		void Read(Data& value)
		{
			AL_ASSERT(IsOpen(), "Device not open");

			OnRead(
				value
			);
		}

		auto& operator = (Device&& device)
		{
			Close();

			isOpen = device.isOpen;
			device.isOpen = false;

			return *this;
		}

	protected:
		// @throw AL::Exceptions::Exception
		virtual void OnOpen() = 0;

		virtual void OnClose() = 0;

		// @throw AL::Exceptions::Exception
		virtual void OnRead(Data& value) = 0;
	};
}
