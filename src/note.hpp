#ifndef CYNTH_NOTE_H
#define CYNTH_NOTE_H


#include <cmath>


namespace cynth
{
	struct Note
	{
		int id;

		float on_time;
		float off_time;

		bool active;
		int channel;

		Note()
		{
			id = 0;
			on_time = 0.0;
			off_time = 0.0;
			active = false;
			channel = 0;
		}

		float frequency(int offset = 0)
		{
			// pow(2.0, 1.0 / 12.0) = 1.059...
			// 256.0, 440.0
			static const float mul = 27.5f;
			return mul * std::pow(1.0594630943592952645618252949463f, id + offset);
		}
	};
}


#endif /* CYNTH_NOTE_H */