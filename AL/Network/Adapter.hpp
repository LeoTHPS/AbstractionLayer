#pragma once
#include "AL/Common.hpp"

#include "IPAddress.hpp"
#include "IPEndPoint.hpp"

#include "AL/OS/ErrorCode.hpp"
#include "AL/OS/SystemException.hpp"

#include "AL/Collections/Array.hpp"

#if defined(AL_PLATFORM_PICO)
	#warning Platform not supported
#elif defined(AL_PLATFORM_LINUX)

#elif defined(AL_PLATFORM_WINDOWS)
	#include "WinSock.hpp"

	#include <iphlpapi.h>

	#if defined(AL_COMPILER_MSVC)
		#pragma comment(lib, "Iphlpapi.lib")
	#endif
#endif

namespace AL::Network
{
	struct Adapter;

	// @throw AL::Exception
	// @return AL::False to stop enumerating
	typedef Function<Bool(const Adapter& adapter)> AdapterEnumCallback;

	enum class AdapterFlags : uint16
	{
#if defined(AL_PLATFORM_LINUX)
		// TODO: implement
#elif defined(AL_PLATFORM_WINDOWS)
		// Dynamic DNS is enabled on this adapter.
		DDNS_ENABLED               = IP_ADAPTER_DDNS_ENABLED,
		// Register the DNS suffix for this adapter.
		REGISTER_ADAPTER_SUFFIX    = IP_ADAPTER_REGISTER_ADAPTER_SUFFIX,
		// The Dynamic Host Configuration Protocol (DHCP) is enabled on this adapter.
		DHCP_ENABLED               = IP_ADAPTER_DHCP_ENABLED,
		// The adapter is a receive-only adapter.
		RECEIVE_ONLY               = IP_ADAPTER_RECEIVE_ONLY,
		// The adapter is not a multicast recipient.
		NO_MULTICAST               = IP_ADAPTER_NO_MULTICAST,
		// The adapter contains other IPv6-specific stateful configuration information.
		IPV6_OTHER_STATEFUL_CONFIG = IP_ADAPTER_IPV6_OTHER_STATEFUL_CONFIG,
		// The adapter is enabled for NetBIOS over TCP/IP.
		NETBIOS_OVER_TCPIP_ENABLED = IP_ADAPTER_NETBIOS_OVER_TCPIP_ENABLED,
		// The adapter is enabled for IPv4.
		IPV4_ENABLED               = IP_ADAPTER_IPV4_ENABLED,
		// The adapter is enabled for IPv6.
		IPV6_ENABLED               = IP_ADAPTER_IPV6_ENABLED,
		// The adapter is enabled for IPv6 managed address configuration.
		IPV6_MANAGE_ADDRESS_CONFIG = IP_ADAPTER_IPV6_MANAGE_ADDRESS_CONFIG
#endif
	};

	AL_DEFINE_ENUM_FLAG_OPERATORS(AdapterFlags);

	enum class AdapterTypes : uint8
	{
#if defined(AL_PLATFORM_LINUX)
		// TODO: implement
#elif defined(AL_PLATFORM_WINDOWS)
		OTHER                   = IF_TYPE_OTHER,
		REGULAR_1822            = IF_TYPE_REGULAR_1822,
		HDH_1822                = IF_TYPE_HDH_1822,
		DDN_X25                 = IF_TYPE_DDN_X25,
		RFC877_X25              = IF_TYPE_RFC877_X25,
		ETHERNET_CSMACD         = IF_TYPE_ETHERNET_CSMACD,
		IS088023_CSMACD         = IF_TYPE_IS088023_CSMACD,
		ISO88024_TOKENBUS       = IF_TYPE_ISO88024_TOKENBUS,
		ISO88025_TOKENRING      = IF_TYPE_ISO88025_TOKENRING,
		ISO88026_MAN            = IF_TYPE_ISO88026_MAN,
		STARLAN                 = IF_TYPE_STARLAN,
		PROTEON_10MBIT          = IF_TYPE_PROTEON_10MBIT,
		PROTEON_80MBIT          = IF_TYPE_PROTEON_80MBIT,
		HYPERCHANNEL            = IF_TYPE_HYPERCHANNEL,
		FDDI                    = IF_TYPE_FDDI,
		LAP_B                   = IF_TYPE_LAP_B,
		SDLC                    = IF_TYPE_SDLC,
		DS1                     = IF_TYPE_DS1,
		E1                      = IF_TYPE_E1,
		BASIC_ISDN              = IF_TYPE_BASIC_ISDN,
		PRIMARY_ISDN            = IF_TYPE_PRIMARY_ISDN,
		PROP_POINT2POINT_SERIAL = IF_TYPE_PROP_POINT2POINT_SERIAL,
		PPP                     = IF_TYPE_PPP,
		SOFTWARE_LOOPBACK       = IF_TYPE_SOFTWARE_LOOPBACK,
		EON                     = IF_TYPE_EON,
		ETHERNET_3MBIT          = IF_TYPE_ETHERNET_3MBIT,
		NSIP                    = IF_TYPE_NSIP,
		SLIP                    = IF_TYPE_SLIP,
		ULTRA                   = IF_TYPE_ULTRA,
		DS3                     = IF_TYPE_DS3,
		SIP                     = IF_TYPE_SIP,
		FRAMERELAY              = IF_TYPE_FRAMERELAY,
		RS232                   = IF_TYPE_RS232,
		PARA                    = IF_TYPE_PARA,
		ARCNET                  = IF_TYPE_ARCNET,
		ARCNET_PLUS             = IF_TYPE_ARCNET_PLUS,
		ATM                     = IF_TYPE_ATM,
		MIO_X25                 = IF_TYPE_MIO_X25,
		SONET                   = IF_TYPE_SONET,
		X25_PLE                 = IF_TYPE_X25_PLE,
		ISO88022_LLC            = IF_TYPE_ISO88022_LLC,
		LOCALTALK               = IF_TYPE_LOCALTALK,
		SMDS_DXI                = IF_TYPE_SMDS_DXI,
		FRAMERELAY_SERVICE      = IF_TYPE_FRAMERELAY_SERVICE,
		V35                     = IF_TYPE_V35,
		HSSI                    = IF_TYPE_HSSI,
		HIPPI                   = IF_TYPE_HIPPI,
		MODEM                   = IF_TYPE_MODEM,
		AAL5                    = IF_TYPE_AAL5,
		SONET_PATH              = IF_TYPE_SONET_PATH,
		SONET_VT                = IF_TYPE_SONET_VT,
		SMDS_ICIP               = IF_TYPE_SMDS_ICIP,
		PROP_VIRTUAL            = IF_TYPE_PROP_VIRTUAL,
		PROP_MULTIPLEXOR        = IF_TYPE_PROP_MULTIPLEXOR,
		IEEE80212               = IF_TYPE_IEEE80212,
		FIBRECHANNEL            = IF_TYPE_FIBRECHANNEL,
		HIPPIINTERFACE          = IF_TYPE_HIPPIINTERFACE,
		FRAMERELAY_INTERCONNECT = IF_TYPE_FRAMERELAY_INTERCONNECT,
		AFLANE_8023             = IF_TYPE_AFLANE_8023,
		AFLANE_8025             = IF_TYPE_AFLANE_8025,
		CCTEMUL                 = IF_TYPE_CCTEMUL,
		FASTETHER               = IF_TYPE_FASTETHER,
		ISDN                    = IF_TYPE_ISDN,
		V11                     = IF_TYPE_V11,
		V36                     = IF_TYPE_V36,
		G703_64K                = IF_TYPE_G703_64K,
		G703_2MB                = IF_TYPE_G703_2MB,
		QLLC                    = IF_TYPE_QLLC,
		FASTETHER_FX            = IF_TYPE_FASTETHER_FX,
		CHANNEL                 = IF_TYPE_CHANNEL,
		IEEE80211               = IF_TYPE_IEEE80211,
		IBM370PARCHAN           = IF_TYPE_IBM370PARCHAN,
		ESCON                   = IF_TYPE_ESCON,
		DLSW                    = IF_TYPE_DLSW,
		ISDN_S                  = IF_TYPE_ISDN_S,
		ISDN_U                  = IF_TYPE_ISDN_U,
		LAP_D                   = IF_TYPE_LAP_D,
		IPSWITCH                = IF_TYPE_IPSWITCH,
		RSRB                    = IF_TYPE_RSRB,
		ATM_LOGICAL             = IF_TYPE_ATM_LOGICAL,
		DS0                     = IF_TYPE_DS0,
		DS0_BUNDLE              = IF_TYPE_DS0_BUNDLE,
		BSC                     = IF_TYPE_BSC,
		ASYNC                   = IF_TYPE_ASYNC,
		CNR                     = IF_TYPE_CNR,
		ISO88025R_DTR           = IF_TYPE_ISO88025R_DTR,
		EPLRS                   = IF_TYPE_EPLRS,
		ARAP                    = IF_TYPE_ARAP,
		PROP_CNLS               = IF_TYPE_PROP_CNLS,
		HOSTPAD                 = IF_TYPE_HOSTPAD,
		TERMPAD                 = IF_TYPE_TERMPAD,
		FRAMERELAY_MPI          = IF_TYPE_FRAMERELAY_MPI,
		X213                    = IF_TYPE_X213,
		ADSL                    = IF_TYPE_ADSL,
		RADSL                   = IF_TYPE_RADSL,
		SDSL                    = IF_TYPE_SDSL,
		VDSL                    = IF_TYPE_VDSL,
		ISO88025_CRFPRINT       = IF_TYPE_ISO88025_CRFPRINT,
		MYRINET                 = IF_TYPE_MYRINET,
		VOICE_EM                = IF_TYPE_VOICE_EM,
		VOICE_FXO               = IF_TYPE_VOICE_FXO,
		VOICE_FXS               = IF_TYPE_VOICE_FXS,
		VOICE_ENCAP             = IF_TYPE_VOICE_ENCAP,
		VOICE_OVERIP            = IF_TYPE_VOICE_OVERIP,
		ATM_DXI                 = IF_TYPE_ATM_DXI,
		ATM_FUNI                = IF_TYPE_ATM_FUNI,
		ATM_IMA                 = IF_TYPE_ATM_IMA,
		PPPMULTILINKBUNDLE      = IF_TYPE_PPPMULTILINKBUNDLE,
		IPOVER_CDLC             = IF_TYPE_IPOVER_CDLC,
		IPOVER_CLAW             = IF_TYPE_IPOVER_CLAW,
		STACKTOSTACK            = IF_TYPE_STACKTOSTACK,
		VIRTUALIPADDRESS        = IF_TYPE_VIRTUALIPADDRESS,
		MPC                     = IF_TYPE_MPC,
		IPOVER_ATM              = IF_TYPE_IPOVER_ATM,
		ISO88025_FIBER          = IF_TYPE_ISO88025_FIBER,
		TDLC                    = IF_TYPE_TDLC,
		GIGABITETHERNET         = IF_TYPE_GIGABITETHERNET,
		HDLC                    = IF_TYPE_HDLC,
		LAP_F                   = IF_TYPE_LAP_F,
		V37                     = IF_TYPE_V37,
		X25_MLP                 = IF_TYPE_X25_MLP,
		X25_HUNTGROUP           = IF_TYPE_X25_HUNTGROUP,
		TRANSPHDLC              = IF_TYPE_TRANSPHDLC,
		INTERLEAVE              = IF_TYPE_INTERLEAVE,
		FAST                    = IF_TYPE_FAST,
		IP                      = IF_TYPE_IP,
		DOCSCABLE_MACLAYER      = IF_TYPE_DOCSCABLE_MACLAYER,
		DOCSCABLE_DOWNSTREAM    = IF_TYPE_DOCSCABLE_DOWNSTREAM,
		DOCSCABLE_UPSTREAM      = IF_TYPE_DOCSCABLE_UPSTREAM,
		A12MPPSWITCH            = IF_TYPE_A12MPPSWITCH,
		TUNNEL                  = IF_TYPE_TUNNEL,
		COFFEE                  = IF_TYPE_COFFEE,
		CES                     = IF_TYPE_CES,
		ATM_SUBINTERFACE        = IF_TYPE_ATM_SUBINTERFACE,
		L2_VLAN                 = IF_TYPE_L2_VLAN,
		L3_IPVLAN               = IF_TYPE_L3_IPVLAN,
		L3_IPXVLAN              = IF_TYPE_L3_IPXVLAN,
		DIGITALPOWERLINE        = IF_TYPE_DIGITALPOWERLINE,
		MEDIAMAILOVERIP         = IF_TYPE_MEDIAMAILOVERIP,
		DTM                     = IF_TYPE_DTM,
		DCN                     = IF_TYPE_DCN,
		IPFORWARD               = IF_TYPE_IPFORWARD,
		MSDSL                   = IF_TYPE_MSDSL,
		IEEE1394                = IF_TYPE_IEEE1394,
		RECEIVE_ONLY            = IF_TYPE_RECEIVE_ONLY
#endif
	};

	enum class AdapterStatus : uint8
	{
#if defined(AL_PLATFORM_LINUX)
		// TODO: implement
#elif defined(AL_PLATFORM_WINDOWS)
		Up             = ::IfOperStatusUp,
		Down           = ::IfOperStatusDown,
		Testing        = ::IfOperStatusTesting,
		Unknown        = ::IfOperStatusUnknown,
		Dormant        = ::IfOperStatusDormant,
		NotPresent     = ::IfOperStatusNotPresent,
		LowerLayerDown = ::IfOperStatusLowerLayerDown
#endif
	};

	typedef Collections::Array<IPAddress> AdapterAddresses;

	struct Adapter
	{
		String                Name;
		String                Description;

		String                MAC;
		uint32                MTU;
		AdapterTypes          Type;
		BitMask<AdapterFlags> Flags;
		AdapterStatus         Status;

		AdapterAddresses      DNS;
		AdapterAddresses      Gateway;
		AdapterAddresses      Addresses;

		struct
		{
			struct
			{
				IPAddress IPv4;
				IPAddress IPv6;
			} Server;
		} DHCP;

		struct
		{
			uint64 RX;
			uint64 TX;
		} LinkSpeed;

		// @throw AL::Exception
		static Void Enumerate(const AdapterEnumCallback& callback)
		{
			Enumerate(
				callback,
				AddressFamilies::NotSpecified
			);
		}
		// @throw AL::Exception
		static Void Enumerate(const AdapterEnumCallback& callback, AddressFamilies addressFamily)
		{
#if defined(AL_PLATFORM_LINUX)
			// TODO: implement
			throw NotImplementedException();
#elif defined(AL_PLATFORM_WINDOWS)
			::ULONG                   errorCode;
			Collections::Array<uint8> addressBuffer(0);
			::ULONG                   addressBufferSize = 0;

			static constexpr ::ULONG FLAGS = GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_INCLUDE_GATEWAYS | GAA_FLAG_INCLUDE_TUNNEL_BINDINGORDER;

			if ((errorCode = ::GetAdaptersAddresses(static_cast<::ULONG>(addressFamily), FLAGS, nullptr, nullptr, &addressBufferSize)) != ERROR_SUCCESS)
			{
				if (errorCode == ERROR_NO_DATA)
				{

					return;
				}

				if (errorCode != ERROR_BUFFER_OVERFLOW)
				{

					throw OS::SystemException(
						"GetAdaptersAddresses",
						errorCode
					);
				}

				addressBuffer.SetCapacity(
					addressBufferSize
				);
			}

			if ((errorCode = ::GetAdaptersAddresses(static_cast<::ULONG>(addressFamily), FLAGS, nullptr, reinterpret_cast<::PIP_ADAPTER_ADDRESSES>(&addressBuffer[0]), &addressBufferSize)) != ERROR_SUCCESS)
			{

				throw OS::SystemException(
					"GetAdaptersAddresses",
					errorCode
				);
			}

			auto GetAdapterAddresses = [](auto _lpAddress)->AdapterAddresses
			{
				size_t count = 0;

				for (auto lpAddress = _lpAddress; lpAddress != nullptr; lpAddress = lpAddress->Next)
				{
					++count;
				}

				AdapterAddresses addresses(
					count
				);

				for (size_t i = 0; i < count; ++i, _lpAddress = _lpAddress->Next)
				{
					switch (_lpAddress->Address.lpSockaddr->sa_family)
					{
						case AF_INET:
							addresses[i] = reinterpret_cast<const ::sockaddr_in*>(_lpAddress->Address.lpSockaddr)->sin_addr;
							break;

						case AF_INET6:
							addresses[i] = reinterpret_cast<const ::sockaddr_in6*>(_lpAddress->Address.lpSockaddr)->sin6_addr;
							break;

						default:
							throw NotImplementedException();
					}
				}

				return addresses;
			};

			for (auto lpAddress = reinterpret_cast<const ::PIP_ADAPTER_ADDRESSES>(&addressBuffer[0]); lpAddress != nullptr; lpAddress = lpAddress->Next)
			{
				Adapter adapter =
				{
					.Name          = lpAddress->AdapterName,
					.Description  = String::Format("%S", lpAddress->Description),

					.MAC          = HexConverter::Encode(&lpAddress->PhysicalAddress[0], lpAddress->PhysicalAddressLength),
					.MTU          = lpAddress->Mtu,
					.Type         = static_cast<AdapterTypes>(lpAddress->IfType),
					.Flags        = static_cast<AdapterFlags>(lpAddress->Flags),
					.Status       = static_cast<AdapterStatus>(lpAddress->OperStatus),

					.DNS          = GetAdapterAddresses(lpAddress->FirstDnsServerAddress),
					.Gateway      = GetAdapterAddresses(lpAddress->FirstGatewayAddress),
					.Addresses    = GetAdapterAddresses(lpAddress->FirstPrefix),

					.DHCP =
					{
						.Server =
						{
							.IPv4 = (lpAddress->Dhcpv4Enabled && (lpAddress->Dhcpv4Server.lpSockaddr != nullptr)) ? reinterpret_cast<const ::sockaddr_in*>(lpAddress->Dhcpv4Server.lpSockaddr)->sin_addr : IPAddress(),
							.IPv6 = reinterpret_cast<const ::sockaddr_in6&>(lpAddress->Dhcpv6Server).sin6_addr
						}
					},

					.LinkSpeed =
					{
						.RX       = lpAddress->ReceiveLinkSpeed,
						.TX       = lpAddress->TransmitLinkSpeed
					}
				};

				if (!callback(adapter))
				{

					break;
				}
			}
#endif
		}
	};
}
