#ifndef CYNTH_INSTRUMENT_H
#define CYNTH_INSTRUMENT_H


#include "wave.hpp"
#include "note.hpp"
#include "envelope.hpp"


namespace cynth
{
	float scale(int note_id, int scale_id = 0)
	{
		static float scale_coefficient = 27.500;

		// static float root_12_of_2 = powf(2.0f, 1.0f / 12.0f);
		switch (scale_id) {
		default:
		case 0:
			return scale_coefficient * powf(1.0594630943592952645618252949463f, note_id);
		}
	}
}


namespace cynth
{
	struct Instrument
	{
		float volume;
		cynth::ADSR envelope;

		virtual float sound(cynth::Note& note, float time, bool& note_finished) = 0;
	};

	struct Bell : public Instrument
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

		float sound(cynth::Note& note, float time, bool& note_finished) override
		{
			float amplitude = this->envelope.amplitude(time, note.on_time, note.off_time);
			if (amplitude <= 0.0)
				note_finished = true;

			static cynth::Sine sine;

			float value = sine.oscillate(cynth::scale(note.id + 12), time - note.on_time, 5.0, 0.001)
				+ 0.5 * sine.oscillate(cynth::scale(note.id + 24), time - note.on_time)
				+ 0.25 * sine.oscillate(cynth::scale(note.id + 36), time - note.on_time);

			return this->volume * amplitude * value;
		}
	};

	struct Harmonica : public Instrument
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

		float sound(cynth::Note& note, float time, bool& note_finished) override
		{
			float amplitude = this->envelope.amplitude(time, note.on_time, note.off_time);
			if (amplitude <= 0.0)
				note_finished = true;

			static cynth::Square square;
			static cynth::Noise noise;

			float value = square.oscillate(cynth::scale(note.id), time - note.on_time, 5.0, 0.001)
				+ 0.5 * square.oscillate(cynth::scale(note.id + 12), time - note.on_time)
				+ 0.05 * noise.oscillate();

			return this->volume * amplitude * value;
		}
	};

	struct Piano : public Instrument
	{
		Piano()
		{
			this->envelope.attack_time = 0.1;
			this->envelope.decay_time = 0.3;
			this->envelope.release_time = 0.2;

			this->envelope.start_amplitude = 1.0;
			this->envelope.sustain_amplitude = 0.8;

			this->volume = 1.0;
		}

		float sound(cynth::Note& note, float time, bool& note_finished) override
		{
			float amplitude = this->envelope.amplitude(time, note.on_time, note.off_time);

			if (amplitude <= 0.0)
				note_finished = true;

			static cynth::Sine sine;

			float value = sine.oscillate(cynth::scale(note.id), time - note.on_time, 5.0, 0.001)
				+ 0.5 * sine.oscillate(cynth::scale(note.id >= 12 ? note.id - 12 : note.id), time - note.on_time)
				+ 0.25 * sine.oscillate(cynth::scale(note.id + 12 < 88 ? note.id + 12 : note.id), time - note.on_time);

			return this->volume * amplitude * value;
		}
	};
}

#endif /* CYNTH_INSTRUMENT_H */