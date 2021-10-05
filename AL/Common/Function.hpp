#pragma once
#include "AL/Common.hpp"

namespace AL
{
	template<typename F>
	class Function;

	template<typename T, typename ... TArgs>
	class Function<T(TArgs ...)>
	{
		class ILambda
		{
		public:
			template<typename F>
			static ILambda* Create(F&& function)
			{
				auto lpLambda = new Lambda<F>(
					Move(function)
				);

				return lpLambda;
			}

			static Void Destroy(ILambda* lpLambda)
			{
				delete lpLambda;
			}

			virtual ~ILambda()
			{
			}

			virtual uint32 GetHash() const = 0;

			virtual ILambda* Copy() const = 0;

			virtual T Execute(TArgs ... args) const = 0;
		};

		template<typename F>
		class Lambda
			: public ILambda
		{
			F function;

			Lambda(Lambda&&) = delete;
			Lambda(const Lambda&) = delete;

		public:
			explicit Lambda(F&& function)
				: function(
					Move(function)
				)
			{
			}

			explicit Lambda(const F& function)
				: function(
					function
				)
			{
			}

			virtual uint32 GetHash() const override
			{
				return Type<F>::Hash;
			}

			virtual ILambda* Copy() const override
			{
				return new Lambda<F>(
					function
				);
			}

			virtual T Execute(TArgs ... args) const override
			{
				return function(
					Forward<TArgs>(args) ...
				);
			}
		};

		ILambda* lpLambda;

	public:
		Function()
			: lpLambda(
				nullptr
			)
		{
		}

		Function(Function&& function)
			: lpLambda(
				function.lpLambda
			)
		{
			function.lpLambda = nullptr;
		}
		Function(const Function& function)
			: lpLambda(
				function.lpLambda ? function.lpLambda->Copy() : nullptr
			)
		{
		}

		template<typename F>
		explicit Function(F&& function)
			: lpLambda(
				ILambda::Create(
					Move(function)
				)
			)
		{
		}
		explicit Function(T(*lpFunction)(TArgs ...))
			: lpLambda(
				ILambda::Create(
					[lpFunction](TArgs ... _args)
					{
						return lpFunction(
							Forward<TArgs>(_args) ...
						);
					}
				)
			)
		{
		}
		template<typename C>
		Function(T(C::*lpFunction)(TArgs ...), C& instance)
			: lpLambda(
				ILambda::Create(
					[lpInstance = &instance, lpFunction](TArgs ... _args)
					{
						return (lpInstance->*lpFunction)(
							Forward<TArgs>(_args) ...
						);
					}
				)
			)
		{
		}
		template<typename C>
		Function(T(C::*lpFunction)(TArgs ...) const, const C& instance)
			: lpLambda(
				ILambda::Create(
					[lpInstance = &instance, lpFunction](TArgs ... _args)
					{
						return (lpInstance->*lpFunction)(
							Forward<TArgs>(_args) ...
						);
					}
				)
			)
		{
		}

		virtual ~Function()
		{
			if (lpLambda != nullptr)
			{
				ILambda::Destroy(
					lpLambda
				);

				lpLambda = nullptr;
			}
		}

		auto GetHash() const
		{
			return lpLambda ? lpLambda->GetHash() : Type<T(TArgs ...)>::Hash;
		}

		template<typename F>
		Void Bind(F&& function)
		{
			if (lpLambda != nullptr)
			{

				ILambda::Destroy(
					lpLambda
				);
			}

			lpLambda = ILambda::Create(
				Move(function)
			);
		}
		Void Bind(T(*lpFunction)(TArgs ...))
		{
			Bind(
				[lpFunction](TArgs ... _args)
				{
					return lpFunction(
						Forward<TArgs>(_args) ...
					);
				}
			);
		}
		template<typename C>
		Void Bind(T(C::*lpFunction)(TArgs ...), C& instance)
		{
			Bind(
				[lpInstance = &instance, lpFunction](TArgs ... _args)
				{
					return (lpInstance->*lpFunction)(
						Forward<TArgs>(_args) ...
					);
				}
			);
		}
		template<typename C>
		Void Bind(T(C::*lpFunction)(TArgs ...) const, const C& instance)
		{
			Bind(
				[lpInstance = &instance, lpFunction](TArgs ... _args)
				{
					return (lpInstance->*lpFunction)(
						Forward<TArgs>(_args) ...
					);
				}
			);
		}

		Void Unbind()
		{
			if (lpLambda != nullptr)
			{
				delete lpLambda;

				lpLambda = nullptr;
			}
		}

		operator Bool () const
		{
			return lpLambda != nullptr;
		}

		T operator () (TArgs ... args) const
		{
			return lpLambda->Execute(
				Forward<TArgs>(args) ...
			);
		}

		Function& operator = (Function&& function)
		{
			if (lpLambda != nullptr)
			{

				ILambda::Destroy(
					lpLambda
				);
			}

			lpLambda = function.lpLambda;
			function.lpLambda = nullptr;

			return *this;
		}
		Function& operator = (const Function& function)
		{
			if (lpLambda)
			{

				ILambda::Destroy(
					lpLambda
				);
			}

			lpLambda = function.lpLambda ? function.lpLambda->Copy() : nullptr;

			return *this;
		}

		Bool operator == (const Function& function) const
		{
			if (lpLambda && function.lpLambda)
			{

				return GetHash() == function.GetHash();
			}

			return !lpLambda && !function.lpLambda;
		}
		Bool operator != (const Function& function) const
		{
			if (operator==(function))
			{

				return False;
			}

			return True;
		}
	};
}
