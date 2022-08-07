#pragma once
#include "AL/Common.hpp"

#include "Text.hpp"

#include "AL/Collections/Array.hpp"
#include "AL/Collections/Tuple.hpp"
#include "AL/Collections/LinkedList.hpp"

namespace AL::Serialization
{
	template<typename ... T_VALUES>
	class CSV
	{
		template<size_t _I>
		using Get_Value_Type = Get_Type_Sequence<_I, T_VALUES ...>;

		typedef Collections::Tuple<T_VALUES ...> T_CONTAINER;

		Collections::Array<T_CONTAINER> container;

	public:
		// @throw AL::Exception
		static Void FromString(CSV& csv, const String& string, size_t lineSkip = 0, TextLineEndings lineEnding = TextLineEndings::Auto)
		{
			auto wstring = string.ToWString();

			FromString(
				csv,
				wstring,
				lineSkip,
				lineEnding
			);
		}
		// @throw AL::Exception
		static Void FromString(CSV& csv, const WString& wstring, size_t lineSkip = 0, TextLineEndings lineEnding = TextLineEndings::Auto)
		{
			Collections::LinkedList<WString> lines;

			{
				WString line;
				Text    text(wstring);

				text.SetLineEnding(
					lineEnding
				);

				for (size_t i = 0; i < lineSkip; ++i)
				{
					if (!text.ReadLine(line))
					{

						break;
					}
				}

				while (text.ReadLine(line))
				{
					lines.PushBack(
						Move(line)
					);
				}
			}

			Collections::Array<T_CONTAINER> container(
				lines.GetSize()
			);

			{
				auto it = lines.cbegin();

				for (size_t i = 0; i < lines.GetSize(); ++i, ++it)
				{
					auto& line       = *it;
					auto  lineLength = line.GetLength();

					Collections::Array<WString> lineChunks(
						sizeof ...(T_VALUES)
					);

					size_t lineChunksCount = 0;

					for (size_t lineOffset = 0; lineOffset < lineLength; )
					{
						if (line[lineOffset] == L'"')
						{
							size_t lineOffsetEndOfChunk;

							if ((lineOffsetEndOfChunk = line.IndexOfAt(L'"', lineOffset + 1)) == WString::NPOS)
							{

								throw Exception(
									"No terminating quote on line %s [Chunk: %s, Offset: %s]",
									AL::ToString(lineSkip + i).GetCString(),
									AL::ToString(lineChunksCount).GetCString(),
									AL::ToString(lineOffset).GetCString()
								);
							}

							lineChunks[lineChunksCount++] = line.SubString(
								lineOffset + 1,
								lineOffsetEndOfChunk - (lineOffset + 1)
							);

							lineOffset = lineOffsetEndOfChunk + 2;

							continue;
						}

						size_t lineOffsetEndOfChunk;

						if ((lineOffsetEndOfChunk = line.IndexOfAt(L',', lineOffset)) == WString::NPOS)
						{

							lineOffsetEndOfChunk = lineLength;
						}

						lineChunks[lineChunksCount++] = line.SubString(
							lineOffset,
							lineOffsetEndOfChunk - lineOffset
						);

						lineOffset = lineOffsetEndOfChunk + 1;
					}

					if (lineChunksCount != lineChunks.GetCapacity())
					{

						throw Exception(
							"Invalid chunk count at line %s",
							AL::ToString(lineSkip + i).GetCString()
						);
					}

					container[i] = FromString_LineChunksToContainer(
						lineChunks
					);
				}
			}

			csv.container = Move(
				container
			);
		}

		CSV()
		{
		}

		CSV(CSV&& csv)
			: container(
				Move(csv.container)
			)
		{
		}
		CSV(const CSV& csv)
			: container(
				csv.container
			)
		{
		}

		virtual ~CSV()
		{
		}

		auto GetLineCount() const
		{
			return container.GetSize();
		}

		template<size_t I>
		const typename Get_Value_Type<I>::Type& Get(size_t line) const
		{
			return container[line].template Get<I>();
		}

		template<size_t I>
		Void Set(size_t line, typename Get_Value_Type<I>::Type&& value)
		{
			container[line].template Set<I>(
				Move(value)
			);
		}
		template<size_t I>
		Void Set(size_t line, const typename Get_Value_Type<I>::Type& value)
		{
			container[line].template Set<I>(
				value
			);
		}

		// @return newly created line index
		size_t Add(const T_VALUES& ... values)
		{
			auto size = container.GetSize();

			container.SetSize(
				size + 1
			);

			container[size] = Collections::Tuple<T_VALUES ...>(
				values ...
			);

			return size;
		}

		String  ToString() const
		{
			auto wstring = ToWString();

			return wstring.ToString();
		}
		WString ToWString() const
		{
			Text text;

			for (auto& subContainer : container)
			{
				auto line = ToWString_ContainerToLine(
					subContainer
				);

				text.WriteLine(
					line
				);
			}

			return text.ToWString();
		}

		CSV& operator = (CSV&& csv)
		{
			container = Move(
				csv.container
			);

			return *this;
		}
		CSV& operator = (const CSV& csv)
		{
			container = csv.container;

			return *this;
		}

		Bool operator == (const CSV& csv) const
		{
			if (container != csv.container)
			{

				return False;
			}

			return True;
		}
		Bool operator != (const CSV& csv) const
		{
			if (operator==(csv))
			{

				return False;
			}

			return True;
		}

	private:
		template<size_t I>
		static constexpr Void    ToWString_ContainerToLine(WString& value, const T_CONTAINER& container)
		{
			if constexpr (I > 0)
			{

				value.Append(
					L','
				);
			}

			if constexpr (Is_Type<String, typename Get_Value_Type<I>::Type>::Value)
			{
				auto _value = String::Format(
					"\"%s\"",
					container.template Get<I>().GetCString()
				);

				value.Append(
					_value.ToWString()
				);
			}
			else if constexpr (Is_Type<WString, typename Get_Value_Type<I>::Type>::Value)
			{
				auto _value = WString::Format(
					L"\"%s\"",
					container.template Get<I>().GetCString()
				);

				value.Append(
					_value
				);
			}
			else
			{
				value.Append(
					AL::ToWString(
						container.template Get<I>()
					)
				);
			}
		}
		template<size_t ... INDEXES>
		static constexpr Void    ToWString_ContainerToLine(WString& value, const T_CONTAINER& container, Index_Sequence<INDEXES ...>)
		{
			(ToWString_ContainerToLine<INDEXES>(value, container), ...);
		}
		static           WString ToWString_ContainerToLine(const T_CONTAINER& container)
		{
			WString value;

			ToWString_ContainerToLine(
				value,
				container,
				typename Make_Index_Sequence<sizeof ...(T_VALUES)>::Type {}
			);

			return value;
		}

		template<size_t I>
		static constexpr Void        FromString_LineChunksToContainer(T_CONTAINER& value, const Collections::Array<WString>& chunks)
		{
			if constexpr (Is_Type<String, typename Get_Value_Type<I>::Type>::Value)
			{
				value.template Set<I>(
					chunks[I].ToString()
				);
			}
			else if constexpr (Is_Type<WString, typename Get_Value_Type<I>::Type>::Value)
			{
				value.template Set<I>(
					Move(chunks[I])
				);
			}
			else
			{
				value.template Set<I>(
					AL::FromWString<typename Get_Value_Type<I>::Type>(
						chunks[I]
					)
				);
			}
		}
		template<size_t ... INDEXES>
		static constexpr Void        FromString_LineChunksToContainer(T_CONTAINER& value, const Collections::Array<WString>& chunks, Index_Sequence<INDEXES ...>)
		{
			(FromString_LineChunksToContainer<INDEXES>(value, chunks), ...);
		}
		static           T_CONTAINER FromString_LineChunksToContainer(const Collections::Array<WString>& chunks)
		{
			T_CONTAINER value;

			FromString_LineChunksToContainer(
				value,
				chunks,
				typename Make_Index_Sequence<sizeof ...(T_VALUES)>::Type {}
			);

			return value;
		}
	};
}
