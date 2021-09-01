#ifndef CYNTH_INSTRUMENT_H
#define CYNTH_INSTRUMENT_H


#include "wave.hpp"
#include "note.hpp"
#include "envelope.hpp"


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
			float time_elapsed = time - note.on_time;

			float value = sine.oscillate(note.frequency(12), time_elapsed, 5.0, 0.001)
				+ 0.5 * sine.oscillate(note.frequency(24), time_elapsed)
				+ 0.25 * sine.oscillate(note.frequency(36), time_elapsed);

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
			float time_elapsed = time - note.on_time;

			float value = square.oscillate(note.frequency(), time_elapsed, 5.0, 0.001)
				+ 0.5 * square.oscillate(note.frequency(12), time_elapsed)
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
			float time_elapsed = time - note.on_time;

			float value = sine.oscillate(note.frequency(), time_elapsed, 5.0, 0.001)
				+ 0.5 * sine.oscillate(note.frequency(note.id >= 12 ? -12 : 0), time_elapsed)
				+ 0.25 * sine.oscillate(note.frequency(note.id + 12 < 88 ? 12 : 0), time_elapsed);

			return this->volume * amplitude * value;
		}
	};
}

#endif /* CYNTH_INSTRUMENT_H */