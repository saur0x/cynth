namespace cynth
{
	/// Translate `note_id` into a frequency.
	template<typename T = float>
	T scale(int note_id, int scale_id = 0)
	{
		// static float root_12_of_2 = powf(2.0f, 1.0f / 12.0f);
		switch (scale_id) {
		default:
		case 0:
			// return 256.0 * pow(1.0594630943592952645618252949463, note_id);
			return 440.0 * pow(1.0594630943592952645618252949463, note_id);
		}
	}
}

namespace cynth::instrument
{
	template<typename T = float>
	struct Instrument
	{
		T volume;
		cynth::envelope::ADSR<T> envelope;

		virtual T sound(cynth::Note<T>& note, T time, bool& note_finished) = 0;
	};

	template<typename T = float>
	struct Bell : public Instrument<T>
	{
		Bell()
		{
			// https://isocpp.org/wiki/faq/templates#nondependent-name-lookup-members
			this->envelope.attack_time = 0.01;
			this->envelope.decay_time = 1.0;
			this->envelope.release_time = 1.0;
			this->envelope.start_amplitude = 1.0;
			this->envelope.sustain_amplitude = 0.0;

			this->volume = 1.0;
		}

		T sound(cynth::Note<T>& note, T time, bool& note_finished) override
		{
			T amplitude = this->envelope.amplitude(time, note.on_time, note.off_time);
			if (amplitude <= 0.0)
				note_finished = true;

			static cynth::wave::Sine<T> sine;

			T value = sine.oscillate(cynth::scale(note.id + 12), time - note.on_time, 5.0, 0.001)
				+ 0.5 * sine.oscillate(cynth::scale(note.id + 24), time - note.on_time)
				+ 0.25 * sine.oscillate(cynth::scale(note.id + 36), time - note.on_time);

			return this->volume * amplitude * value;
		}
	};

	template<typename T = float>
	struct Harmonica : public Instrument<T>
	{
		Harmonica()
		{
			this->envelope.attack_time = 0.05;
			this->envelope.decay_time = 1.0;
			this->envelope.release_time = 0.1;

			this->envelope.start_amplitude = 1.0;
			this->envelope.sustain_amplitude = 0.95;

			this->volume = 1.0;
		}

		T sound(cynth::Note<T>& note, T time, bool& note_finished) override
		{
			T amplitude = this->envelope.amplitude(time, note.on_time, note.off_time);
			if (amplitude <= 0.0)
				note_finished = true;

			static cynth::wave::Square<T> square;
			static cynth::wave::Noise<T> noise;

			T value = square.oscillate(cynth::scale(note.id), time - note.on_time, 5.0, 0.001)
				+ 0.5 * square.oscillate(cynth::scale(note.id + 12), time - note.on_time)
				+ 0.05 * noise.oscillate(cynth::scale(note.id + 24), time - note.on_time);

			return this->volume * amplitude * value;
		}
	};
}