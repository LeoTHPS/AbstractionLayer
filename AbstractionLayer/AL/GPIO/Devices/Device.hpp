#pragma once
#include "AL/Common.hpp"

namespace AL::GPIO::Devices
{
	typedef uint32 DeviceChannel;

	template<typename T>
	class Device
	{
		bool isOpen = false;

		Device(const Device&) = delete;

	public:
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
		void Read(T& value, DeviceChannel channel)
		{
			AL_ASSERT(IsOpen(), "Device not open");

			OnRead(
				value,
				channel
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
		virtual void OnRead(T& value, DeviceChannel channel) = 0;
	};
}
