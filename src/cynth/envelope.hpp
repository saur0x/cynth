namespace cynth::envelope
{
	template<typename T = float>
	struct Envelope
	{
		virtual T amplitude(T time, T on_time, T off_time) = 0;
	};

	/// FIXED: Releasing the key while it's still in attack phase.
	template<typename T = float>
	class ADSR : public Envelope<T>
	{
	public:
		T attack_time;
		T decay_time;
		T release_time;

		T start_amplitude;
		T sustain_amplitude;
	
		ADSR()
		{
			attack_time = 0.1;
			decay_time = 0.1;
			release_time = 0.2;
			start_amplitude = 1.0;
			sustain_amplitude = 1.0;
		}

		virtual T amplitude(T time, T on_time, T off_time)
		{
			T amplitude = 0.0;
			T life_time = (on_time > off_time) ? time - on_time : off_time - on_time;

			// Common case for both note on and note off
			if (life_time <= attack_time)
				amplitude = life_time / attack_time * start_amplitude;
			else if (life_time <= attack_time + decay_time)
				amplitude = start_amplitude
					- ((life_time - attack_time) / decay_time)
					* (start_amplitude - sustain_amplitude);
			else
				amplitude = sustain_amplitude;
			
			// Note is off
			if (on_time <= off_time)
				amplitude = ((time - off_time) / release_time) * -amplitude + amplitude;

			// Amplitude should not be negative
			if (amplitude <= 0.00001)
				amplitude = 0.0;

			return amplitude;
		}
	};
}