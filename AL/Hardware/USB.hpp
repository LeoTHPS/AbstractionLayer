#pragma once
#include <AL/Common.hpp>

#if defined(AL_PLATFORM_PICO)
	#warning Platform not supported
#elif defined(AL_PLATFORM_LINUX)
	#if AL_HAS_INCLUDE(<libusb-1.0/libusb.h>)
		#define AL_DEPENDENCY_LIBUSB

		#include <libusb-1.0/libusb.h>
	#endif
#elif defined(AL_PLATFORM_WINDOWS)
	#include <winusb.h>
#endif

namespace AL::Hardware
{
	typedef uint16 USBVendorId;
	typedef uint16 USBProductId;
	typedef uint32 USBInterfaceId;

	typedef Function<Void(USBVendorId vendorId, USBProductId product)> USBDeviceEnumCallback;

	class USBDevice
	{
		Bool           isOpen          = False;
		Bool           isInterfaceOpen = False;

		USBVendorId    vendorId        = 0;
		USBProductId   productId       = 0;
		USBInterfaceId interfaceId     = 0;

#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_LIBUSB)
		class LibUSB
		{
			Bool isInitialized;

		public:
			LibUSB()
				: isInitialized(
					True
				)
			{
				::libusb_init(
					nullptr
				);
			}

			LibUSB(LibUSB&& libUSB)
				: isInitialized(
					libUSB.isInitialized
				)
			{
				libUSB.isInitialized = False;
			}

			LibUSB(const LibUSB& libUSB)
				: isInitialized(
					libUSB.isInitialized
				)
			{
				if (libUSB.IsInitialized())
				{
					::libusb_init(
						nullptr
					);
				}
			}

			virtual ~LibUSB()
			{
				if (IsInitialized())
				{
					::libusb_exit(
						nullptr
					);
				}
			}

			Bool IsInitialized() const
			{
				return isInitialized;
			}

			LibUSB& operator = (LibUSB&& libUSB)
			{
				if (IsInitialized())
				{
					::libusb_exit(
						nullptr
					);
				}

				isInitialized        = libUSB.isInitialized;
				libUSB.isInitialized = False;

				return *this;
			}
			LibUSB& operator = (const LibUSB& libUSB)
			{
				if (IsInitialized())
				{
					::libusb_exit(
						nullptr
					);
				}

				if ((isInitialized = libUSB.isInitialized) == True)
				{
					::libusb_init(
						nullptr
					);
				}

				return *this;
			}
		};

		LibUSB                     libUSB;
		::libusb_device_descriptor descriptor;
		::libusb_device_handle*    hInterface;
	#endif
#elif defined(AL_PLATFORM_WINDOWS)

#endif

		USBDevice(const USBDevice&) = delete;

#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_LIBUSB)
		explicit USBDevice(::libusb_device_descriptor&& descriptor)
			: isOpen(
				True
			),
			vendorId(
				descriptor.idVendor
			),
			productId(
				descriptor.idProduct
			),
			descriptor(
				Move(descriptor)
			)
		{
		}
	#endif
#elif defined(AL_PLATFORM_WINDOWS)
		USBDevice(USBVendorId vendorId, USBProductId productId)
			: isOpen(
				True
			),
			vendorId(
				vendorId
			),
			productId(
				productId
			)
		{
		}
#endif

	public:
		// @throw AL::Exception
		// @return AL::False if device does not exist
		static Bool Open(USBDevice& device, USBVendorId vendorId, USBProductId productId)
		{
#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_LIBUSB)
			LibUSB libUSB;

			::libusb_device** deviceList;
			int               deviceListCount;

			if ((deviceListCount = ::libusb_get_device_list(nullptr, &deviceList)) < 0)
			{

				throw Exception(
					"Error calling 'libusb_get_device_list': %i",
					deviceListCount
				);
			}

			Bool deviceDescriptorFound = False;

			::libusb_device_descriptor deviceDescriptor = { };

			try
			{
				for (int errorCode, i = 0; i < deviceListCount; ++i)
				{
					if ((errorCode = ::libusb_get_device_descriptor(deviceList[i], &deviceDescriptor)) < 0)
					{

						throw Exception(
							"Error calling 'libusb_get_device_descriptor': %i",
							errorCode
						);
					}

					if ((deviceDescriptor.idVendor == vendorId) && (deviceDescriptor.idProduct == productId))
					{
						deviceDescriptorFound = True;

						break;
					}
				}
			}
			catch (Exception&)
			{
				::libusb_free_device_list(
					deviceList,
					1
				);

				throw;
			}

			::libusb_free_device_list(
				deviceList,
				1
			);

			if (!deviceDescriptorFound)
			{

				return False;
			}

			device = USBDevice(
				Move(deviceDescriptor)
			);
	#else
			throw DependencyMissingException(
				"libusb"
			);
	#endif
#elif defined(AL_PLATFORM_WINDOWS)
			// TODO: implement
			throw NotImplementedException();
#endif

			return True;
		}

		// @throw AL::Exception
		static Void Enumerate(const USBDeviceEnumCallback& callback)
		{
#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_LIBUSB)
			LibUSB libUSB;

			::libusb_device** deviceList;
			int               deviceListCount;

			if ((deviceListCount = ::libusb_get_device_list(nullptr, &deviceList)) < 0)
			{

				throw Exception(
					"Error calling 'libusb_get_device_list': %i",
					deviceListCount
				);
			}

			::libusb_device_descriptor deviceDescriptor = { };

			try
			{
				for (int errorCode, i = 0; i < deviceListCount; ++i)
				{
					if ((errorCode = ::libusb_get_device_descriptor(deviceList[i], &deviceDescriptor)) < 0)
					{

						throw Exception(
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
			catch (Exception&)
			{
				::libusb_free_device_list(
					deviceList,
					1
				);

				throw;
			}

			::libusb_free_device_list(
				deviceList,
				1
			);
	#else
			throw DependencyMissingException(
				"libusb"
			);
	#endif
#elif defined(AL_PLATFORM_WINDOWS)
			// TODO: implement
			throw NotImplementedException();
#endif
		}

		USBDevice()
		{
		}

		USBDevice(USBDevice&& usbDevice)
			: isOpen(
				usbDevice.isOpen
			),
			isInterfaceOpen(
				usbDevice.isInterfaceOpen
			),
			vendorId(
				usbDevice.vendorId
			),
			productId(
				usbDevice.productId
			),
			interfaceId(
				usbDevice.interfaceId
			)
#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_LIBUSB)
			,
			libUSB(
				Move(usbDevice.libUSB)
			),
			descriptor(
				Move(usbDevice.descriptor)
			),
			hInterface(
				usbDevice.hInterface
			)
	#endif
#elif defined(AL_PLATFORM_WINDOWS)
			// TODO: implement
#endif
		{
			usbDevice.isOpen = False;
			usbDevice.isInterfaceOpen = False;

#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_LIBUSB)
			usbDevice.hInterface = nullptr;
	#endif
#elif defined(AL_PLATFORM_WINDOWS)
			// TODO: implement
#endif
		}

		virtual ~USBDevice()
		{
			if (IsOpen())
			{

				Close();
			}
		}

		Bool IsOpen() const
		{
			return isOpen;
		}

		Bool IsInterfaceOpen() const
		{
			return isInterfaceOpen;
		}

		auto GetVendorId() const
		{
			return vendorId;
		}

		auto GetProductId() const
		{
			return productId;
		}

		auto GetInterfaceId() const
		{
			return interfaceId;
		}

		Void Close()
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
				// TODO: implement
#endif

				isOpen = False;
			}
		}

		// @throw AL::Exception
		Void OpenInterface(uint32 interfaceId)
		{
			AL_ASSERT(
				IsOpen(),
				"USBDevice not open"
			);

			AL_ASSERT(
				!IsInterfaceOpen(),
				"USBDevice interface already open"
			);

#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_LIBUSB)
			if (!(hInterface = ::libusb_open_device_with_vid_pid(nullptr, GetVendorId(), GetProductId())))
			{

				throw Exception(
					"Error calling libusb_open_device_with_vid_pid(nullptr, %04x, %04x)"
				);
			}

			if (::libusb_claim_interface(hInterface, static_cast<int>(interfaceId)) != 0)
			{
				::libusb_close(
					hInterface
				);

				hInterface = nullptr;

				throw Exception(
					"Error calling 'libusb_claim_interface'"
				);
			};
	#else
			throw DependencyMissingException(
				"libusb"
			);
	#endif
#elif defined(AL_PLATFORM_WINDOWS)
			// TODO: implement
			throw NotImplementedException();
#endif

			this->interfaceId = interfaceId;

			isInterfaceOpen = True;
		}

		Void CloseInterface()
		{
			if (IsInterfaceOpen())
			{
#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_LIBUSB)
				libusb_release_interface(
					hInterface,
					GetInterfaceId()
				);

				libusb_close(
					hInterface
				);

				hInterface = nullptr;
	#endif
#elif defined(AL_PLATFORM_WINDOWS)
				// TODO: implement
#endif

				isInterfaceOpen = False;
			}
		}

		// @throw AL::Exception
		// @return number of bytes read
		size_t Read(uint8 endPoint, Void* lpBuffer, size_t size) const
		{
			AL_ASSERT(
				IsOpen(),
				"USBDevice not open"
			);

			AL_ASSERT(
				IsInterfaceOpen(),
				"USBDevice interface not open"
			);

#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_LIBUSB)
			int bytesRead;

			if (::libusb_bulk_transfer(hInterface, static_cast<unsigned char>(endPoint) | LIBUSB_ENDPOINT_IN, reinterpret_cast<unsigned char*>(lpBuffer), static_cast<int>(size), &bytesRead, 0) != 0)
			{

				throw Exception(
					"Error calling 'libusb_bulk_transfer'"
				);
			}

			return static_cast<size_t>(
				bytesRead
			);
			
	#else
			throw DependencyMissingException(
				"libusb"
			);
	#endif
#elif defined(AL_PLATFORM_WINDOWS)
			// TODO: implement
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		// @return number of bytes written
		size_t Write(uint8 endPoint, const Void* lpBuffer, size_t size)
		{
			AL_ASSERT(
				IsOpen(),
				"USBDevice not open"
			);

			AL_ASSERT(
				IsInterfaceOpen(),
				"USBDevice interface not open"
			);

#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_LIBUSB)
			int bytesWritten;

			if (::libusb_bulk_transfer(hInterface, static_cast<unsigned char>(endPoint) | LIBUSB_ENDPOINT_OUT, const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(lpBuffer)), static_cast<int>(size), &bytesWritten, 0) != 0)
			{

				throw Exception(
					"Error calling 'libusb_bulk_transfer'"
				);
			}

			return static_cast<size_t>(
				bytesWritten
			);
			
	#else
			throw DependencyMissingException(
				"libusb"
			);
	#endif
#elif defined(AL_PLATFORM_WINDOWS)
			// TODO: implement
			throw NotImplementedException();
#endif
		}

		USBDevice& operator = (USBDevice&& usbDevice)
		{
			isOpen                    = usbDevice.isOpen;
			usbDevice.isOpen          = False;

			isInterfaceOpen           = usbDevice.isInterfaceOpen;
			usbDevice.isInterfaceOpen = False;

			vendorId                  = usbDevice.vendorId;
			productId                 = usbDevice.productId;
			interfaceId               = usbDevice.interfaceId;

#if defined(AL_PLATFORM_LINUX)
	#if defined(AL_DEPENDENCY_LIBUSB)
			libUSB                    = Move(usbDevice.libUSB);
			descriptor                = Move(usbDevice.descriptor);

			hInterface                = usbDevice.hInterface;
			usbDevice.hInterface      = nullptr;
	#endif
#elif defined(AL_PLATFORM_WINDOWS)
			// TODO: implement
#endif

			return *this;
		}

		Bool operator == (const USBDevice& usbDevice) const
		{
			if (GetVendorId() != usbDevice.GetVendorId())
			{

				return False;
			}

			if (GetProductId() != usbDevice.GetProductId())
			{

				return False;
			}

			return True;
		}
		Bool operator != (const USBDevice& usbDevice) const
		{
			if (operator==(usbDevice))
			{

				return False;
			}

			return True;
		}
	};
}
