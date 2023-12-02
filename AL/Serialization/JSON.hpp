#pragma once
#include "AL/Common.hpp"

#include "AL/Collections/LinkedList.hpp"

#if defined(AL_PLATFORM_WINDOWS)
	#undef GetObject
#endif

namespace AL::Serialization
{
	class IJSON
	{
	public:
		IJSON()
		{
		}

		virtual ~IJSON()
		{
		}

		virtual Bool IsArray() const = 0;

		virtual Bool IsValue() const = 0;

		virtual Bool IsObject() const = 0;

		virtual String  ToString() const
		{
			auto wstring = ToWString();

			return wstring.ToString();
		}
		virtual WString ToWString() const = 0;
	};

	class JSONArray;
	class JSONValue;
	class JSONObject;

	class JSONValue
		: public IJSON
	{
		static constexpr const WString::Char REGEX_PATTERN_NULL[]    = L"";
		static constexpr const WString::Char REGEX_PATTERN_NUMBER[]  = L"";
		static constexpr const WString::Char REGEX_PATTERN_STRING[]  = L"";
		static constexpr const WString::Char REGEX_PATTERN_BOOLEAN[] = L"";
		static constexpr const WString::Char REGEX_PATTERN_DECIMAL[] = L"";
		static constexpr const WString::Char REGEX_PATTERN_INTEGER[] = L"";

		Bool isNull    = False;
		Bool isNumber  = False;
		Bool isString  = False;
		Bool isBoolean = False;
		Bool isDecimal = False;
		Bool isInteger = False;

		struct
		{
			AL::String String;

			union
			{
				Bool   Boolean;
				Double Decimal;
				int64  Integer;
			};
		} value;

	public:
		// @throw AL::Exception
		static Void FromString(JSONValue& value, const String& string)
		{
			auto wstring = string.ToWString();

			FromString(
				value,
				wstring
			);
		}
		// @throw AL::Exception
		static Void FromString(JSONValue& value, const WString& wstring)
		{
			// TODO: implement
			throw NotImplementedException();
		}

		JSONValue()
			: isNull(
				True
			)
		{
		}

		JSONValue(JSONValue&& value)
			: isNull(
				value.isNull
			),
			isNumber(
				value.isNumber
			),
			isString(
				value.isString
			),
			isBoolean(
				value.isBoolean
			),
			isDecimal(
				value.isDecimal
			),
			isInteger(
				value.isInteger
			),
			value(
				Move(value.value)
			)
		{
			value.isNull    = True;
			value.isNumber  = False;
			value.isString  = False;
			value.isBoolean = False;
			value.isDecimal = False;
			value.isInteger = False;
		}
		JSONValue(const JSONValue& value)
			: isNull(
				value.isNull
			),
			isNumber(
				value.isNumber
			),
			isString(
				value.isString
			),
			isBoolean(
				value.isBoolean
			),
			isDecimal(
				value.isDecimal
			),
			isInteger(
				value.isInteger
			),
			value(
				value.value
			)
		{
		}

		JSONValue(Bool value)
			: isBoolean(
				true
			),
			value{
				.Boolean = value
			}
		{
		}
		JSONValue(int8 value)
			: isNumber(
				True
			),
			isInteger(
				True
			),
			value{
				.Integer = value
			}
		{
		}
		JSONValue(int16 value)
			: isNumber(
				True
			),
			isInteger(
				True
			),
			value{
				.Integer = value
			}
		{
		}
		JSONValue(int32 value)
			: isNumber(
				True
			),
			isInteger(
				True
			),
			value{
				.Integer = value
			}
		{
		}
		JSONValue(int64 value)
			: isNumber(
				True
			),
			isInteger(
				True
			),
			value{
				.Integer = value
			}
		{
		}
		JSONValue(uint8 value)
			: isNumber(
				True
			),
			isInteger(
				True
			),
			value{
				.Integer = value
			}
		{
		}
		JSONValue(uint16 value)
			: isNumber(
				True
			),
			isInteger(
				True
			),
			value{
				.Integer = value
			}
		{
		}
		JSONValue(uint32 value)
			: isNumber(
				True
			),
			isInteger(
				True
			),
			value{
				.Integer = value
			}
		{
		}
		JSONValue(uint64 value)
			: isNumber(
				True
			),
			isInteger(
				True
			),
			value{
				.Integer = static_cast<int64>(value)
			}
		{
		}
		JSONValue(Float value)
			: isNumber(
				True
			),
			isDecimal(
				True
			),
			value{
				.Decimal = value
			}
		{
		}
		JSONValue(Double value)
			: isNumber(
				True
			),
			isDecimal(
				True
			),
			value{
				.Decimal = value
			}
		{
		}
		JSONValue(String&& value)
			: isString(
				true
			),
			value{
				.String = Move(value)
			}
		{
		}
		JSONValue(const String& value)
			: isString(
				true
			),
			value{
				.String = value
			}
		{
		}

		virtual ~JSONValue()
		{
		}

		virtual Bool IsNull() const
		{
			return isNull;
		}

		virtual Bool IsArray() const override
		{
			return False;
		}

		virtual Bool IsValue() const override
		{
			return True;
		}

		virtual Bool IsNumber() const
		{
			return isNumber;
		}

		virtual Bool IsObject() const override
		{
			return False;
		}

		virtual Bool IsString() const
		{
			return isString;
		}

		virtual Bool IsBoolean() const
		{
			return isBoolean;
		}

		virtual Bool IsDecimal() const
		{
			return isDecimal;
		}

		virtual Bool IsInteger() const
		{
			return isInteger;
		}

		virtual String GetString() const
		{
			AL_ASSERT(
				IsString(),
				"JSONValue is not a string"
			);

			return value.String;
		}

		virtual Bool GetBoolean() const
		{
			AL_ASSERT(
				IsBoolean(),
				"JSONValue is not a boolean"
			);

			return value.Boolean;
		}

		virtual Double GetDecimal() const
		{
			AL_ASSERT(
				IsBoolean(),
				"JSONValue is not a decimal"
			);

			return value.Decimal;
		}

		virtual int64 GetInteger() const
		{
			AL_ASSERT(
				IsBoolean(),
				"JSONValue is not an integer"
			);

			return value.Integer;
		}

		virtual WString ToWString() const override
		{
			WStringBuilder sb;

			if (IsNull())
				sb.Append(L"null");
			else if (IsNumber())
			{
				if (IsDecimal())
					sb.Append(GetDecimal());
				else if (IsInteger())
					sb.Append(GetInteger());
			}
			else if (IsString())
			{
				sb.Append(L'"');
				sb.Append(GetString());
				sb.Append(L'"');
			}
			else if (IsBoolean())
				sb.Append(GetBoolean());

			return sb.ToString();
		}

		JSONValue& operator = (JSONValue&& value)
		{
			isNull = value.isNull;
			value.isNull = True;

			isNumber = value.isNumber;
			value.isNumber = False;

			isString = value.isString;
			value.isString = False;

			isBoolean = value.isBoolean;
			value.isBoolean = False;

			isDecimal = value.isDecimal;
			value.isDecimal = False;

			isInteger = value.isInteger;
			value.isInteger = False;

			this->value = Move(
				value.value
			);

			return *this;
		}
		JSONValue& operator = (const JSONValue& value)
		{
			isNull      = value.isNull;
			isNumber    = value.isNumber;
			isString    = value.isString;
			isBoolean   = value.isBoolean;
			isDecimal   = value.isDecimal;
			isInteger   = value.isInteger;
			this->value = value.value;

			return *this;
		}

		Bool operator == (const JSONValue& value) const
		{
			if (IsNumber())
			{
				if (IsDecimal())
					return GetDecimal() == value.GetDecimal();
				else if (IsInteger())
					return GetInteger() == value.GetInteger();
			}
			else if (IsString())
				return GetString() == value.GetString();
			else if (IsBoolean())
				return GetBoolean() == value.GetBoolean();

			return False;
		}
		Bool operator != (const JSONValue& value) const
		{
			if (operator==(value))
			{

				return False;
			}

			return True;
		}
	};

	class JSONArray
		: public IJSON
	{
		static constexpr const WString::Char REGEX_PATTERN_ARRAY[]  = L"";
		static constexpr const WString::Char REGEX_PATTERN_VALUE[]  = L"";
		static constexpr const WString::Char REGEX_PATTERN_OBJECT[] = L"";

		Collections::LinkedList<IJSON*> values;

	public:
		// @throw AL::Exception
		static Void FromString(JSONArray& array, const String& string)
		{
			auto wstring = string.ToWString();

			FromString(
				array,
				wstring
			);
		}
		// @throw AL::Exception
		static Void FromString(JSONArray& array, const WString& wstring)
		{
			// TODO: implement
			throw NotImplementedException();
		}

		JSONArray()
		{
		}

		JSONArray(JSONArray&& array)
			: values(
				Move(array.values)
			)
		{
		}
		JSONArray(const JSONArray& array);

		template<size_t S>
		JSONArray(IJSON(&&json)[S])
		{
			for (auto& j : json)
			{
				if (j.IsArray())       Add(Move(reinterpret_cast<JSONArray&>(j)));
				else if (j.IsValue())  Add(Move(reinterpret_cast<JSONValue&>(j)));
				else if (j.IsObject()) Add(Move(reinterpret_cast<JSONObject&>(j)));
			}
		}

		virtual ~JSONArray()
		{
			Clear();
		}

		virtual Bool IsArray() const override
		{
			return True;
		}

		virtual Bool IsValue() const override
		{
			return False;
		}

		virtual Bool IsObject() const override
		{
			return False;
		}

		virtual size_t GetSize() const
		{
			return values.GetSize();
		}

		virtual size_t GetCapacity() const
		{
			return values.GetCapacity();
		}

		virtual WString ToWString() const override
		{
			size_t         i = 0;
			WStringBuilder sb;

			sb.Append(L'[');

			for (auto lpValue : values)
			{
				if (i++ != 0)
					sb.Append(L',');

				sb.Append(
					lpValue->ToWString()
				);
			}

			sb.Append(L']');

			return sb.ToString();
		}

		Void Clear()
		{
			for (auto it = values.begin(); it != values.end(); )
			{
				delete *it;

				values.Erase(
					it++
				);
			}
		}

		Void Add(JSONArray&& array)
		{
			values.PushBack(new JSONArray(Move(array)));
		}
		Void Add(JSONValue&& value)
		{
			values.PushBack(new JSONValue(Move(value)));
		}
		Void Add(JSONObject&& object);

		Void Remove(size_t index)
		{
			size_t i = 0;

			for (auto it = values.begin(); it != values.end(); )
			{
				if (i++ == index)
				{
					delete *it;

					values.Erase(
						it
					);

					break;
				}

				++it;
			}
		}
		Void Remove(const IJSON& json)
		{
			for (auto it = values.begin(); it != values.end(); )
			{
				if (*it == &json)
				{
					delete *it;

					values.Erase(
						it
					);

					break;
				}

				++it;
			}
		}

		Bool Get(IJSON*& lpJSON, size_t index)
		{
			size_t i = 0;

			for (auto lpValue : values)
			{
				if (i++ == index)
				{
					lpJSON = lpValue;

					return True;
				}
			}

			return False;
		}
		Bool Get(JSONArray*& lpArray, size_t index)
		{
			if (!Get(reinterpret_cast<IJSON*&>(lpArray), index))
			{

				return False;
			}

			if (!lpArray->IsArray())
			{

				return False;
			}

			return True;
		}
		Bool Get(JSONValue*& lpValue, size_t index)
		{
			if (!Get(reinterpret_cast<IJSON*&>(lpValue), index))
			{

				return False;
			}

			if (!lpValue->IsValue())
			{

				return False;
			}

			return True;
		}
		Bool Get(JSONObject*& lpObject, size_t index);

		JSONArray& operator = (JSONArray&& array)
		{
			values = Move(
				array.values
			);

			return *this;
		}
		JSONArray& operator = (const JSONArray& array);

		Bool operator == (const JSONArray& array) const;
		Bool operator != (const JSONArray& array) const
		{
			if (operator==(array))
			{

				return False;
			}

			return True;
		}
	};

	class JSONObject
		: public IJSON
	{
		static constexpr const WString::Char REGEX_PATTERN_ARRAY[]  = L"";
		static constexpr const WString::Char REGEX_PATTERN_VALUE[]  = L"";
		static constexpr const WString::Char REGEX_PATTERN_OBJECT[] = L"";

		struct Member
		{
			WString Name;
			IJSON*  lpValue;
		};

		Collections::LinkedList<Member> members;

	public:
		// @throw AL::Exception
		static Void FromString(JSONObject& object, const String& string)
		{
			auto wstring = string.ToWString();

			FromString(
				object,
				wstring
			);
		}
		// @throw AL::Exception
		static Void FromString(JSONObject& object, const WString& wstring)
		{
			// TODO: implement
			throw NotImplementedException();
		}

		JSONObject()
		{
		}

		JSONObject(JSONObject&& object)
			: members(
				Move(object.members)
			)
		{
		}
		JSONObject(const JSONObject& object)
		{
			for (auto& member : object.members)
			{
				Member _member =
				{
					.Name = member.Name
				};

				if (member.lpValue->IsArray())
					_member.lpValue = new JSONArray(*reinterpret_cast<const JSONArray*>(member.lpValue));
				else if (member.lpValue->IsValue())
					_member.lpValue = new JSONValue(*reinterpret_cast<const JSONValue*>(member.lpValue));
				else if (member.lpValue->IsObject())
					_member.lpValue = new JSONObject(*reinterpret_cast<const JSONObject*>(member.lpValue));

				members.PushBack(
					Move(_member)
				);
			}
		}

		virtual ~JSONObject()
		{
			Clear();
		}

		virtual Bool IsArray() const override
		{
			return False;
		}

		virtual Bool IsValue() const override
		{
			return False;
		}

		virtual Bool IsObject() const override
		{
			return True;
		}

		virtual size_t GetSize() const
		{
			return members.GetSize();
		}

		virtual size_t GetCapacity() const
		{
			return members.GetCapacity();
		}

		virtual WString ToWString() const override
		{
			size_t         i = 0;
			WStringBuilder sb;

			sb.Append(L'{');

			for (auto& member : members)
			{
				if (i++ != 0)
					sb.Append(L',');

				if (member.Name.GetLength() == 0)
					sb.Append(member.lpValue->ToWString());
				else
				{
					sb.Append(L'"');
					sb.Append(member.Name);
					sb.Append(L"\":");
					sb.Append(member.lpValue->ToWString());
				}
			}

			sb.Append(L'}');

			return sb.ToString();
		}

		Void Clear()
		{
			for (auto it = members.begin(); it != members.end(); )
			{
				delete it->lpValue;

				members.Erase(
					it++
				);
			}
		}

		Void Add(String&& name, JSONArray&& array)
		{
			Add(
				name.ToWString(),
				Move(array)
			);
		}
		Void Add(WString&& name, JSONArray&& array)
		{
			Add(
				{
					.Name    = Move(name),
					.lpValue = new JSONArray(Move(array))
				}
			);
		}
		Void Add(String&& name, JSONValue&& value)
		{
			Add(
				name.ToWString(),
				Move(value)
			);
		}
		Void Add(WString&& name, JSONValue&& value)
		{
			Add(
				{
					.Name    = Move(name),
					.lpValue = new JSONValue(Move(value))
				}
			);
		}
		Void Add(String&& name, JSONObject&& object)
		{
			Add(
				name.ToWString(),
				Move(object)
			);
		}
		Void Add(WString&& name, JSONObject&& object)
		{
			Add(
				{
					.Name    = Move(name),
					.lpValue = new JSONObject(Move(object))
				}
			);
		}

		Void Remove(const IJSON& json)
		{
			for (auto it = members.begin(); it != members.end(); )
			{
				if (it->lpValue == &json)
				{
					delete it->lpValue;

					members.Erase(
						it
					);

					break;
				}

				++it;
			}
		}
		Void Remove(const String& name)
		{
			Remove(
				name.ToWString()
			);
		}
		Void Remove(const WString& name)
		{
			for (auto it = members.begin(); it != members.end(); )
			{
				if (it->Name == name)
				{
					delete it->lpValue;

					members.Erase(
						it
					);

					break;
				}

				++it;
			}
		}

		Bool Get(String& name, IJSON*& lpJSON, size_t index)
		{
			WString wstring;

			if (!Get(wstring, lpJSON, index))
			{

				return False;
			}

			name = wstring.ToString();

			return True;
		}
		Bool Get(WString& name, IJSON*& lpJSON, size_t index)
		{
			size_t i = 0;

			for (auto& member : members)
			{
				if (i++ == index)
				{
					name   = member.Name;
					lpJSON = member.lpValue;

					return True;
				}
			}

			return False;
		}
		Bool Get(String& name, JSONArray*& lpArray, size_t index)
		{
			WString wstring;

			if (!Get(wstring, lpArray, index))
			{

				return False;
			}

			name = wstring.ToString();

			return True;
		}
		Bool Get(WString& name, JSONArray*& lpArray, size_t index)
		{
			if (!Get(name, reinterpret_cast<IJSON*&>(lpArray), index))
			{

				return False;
			}

			if (!lpArray->IsArray())
			{

				return False;
			}

			return True;
		}
		Bool Get(String& name, JSONValue*& lpValue, size_t index)
		{
			WString wstring;

			if (!Get(wstring, lpValue, index))
			{

				return False;
			}

			name = wstring.ToString();

			return True;
		}
		Bool Get(WString& name, JSONValue*& lpValue, size_t index)
		{
			if (!Get(name, reinterpret_cast<IJSON*&>(lpValue), index))
			{

				return False;
			}

			if (!lpValue->IsValue())
			{

				return False;
			}

			return True;
		}
		Bool Get(String& name, JSONObject*& lpObject, size_t index)
		{
			WString wstring;

			if (!Get(wstring, lpObject, index))
			{

				return False;
			}

			name = wstring.ToString();

			return True;
		}
		Bool Get(WString& name, JSONObject*& lpObject, size_t index)
		{
			if (!Get(name, reinterpret_cast<IJSON*&>(lpObject), index))
			{

				return False;
			}

			if (!lpObject->IsObject())
			{

				return False;
			}

			return True;
		}

		Bool GetMemberByName(IJSON*& lpJSON, const String& name)
		{
			if (!GetMemberByName(lpJSON, name.ToWString()))
			{

				return False;
			}

			return True;
		}
		Bool GetMemberByName(IJSON*& lpJSON, const WString& name)
		{
			for (auto& member : members)
			{
				if (member.Name == name)
				{
					lpJSON = member.lpValue;

					return True;
				}
			}

			return False;
		}
		Bool GetMemberByName(JSONArray*& lpArray, const String& name)
		{
			if (!GetMemberByName(lpArray, name.ToWString()))
			{

				return False;
			}

			return True;
		}
		Bool GetMemberByName(JSONArray*& lpArray, const WString& name)
		{
			if (!GetMemberByName(reinterpret_cast<IJSON*&>(lpArray), name))
			{

				return False;
			}

			if (!lpArray->IsArray())
			{

				return False;
			}

			return True;
		}
		Bool GetMemberByName(JSONValue*& lpValue, const String& name)
		{
			if (!GetMemberByName(lpValue, name.ToWString()))
			{

				return False;
			}

			return True;
		}
		Bool GetMemberByName(JSONValue*& lpValue, const WString& name)
		{
			if (!GetMemberByName(reinterpret_cast<IJSON*&>(lpValue), name))
			{

				return False;
			}

			if (!lpValue->IsValue())
			{

				return False;
			}

			return True;
		}
		Bool GetMemberByName(JSONObject*& lpObject, const String& name)
		{
			if (!GetMemberByName(lpObject, name.ToWString()))
			{

				return False;
			}

			return True;
		}
		Bool GetMemberByName(JSONObject*& lpObject, const WString& name)
		{
			if (!GetMemberByName(reinterpret_cast<IJSON*&>(lpObject), name))
			{

				return False;
			}

			if (!lpObject->IsObject())
			{

				return False;
			}

			return True;
		}

		Bool GetMemberByIndex(IJSON*& lpJSON, size_t index)
		{
			size_t i = 0;

			for (auto& member : members)
			{
				if (i++ == index)
				{
					lpJSON = member.lpValue;

					return True;
				}
			}

			return False;
		}
		Bool GetMemberByIndex(JSONArray*& lpArray, size_t index)
		{
			if (!GetMemberByIndex(reinterpret_cast<IJSON*&>(lpArray), index))
			{

				return False;
			}

			if (!lpArray->IsArray())
			{

				return False;
			}

			return True;
		}
		Bool GetMemberByIndex(JSONValue*& lpValue, size_t index)
		{
			if (!GetMemberByIndex(reinterpret_cast<IJSON*&>(lpValue), index))
			{

				return False;
			}

			if (!lpValue->IsValue())
			{

				return False;
			}

			return True;
		}
		Bool GetMemberByIndex(JSONObject*& lpObject, size_t index)
		{
			if (!GetMemberByIndex(reinterpret_cast<IJSON*&>(lpObject), index))
			{

				return False;
			}

			if (!lpObject->IsObject())
			{

				return False;
			}

			return True;
		}

		JSONObject& operator = (JSONObject&& object)
		{
			members = Move(
				object.members
			);

			return *this;
		}
		JSONObject& operator = (const JSONObject& object)
		{
			Clear();

			for (auto& member : object.members)
			{
				Member _member =
				{
					.Name = member.Name
				};

				if (member.lpValue->IsArray())
					_member.lpValue = new JSONArray(*reinterpret_cast<const JSONArray*>(member.lpValue));
				else if (member.lpValue->IsValue())
					_member.lpValue = new JSONValue(*reinterpret_cast<const JSONValue*>(member.lpValue));
				else if (member.lpValue->IsObject())
					_member.lpValue = new JSONObject(*reinterpret_cast<const JSONObject*>(member.lpValue));

				members.PushBack(
					Move(_member)
				);
			}

			return *this;
		}

		Bool operator == (const JSONObject& object) const
		{
			if (GetSize() != object.GetSize())
			{

				return False;
			}

			for (auto it1 = members.begin(), it2 = object.members.begin(); it1 != members.end(); ++it1, ++it2)
			{
				if (it1->Name != it2->Name)
				{

					return False;
				}

				auto lpJSON1 = it1->lpValue;
				auto lpJSON2 = it2->lpValue;

				if (lpJSON1->IsArray() && lpJSON2->IsArray())
				{
					if (*reinterpret_cast<const JSONArray*>(lpJSON1) != *reinterpret_cast<const JSONArray*>(lpJSON2))
					{

						return False;
					}

					continue;
				}
				else if (lpJSON1->IsValue() && lpJSON2->IsValue())
				{
					if (*reinterpret_cast<const JSONValue*>(lpJSON1) != *reinterpret_cast<const JSONValue*>(lpJSON2))
					{

						return False;
					}

					continue;
				}
				else if (lpJSON1->IsObject() && lpJSON2->IsObject())
				{
					if (*reinterpret_cast<const JSONObject*>(lpJSON1) != *reinterpret_cast<const JSONObject*>(lpJSON2))
					{

						return False;
					}

					continue;
				}

				return False;
			}

			return True;
		}
		Bool operator != (const JSONObject& object) const
		{
			if (operator==(object))
			{

				return False;
			}

			return True;
		}

	private:
		Void Add(Member&& member)
		{
			for (auto it = members.begin(); it != members.end(); ++it)
			{
				if (it->Name == member.Name)
				{
					delete it->lpValue;

					it->lpValue = member.lpValue;

					return;
				}
			}

			members.PushBack(
				Move(member)
			);
		}
	};

	class JSON
		: public JSONObject
	{
	public:
		typedef JSONArray  Array;
		typedef JSONValue  Value;
		typedef JSONObject Object;

		using JSONObject::JSONObject;
	};
}

inline AL::Serialization::JSONArray::JSONArray(const JSONArray& array)
{
	for (auto lpValue : array.values)
	{
		if (lpValue->IsArray())
			values.PushBack(new JSONArray(*reinterpret_cast<const JSONArray*>(lpValue)));
		else if (lpValue->IsValue())
			values.PushBack(new JSONValue(*reinterpret_cast<const JSONValue*>(lpValue)));
		else if (lpValue->IsObject())
			values.PushBack(new JSONObject(*reinterpret_cast<const JSONObject*>(lpValue)));
	}
}

inline AL::Void AL::Serialization::JSONArray::Add(JSONObject&& object)
{
	values.PushBack(new JSONObject(Move(object)));
}

inline AL::Bool AL::Serialization::JSONArray::Get(JSONObject*& lpObject, size_t index)
{
	if (!Get(reinterpret_cast<IJSON*&>(lpObject), index))
	{

		return False;
	}

	if (!lpObject->IsObject())
	{

		return False;
	}

	return True;
}

inline AL::Serialization::JSONArray& AL::Serialization::JSONArray::operator = (const JSONArray& array)
{
	values.Clear();

	for (auto lpValue : array.values)
	{
		if (lpValue->IsArray())
			values.PushBack(new JSONArray(*reinterpret_cast<const JSONArray*>(lpValue)));
		else if (lpValue->IsValue())
			values.PushBack(new JSONValue(*reinterpret_cast<const JSONValue*>(lpValue)));
		else if (lpValue->IsObject())
			values.PushBack(new JSONObject(*reinterpret_cast<const JSONObject*>(lpValue)));
	}

	return *this;
}

inline AL::Bool AL::Serialization::JSONArray::operator == (const JSONArray& array) const
{
	if (GetSize() != array.GetSize())
	{

		return False;
	}

	for (auto it1 = values.begin(), it2 = array.values.begin(); it1 != values.end(); ++it1, ++it2)
	{
		auto lpJSON1 = *it1;
		auto lpJSON2 = *it2;

		if (lpJSON1->IsArray() && lpJSON2->IsArray())
		{
			if (*reinterpret_cast<const JSONArray*>(lpJSON1) != *reinterpret_cast<const JSONArray*>(lpJSON2))
			{

				return False;
			}

			continue;
		}
		else if (lpJSON1->IsValue() && lpJSON2->IsValue())
		{
			if (*reinterpret_cast<const JSONValue*>(lpJSON1) != *reinterpret_cast<const JSONValue*>(lpJSON2))
			{

				return False;
			}

			continue;
		}
		else if (lpJSON1->IsObject() && lpJSON2->IsObject())
		{
			if (*reinterpret_cast<const JSONObject*>(lpJSON1) != *reinterpret_cast<const JSONObject*>(lpJSON2))
			{

				return False;
			}

			continue;
		}

		return False;
	}

	return True;
}
