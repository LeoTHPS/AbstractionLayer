#pragma once
#include "AL/Common.hpp"

#include "IDriver.hpp"

#include "AL/Collections/Array.hpp"

namespace AL::Hardware::Drivers
{
	// @throw AL::Exception
	typedef Function<Void(Void* lpBuffer, size_t size)> SDRAsyncReadCallback;

	class ISDR
		: public IDriver<Void, Void, Void>
	{
	public:
		// @throw AL::Exception
		virtual Bool IsAutoGainEnabled() const = 0;

		// @throw AL::Exception
		virtual Bool IsOffsetTuningEnabled() const = 0;

		// @throw AL::Exception
		virtual Bool IsTunerAutoGainEnabled() const = 0;

		// @throw AL::Exception
		virtual Bool IsDirectSamplingEnabled() const = 0;

		// @throw AL::Exception
		virtual int32 GetTunerGain() const = 0;

		// @throw AL::Exception
		virtual int32 GetTunerIFGain() const = 0;

		// @throw AL::Exception
		virtual uint32 GetSampleRate() const = 0;

		// @throw AL::Exception
		virtual uint64 GetCenterFrequency() const = 0;

		// @throw AL::Exception
		virtual int32 GetFrequencyCorrection() const = 0;

		// @throw AL::Exception
		virtual Void GetSupportedTunerGains(Collections::Array<int32>& gains) const = 0;

		// @throw AL::Exception
		virtual Void GetSupportedTunerIFGains(Collections::Array<int32>& gains) const = 0;

		// @throw AL::Exception
		virtual Void SetTunerGain(int32 value) = 0;

		// @throw AL::Exception
		virtual Void SetTunerIFGain(uint8 stage, int32 value) = 0;

		// @throw AL::Exception
		virtual Void SetSampleRate(uint32 value) = 0;

		// @throw AL::Exception
		virtual Void SetCenterFrequency(uint64 value) = 0;

		// @throw AL::Exception
		virtual Void SetFrequencyCorrection(int32 value) = 0;

		// @throw AL::Exception
		virtual Void EnableAutoGain(Bool set) = 0;

		// @throw AL::Exception
		virtual Void EnableOffsetTuning(Bool set) = 0;

		// @throw AL::Exception
		virtual Void EnableTunerAutoGain(Bool set) = 0;

		// @throw AL::Exception
		virtual Void EnableDirectSampling(Bool set) = 0;

		// @throw AL::Exception
		virtual Void Reset() = 0;

		// @throw AL::Exception
		// @return Number of bytes read
		virtual size_t Read(Void* lpBuffer, size_t size) = 0;

		// @throw AL::Exception
		virtual Void ReadAsync(Void* lpBuffer, size_t size, SDRAsyncReadCallback&& callback) = 0;

		// @throw AL::Exception
		virtual Void CancelAsync() = 0;
	};
}
