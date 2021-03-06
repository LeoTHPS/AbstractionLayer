#pragma once
#include "AL/Common.hpp"

#if defined(AL_PLATFORM_LINUX)
	#include <libusb-1.0/libusb.h>

	#if __has_include(<libusb-1.0/libusb.h>)
		#define AL_DEPENDENCY_LIBUSB
	#endif
#elif defined(AL_PLATFORM_WINDOWS)
	#include <winusb.h>
#endif

namespace AL::USB
{
	typedef uint16 VendorId;
	typedef uint16 ProductId;

	typedef Function<void(VendorId vendor, ProductId product)> DeviceEnumCallback;

	class Device
	{
		bool isOpen = false;
		bool isInterfaceOpen = false;

		uint32 interfaceId = 0;

#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_LIBUSB)
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
		uint32 _interface;

		libusb_device_handle* lpHandle = nullptr;
		libusb_device_descriptor descriptor;
	#endif
#elif defined(AL_PLATFORM_WINDOWS)
		VendorId vendorId;
		ProductId productId;
#endif

		Device(Device&&) = delete;
		Device(const Device&) = delete;

#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_LIBUSB)
		explicit Device(libusb_device_descriptor&& descriptor)
			: descriptor(
				Move(descriptor)
			)
		{
			isOpen = true;
		}
	#endif
#elif defined(AL_PLATFORM_WINDOWS)
		Device(VendorId vendorId, ProductId productId)
			: vendorId(
				vendorId
			),
			productId(
				productId
			)
		{
			isOpen = true;
		}
#endif

	public:
		// @throw AL::Exceptions::Exception
		// @return false if device does not exist
		static bool Open(Device& device, VendorId vendorId, ProductId productId)
		{
#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_LIBUSB)
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
	#else
			throw Exceptions::DependencyMissingException(
				"libusb"
			);
	#endif
#elif defined(AL_PLATFORM_WINDOWS)
			throw Exceptions::NotImplementedException();
#endif

			return true;
		}

		// @throw AL::Exceptions::Exception
		static void Enumerate(const DeviceEnumCallback& callback)
		{
#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_LIBUSB)
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

					callback(
						deviceDescriptor.idVendor,
						deviceDescriptor.idProduct
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
	#else
			throw Exceptions::DependencyMissingException(
				"libusb"
			);
	#endif
#elif defined(AL_PLATFORM_WINDOWS)
			throw Exceptions::NotImplementedException();
#endif
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
			return isOpen;
		}

		bool IsInterfaceOpen() const
		{
			return isInterfaceOpen;
		}

		auto GetVendorId() const
		{
#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_LIBUSB)
			return VendorId(
				descriptor.idVendor
			);
	#endif
#elif defined(AL_PLATFORM_WINDOWS)
			return vendorId;
#endif
		}

		auto GetProductId() const
		{
#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_LIBUSB)
			return ProductId(
				descriptor.idProduct
			);
	#endif
#elif defined(AL_PLATFORM_WINDOWS)
			return productId;
#endif
		}

		auto GetInterfaceId() const
		{
			return interfaceId;
		}

		// @throw AL::Exceptions::Exception
		void OpenInterface(uint32 interfaceId)
		{
			AL_ASSERT(IsOpen(), "Device not open");
			AL_ASSERT(!IsInterfaceOpen(), "Device interface already open");

#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_LIBUSB)
			if (!(lpHandle = libusb_open_device_with_vid_pid(nullptr, GetVendorId(), GetProductId())))
			{

				throw Exceptions::Exception(
					"Error calling libusb_open_device_with_vid_pid(nullptr, %04x, %04x)"
				);
			}

			if (libusb_claim_interface(lpHandle, static_cast<int>(_interface)) != 0)
			{
				libusb_close(
					lpHandle
				);

				lpHandle = nullptr;

				throw Exceptions::Exception(
					"Error calling 'libusb_claim_interface'"
				);
			}

			this->_interface = _interface;
	#else
			throw Exceptions::DependencyMissingException(
				"libusb"
			);
	#endif
#elif defined(AL_PLATFORM_WINDOWS)
			throw Exceptions::NotImplementedException();
#endif

			this->interfaceId = interfaceId;

			isInterfaceOpen = true;
		}

		void CloseInterface()
		{
			if (IsInterfaceOpen())
			{
#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_LIBUSB)
				libusb_release_interface(
					lpHandle,
					_interface
				);

				libusb_close(
					lpHandle
				);

				lpHandle = nullptr;
	#endif
#elif defined(AL_PLATFORM_WINDOWS)

#endif

				isInterfaceOpen = false;
			}
		}

		// @throw AL::Exceptions::Exception
		// @return number of bytes read
		size_t Read(uint8 endPoint, void* lpBuffer, size_t size)
		{
			AL_ASSERT(IsOpen(), "Device not open");
			AL_ASSERT(IsInterfaceOpen(), "Device interface not open");

#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_LIBUSB)
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
			
	#else
			throw Exceptions::DependencyMissingException(
				"libusb"
			);
	#endif
#elif defined(AL_PLATFORM_WINDOWS)
			throw Exceptions::NotImplementedException();
#endif
		}

		// @throw AL::Exceptions::Exception
		// @return number of bytes written
		size_t Write(uint8 endPoint, const void* lpBuffer, size_t size)
		{
			AL_ASSERT(IsOpen(), "Device not open");
			AL_ASSERT(IsInterfaceOpen(), "Device interface not open");

#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_LIBUSB)
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
			
	#else
			throw Exceptions::DependencyMissingException(
				"libusb"
			);
	#endif
#elif defined(AL_PLATFORM_WINDOWS)
			throw Exceptions::NotImplementedException();
#endif
		}

	private:
		void Close()
		{
			if (IsOpen())
			{
				if (IsInterfaceOpen())
				{

					CloseInterface();
				}

#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_LIBUSB)

	#endif
#elif defined(AL_PLATFORM_WINDOWS)

#endif
			}
		}
	};
}
