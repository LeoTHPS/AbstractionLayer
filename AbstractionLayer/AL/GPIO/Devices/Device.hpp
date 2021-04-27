#pragma once
#include "AL/Common.hpp"

namespace AL::GPIO::Devices
{
	// Read(channel, value) + Write(channel, value)
	template<typename T_CHANNEL, typename T_DATA_READ, typename T_DATA_WRITE>
	class Device
	{
		bool isOpen = false;

		Device(const Device&) = delete;

	public:
		typedef T_DATA_READ DataR;
		typedef T_DATA_WRITE DataW;
		
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
		void Read(Channel channel, DataR& value)
		{
			AL_ASSERT(IsOpen(), "Device not open");

			OnRead(
				channel,
				value
			);
		}

		// @throw AL::Exceptions::Exception
		void Write(Channel channel, const DataW& value)
		{
			AL_ASSERT(IsOpen(), "Device not open");

			OnWrite(
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
		virtual void OnRead(Channel channel, DataR& value) = 0;

		// @throw AL::Exceptions::Exception
		virtual void OnWrite(Channel channel, const DataW& value) = 0;
	};
	
	// Read(channel, value)
	template<typename T_CHANNEL, typename T_DATA_READ>
	class Device<T_CHANNEL, T_DATA_READ, void>
	{
		bool isOpen = false;

		Device(const Device&) = delete;

	public:
		typedef T_DATA_READ DataR;
		typedef void DataW;
		
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
		void Read(Channel channel, DataR& value)
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
		virtual void OnRead(Channel channel, DataR& value) = 0;
	};

	// Write(channel, value)
	template<typename T_CHANNEL, typename T_DATA_WRITE>
	class Device<T_CHANNEL, void, T_DATA_WRITE>
	{
		bool isOpen = false;

		Device(const Device&) = delete;

	public:
		typedef void DataR;
		typedef T_DATA_WRITE DataW;

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
		void Write(Channel channel, DataW& value)
		{
			AL_ASSERT(IsOpen(), "Device not open");

			OnWrite(
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
		virtual void OnWrite(Channel channel, const DataW& value) = 0;
	};
	
	// Read(value) + Write(value)
	template<typename T_DATA_READ, typename T_DATA_WRITE>
	class Device<void, T_DATA_READ, T_DATA_WRITE>
	{
		bool isOpen = false;

		Device(const Device&) = delete;

	public:
		typedef T_DATA_READ DataR;
		typedef T_DATA_WRITE DataW;

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
		void Read(DataR& value)
		{
			AL_ASSERT(IsOpen(), "Device not open");

			OnRead(
				value
			);
		}

		// @throw AL::Exceptions::Exception
		void Write(const DataW& value)
		{
			AL_ASSERT(IsOpen(), "Device not open");

			OnWrite(
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
		virtual void OnRead(DataR& value) = 0;

		// @throw AL::Exceptions::Exception
		virtual void OnWrite(const DataW& value) = 0;
	};
	
	// Read(value)
	template<typename T_DATA_READ>
	class Device<void, T_DATA_READ, void>
	{
		bool isOpen = false;

		Device(const Device&) = delete;

	public:
		typedef T_DATA_READ DataR;
		typedef void DataW;

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
		void Read(DataR& value)
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
		virtual void OnRead(DataR& value) = 0;
	};
	
	// Write(value)
	template<typename T_DATA_WRITE>
	class Device<void, void, T_DATA_WRITE>
	{
		bool isOpen = false;

		Device(const Device&) = delete;

	public:
		typedef void DataR;
		typedef T_DATA_WRITE DataW;

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
		void Write(DataW& value)
		{
			AL_ASSERT(IsOpen(), "Device not open");

			OnWrite(
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
		virtual void OnWrite(const DataW& value) = 0;
	};
}
