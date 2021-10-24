#pragma once
#include <AL/Common.hpp>

#include <AL/OS/Console.hpp>

#include <AL/HTML/Document.hpp>

static void AL_HTML_Document()
{
	using namespace AL;
	using namespace AL::HTML;

	Document document;

	auto& html           = document.GetHTML();

	auto& head           = html.AddChild("head");
	auto& head_Title     = head.AddChild("title", "Abstraction Layer");
	auto& head_Style     = head.AddChild("style",
		"body"
		"{"
			"color: #000;"
			"background: #fff;"
		"}"
	);

	auto& body           = html.AddChild("body");
	auto& body_Content   = body.AddChild("div",
		{
			DOM::ElementAttribute { .Name = "name",  .Value = "content" },
			DOM::ElementAttribute { .Name = "style", .Value = "color: #00FF00;" }
		},
		"Hello World!"
	);

	// prevent optimization when AL_TEST_SHOW_CONSOLE_OUTPUT is not defined
	volatile auto source = document.ToString();

#if defined(AL_TEST_SHOW_CONSOLE_OUTPUT)
	OS::Console::WriteLine(
		const_cast<const String&>(source) // use const_cast to remove volatile
	);
#endif
}
