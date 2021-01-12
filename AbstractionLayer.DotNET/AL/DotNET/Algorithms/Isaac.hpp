#pragma once
#include "AL/DotNET/Common.hpp"

#include <AL/Algorithms/Isaac.hpp>

namespace AL::DotNET::Algorithms
{
	public ref class Isaac
	{
		AL::Algorithms::Isaac* const lpIsaac;

	public:
		Isaac()
			: lpIsaac(
				new AL::Algorithms::Isaac()
			)
		{
		}

		explicit Isaac(System::UInt32 seed)
			: lpIsaac(
				new AL::Algorithms::Isaac(
					seed
				)
			)
		{
		}

		virtual ~Isaac()
		{
			delete lpIsaac;
		}

		System::UInt32 Next()
		{
			return lpIsaac->Next();
		}
		System::UInt32 Next(System::UInt32 min, System::UInt32 max)
		{
			return lpIsaac->Next(
				min,
				max
			);
		}
	};

	public ref class Isaac64
	{
		AL::Algorithms::Isaac64* const lpIsaac;

	public:
		Isaac64()
			: lpIsaac(
				new AL::Algorithms::Isaac64()
			)
		{
		}

		explicit Isaac64(System::UInt32 seed)
			: lpIsaac(
				new AL::Algorithms::Isaac64(
					seed
				)
			)
		{
		}

		virtual ~Isaac64()
		{
			delete lpIsaac;
		}

		System::UInt64 Next()
		{
			return lpIsaac->Next();
		}
		System::UInt64 Next(System::UInt64 min, System::UInt64 max)
		{
			return lpIsaac->Next(
				min,
				max
			);
		}
	};
}
