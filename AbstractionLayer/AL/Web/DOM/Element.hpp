#pragma once
#include "AL/Common.hpp"

#include "ElementAttribute.hpp"

#include "AL/Collections/List.hpp"

namespace AL::Web::DOM
{
	class Element
	{
		const bool cantSelfTerminate;
		const bool mustSelfTerminate;

		bool isTextSet = false;

		bool selfTerminating = false;

		String type;
		String text;
		Element* lpParent;
		Collections::List<Element*> children;
		Collections::List<ElementAttribute> attributes;

		Element(Element&&) = delete;
		Element(const Element&) = delete;

	public:
		static bool FromString(Element& element, const String& string);

		Element(String&& type, Element* lpParent = nullptr)
			: cantSelfTerminate(
				TypeCantSelfTerminate(type)
			),
			mustSelfTerminate(
				TypeMustSelfTerminate(type)
			),
			selfTerminating(
				cantSelfTerminate || mustSelfTerminate
			),
			type(
				Move(type)
			),
			lpParent(
				lpParent
			)
		{
		}

		virtual ~Element()
		{
			for (auto lpElement : children)
			{
				delete lpElement;
			}
		}

		auto& GetType() const
		{
			return type;
		}

		auto& GetText() const
		{
			return text;
		}

		auto GetParent()
		{
			return lpParent;
		}
		auto GetParent() const
		{
			return lpParent;
		}

		auto GetChildCount() const
		{
			return children.GetSize();
		}

		auto GetAttributeCount() const
		{
			return attributes.GetSize();
		}

		Element* FindChildByType(const String& type, size_t skip = 0) const
		{
			for (auto lpElement : children)
			{
				if (lpElement->type.Compare(type, true) && skip--)
				{

					return lpElement;
				}
			}

			return nullptr;
		}

		ElementAttribute* FindAttributeByName(const String& name)
		{
			for (auto& attribute : attributes)
			{
				if (attribute.Name.Compare(name, true))
				{

					return &attribute;
				}
			}

			return nullptr;
		}
		const ElementAttribute* FindAttributeByName(const String& name) const
		{
			for (auto& attribute : attributes)
			{
				if (attribute.Name.Compare(name, true))
				{

					return &attribute;
				}
			}

			return nullptr;
		}

		void SetText(String&& value)
		{
			text = Move(
				value
			);

			isTextSet = true;

			UpdateSelfTerminatingState();
		}
		void SetText(const String& value)
		{
			SetText(
				String(value)
			);
		}

		void RemoveText()
		{
			text.Clear();

			isTextSet = false;

			UpdateSelfTerminatingState();
		}

		void SetAttribute(String&& name, String& value)
		{
			for (auto& attribute : attributes)
			{
				if (attribute.Name.Compare(name, true))
				{
					attribute.Value = Move(
						value
					);

					return;
				}
			}

			attributes.EmplaceBack(
				Move(name),
				Move(value)
			);
		}
		void SetAttribute(String&& name, const String& value)
		{
			SetAttribute(
				Move(name),
				String(value)
			);
		}
		void SetAttribute(const String& name, const String& value)
		{
			SetAttribute(
				String(name),
				String(value)
			);
		}

		void RemoveAttribute(const String& name)
		{
			for (auto it = attributes.begin(); it != attributes.end(); )
			{
				if (it->Name.Compare(name, true))
				{
					attributes.Erase(
						it
					);

					UpdateSelfTerminatingState();

					return;
				}

				++it;
			}
		}

		// @return nullptr if type does not support children
		Element* CreateChild(String&& type)
		{
			auto lpElement = new Element(
				Move(type),
				this
			);

			children.PushBack(
				lpElement
			);

			UpdateSelfTerminatingState();

			return lpElement;
		}

		Element& DestroyChild(Element* lpElement)
		{
			if (lpElement)
			{
				children.Remove(
					lpElement
				);

				delete lpElement;

				UpdateSelfTerminatingState();
			}

			return *this;
		}

		Element& DestroyLastChild()
		{
			if (auto lppElement = children.GetBack())
			{
				DestroyChild(
					*lppElement
				);
			}

			return *this;
		}

		String ToString() const
		{
			return selfTerminating ? ToString_TrySelfTerminating() : ToString_TryAttributesChildrenText();
		}

		bool operator == (const Element& element) const
		{
			return this == &element;
		}
		bool operator != (const Element& element) const
		{
			return !operator==(
				element
			);
		}

	protected:
		virtual bool Options_EnableChildren() const
		{
			return true;
		}

		virtual bool Options_EnableAttributes() const
		{
			return true;
		}

	private:
		void UpdateSelfTerminatingState()
		{
			if (mustSelfTerminate)
			{

				selfTerminating = true;
			}
			else if (cantSelfTerminate)
			{

				selfTerminating = false;
			}
			else
			{
				selfTerminating = true;

				if (isTextSet)
				{

					selfTerminating = false;
				}
				
				if (GetChildCount() > 0)
				{

					selfTerminating = false;
				}
			}
		}

		String ToString_Children() const
		{
			String string;

			size_t i = 0;

			for (auto lpChild : children)
			{
				string.Append(
					lpChild->ToString()
				);
			}

			return string;
		}

		String ToString_Attributes() const
		{
			String string;

			size_t i = 0;

			for (auto& attribute : attributes)
			{
				auto _attribute = String::Format(
					"%s=%s",
					attribute.Name.GetCString(),
					attribute.Value.GetCString()
				);

				if (i++ != 0)
				{
					string.Append(
						' '
					);
				}

				string.Append(
					_attribute
				);
			}

			return string;
		}

		String ToString_SelfTerminatingWithAttributes() const
		{
			return String::Format(
				"<%s %s />",
				GetType().GetCString(),
				ToString_Attributes().GetCString()
			);
		}

		String ToString_SelfTerminatingWithoutAttributes() const
		{
			return String::Format(
				"<%s />",
				GetType().GetCString()
			);
		}

		String ToString_TrySelfTerminating() const
		{
			return GetAttributeCount() ? ToString_SelfTerminatingWithAttributes() : ToString_SelfTerminatingWithoutAttributes();
		}

		String ToString_TryAttributesChildrenText() const
		{
			return GetAttributeCount() ? ToString_TryAttributesAndChildrenText() : ToString_TryChildrenText();
		}

		String ToString_TryChildrenText() const
		{
			return GetChildCount() ? ToString_TryChildrenAndText() : ToString_TryText();
		}
		
		String ToString_TryAttributesAndChildrenText() const
		{
			return GetChildCount() ? ToString_TryAttributesChildrenAndText() : ToString_TryAttributesAndText();
		}

		String ToString_TryText() const
		{
			return isTextSet ? ToString_TextWrapper() : ToString_Empty();
		}

		String ToString_TryChildrenAndText() const
		{
			return GetChildCount() ? ToString_ChildrenText() : ToString_ChildWrapper();
		}

		String ToString_TryAttributesAndText() const
		{
			return isTextSet ? ToString_AttributesText() : ToString_AttributesEmpty();
		}

		String ToString_TryAttributesChildrenAndText() const
		{
			return isTextSet ? ToString_AttributesChildrenText() : ToString_AttributesChildren();
		}

		String ToString_Empty() const
		{
			return String::Format(
				"<%s></%s>",
				GetType().GetCString(),
				GetType().GetCString()
			);
		}

		String ToString_AttributesText() const
		{
			return String::Format(
				"<%s %s>%s</%s>",
				GetType().GetCString(),
				ToString_Attributes().GetCString(),
				GetText().GetCString(),
				GetType().GetCString()
			);
		}

		String ToString_AttributesEmpty() const
		{
			return String::Format(
				"<%s %s></%s>",
				GetType().GetCString(),
				ToString_Attributes().GetCString(),
				GetType().GetCString()
			);
		}

		String ToString_TextWrapper() const
		{
			return String::Format(
				"<%s>%s</%s>",
				GetType().GetCString(),
				GetText().GetCString(),
				GetType().GetCString()
			);
		}

		String ToString_ChildrenText() const
		{
			return String::Format(
				"<%s>%s%s</%s>",
				GetType().GetCString(),
				GetText().GetCString(),
				ToString_Children().GetCString(),
				GetType().GetCString()
			);
		}
		
		String ToString_ChildWrapper() const
		{
			return String::Format(
				"<%s>%s</%s>",
				GetType().GetCString(),
				ToString_Children().GetCString(),
				GetType().GetCString()
			);
		}

		String ToString_AttributesChildren() const
		{
			return String::Format(
				"<%s %s>%s</%s>",
				GetType().GetCString(),
				ToString_Attributes().GetCString(),
				ToString_Children().GetCString(),
				GetType().GetCString()
			);
		}

		String ToString_AttributesChildrenText() const
		{
			return String::Format(
				"<%s %s>%s%s</%s>",
				GetType().GetCString(),
				ToString_Attributes().GetCString(),
				GetText().GetCString(),
				ToString_Children().GetCString(),
				GetType().GetCString()
			);
		}

#define ELEMENT_HPP_COMPARE_TYPE_AND_RETURN_IF_TRUE(type, type2) if (type.Compare(type2, true)) { return true; }

		static bool TypeCantSelfTerminate(const String& type)
		{
			ELEMENT_HPP_COMPARE_TYPE_AND_RETURN_IF_TRUE(type, "html");
			ELEMENT_HPP_COMPARE_TYPE_AND_RETURN_IF_TRUE(type, "head");
			ELEMENT_HPP_COMPARE_TYPE_AND_RETURN_IF_TRUE(type, "title");
			ELEMENT_HPP_COMPARE_TYPE_AND_RETURN_IF_TRUE(type, "style");
			ELEMENT_HPP_COMPARE_TYPE_AND_RETURN_IF_TRUE(type, "body");
			ELEMENT_HPP_COMPARE_TYPE_AND_RETURN_IF_TRUE(type, "h1");
			ELEMENT_HPP_COMPARE_TYPE_AND_RETURN_IF_TRUE(type, "h2");
			ELEMENT_HPP_COMPARE_TYPE_AND_RETURN_IF_TRUE(type, "h3");
			ELEMENT_HPP_COMPARE_TYPE_AND_RETURN_IF_TRUE(type, "h4");
			ELEMENT_HPP_COMPARE_TYPE_AND_RETURN_IF_TRUE(type, "h5");
			ELEMENT_HPP_COMPARE_TYPE_AND_RETURN_IF_TRUE(type, "h6");
			ELEMENT_HPP_COMPARE_TYPE_AND_RETURN_IF_TRUE(type, "p");
			ELEMENT_HPP_COMPARE_TYPE_AND_RETURN_IF_TRUE(type, "font");
			ELEMENT_HPP_COMPARE_TYPE_AND_RETURN_IF_TRUE(type, "div");
			ELEMENT_HPP_COMPARE_TYPE_AND_RETURN_IF_TRUE(type, "span");
			ELEMENT_HPP_COMPARE_TYPE_AND_RETURN_IF_TRUE(type, "form");
			ELEMENT_HPP_COMPARE_TYPE_AND_RETURN_IF_TRUE(type, "center");
			ELEMENT_HPP_COMPARE_TYPE_AND_RETURN_IF_TRUE(type, "table");
			ELEMENT_HPP_COMPARE_TYPE_AND_RETURN_IF_TRUE(type, "tr");
			ELEMENT_HPP_COMPARE_TYPE_AND_RETURN_IF_TRUE(type, "td");
			ELEMENT_HPP_COMPARE_TYPE_AND_RETURN_IF_TRUE(type, "ul");
			ELEMENT_HPP_COMPARE_TYPE_AND_RETURN_IF_TRUE(type, "li");

			return false;
		}

		static bool TypeMustSelfTerminate(const String& type)
		{
			ELEMENT_HPP_COMPARE_TYPE_AND_RETURN_IF_TRUE(type, "hr");
			ELEMENT_HPP_COMPARE_TYPE_AND_RETURN_IF_TRUE(type, "img");

			return false;
		}

#undef ELEMENT_HPP_COMPARE_TYPE_AND_RETURN_IF_TRUE
	};
}
