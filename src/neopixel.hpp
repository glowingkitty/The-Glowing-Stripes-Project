
#pragma once
#include <Adafruit_NeoPixel.h>
#include <vector>
using namespace std;

class NeoPixel  
{
	private:
		std::string previous_animation;
		std::string new_animation;
		int num_pin;
		int num_leds;
		Adafruit_NeoPixel leds;
		int num_random_colors;
		int counter_current_color;
		vector<vector<int>> rgb_colors;


	public:

		NeoPixel();
		~NeoPixel();

		void generate_random_colors();
		boolean animation_has_changed();

		void glow();
};