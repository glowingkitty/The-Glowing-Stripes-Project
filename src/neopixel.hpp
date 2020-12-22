
#pragma once
#include <Adafruit_NeoPixel.h>

class NeoPixel  
{
	private:
		std::string current_animation;
		int num_pin;
		int num_leds;
		Adafruit_NeoPixel leds;

	public:

		NeoPixel();
		~NeoPixel();

		void glow();
};