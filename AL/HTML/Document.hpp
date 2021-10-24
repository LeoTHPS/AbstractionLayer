#pragma once
#include "AL/Common.hpp"

#include "DOM/Element.hpp"

namespace AL::HTML
{
	class Document
	{
		DOM::Element html;

	public:
		Document()
			: html(
				"html"
			)
		{
		}

		Document(Document&& document)
			: html(
				Move(document.html)
			)
		{
		}

		Document(const Document& document)
			: html(
				document.html
			)
		{
		}

		virtual ~Document()
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
