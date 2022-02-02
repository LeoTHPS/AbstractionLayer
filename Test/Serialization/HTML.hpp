#pragma once
#include <AL/Common.hpp>

#include <AL/OS/Console.hpp>

#include <AL/Serialization/HTML.hpp>

// @throw AL::Exception
static void AL_Serialization_HTML()
{
	using namespace AL;
	using namespace AL::Serialization;

	HTMLDocument document;

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
			HTMLElementAttribute { .Name = "name",  .Value = "content" },
			HTMLElementAttribute { .Name = "style", .Value = "color: #00FF00;" }
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
