#pragma once
#include "AL/Common.hpp"

#if defined(AL_PLATFORM_LINUX)
	#include <libusb-1.0/libusb.h>
#elif defined(AL_PLATFORM_WINDOWS)
	#error Platform not supported
#endif

namespace AL::USB
{
	typedef uint16 VendorId;
	typedef uint16 ProductId;

	class Device;

	typedef Function<void(Device& device)> DeviceEnumCallback;

	class Device
	{
		class LibUSB final
		{
		public:
			LibUSB()
			{
				libusb_init(nullptr);
			}

			~LibUSB()
			{
				libusb_exit(nullptr);
			}
		};

		LibUSB libUSB;
		uint32 interface;

		libusb_device_handle* lpHandle = nullptr;
		libusb_device_descriptor descriptor;

		Device(Device&&) = delete;
		Device(const Device&) = delete;

		explicit Device(libusb_device_descriptor&& descriptor)
			: descriptor(
				Move(descriptor)
			)
		{
		}

	public:
		// @throw AL::Exceptions::Exception
		// @return false if device is not found
		static bool FindDevice(Device& device, VendorId vendorId, ProductId productId)
		{
			LibUSB libUSB;

			libusb_device** deviceList;
			int deviceListCount;

			if ((deviceListCount = libusb_get_device_list(nullptr, &deviceList)) < 0)
			{

				throw Exceptions::Exception(
					"Error calling 'libusb_get_device_list': %i",
					deviceListCount
				);
			}

			bool deviceDescriptorFound = false;

			libusb_device_descriptor deviceDescriptor = { 0 };

			try
			{
				for (int errorCode, i = 0; i < deviceListCount; ++i)
				{
					if ((errorCode = libusb_get_device_descriptor(deviceList[i], &deviceDescriptor)) < 0)
					{

						throw Exceptions::Exception(
							"Error calling 'libusb_get_device_descriptor': %i",
							errorCode
						);
					}

					if ((deviceDescriptor.idVendor == vendorId) && (deviceDescriptor.idProduct == productId))
					{
						deviceDescriptorFound = true;

						break;
					}
				}
			}
			catch (Exceptions::Exception&)
			{
				libusb_free_device_list(
					deviceList,
					1
				);

				throw;
			}

			libusb_free_device_list(
				deviceList,
				1
			);

			if (!deviceDescriptorFound)
			{

				return false;
			}

			device.~Device();

			new (&device) Device(
				Move(deviceDescriptor)
			);

			return true;
		}

		// @throw AL::Exceptions::Exception
		static void EnumerateDevices(const DeviceEnumCallback& callback)
		{
			LibUSB libUSB;

			libusb_device** deviceList;
			int deviceListCount;

			if ((deviceListCount = libusb_get_device_list(nullptr, &deviceList)) < 0)
			{

				throw Exceptions::Exception(
					"Error calling 'libusb_get_device_list': %i",
					deviceListCount
				);
			}

			libusb_device_descriptor deviceDescriptor = { 0 };

			try
			{
				for (int errorCode, i = 0; i < deviceListCount; ++i)
				{
					if ((errorCode = libusb_get_device_descriptor(deviceList[i], &deviceDescriptor)) < 0)
					{

						throw Exceptions::Exception(
							"Error calling 'libusb_get_device_descriptor': %i",
							errorCode
						);
					}

					Device device(
						Move(deviceDescriptor)
					);

					callback(
						device
					);
				}
			}
			catch (Exceptions::Exception&)
			{
				libusb_free_device_list(
					deviceList,
					1
				);

				throw;
			}

			libusb_free_device_list(
				deviceList,
				1
			);
		}

		Device()
		{
		}

		virtual ~Device()
		{
			Close();
		}

		bool IsOpen() const
		{
			return lpHandle != nullptr;
		}

		auto GetVendorId() const
		{
			return VendorId(
				descriptor.idVendor
			);
		}

		auto GetProductId() const
		{
			return ProductId(
				descriptor.idProduct
			);
		}

		// @throw AL::Exceptions::Exception
		void Open(uint32 interface)
		{
			AL_ASSERT(!IsOpen(), "Device already open");

			if (!(lpHandle = libusb_open_device_with_vid_pid(nullptr, GetVendorId(), GetProductId())))
			{

				throw Exceptions::Exception(
					"Error calling libusb_open_device_with_vid_pid(nullptr, %04x, %04x)"
				);
			}

			if (libusb_claim_interface(lpHandle, static_cast<int>(interface)) != 0)
			{
				libusb_close(
					lpHandle
				);

				lpHandle = nullptr;

				throw Exceptions::Exception(
					"Error calling 'libusb_claim_interface'"
				);
			}

			this->interface = interface;
		}

		void Close()
		{
			if (IsOpen())
			{
				libusb_release_interface(
					lpHandle,
					interface
				);

				libusb_close(
					lpHandle
				);

				lpHandle = nullptr;
			}
		}

		// @throw AL::Exceptions::Exception
		// @return number of bytes read
		size_t Read(uint8 endPoint, void* lpBuffer, size_t size)
		{
			AL_ASSERT(IsOpen(), "Device not open");

			int bytesRead;

			if (libusb_bulk_transfer(lpHandle, static_cast<unsigned char>(endPoint) | LIBUSB_ENDPOINT_IN, reinterpret_cast<unsigned char*>(lpBuffer), static_cast<int>(size), &bytesRead, 0) != 0)
			{

				throw Exceptions::Exception(
					"Error calling 'libusb_bulk_transfer'"
				);
			}

			return static_cast<size_t>(
				bytesRead
			);
		}

		// @throw AL::Exceptions::Exception
		// @return number of bytes written
		size_t Write(uint8 endPoint, const void* lpBuffer, size_t size)
		{
			AL_ASSERT(IsOpen(), "Device not open");

			int bytesWritten;

			if (libusb_bulk_transfer(lpHandle, static_cast<unsigned char>(endPoint) | LIBUSB_ENDPOINT_OUT, const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(lpBuffer)), static_cast<int>(size), &bytesWritten, 0) != 0)
			{

				throw Exceptions::Exception(
					"Error calling 'libusb_bulk_transfer'"
				);
			}

			return static_cast<size_t>(
				bytesWritten
			);
		}
	};
}
