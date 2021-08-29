#ifndef CYNTH_ENVELOPE_H
#define CYNTH_ENVELOPE_H


namespace cynth
{
	struct ADSR
	{
		float attack_time;
		float decay_time;
		float release_time;

		float start_amplitude;
		float sustain_amplitude;

		ADSR()
		{
			attack_time = 0.1;
			decay_time = 0.1;
			release_time = 0.2;
			start_amplitude = 1.0;
			sustain_amplitude = 1.0;
		}

		float amplitude(float time, float on_time, float off_time)
		{
			float current_amplitude = 0.0;
			float life_time = (on_time > off_time) ? time - on_time : off_time - on_time;

			// Common case for both note on and note off
			if (life_time <= attack_time)
				current_amplitude = life_time / attack_time * start_amplitude;
			else if (life_time <= attack_time + decay_time)
				current_amplitude = start_amplitude
					- ((life_time - attack_time) / decay_time)
					* (start_amplitude - sustain_amplitude);
			else
				current_amplitude = sustain_amplitude;

			// Note is off
			if (on_time <= off_time)
				current_amplitude = ((time - off_time) / release_time) * -current_amplitude + current_amplitude;

			// Amplitude should not be negative
			if (current_amplitude <= 0.00001)
				current_amplitude = 0.0;

			return current_amplitude;
		}
	};
}


#endif /* CYNTH_ENVELOPE_H */