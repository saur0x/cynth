#include <cstdint>
#include <iostream>
#include <SDL2/SDL.h>

#define SDL_FAIL() { fprintf(stderr, "SDL error: %s", SDL_GetError()); }

constexpr float PI = 3.1415926535f;


template<int sample_rate = 44100>
class Speaker
{
    SDL_AudioDeviceID device;
    SDL_AudioSpec specification;

public:
	Speaker()
	{
		SDL_zero(specification);

		specification.freq = sample_rate;
		specification.format = AUDIO_F32;
		specification.channels = 1;
		specification.samples = 1024;
		specification.callback = NULL;

		if (!(device = SDL_OpenAudioDevice(NULL, 0, &specification, NULL, 0)));
	}

	~Speaker()
	{
	}

	void close()
	{
		SDL_CloseAudioDevice(device);
	}

	void queue(const void *data, Uint32 len)
	{
        SDL_QueueAudio(device, data, len * sizeof(float));
	}

	void play()
	{
		SDL_PauseAudioDevice(device, 0);
	}

	void pause(bool pause_on = true)
	{
		SDL_PauseAudioDevice(device, pause_on);
	}
};


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

	template<typename T = float>
	class Wave
	{
	public:
		virtual T oscillate(T time, T frequency);
	};

	template<typename T = float>
	class SineWave : public Wave<T>
	{
	public:
		T oscillate(T time, T frequency)
		{
			return 0.2f * sinf(2.0f * PI * frequency * time);
		}
	};

	// enum class Wave : uint8_t {
	// 	Sine, Square, Triangle, Sawtooth, SawtoothAnalog, Noise
	// };
}

int main(int argc, char **argv)
{
    SDL_Init(SDL_INIT_AUDIO);

	constexpr int sample_rate = 44100;
	Speaker<sample_rate> speaker;

	float frequency = 440.0f;

	float seconds_per_frame = 1.0 / (float) sample_rate;
	float play_time = 3.0;
	float time = 0.0;

	size_t buffer_s = 2048;
	float buffer[buffer_s];

	cynth::SineWave<> sine;

	while (time < 3.0) {
		for (size_t i = 0; i < buffer_s; ++i) {
			buffer[i] = sine.oscillate(time, frequency);
			time += seconds_per_frame;
		}

		speaker.queue(buffer, buffer_s);
	}

	speaker.play();

    SDL_Delay(play_time * 1000);

	speaker.close();

    SDL_Quit();

	return 0;
}