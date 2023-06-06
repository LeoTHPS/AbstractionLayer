#pragma once
#include "AL/Common.hpp"

#include "ProductKey.hpp"

#include "AL/OS/System.hpp"

#include "AL/Algorithms/Isaac.hpp"

namespace AL::Game::Security
{
	struct ProductKeyGeneratorAlgorithm
	{
		static constexpr uint32 Next(uint64 seed)
		{
			return (seed % 0x478921) ^ 0x67033;
		}
	};

	template<typename T_ALGORITHM = ProductKeyGeneratorAlgorithm>
	class ProductKeyGenerator
	{
		uint64              seed : 48;
		Algorithms::Isaac64 isaac;
		T_ALGORITHM         algorithm;

	public:
		typedef ProductKey<T_ALGORITHM>            Key;
		typedef typename Algorithms::Isaac64::Seed Seed;
		typedef T_ALGORITHM                        Algorithm;

		ProductKeyGenerator()
			: ProductKeyGenerator(
				GetDefaultSeed()
			)
		{
		}

		explicit ProductKeyGenerator(const Seed& seed)
			: isaac(
				seed
			)
		{
		}

		ProductKeyGenerator(ProductKeyGenerator&& productKeyGenerator)
			: seed(
				productKeyGenerator.seed
			),
			isaac(
				Move(productKeyGenerator.isaac)
			)
		{
		}
		ProductKeyGenerator(const ProductKeyGenerator& productKeyGenerator)
			: seed(
				productKeyGenerator.seed
			),
			isaac(
				productKeyGenerator.isaac
			)
		{
		}

		Void Next(Key& key)
		{
			key.Random = static_cast<uint8>(
				isaac.Next(
					Integer<uint8>::Minimum,
					Integer<uint8>::Maximum
				)
			);

			key.Seed = (seed += isaac.Next(
				Integer<uint64>::Minimum,
				Integer<uint64>::Maximum
			));

			key.Check = algorithm.Calculate(
				key.Seed
			);
		}

		ProductKeyGenerator& operator = (ProductKeyGenerator&& productKeyGenerator)
		{
			seed = productKeyGenerator.seed;

			isaac = Move(
				productKeyGenerator.isaac
			);

			return *this;
		}
		ProductKeyGenerator& operator = (const ProductKeyGenerator& productKeyGenerator)
		{
			seed  = productKeyGenerator.seed;
			isaac = productKeyGenerator.isaac;

			return *this;
		}

		Bool operator == (const ProductKeyGenerator& productKeyGenerator) const
		{
			if (seed != productKeyGenerator.seed)
			{

				return False;
			}

			if (isaac != productKeyGenerator.isaac)
			{

				return False;
			}

			return True;
		}
		Bool operator != (const ProductKeyGenerator& productKeyGenerator) const
		{
			if (operator==(productKeyGenerator))
			{

				return False;
			}

			return True;
		}

	private:
		static Seed GetDefaultSeed()
		{
			Algorithms::Isaac isaac(
				static_cast<uint32>(OS::System::GetTimestamp().ToHours())
			);

			Seed seed;

			for (auto& i : seed)
			{
				i = isaac.Next();
			}

			return seed;
		}
	};
}
