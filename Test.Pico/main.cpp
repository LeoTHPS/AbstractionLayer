#include <AL/Common.hpp>

#include <AL/OS/System.hpp>

int main()
{
	auto dateTime = AL::OS::System::GetDateTime();
	auto timezone = AL::OS::System::GetTimezone();
	auto timestamp = AL::OS::System::GetTimestamp();

	return 0;
}
