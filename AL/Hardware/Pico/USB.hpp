#pragma once
#include "AL/Common.hpp"

#if !defined(AL_PLATFORM_PICO)
	#error Platform not supported
#endif

#include "AL/Collections/Array.hpp"
#include "AL/Collections/LinkedList.hpp"

#include <hardware/irq.h>
#include <hardware/resets.h>

#include <hardware/regs/usb.h>

#include <hardware/structs/usb.h>

namespace AL::Hardware::Pico
{
	typedef uint16 USBVendorId;
	typedef uint16 USBProductId;
	typedef uint8  USBInterfaceId;
	typedef uint8  USBInterfaceClass;
	typedef uint8  USBInterfaceProtocol;
	typedef uint8  USBInterfaceEndPointAddress;

	enum class USBConfigFlags : uint8
	{
		None         = 0b00000000,
		SelfPowered  = 0b01000000,
		RemoteWakeUp = 0b00100000
	};

	AL_DEFINE_ENUM_FLAG_OPERATORS(USBConfigFlags);

	enum class USBInterfaceEndPointTypes : uint8
	{
		RX = 0x80,
		TX = 0x00
	};

	enum class USBInterfaceEndPointAttributes : uint8
	{
		SyncType_None                      = 0b00000000,
		SyncType_Sync                      = 0b00001100,
		SyncType_Async                     = 0b00000100,
		SyncType_Adaptive                  = 0b00001000,

		UsageType_DataEndPoint             = 0b00000000,
		UsageType_FeedbackEndPoint         = 0b00010000,
		UsageType_ExplicitFeedbackEndPoint = 0b00100000,

		TransferType_Bulk                  = 0b00000010,
		TransferType_Control               = 0b00000000,
		TransferType_Interrupt             = 0b00000011,
		TransferType_Isochronous           = 0b00000001
	};

	AL_DEFINE_ENUM_FLAG_OPERATORS(USBInterfaceEndPointAttributes);

	typedef Void(*USBInterfaceEndPointHandler)(const Void* lpBuffer, uint16 size);

	struct USBInterfaceEndPointConfig
	{
		USBInterfaceEndPointTypes               Type;
		USBInterfaceEndPointAddress             Address;
		USBInterfaceEndPointHandler             Handler;
		uint8                                   Interval;
		BitMask<USBInterfaceEndPointAttributes> Attributes;
		uint16                                  MaxPacketSize;

		volatile uint8*                         DataBuffer;
		volatile uint32_t*                      BufferControl;
		volatile uint32_t*                      EndPointControl;
	};

	typedef Collections::LinkedList<USBInterfaceEndPointConfig> USBInterfaceEndPointConfigs;

	struct USBInterfaceConfig
	{
		USBInterfaceId              Id;
		USBInterfaceClass           Class;
		USBInterfaceClass           SubClass;
		USBInterfaceProtocol        Protocol;
		USBInterfaceEndPointConfigs EndPoints;
		String                      Descriptor;
	};

	typedef Collections::LinkedList<USBInterfaceConfig> USBInterfaceConfigs;

	struct USBConfig
	{
		String                  Vendor;
		USBVendorId             VendorId;

		String                  Product;
		USBProductId            ProductId;
		uint16                  ProductVersion;

		BitMask<USBConfigFlags> DeviceFlags;
		uint16                  DeviceMaxPower = 100; // 100ma
		USBInterfaceConfigs     DeviceInterfaces;
		String                  DeviceSerialNumber;
	};

	class USB
	{
		enum
		{
			USB_REQ_TYPE_STANDARD            = 0x00,
			USB_REQ_TYPE_TYPE_MASK           = 0x60,
			USB_REQ_TYPE_TYPE_CLASS          = 0x20,
			USB_REQ_TYPE_TYPE_VENDOR         = 0x40,

			USB_REQ_TYPE_RECIPIENT_MASK      = 0x1F,
			USB_REQ_TYPE_RECIPIENT_DEVICE    = 0x00,
			USB_REQ_TYPE_RECIPIENT_INTERFACE = 0x01,
			USB_REQ_TYPE_RECIPIENT_ENDPOINT  = 0x02,

			USB_DIR_OUT                      = 0x00,
			USB_DIR_IN                       = 0x80,

			USB_TRANSFER_TYPE_CONTROL        = 0x00,
			USB_TRANSFER_TYPE_ISOCHRONOUS    = 0x01,
			USB_TRANSFER_TYPE_BULK           = 0x02,
			USB_TRANSFER_TYPE_INTERRUPT      = 0x03,
			USB_TRANSFER_TYPE_BITS           = 0x03,

			USB_DT_DEVICE                    = 0x01,
			USB_DT_CONFIG                    = 0x02,
			USB_DT_STRING                    = 0x03,
			USB_DT_INTERFACE                 = 0x04,
			USB_DT_ENDPOINT                  = 0x05,

			USB_REQUEST_GET_STATUS           = 0x00,
			USB_REQUEST_CLEAR_FEATURE        = 0x01,
			USB_REQUEST_SET_FEATURE          = 0x03,
			USB_REQUEST_SET_ADDRESS          = 0x05,
			USB_REQUEST_GET_DESCRIPTOR       = 0x06,
			USB_REQUEST_SET_DESCRIPTOR       = 0x07,
			USB_REQUEST_GET_CONFIGURATION    = 0x08,
			USB_REQUEST_SET_CONFIGURATION    = 0x09,
			USB_REQUEST_GET_INTERFACE        = 0x0A,
			USB_REQUEST_SET_INTERFACE        = 0x0B,
			USB_REQUEST_SYNC_FRAME           = 0x0C,

			USB_REQUEST_MSC_GET_MAX_LUN      = 0xFE,
			USB_REQUEST_MSC_RESET            = 0xFF,

			USB_FEAT_ENDPOINT_HALT           = 0x00,
			USB_FEAT_DEVICE_REMOTE_WAKEUP    = 0x01,
			USB_FEAT_TEST_MODE               = 0x02,

			USB_DESCRIPTOR_TYPE_ENDPOINT     = 0x05
		};

#pragma pack(push, 1)
		struct usb_setup_packet
		{
			uint8  bmRequestType;
			uint8  bRequest;
			uint16 wValue;
			uint16 wIndex;
			uint16 wLength;
		};

		struct usb_device_descriptor
		{
			uint8  bLength;
			uint8  bDescriptorType;
			uint16 bcdUSB;
			uint8  bDeviceClass;
			uint8  bDeviceSubClass;
			uint8  bDeviceProtocol;
			uint8  bMaxPacketSize;
			uint16 idVendor;
			uint16 idProduct;
			uint16 bcdDevice;
			uint8  iManufacturer;
			uint8  iProduct;
			uint8  iSerialNumber;
			uint8  bNumConfigurations;
		};

		struct usb_configuration_descriptor
		{
			uint8  bLength;
			uint8  bDescriptorType;
			uint16 wTotalLength;
			uint8  bNumInterfaces;
			uint8  bConfigurationValue;
			uint8  iConfiguration;
			uint8  bmAttributes;
			uint8  bMaxPower;
		};

		struct usb_interface_descriptor
		{
			uint8 bLength;
			uint8 bDescriptorType;
			uint8 bInterfaceNumber;
			uint8 bAlternateSetting;
			uint8 bNumEndpoints;
			uint8 bInterfaceClass;
			uint8 bInterfaceSubClass;
			uint8 bInterfaceProtocol;
			uint8 iInterface;
		};

		struct usb_endpoint_descriptor
		{
			uint8  bLength;
			uint8  bDescriptorType;
			uint8  bEndpointAddress;
			uint8  bmAttributes;
			uint16 wMaxPacketSize;
			uint8  bInterval;
		};

		template<size_t LANGUAGE_COUNT>
		struct usb_language_descriptor
		{
			uint8  bLength;
			uint8  bDescriptorType;
			uint16 wLANGID[LANGUAGE_COUNT];
		};
#pragma pack(pop)

		struct usb_endpoint_context
		{
			USBInterfaceEndPointConfig* lpConfig;

			USBInterfaceEndPointHandler handler;
			usb_endpoint_descriptor     descriptor;

			// Toggle after each packet (unless replying to a SETUP)
			uint8                       next_pid;

			// Pointers to endpoint + buffer control registers in the USB controller DPSRAM
			volatile uint8*             data_buffer;
			volatile uint32_t*          buffer_control;
			volatile uint32_t*          endpoint_control;
		};

		struct usb_interface_context
		{
			USBInterfaceConfig*                      lpConfig;
			Collections::Array<usb_endpoint_context> endpoints;
			usb_interface_descriptor                 descriptor;
		};

		struct usb_configuration_context
		{
			usb_configuration_descriptor descriptor;
		};

		struct usb_device_context
		{
			usb_device_descriptor                         device;
			usb_configuration_context                     config;
			Collections::Array<const char*>               strings;
			Collections::Array<usb_interface_context>     interfaces;
		};

		enum USB_DESCRIPTORS : uint8
		{
			USB_DESCRIPTOR_VENDOR        = 1,
			USB_DESCRIPTOR_PRODUCT       = 2,
			USB_DESCRIPTOR_DEVICE_SERIAL = 3,
			USB_DESCRIPTOR_USER_DEFINED  = 4
		};

		inline static Bool               isOpen           = False;
		inline static Bool               isConfigured     = False;
		inline static Bool               isAddressPending = False;

		inline static USBConfig          config;
		inline static uint8              buffer[64];
		inline static uint8              address = 0;
		inline static usb_device_context context;

		USB() = delete;

	public:
		static Bool IsOpen()
		{
			return isOpen;
		}

		static const auto& GetConfig()
		{
			return config;
		}

		// @throw AL::Exception
		static Void Open(USBConfig&& config)
		{
			AL_ASSERT(
				!IsOpen(),
				"USB already open"
			);

			Open_InitConfig(
				Move(config)
			);

			Open_InitUSBConfig(
				USB::config,
				USB::context
			);

			Open_InitUSB(
				USB::config,
				USB::context
			);

			isOpen = True;
		}
		// @throw AL::Exception
		static Void Open(const USBConfig& config)
		{
			Open(USBConfig(config));
		}

		static Void Close()
		{
			if (IsOpen())
			{
				::irq_remove_handler(
					USBCTRL_IRQ,
					&USB::isr_usbctrl
				);

				::reset_block(
					RESETS_RESET_USBCTRL_BITS
				);

				::unreset_block_wait(
					RESETS_RESET_USBCTRL_BITS
				);

				memset(
					usb_dpram,
					0,
					sizeof(*usb_dpram)
				);

				::irq_set_enabled(
					USBCTRL_IRQ,
					false
				);

				isOpen = False;
			}
		}

		static Bool BeginTransfer(USBInterfaceId interface, USBInterfaceEndPointAddress address, const Void* lpBuffer, uint16 size)
		{
			AL_ASSERT(
				IsOpen(),
				"USB not open"
			);

			if (auto lpEndPoint = usb_get_endpoint_context(interface, address, true))
			{
				usb_start_transfer(
					lpEndPoint,
					reinterpret_cast<uint8*>(const_cast<Void*>(lpBuffer)),
					size
				);

				return True;
			}

			return False;
		}

	private:
		// @throw AL::Exception
		static Void Open_InitConfig(USBConfig&& config)
		{
			if (config.DeviceMaxPower > 500)
			{

				config.DeviceMaxPower = 500;
			}

			while (config.DeviceInterfaces.GetSize() > Integer<uint8>::Maximum)
			{

				config.DeviceInterfaces.PopBack();
			}

			for (auto& interface : config.DeviceInterfaces)
			{
				if (interface.Id == 0)
				{

					throw Exception(
						"Interface 0 is reserved by the API"
					);
				}

				while (interface.EndPoints.GetSize() > Integer<uint8>::Maximum)
				{

					interface.EndPoints.PopBack();
				}

				for (auto& endPoint : interface.EndPoints)
				{
					if (Math::IsInRange<uint8>(endPoint.MaxPacketSize, 0, 8))
					{

						endPoint.MaxPacketSize = 8;
					}
					else if (Math::IsInRange<uint8>(endPoint.MaxPacketSize, 9, 16))
					{

						endPoint.MaxPacketSize = 16;
					}
					else if (Math::IsInRange<uint8>(endPoint.MaxPacketSize, 17, 32))
					{

						endPoint.MaxPacketSize = 32;
					}
					else if (Math::IsInRange<uint8>(endPoint.MaxPacketSize, 33, ~0))
					{

						endPoint.MaxPacketSize = 64;
					}
				}
			}

			{
				USBInterfaceConfig interface0 =
				{
					.Id         = 0,
					.Class      = 0x00,
					.SubClass   = 0x00,
					.Protocol   = 0x00,
					.Descriptor = "Interface0"
				};

				interface0.EndPoints.PushBack(
					{
						.Type            = USBInterfaceEndPointTypes::RX,
						.Address         = 0,
						.Handler         = &EndPointHandler_0_0,
						.Interval        = 0,
						.Attributes      = USBInterfaceEndPointAttributes::TransferType_Control,
						.MaxPacketSize   = 64,
						.DataBuffer      = &usb_dpram->ep0_buf_a[0],
						.BufferControl   = &usb_dpram->ep_buf_ctrl[0].in,
						.EndPointControl = nullptr
					}
				);

				interface0.EndPoints.PushBack(
					{
						.Type            = USBInterfaceEndPointTypes::TX,
						.Address         = 0,
						.Handler         = &EndPointHandler_0_1,
						.Interval        = 0,
						.Attributes      = USBInterfaceEndPointAttributes::TransferType_Control,
						.MaxPacketSize   = 64,
						.DataBuffer      = &usb_dpram->ep0_buf_a[0],
						.BufferControl   = &usb_dpram->ep_buf_ctrl[0].out,
						.EndPointControl = nullptr
					}
				);

				config.DeviceInterfaces.PushFront(
					Move(interface0)
				);
			}

			USB::config = Move(
				config
			);
		}
		// @throw AL::Exception
		static Void Open_InitUSBConfig(USBConfig& config, usb_device_context& context)
		{
			context.device =
			{
				.bLength            = sizeof(usb_device_descriptor),
				.bDescriptorType    = USB_DT_DEVICE,
				.bcdUSB             = 0x0110,
				.bDeviceClass       = 0,
				.bDeviceSubClass    = 0,
				.bDeviceProtocol    = 0,
				.bMaxPacketSize     = 64,
				.idVendor           = config.VendorId,
				.idProduct          = config.ProductId,
				.bcdDevice          = config.ProductVersion,
				.iManufacturer      = USB_DESCRIPTOR_VENDOR,
				.iProduct           = USB_DESCRIPTOR_PRODUCT,
				.iSerialNumber      = static_cast<uint8>((config.DeviceSerialNumber.GetLength() > 0) ? USB_DESCRIPTOR_DEVICE_SERIAL : 0),
				.bNumConfigurations = 1
			};

			size_t interfaceCount         = 0;
			size_t interfaceEndPointCount = 0;

			{
				size_t stringCount = USB_DESCRIPTOR_USER_DEFINED - 1;

				for (auto& interface : config.DeviceInterfaces)
				{
					++stringCount;
					++interfaceCount;

					for (auto& endPoint : interface.EndPoints)
					{
						++interfaceEndPointCount;
					}
				}

				context.strings.SetCapacity(
					stringCount
				);

				context.interfaces.SetCapacity(
					interfaceCount
				);
			}

			context.strings[USB_DESCRIPTOR_VENDOR - 1]        = config.Vendor.GetCString();
			context.strings[USB_DESCRIPTOR_PRODUCT - 1]       = config.Product.GetCString();
			context.strings[USB_DESCRIPTOR_DEVICE_SERIAL - 1] = config.DeviceSerialNumber.GetCString();

			{
				auto interfaces = config.DeviceInterfaces.begin();

				for (uint8 i = 0, j = (USB_DESCRIPTOR_USER_DEFINED - 1); i < config.DeviceInterfaces.GetSize(); ++i, ++interfaces)
				{
					auto& interface              = *interfaces;
					auto& interfaceEndPoints     = interface.EndPoints;
					auto  interfaceEndPointCount = interfaceEndPoints.GetSize();

					context.strings[j] = interfaces->Descriptor.GetCString();

					auto lpInterface = &context.interfaces[i];

					*lpInterface =
					{
						.lpConfig   = &(*interfaces),
						.descriptor =
						{
							.bLength            = sizeof(usb_interface_descriptor),
							.bDescriptorType    = USB_DT_INTERFACE,
							.bInterfaceNumber   = interfaces->Id,
							.bAlternateSetting  = 0,
							.bNumEndpoints      = static_cast<uint8>((interfaceEndPointCount <= 1) ? 0 : (interfaceEndPointCount - 1)),
							.bInterfaceClass    = interfaces->Class,
							.bInterfaceSubClass = interfaces->SubClass,
							.bInterfaceProtocol = interfaces->Protocol,
							.iInterface         = static_cast<uint8>(1 + j)
						}
					};

					lpInterface->endpoints.SetCapacity(
						interfaceEndPointCount
					);

					auto interfaceEndPoint = interfaceEndPoints.begin();

					for (uint8 l = 0; l < interfaceEndPointCount; ++l, ++interfaceEndPoint)
					{
						lpInterface->endpoints[l] =
						{
							.lpConfig   = &(*interfaceEndPoint),
							.handler    = interfaceEndPoint->Handler,
							.descriptor =
							{
								.bLength          = sizeof(usb_endpoint_descriptor),
								.bDescriptorType  = USB_DT_ENDPOINT,
								.bEndpointAddress = static_cast<uint8>(((interfaceEndPoint->Type == USBInterfaceEndPointTypes::RX) ? USB_DIR_IN : USB_DIR_OUT) | (interfaceEndPoint->Address & 0x0F)),
								.bmAttributes     = static_cast<uint8>(interfaceEndPoint->Attributes.Value),
								.wMaxPacketSize   = interfaceEndPoint->MaxPacketSize,
								.bInterval        = interfaceEndPoint->Interval
							},
							.data_buffer      = interfaceEndPoint->DataBuffer,
							.buffer_control   = interfaceEndPoint->BufferControl,
							.endpoint_control = interfaceEndPoint->EndPointControl
						};
					}
				}
			}

			context.config =
			{
				.descriptor =
				{
					.bLength             = sizeof(usb_configuration_descriptor),
					.bDescriptorType     = USB_DT_CONFIG,
					.wTotalLength        = static_cast<uint16>(sizeof(usb_configuration_descriptor) + (interfaceCount * sizeof(usb_interface_descriptor)) + (interfaceEndPointCount * sizeof(usb_endpoint_descriptor))),
					.bNumInterfaces      = static_cast<uint8>(interfaceCount),
					.bConfigurationValue = 1,
					.iConfiguration      = 0,
					.bmAttributes        = static_cast<uint8>(config.DeviceFlags.Value),
					.bMaxPower           = static_cast<uint8>(config.DeviceMaxPower / 2)
				}
			};
		}

		static Void Open_InitUSB(const USBConfig& config, const usb_device_context& context)
		{
			::reset_block(
				RESETS_RESET_USBCTRL_BITS
			);

			::unreset_block_wait(
				RESETS_RESET_USBCTRL_BITS
			);

			memset(
				usb_dpram,
				0,
				sizeof(*usb_dpram)
			);

			::irq_add_shared_handler(
				USBCTRL_IRQ,
				&USB::isr_usbctrl,
				PICO_SHARED_IRQ_HANDLER_HIGHEST_ORDER_PRIORITY
			);

			::irq_set_enabled(
				USBCTRL_IRQ,
				true
			);

			usb_hw->muxing = USB_USB_MUXING_TO_PHY_BITS | USB_USB_MUXING_SOFTCON_BITS;
			usb_hw->pwr = USB_USB_PWR_VBUS_DETECT_BITS | USB_USB_PWR_VBUS_DETECT_OVERRIDE_EN_BITS;
			usb_hw->main_ctrl = USB_MAIN_CTRL_CONTROLLER_EN_BITS;
			usb_hw->sie_ctrl = USB_SIE_CTRL_EP0_INT_1BUF_BITS;
			usb_hw->inte = USB_INTS_BUFF_STATUS_BITS | USB_INTS_BUS_RESET_BITS | USB_INTS_SETUP_REQ_BITS;

			for (auto& interface : context.interfaces)
			{
				for (auto& endPoint : interface.endpoints)
				{
					auto dpram_offset          = reinterpret_cast<uint32>(endPoint.data_buffer) ^ reinterpret_cast<uint32>(usb_dpram);
					*endPoint.endpoint_control = EP_CTRL_ENABLE_BITS | EP_CTRL_INTERRUPT_PER_BUFFER | (endPoint.descriptor.bmAttributes << EP_CTRL_BUFFER_TYPE_LSB) | dpram_offset;
				}
			}

			hw_set_alias(usb_hw)->sie_ctrl = USB_SIE_CTRL_PULLUP_EN_BITS;
		}

	private:
		static Void EndPointHandler_0_0(const Void* lpBuffer, uint16 size)
		{
			// LED::Write(False);

			if (isAddressPending)
			{
				usb_hw->dev_addr_ctrl = address;
				isAddressPending = False;
			}
			else
			{
				auto lpEndPoint = usb_get_endpoint_context(
					0,
					0,
					true
				);

				usb_start_transfer(
					lpEndPoint,
					nullptr,
					0
				);
			}
		}
		static Void EndPointHandler_0_1(const Void* lpBuffer, uint16 size)
		{
			// do nothing
		}

	private:
		static Void isr_usbctrl()
		{
			uint32 status  = usb_hw->ints;
			uint32 handled = 0;

			if (status & USB_INTS_SETUP_REQ_BITS)
			{
				handled |= USB_INTS_SETUP_REQ_BITS;
				hw_clear_alias(usb_hw)->sie_status = USB_SIE_STATUS_SETUP_REC_BITS;

				auto lpPacket = reinterpret_cast<volatile struct usb_setup_packet*>(
					&usb_dpram->setup_packet
				);

				usb_get_endpoint_context(0, 0, false)->next_pid = 1;

				switch (lpPacket->bmRequestType)
				{
					case USB_DIR_IN:
					{
						if (lpPacket->bRequest == USB_REQUEST_GET_DESCRIPTOR)
						{
							switch (lpPacket->wValue >> 8)
							{
								case USB_DT_DEVICE:
								{
									auto lpEndPoint = usb_get_endpoint_context(
										0,
										0,
										false
									);

									lpEndPoint->next_pid = 1;

									usb_start_transfer(
										lpEndPoint,
										reinterpret_cast<uint8*>(&context.device),
										sizeof(usb_device_descriptor)
									);
								}
								break;

								case USB_DT_CONFIG:
								{
									uint8 i                         = lpPacket->wValue & 0xFF;
									auto  lpBuffer                  = &buffer[0];
									auto  lpConfigurationDescriptor = &context.config;

									memcpy(
										lpBuffer,
										lpConfigurationDescriptor,
										sizeof(usb_configuration_descriptor)
									);

									lpBuffer += sizeof(usb_configuration_descriptor);

									if (lpPacket->wLength >= lpConfigurationDescriptor->descriptor.wTotalLength)
									{
										for (auto& interface : context.interfaces)
										{
											memcpy(
												lpBuffer,
												&interface.descriptor,
												sizeof(usb_interface_descriptor)
											);

											lpBuffer += sizeof(usb_interface_descriptor);

											for (auto& endPoint : interface.endpoints)
											{
												memcpy(
													lpBuffer,
													&endPoint.descriptor,
													sizeof(usb_endpoint_descriptor)
												);

												lpBuffer += sizeof(usb_endpoint_descriptor);
											}
										}
									}

									usb_start_transfer(
										usb_get_endpoint_context(0, 0, false),
										&buffer[0],
										lpBuffer - &buffer[0]
									);
								}
								break;

								case USB_DT_STRING:
								{
									uint8 i      = lpPacket->wValue & 0xFF;
									uint8 length = 0;

									if (i == 0)
									{
										usb_language_descriptor<1> descriptor =
										{
											.bLength         = sizeof(usb_language_descriptor<1>),
											.bDescriptorType = USB_DT_STRING,
											.wLANGID         =
											{
												0x0409 // en-us
											}
										};

										memcpy(
											&buffer[0],
											&descriptor,
											length = sizeof(descriptor)
										);
									}
									else if (i <= context.strings.GetSize())
									{
										auto lpBuffer     = &buffer[2];
										auto lpDescriptor = context.strings[i - 1];

										while (*lpDescriptor != '\0')
										{
											length     += 2;
											*lpBuffer++ = *lpDescriptor++;
											*lpBuffer++ = 0;
										}

										buffer[0] = length += 2;   // bLength
										buffer[1] = USB_DT_STRING; // bDescriptorType
									}

									usb_start_transfer(
										usb_get_endpoint_context(0, 0, false),
										&buffer[0],
										length
									);
								}
								break;
							}
						}
					}
					break;

					case USB_DIR_OUT:
					{
						switch (lpPacket->bRequest)
						{
							case USB_REQUEST_SET_ADDRESS:
							{
								address = lpPacket->wValue & 0xFF;

								isAddressPending = True;

								usb_start_transfer(usb_get_endpoint_context(0, 0, false), nullptr, 0);
							}
							break;

							case USB_REQUEST_SET_CONFIGURATION:
							{
								usb_start_transfer(usb_get_endpoint_context(0, 0, false), nullptr, 0);

								isConfigured = True;
							}
							break;

							default:
								usb_start_transfer(usb_get_endpoint_context(0, 0, false), nullptr, 0);
								break;
						}
					}
					break;
				}
			}

			if (status & USB_INTS_BUFF_STATUS_BITS)
			{
				handled |= USB_INTS_BUFF_STATUS_BITS;

				uint32 bit              = 1;
				uint32 buffers          = usb_hw->buf_status;
				uint32 remainingBuffers = buffers;

				for (uint32 i = 0; remainingBuffers && (i < (USB_NUM_ENDPOINTS * 2)); ++i)
				{
					if (remainingBuffers & bit)
					{
						hw_clear_alias(usb_hw)->buf_status = bit;

						uint8 address = (i >> 1) | (!(i & 1) ? USB_DIR_IN : 0);

						usb_call_endpoint_handler();

						remainingBuffers &= ~bit;
					}

					bit <<= 1;
				}
			}

			if (status & USB_INTS_BUS_RESET_BITS)
			{
				handled |= USB_INTS_BUS_RESET_BITS;
				hw_clear_alias(usb_hw)->sie_status = USB_SIE_STATUS_BUS_RESET_BITS;

				address               = 0;
				usb_hw->dev_addr_ctrl = 0;
				isConfigured          = False;
			}
		}

		static Bool usb_call_endpoint_handler()
		{
			for (auto& interface : context.interfaces)
			{
				for (auto& endPoint : interface.endpoints)
				{
					if ((endPoint.descriptor.bEndpointAddress == address) && (endPoint.handler != nullptr))
					{
						endPoint.handler(
							const_cast<uint8*>(endPoint.data_buffer),
							*endPoint.buffer_control & USB_BUF_CTRL_LEN_MASK
						);

						return True;
					}
				}
			}

			return False;
		}

		static usb_endpoint_context* usb_get_endpoint_context(uint8 interface, uint8 address, bool tx)
		{
			address = (tx ? USB_DIR_OUT : USB_DIR_IN) | (address & 0x0F);

			for (auto& _interface : context.interfaces)
			{
				if (_interface.lpConfig->Id != interface)
				{

					continue;
				}

				for (auto& endPoint : _interface.endpoints)
				{
					if (endPoint.descriptor.bEndpointAddress == address)
					{

						return &endPoint;
					}
				}
			}

			return nullptr;
		}

		static Void usb_start_transfer(usb_endpoint_context* lpEndPoint, uint8* lpBuffer, uint16 size)
		{
			if (size > lpEndPoint->lpConfig->MaxPacketSize)
			{

				size = lpEndPoint->lpConfig->MaxPacketSize;
			}

			uint32 value = size | USB_BUF_CTRL_AVAIL;

			if (lpEndPoint->descriptor.bEndpointAddress & USB_DIR_IN)
			{
				memcpy(
					const_cast<uint8*>(lpEndPoint->data_buffer),
					lpBuffer,
					size
				);

				value |= USB_BUF_CTRL_FULL;
			}

			value |= lpEndPoint->next_pid ? USB_BUF_CTRL_DATA1_PID : USB_BUF_CTRL_DATA0_PID;

			lpEndPoint->next_pid        ^= 1;
			*lpEndPoint->buffer_control  = value;
		}
	};
}
