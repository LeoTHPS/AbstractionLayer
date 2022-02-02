#pragma once
#include "AL/Common.hpp"

#include "AL/Collections/LinkedList.hpp"

namespace AL::Serialization
{
	struct HTMLElementAttribute
	{
		String Name;
		String Value;
	};

	class HTMLElement
	{
		String                                        type;
		String                                        innerHTML;

		Collections::LinkedList<HTMLElement*>         children;
		Collections::LinkedList<HTMLElementAttribute> attributes;

	public:
		HTMLElement()
		{
		}

		HTMLElement(HTMLElement&& element)
			: type(
				Move(element.type)
			),
			innerHTML(
				Move(element.innerHTML)
			),
			children(
				Move(element.children)
			),
			attributes(
				Move(element.attributes)
			)
		{
		}

		HTMLElement(const HTMLElement& element)
			: type(
				element.type
			),
			innerHTML(
				element.innerHTML
			),
			attributes(
				element.attributes
			)
		{
			for (auto lpElement : element.children)
			{
				children.PushBack(
					new HTMLElement(
						*lpElement
					)
				);
			}
		}

		explicit HTMLElement(String&& type)
			: type(
				Move(type)
			)
		{
		}

		virtual ~HTMLElement()
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

		auto& GetInnerHTML() const
		{
			return innerHTML;
		}

		auto GetChildCount() const
		{
			return children.GetSize();
		}

		auto GetAttributeCount() const
		{
			return attributes.GetSize();
		}

		Void SetInnerHTML(String&& value)
		{
			innerHTML = Move(
				value
			);
		}

		auto& AddChild(String&& type)
		{
			auto lpElement = new HTMLElement(
				Move(type)
			);

			children.PushBack(
				lpElement
			);

			return **(--children.end());
		}
		auto& AddChild(String&& type, String&& innerHTML)
		{
			auto& element = AddChild(
				Move(type)
			);

			element.SetInnerHTML(
				Move(innerHTML)
			);

			return element;
		}
		template<size_t S>
		auto& AddChild(String&& type, HTMLElementAttribute(&&attributes)[S])
		{
			auto& element = AddChild(
				Move(type)
			);

			element.AddAttributes(
				Move(attributes),
				typename Make_Index_Sequence<S>::Type {}
			);

			return element;
		}
		template<size_t S>
		auto& AddChild(String&& type, HTMLElementAttribute(&&attributes)[S], String&& innerHTML)
		{
			auto& element = AddChild(
				Move(type),
				Move(attributes)
			);

			element.SetInnerHTML(
				Move(innerHTML)
			);

			return element;
		}

		auto& AddAttribute(String&& name, String&& value)
		{
			for (auto it = attributes.end(); it != attributes.end(); ++it)
			{
				if (it->Name.Compare(name, True))
				{
					it->Value = Move(
						value
					);

					return *it;
				}
			}

			attributes.PushBack(
				HTMLElementAttribute
				{
					.Name  = Move(name),
					.Value = Move(value)
				}
			);

			return *(--attributes.end());
		}

		Void RemoveChild(const HTMLElement& element)
		{
			for (auto it = children.begin(); it != children.end(); ++it)
			{
				if (*it == &element)
				{
					delete *it;

					children.Erase(
						it
					);

					break;
				}
			}
		}

		Void RemoveAttribute(const String& name)
		{
			for (auto it = attributes.begin(); it != attributes.end(); ++it)
			{
				if (it->Name.Compare(name, True))
				{
					attributes.Erase(
						it
					);

					break;
				}
			}
		}

		String ToString() const
		{
			StringBuilder sb;

			sb << '<' << GetType();

			for (auto& attribute : attributes)
			{
				sb << ' ' << attribute.Name;

				if (attribute.Value.GetLength() > 0)
				{

					sb << "=\"" << attribute.Value << '"';
				}
			}

			if ((GetChildCount() > 0) || (GetInnerHTML().GetLength() > 0))
			{
				sb << '>';
			}
			else
			{
				sb << " />";
			}

			for (auto lpElement : children)
			{
				sb << lpElement->ToString();
			}
			
			if (GetInnerHTML().GetLength() > 0)
			{

				sb << GetInnerHTML();
			}
			
			if ((GetChildCount() > 0) || (GetInnerHTML().GetLength() > 0))
			{
				sb << "</" << GetType() << '>';
			}

			return sb.ToString();
		}

		HTMLElement& operator = (HTMLElement&& element)
		{
			type = Move(
				element.type
			);

			innerHTML = Move(
				element.innerHTML
			);

			for (auto it = children.begin(); it != children.end(); )
			{
				delete *it;

				children.Erase(
					it++
				);
			}

			children = Move(
				element.children
			);

			attributes = Move(
				element.attributes
			);

			return *this;
		}
		HTMLElement& operator = (const HTMLElement& element)
		{
			type      = element.type;
			innerHTML = element.innerHTML;

			for (auto it = children.begin(); it != children.end(); )
			{
				delete *it;

				children.Erase(
					it++
				);
			}

			for (auto lpElement : element.children)
			{
				children.PushBack(
					new HTMLElement(
						*lpElement
					)
				);
			}

			attributes = element.attributes;

			return *this;
		}

		Bool operator == (const HTMLElement& element) const
		{
			if (GetType() != element.GetType())
			{

				return False;
			}

			if (GetInnerHTML() != element.GetInnerHTML())
			{

				return False;
			}

			if (GetChildCount() != element.GetChildCount())
			{
				
				return False;
			}

			if (GetAttributeCount() != element.GetAttributeCount())
			{

				return False;
			}

			for (auto it1 = children.cbegin(), it2 = element.children.cbegin(); it1 != children.cbegin(); ++it1, ++it2)
			{
				if (*it1 != *it2)
				{

					return False;
				}
			}

			for (auto it1 = attributes.cbegin(), it2 = element.attributes.cbegin(); it1 != attributes.cbegin(); ++it1, ++it2)
			{
				if (!it1->Name.Compare(it2->Name, True))
				{

					return False;
				}

				if (!it1->Value.Compare(it2->Value, True))
				{

					return False;
				}
			}

			return True;
		}
		Bool operator != (const HTMLElement& element) const
		{
			if (operator==(element))
			{

				return False;
			}

			return True;
		}

	private:
		template<size_t I, size_t S>
		constexpr Bool AddAttributes(HTMLElementAttribute(&&attributes)[S])
		{
			AddAttribute(
				Move(attributes[I].Name),
				Move(attributes[I].Value)
			);

			return True;
		}
		template<size_t S, size_t ... INDEXES>
		constexpr Void AddAttributes(HTMLElementAttribute(&&attributes)[S], Index_Sequence<INDEXES ...>)
		{
			(AddAttributes<INDEXES>(Move(attributes)) && ...);
		}
	};

	class HTMLDocument
	{
		HTMLElement html;

	public:
		HTMLDocument()
			: html(
				"html"
			)
		{
		}

		HTMLDocument(HTMLDocument&& document)
			: html(
				Move(document.html)
			)
		{
		}

		HTMLDocument(const HTMLDocument& document)
			: html(
				document.html
			)
		{
		}

		virtual ~HTMLDocument()
		{
		}

		auto& GetHTML()
		{
			return html;
		}
		auto& GetHTML() const
		{
			return html;
		}

		String ToString() const
		{
			auto html = GetHTML().ToString();

			return html;
		}
	};
}
