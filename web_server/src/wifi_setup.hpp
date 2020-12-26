
#pragma once

#include <string>

class WifiSetup  
{
	private:
		std::string wifi_ssid;
		std::string wifi_password;
		std::string hotspot_ssid;
		std::string hotspot_password;
		std::string role;

	public:

		WifiSetup();
		~WifiSetup();

		std::string get_role();

		void start_hotspot();
		boolean connect_to_wifi();
		void start_wifi();

		boolean host_is_online();
};