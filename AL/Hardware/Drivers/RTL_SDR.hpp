#pragma once
#include "AL/Common.hpp"

#include "ISDR.hpp"

#if __has_include(<rtl-sdr.h>)
	#define AL_DEPENDENCY_RTL_SDR

	#include <rtl-sdr.h>

	#if defined(AL_COMPILER_MSVC)
		#pragma comment(lib, "rtlsdr_static.lib")
	#endif
#else
	typedef struct rtlsdr_dev rtlsdr_dev_t;
#endif

namespace AL::Hardware::Drivers
{
	struct RTL_SDR_DeviceInfo
	{
		String Name;
		String Serial;
		String Product;
		String Manufacturer;
	};

	class RTL_SDR
		: public ISDR
	{
		Bool            isAutoGainEnabled      = False;
		Bool            isTunerAutoGainEnabled = False;

		::rtlsdr_dev_t* device;
		uint32          deviceIndex;
		int32           tunerIFGain = 0;

	public:
		// @throw AL::Exception
		static uint32 GetDeviceCount()
		{
#if defined(AL_DEPENDENCY_RTL_SDR)
			auto value = ::rtlsdr_get_device_count();

			return static_cast<uint32>(
				value
			);
#else
			throw DependencyMissingException(
				"rtl-sdr.h"
			);
#endif
		}

		// @throw AL::Exception
		static Void   ReadDeviceInfo(uint32 index, RTL_SDR_DeviceInfo& deviceInfo)
		{
			AL_ASSERT(
				IsOpen(),
				"RTL_SDR not open"
			);

#if defined(AL_DEPENDENCY_RTL_SDR)
			auto name = ::rtlsdr_get_device_name(
				index
			);

			char serial[256]       = { 0 };
			char product[256]      = { 0 };
			char manufacturer[256] = { 0 };

			if (::rtlsdr_get_device_usb_strings(index, manufacturer, product, serial) != 0)
			{

				throw Exception(
					"Error calling 'rtlsdr_get_device_usb_strings'"
				);
			}

			deviceInfo.Name         = name;
			deviceInfo.Serial       = serial;
			deviceInfo.Product      = product;
			deviceInfo.Manufacturer = manufacturer;
#else
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		static Void   WriteDeviceInfo(uint32 index, const RTL_SDR_DeviceInfo& deviceInfo)
		{
			AL_ASSERT(
				IsOpen(),
				"RTL_SDR not open"
			);

#if defined(AL_DEPENDENCY_RTL_SDR)
			// TODO: implement
			throw NotImplementedException();
#else
			throw NotImplementedException();
#endif
		}

		RTL_SDR()
			: RTL_SDR(
				0
			)
		{
		}

		RTL_SDR(RTL_SDR&& rtl_sdr)
			: isAutoGainEnabled(
				rtl_sdr.isAutoGainEnabled
			),
			isTunerAutoGainEnabled(
				rtl_sdr.isTunerAutoGainEnabled
			),
			device(
				rtl_sdr.device
			),
			deviceIndex(
				rtl_sdr.deviceIndex
			),
			tunerIFGain(
				rtl_sdr.tunerIFGain
			)
		{
			rtl_sdr.isAutoGainEnabled      = False;
			rtl_sdr.isTunerAutoGainEnabled = False;

			rtl_sdr.device      = nullptr;
			rtl_sdr.deviceIndex = 0;
			rtl_sdr.tunerIFGain = 0;
		}

		explicit RTL_SDR(uint32 index)
			: device(
				nullptr
			),
			deviceIndex(
				index
			)
		{
		}

		virtual Bool IsOpen() const override
		{
			return device != nullptr;
		}

		auto GetIndex() const
		{
			return deviceIndex;
		}

		auto GetHandle() const
		{
#if defined(AL_DEPENDENCY_RTL_SDR)
			return device;
#endif
		}

		// @throw AL::Exception
		virtual Bool IsAutoGainEnabled() const override
		{
			AL_ASSERT(
				IsOpen(),
				"RTL_SDR not open"
			);

#if defined(AL_DEPENDENCY_RTL_SDR)
			return isAutoGainEnabled;
#else
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		virtual Bool IsOffsetTuningEnabled() const override
		{
			AL_ASSERT(
				IsOpen(),
				"RTL_SDR not open"
			);

#if defined(AL_DEPENDENCY_RTL_SDR)
			int value;

			if ((value = ::rtlsdr_get_offset_tuning(GetHandle())) == -1)
			{

				throw Exception(
					"Error calling 'rtlsdr_get_offset_tuning'"
				);
			}

			return value != 0;
#else
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		virtual Bool IsTunerAutoGainEnabled() const override
		{
			AL_ASSERT(
				IsOpen(),
				"RTL_SDR not open"
			);

#if defined(AL_DEPENDENCY_RTL_SDR)
			return isTunerAutoGainEnabled;
#else
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		virtual Bool IsDirectSamplingEnabled() const override
		{
			AL_ASSERT(
				IsOpen(),
				"RTL_SDR not open"
			);

#if defined(AL_DEPENDENCY_RTL_SDR)
			int value;

			if ((value = ::rtlsdr_get_direct_sampling(GetHandle())) == -1)
			{

				throw Exception(
					"Error calling 'rtlsdr_get_direct_sampling'"
				);
			}

			return value != 0;
#else
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		virtual int32 GetTunerGain() const override
		{
			AL_ASSERT(
				IsOpen(),
				"RTL_SDR not open"
			);

#if defined(AL_DEPENDENCY_RTL_SDR)
			int32 value;

			if ((value = ::rtlsdr_get_tuner_gain(GetHandle())) == 0)
			{

				throw Exception(
					"Error calling 'rtlsdr_get_tuner_gain'"
				);
			}

			return value;
#else
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		virtual int32 GetTunerIFGain() const override
		{
			AL_ASSERT(
				IsOpen(),
				"RTL_SDR not open"
			);

#if defined(AL_DEPENDENCY_RTL_SDR)
			return tunerIFGain;
#else
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		virtual uint32 GetSampleRate() const override
		{
			AL_ASSERT(
				IsOpen(),
				"RTL_SDR not open"
			);

#if defined(AL_DEPENDENCY_RTL_SDR)
			uint32 value;

			if ((value = ::rtlsdr_get_sample_rate(GetHandle())) == 0)
			{

				throw Exception(
					"Error calling 'rtlsdr_get_sample_rate'"
				);
			}

			return value;
#else
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		virtual uint64 GetCenterFrequency() const override
		{
			AL_ASSERT(
				IsOpen(),
				"RTL_SDR not open"
			);

#if defined(AL_DEPENDENCY_RTL_SDR)
			uint64 value;

			if ((value = ::rtlsdr_get_center_freq(GetHandle())) == 0)
			{

				throw Exception(
					"Error calling 'rtlsdr_get_center_freq'"
				);
			}

			return value;
#else
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		virtual int32 GetFrequencyCorrection() const override
		{
			AL_ASSERT(
				IsOpen(),
				"RTL_SDR not open"
			);

#if defined(AL_DEPENDENCY_RTL_SDR)
			int32 value;

			if ((value = ::rtlsdr_get_freq_correction(GetHandle())) == 0)
			{
			}

			return value;
#else
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		virtual Void GetSupportedTunerGains(Collections::Array<int32>& gains) const override
		{
			AL_ASSERT(
				IsOpen(),
				"RTL_SDR not open"
			);

#if defined(AL_DEPENDENCY_RTL_SDR)
			auto gainCount = ::rtlsdr_get_tuner_gains(
				GetHandle(),
				nullptr
			);

			if constexpr (Is_Type<int32, int>::Value)
			{
				gains.SetCapacity(
					static_cast<size_t>(gainCount)
				);

				if (::rtlsdr_get_tuner_gains(GetHandle(), &gains[0]) <= 0)
				{

					throw Exception(
						"Error calling 'rtlsdr_get_tuner_gains'"
					);
				}
			}
			else
			{
				auto lpGains = new int[gainCount];

				if (::rtlsdr_get_tuner_gains(GetHandle(), lpGains) <= 0)
				{
					delete[] lpGains;

					throw Exception(
						"Error calling 'rtlsdr_get_tuner_gains'"
					);
				}

				gains.SetCapacity(
					static_cast<size_t>(gainCount)
				);

				for (size_t i = 0; i < gainCount; ++i)
				{
					gains[i] = lpGains[i];
				}

				delete[] lpGains;
			}
#else
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		virtual Void GetSupportedTunerIFGains(Collections::Array<int32>& gains) const override
		{
			AL_ASSERT(
				IsOpen(),
				"RTL_SDR not open"
			);

#if defined(AL_DEPENDENCY_RTL_SDR)
			// TODO: implement
			throw NotImplementedException();
#else
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		virtual Void Open() override
		{
			AL_ASSERT(
				!IsOpen(),
				"RTL_SDR already open"
			);

#if defined(AL_DEPENDENCY_RTL_SDR)
			if (::rtlsdr_open(&device, static_cast<::uint32_t>(GetIndex())) != 0)
			{

				throw Exception(
					"Error calling 'rtlsdr_open'"
				);
			}
#else
			throw DependencyMissingException(
				"rtl-sdr.h"
			);
#endif
		}

		virtual Void Close() override
		{
			if (IsOpen())
			{
#if defined(AL_DEPENDENCY_RTL_SDR)
				::rtlsdr_close(
					device
				);
#endif

				device = nullptr;
			}
		}

		// @throw AL::Exception
		virtual Void SetTunerGain(int32 value) override
		{
			AL_ASSERT(
				IsOpen(),
				"RTL_SDR not open"
			);

#if defined(AL_DEPENDENCY_RTL_SDR)
			if (::rtlsdr_set_tuner_gain_mode(GetHandle(), 1) != 0)
			{

				throw Exception(
					"Error calling 'rtlsdr_set_tuner_gain_mode'"
				);
			}

			isAutoGainEnabled = False;

			if (::rtlsdr_set_tuner_gain(GetHandle(), static_cast<int>(value)) != 0)
			{

				throw Exception(
					"Error calling 'rtlsdr_set_tuner_gain'"
				);
			}
#else
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		virtual Void SetTunerIFGain(uint8 stage, int32 value) override
		{
			AL_ASSERT(
				IsOpen(),
				"RTL_SDR not open"
			);

#if defined(AL_DEPENDENCY_RTL_SDR)
			if (::rtlsdr_set_tuner_gain_mode(GetHandle(), 1) != 0)
			{

				throw Exception(
					"Error calling 'rtlsdr_set_tuner_gain_mode'"
				);
			}

			if (::rtlsdr_set_tuner_if_gain(GetHandle(), static_cast<int>(stage), static_cast<int>(value)) != 0)
			{

				throw Exception(
					"Error calling 'rtlsdr_set_tuner_if_gain'"
				);
			}

			isTunerAutoGainEnabled = False;
			tunerIFGain            = value;
#else
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		virtual Void SetSampleRate(uint32 value) override
		{
			AL_ASSERT(
				IsOpen(),
				"RTL_SDR not open"
			);

#if defined(AL_DEPENDENCY_RTL_SDR)
			if (::rtlsdr_set_sample_rate(GetHandle(), static_cast<::uint32_t>(value)) != 0)
			{

				throw Exception(
					"Error calling 'rtlsdr_set_sample_rate'"
				);
			}
#else
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		virtual Void SetCenterFrequency(uint64 value) override
		{
			AL_ASSERT(
				IsOpen(),
				"RTL_SDR not open"
			);

#if defined(AL_DEPENDENCY_RTL_SDR)
			if (::rtlsdr_set_center_freq(GetHandle(), static_cast<::uint32_t>(value & Integer<::uint32_t>::UnsignedCastMask)) != 0)
			{

				throw Exception(
					"Error calling 'rtlsdr_set_center_freq'"
				);
			}
#else
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		virtual Void SetFrequencyCorrection(int32 value) override
		{
			AL_ASSERT(
				IsOpen(),
				"RTL_SDR not open"
			);

#if defined(AL_DEPENDENCY_RTL_SDR)
			if (::rtlsdr_set_freq_correction(GetHandle(), static_cast<int>(value)))
			{

				throw Exception(
					"Error calling 'rtlsdr_set_freq_correction'"
				);
			}
#else
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		virtual Void EnableAutoGain(Bool set) override
		{
			AL_ASSERT(
				IsOpen(),
				"RTL_SDR not open"
			);

#if defined(AL_DEPENDENCY_RTL_SDR)
			if (::rtlsdr_set_agc_mode(GetHandle(), set ? 1 : 0) != 0)
			{

				throw Exception(
					"Error calling 'rtlsdr_set_agc_mode'"
				);
			}

			isAutoGainEnabled = set;
#else
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		virtual Void EnableOffsetTuning(Bool set) override
		{
			AL_ASSERT(
				IsOpen(),
				"RTL_SDR not open"
			);

#if defined(AL_DEPENDENCY_RTL_SDR)
			if (::rtlsdr_set_offset_tuning(GetHandle(), set ? 1 : 0) != 0)
			{

				throw Exception(
					"Error calling 'rtlsdr_set_offset_tuning'"
				);
			}
#else
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		virtual Void EnableTunerAutoGain(Bool set) override
		{
			AL_ASSERT(
				IsOpen(),
				"RTL_SDR not open"
			);

#if defined(AL_DEPENDENCY_RTL_SDR)
			if (::rtlsdr_set_tuner_gain_mode(GetHandle(), set ? 0 : 1) != 0)
			{

				throw Exception(
					"Error calling 'rtlsdr_set_tuner_gain_mode'"
				);
			}

			isTunerAutoGainEnabled = set;
#else
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		virtual Void EnableDirectSampling(Bool set) override
		{
			AL_ASSERT(
				IsOpen(),
				"RTL_SDR not open"
			);

#if defined(AL_DEPENDENCY_RTL_SDR)
			if (::rtlsdr_set_direct_sampling(GetHandle(), set ? 2 : 0) != 0)
			{

				throw Exception(
					"Error calling 'rtlsdr_set_direct_sampling'"
				);
			}
#else
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		virtual Void Reset() override
		{
			AL_ASSERT(
				IsOpen(),
				"RTL_SDR not open"
			);

#if defined(AL_DEPENDENCY_RTL_SDR)
			if (::rtlsdr_reset_buffer(GetHandle()) != 0)
			{

				throw Exception(
					"Error calling 'rtlsdr_reset_buffer'"
				);
			}
#else
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		// @return Number of bytes read
		virtual size_t Read(Void* lpBuffer, size_t size) override
		{
			AL_ASSERT(
				IsOpen(),
				"RTL_SDR not open"
			);

#if defined(AL_DEPENDENCY_RTL_SDR)
			int numberOfBytesRead;

			if (::rtlsdr_read_sync(GetHandle(), lpBuffer, static_cast<int>(size & Integer<int>::SignedCastMask), &numberOfBytesRead) <= 0)
			{

				throw Exception(
					"Error calling 'rtlsdr_read_sync'"
				);
			}

			return static_cast<size_t>(
				numberOfBytesRead
			);
#else
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		virtual Void ReadAsync(Void* lpBuffer, size_t size, SDRAsyncReadCallback&& callback) override
		{
			AL_ASSERT(
				IsOpen(),
				"RTL_SDR not open"
			);

#if defined(AL_DEPENDENCY_RTL_SDR)
			auto lpContext = new ReadAsync_Context
			{
				.lpSDR = this,
				.Callback = Move(callback)
			};

			if (::rtlsdr_read_async(GetHandle(), &ReadAsync_Detour, lpContext, 0, 0) != 0)
			{
				callback = Move(
					lpContext->Callback
				);

				delete lpContext;

				throw Exception(
					"Error calling 'rtlsdr_read_async'"
				);
			}
#else
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		virtual Void CancelAsync() override
		{
			AL_ASSERT(
				IsOpen(),
				"RTL_SDR not open"
			);

#if defined(AL_DEPENDENCY_RTL_SDR)
			if (::rtlsdr_cancel_async(GetHandle()) != 0)
			{

				throw Exception(
					"Error calling 'rtlsdr_cancel_async'"
				);
			}
#else
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		virtual Void ReadEEPROM(uint8 offset, Void* lpBuffer, uint16 size)
		{
			AL_ASSERT(
				IsOpen(),
				"RTL_SDR not open"
			);

#if defined(AL_DEPENDENCY_RTL_SDR)
			switch (::rtlsdr_read_eeprom(GetHandle(), reinterpret_cast<::uint8_t*>(lpBuffer), static_cast<::uint8_t>(offset), static_cast<::uint16_t>(size)))
			{
				case -1: throw Exception("Error calling 'rtlsdr_write_eeprom': Device handle is invalid");
				case -2: throw Exception("Error calling 'rtlsdr_write_eeprom': EEPROM size exceeded");
				case -3: throw Exception("Error calling 'rtlsdr_write_eeprom': No EEPROM found");
			}
#else
			throw NotImplementedException();
#endif
		}

		// @throw AL::Exception
		virtual Void WriteEEPROM(uint8 offset, const Void* lpBuffer, uint16 size)
		{
			AL_ASSERT(
				IsOpen(),
				"RTL_SDR not open"
			);

#if defined(AL_DEPENDENCY_RTL_SDR)
			switch (::rtlsdr_write_eeprom(GetHandle(), reinterpret_cast<::uint8_t*>(const_cast<Void*>(lpBuffer)), static_cast<::uint8_t>(offset), static_cast<::uint16_t>(size)))
			{
				case -1: throw Exception("Error calling 'rtlsdr_write_eeprom': Device handle is invalid");
				case -2: throw Exception("Error calling 'rtlsdr_write_eeprom': EEPROM size exceeded");
				case -3: throw Exception("Error calling 'rtlsdr_write_eeprom': No EEPROM found");
			}
#else
			throw NotImplementedException();
#endif
		}

		RTL_SDR& operator = (RTL_SDR&& rtl_sdr)
		{
#if defined(AL_DEPENDENCY_RTL_SDR)
			::rtlsdr_close(
				device
			);
#endif

			isAutoGainEnabled = rtl_sdr.isAutoGainEnabled;
			rtl_sdr.isAutoGainEnabled = False;

			isTunerAutoGainEnabled = rtl_sdr.isTunerAutoGainEnabled;
			rtl_sdr.isTunerAutoGainEnabled = False;

			device = rtl_sdr.device;
			rtl_sdr.device = nullptr;

			deviceIndex = rtl_sdr.deviceIndex;
			rtl_sdr.deviceIndex = 0;

			tunerIFGain = rtl_sdr.tunerIFGain;
			rtl_sdr.tunerIFGain = 0;

			return *this;
		}

		Bool operator == (const RTL_SDR& rtl_sdr) const
		{
			if (device != rtl_sdr.device)
			{

				return False;
			}

			return True;
		}
		Bool operator != (const RTL_SDR& rtl_sdr) const
		{
			if (operator==(rtl_sdr))
			{

				return False;
			}

			return True;
		}

	private:
#if defined(AL_DEPENDENCY_RTL_SDR)
		struct ReadAsync_Context
		{
			RTL_SDR*             lpSDR;
			SDRAsyncReadCallback Callback;
		};

		static Void ReadAsync_Detour(unsigned char* buf, uint32_t len, void* ctx)
		{
			auto lpContext = reinterpret_cast<ReadAsync_Context*>(
				ctx
			);

			lpContext->Callback(
				buf,
				len
			);

			delete lpContext;
		}
#endif
	};
}
