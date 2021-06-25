namespace cynth::instrument
{
	template<typename T = float>
	struct Instrument
	{
		T volume;
		cynth::EnvelopeADSR<T> envelope;

		virtual T sound(T frequency, T time) = 0;
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
		}

		T sound(T frequency, T time)
		{
			static cynth::wave::Sine<> sine;

			T value = sine.oscillate(frequency * 2.0, time, 5.0, 0.001)
				+ 0.5 * sine.oscillate(frequency * 3.0, time)
				+ 0.25 * sine.oscillate(frequency * 4.0, time);

			value *= this->envelope.get_amplitude(time);
			return value;
		}
	};

	template<typename T = float>
	struct Harmonica : public Instrument<T>
	{
		Harmonica()
		{
			// https://isocpp.org/wiki/faq/templates#nondependent-name-lookup-members
			this->envelope.attack_time = 0.1;
			this->envelope.decay_time = 0.01;
			this->envelope.release_time = 0.02;
			this->envelope.start_amplitude = 1.0;
			this->envelope.sustain_amplitude = 0.8;
		}

		T sound(T frequency, T time)
		{
			static cynth::wave::Square<> square;
			static cynth::wave::Noise<> noise;

			T value = square.oscillate(frequency, time, 5.0, 0.001)
				+ 0.5 * square.oscillate(frequency * 1.5, time)
				+ 0.25 * square.oscillate(frequency * 2.0, time)
				+ 0.05 * noise.oscillate();

			value *= this->envelope.get_amplitude(time);
			return value;
		}
	};
}