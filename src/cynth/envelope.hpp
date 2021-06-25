namespace cynth
{
	/// Attack, decay, sustain, release
	template<typename T = float>
	class EnvelopeADSR
	{
	private:
		T attack_time;
		T decay_time;
		T release_time;

		T start_amplitude;
		T sustain_amplitude;

		T on_time;
		T off_time;

		bool is_note_on;

	public:
		EnvelopeADSR()
			: attack_time(0.0), decay_time(0.0), release_time(0.0),
			start_amplitude(0.0), sustain_amplitude(0.0), is_note_on(false)
		{}

		void note_on(T time)
		{
			is_note_on = true;
			on_time = time;
		}

		void note_off(T time)
		{
			is_note_on = false;
			on_time = time;
		}

		T get_amplitude(T time)
		{
			T amplitude = 0.0;
			T life_time = time - on_time;

			if (is_note_on) {
				if (life_time <= attack_time) {
					amplitude = life_time / attack_time * start_amplitude;
				} else if (life_time <= attack_time + decay_time) {
					amplitude = start_amplitude
						- ((life_time - attack_time) / decay_time)
						* (start_amplitude - sustain_amplitude);
				} else {
					amplitude = sustain_amplitude;
				}
			} else {
				amplitude = sustain_amplitude
					- ((time - off_time) / release_time) * sustain_amplitude;
			}

			// Amplitude should not be negative
			if (amplitude <= 0.0001)
				amplitude = 0.0;

			return amplitude;
		}
	};
}