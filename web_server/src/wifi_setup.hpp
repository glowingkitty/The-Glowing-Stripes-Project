
#pragma once

#include <string>

class WifiSetup  
{
	private:
		std::string wifi_ssid;
		std::string wifi_password;
		std::string hotspot_ssid;
		std::string hotspot_password;

	public:

		WifiSetup();
		~WifiSetup();

		void start_hotspot();
		boolean connect_to_wifi();
		void start_wifi();
};