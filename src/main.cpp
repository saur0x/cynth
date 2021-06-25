#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <SDL2/SDL.h>

#include "cynth/wave.hpp"
#include "audio/speaker.hpp"


#define SDL_FAIL() { fprintf(stderr, "SDL error: %s", SDL_GetError()); }


template<typename T = float>
T clamp(T value, T min, T max)
{
    if (value < min)
        return min;
    else if (value > max)
        return max;
    else
        return value;
}

/// Returns a value in range (-1.0 to +1.0) as a function of time.
void make_noise(void *userdata, float *stream, int len)
{
	static float frequency = 440.0;
	static float amplitude = 0.2;

	len /= sizeof(float);

	for (size_t i = 0; i < len; ++i) {
		float time = (float) SDL_GetTicks() / 1000.0f;
		float wave = ((cynth::wave::Sine<> *) userdata)->oscillate(frequency, time);
		wave *= amplitude;
		stream[i] = wave;
	}

    // float amplitude = envelope_ADSR_get_amplitude(&envelope, time);
    // float wave =
    //     1.0f * oscillate((float) atomic_frequency * 0.5f, time, SAWTOOTH_DIGITAL_WAVE)
    //     + 1.0f * oscillate((float) atomic_frequency, time, SAWTOOTH_DIGITAL_WAVE);
    // float sound = amplitude * wave;
    // // printf("%f %f %f\n", amplitude, wave, sound);
    // return clamp(sound * 0.4, -1.0f, 1.0f);
}

class Player
{
	int sample_rate;
	cynth::wave::Wave<>& wave;
	float time;
	float frequency;
	float amplitude = 0.2;

public:
	Player(float frequency, int sample_rate, cynth::wave::Wave<>& wave)
		: frequency(frequency), sample_rate(sample_rate), wave(wave), time(0.0)
	{}

	void set_frequency(float frequency)
	{
		this->frequency = frequency;
	}

	void write_samples(float *stream, int len)
	{

		len /= sizeof(float);

		// float time = (float) SDL_GetTicks() / 1000.0f;
		float seconds_per_frame = 1.0 / sample_rate;

		for (size_t i = 0; i < len; ++i) {
			float value = wave.oscillate(frequency, time);
			value *= amplitude;
			stream[i] = value;
			time += seconds_per_frame;
		}
	}

	static void callback(void *userdata, float *stream, int len)
	{
		((Player *) userdata)->write_samples(stream, len);

		// static float frequency = 440.0;
		// static float amplitude = 0.2;

		// len /= sizeof(float);

		// // float time = (float) SDL_GetTicks() / 1000.0f;
		// float seconds_per_frame = 1.0 / self->sample_rate;
		// float time = self->time;

		// for (size_t i = 0; i < len; ++i) {
		// 	float wave = self->wave.oscillate(frequency, time);
		// 	wave *= amplitude;
		// 	stream[i] = wave;
		// 	time += seconds_per_frame;
		// }

		// self->time = time;
	}
};

void show_keymap()
{
	puts("|   |   |   |   |   | |   |   |   |   | |   | |   |   |   |");
	puts("|   | S |   |   | F | | G |   |   | J | | K | | L |   |   |");
	puts("|   |___|   |   |___| |___|   |   |___| |___| |___|   |   |__");
	puts("|     |     |     |     |     |     |     |     |     |     |");
	puts("|  Z  |  X  |  C  |  V  |  B  |  N  |  M  |  ,  |  .  |  /  |");
	puts("|_____|_____|_____|_____|_____|_____|_____|_____|_____|_____|");
}

int main(int argc, char **argv)
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    // SDL begin
    char window_title[] = "SDL Test";

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("ERROR: Couldn't initialize SDL.\n%s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
		window_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		800, 400, SDL_WINDOW_RESIZABLE
	);

    if (window == NULL)
    {
        printf("SDL couldn't create the specified window.\n");
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Event event;
    int running = 1;

	constexpr float PI = 3.1415926535f;
	constexpr int sample_rate = 44100;
	// cynth::audio::Speaker<sample_rate> speaker;


	float frequency = 440.0f;

	float seconds_per_frame = 1.0 / (float) sample_rate;
	float play_time = 3.0;
	float time = 0.0;

	// size_t buffer_s = 2048;
	// float buffer[buffer_s];


	// cynth::wave::Square<> sine;
	// while (time < 3.0) {
	// 	for (size_t i = 0; i < buffer_s; ++i) {
	// 		buffer[i] = 0.1 * sine.oscillate(frequency, time);
	// 		time += seconds_per_frame;
	// 	}

	// 	speaker.queue(buffer, buffer_s);
	// }

	constexpr int samples = 1024;
	cynth::wave::Square<> sine;
	Player player(0.0, sample_rate, sine);

	cynth::audio::Speaker<sample_rate> speaker((void *) Player::callback, samples, &player);

	// for (int i = 0; i < 10; ++i) {
	// 	speaker.pause(i % 2);
	// 	SDL_Delay(100);
	// }

	constexpr float octave_base_frequency = 4.0 * 110.0f;
	static float root_12_of_2 = powf(2.0f, 1.0f / 12.0f);

	std::string keys("zsxcfvgbnjmk,l./");
	std::unordered_map<char, int> keymap;
	for (int i = 0; i < keys.size(); ++i) {
		keymap[keys[i]] = i;
	}

	std::unordered_map<char, bool> keystate;
	for (auto c : keys) {
		keystate[c] = false;
	}

	show_keymap();
	speaker.play();

    while (running) {
        while (SDL_PollEvent(&event) > 0) {
            switch (event.type) {
				case SDL_QUIT:
					running = 0;
					break;

				case SDL_KEYDOWN: {
					char key = (char) event.key.keysym.sym;
					if (keystate.find(key) == keystate.end() || keystate[key])
						break;

					keystate[key] = true;
					player.set_frequency(octave_base_frequency * powf(root_12_of_2, keymap[key]));
					break;
				}

				case SDL_KEYUP: {
					char key = (char) event.key.keysym.sym;
					if (keystate.find(key) == keystate.end() || !keystate[key])
						break;

					keystate[key] = false;
					player.set_frequency(0.0);
					break;
				}
            }
        }

        SDL_RenderPresent(renderer);
    }

	speaker.close();
    SDL_DestroyWindow(window);
    SDL_Quit();

	return 0;
}