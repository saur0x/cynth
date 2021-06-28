#include <cstdint>
#include <iostream>
#include <atomic>
#include <mutex>
#include <vector>
#include <algorithm>
#include <memory>

#include <SDL2/SDL.h>

#define CYNTH_T float

#include "cynth/envelope.hpp"
#include "cynth/wave.hpp"
#include "cynth/note.hpp"
#include "cynth/instrument.hpp"
#include "audio/speaker.hpp"


#define SDL_FAIL() { fprintf(stderr, "SDL error: %s", SDL_GetError()); }


std::mutex notes_mutex;


template<typename T>
T clamp(T value, T min, T max)
{
    if (value < min)
        return min;
    else if (value > max)
        return max;
    else
        return value;
}


template<typename T, typename F>
static inline void erase_if(T &vector, F fn)
{
	for (auto i = vector.begin(); i != vector.end();) {
		if (fn(*i))
			i = vector.erase(i);
		else
			++i;
	}
}


class Player
{
	int sample_rate;
	std::vector<std::unique_ptr<cynth::instrument::Instrument<>>>& instruments;

public:
	float time;
	std::vector<cynth::Note<>> notes;

	Player(int sample_rate, std::vector<std::unique_ptr<cynth::instrument::Instrument<>>>& instruments)
		: instruments(instruments)
	{
		this->sample_rate = sample_rate;
		this->time = 0.0;
	}

	void write_samples(float *stream, int len)
	{
		std::unique_lock<std::mutex> lm(notes_mutex);

		static float seconds_per_frame = 1.0 / sample_rate;
		len /= sizeof(float);

		for (size_t i = 0; i < len; ++i) {
			float value = 0.0;

			for (auto& note : notes) {
				bool note_finished = false;

				std::unique_ptr<cynth::instrument::Instrument<>>& instrument = instruments[note.channel];
				value += instrument->sound(note, time, note_finished);

				if (note_finished && note.off_time > note.on_time)
					note.active = false;
			}

			// if (notes.size()) value /= notes.size();
			// value = clamp<float>(value, -1.0, 1.0);
			// stream[i] = 0.2 * value;

			stream[i] = value;
			time += seconds_per_frame;
			erase_if(notes, [](cynth::Note<> const& note) { return !note.active; });
		}
	}

	static void callback(void *userdata, float *stream, int len)
	{
		((Player *) userdata)->write_samples(stream, len);
	}
};


void show_keymap()
{
	puts("___________________________________________________________");
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

	std::string window_title = "SDL Test";

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("ERROR: Couldn't initialize SDL.\n%s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
		window_title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
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

	constexpr int samples = 1024;
	constexpr int sample_rate = 44100;

	std::vector<std::unique_ptr<cynth::instrument::Instrument<>>> instruments;
	instruments.emplace_back(std::make_unique<cynth::instrument::Piano<>>());
	instruments.emplace_back(std::make_unique<cynth::instrument::Bell<>>());
	instruments.emplace_back(std::make_unique<cynth::instrument::Harmonica<>>());

	Player player(sample_rate, instruments);
	cynth::audio::Speaker speaker(sample_rate, (void *) Player::callback, samples, &player);

	std::string keys("zsxcfvgbnjmk,l./");

	show_keymap();
	speaker.play();

    while (running) {
        while (SDL_PollEvent(&event) > 0) {
            switch (event.type) {
				case SDL_QUIT: {
					running = 0;
					break;
				}
				case SDL_KEYDOWN: {
					// Check if note already exists in currently playing notes
					char key = (char) event.key.keysym.sym;
					int index = keys.find(key);
					if (index == std::string::npos)
						break;

					notes_mutex.lock();
					std::vector<cynth::Note<>>& notes = player.notes;

					auto note_found = std::find_if(notes.begin(), notes.end(),
						[&index](cynth::Note<> const& note) { return note.id == index; });
					
					// Note not found in vector
					if (note_found == notes.end()) {
						// Key has been pressed so create a new note
						cynth::Note<> new_note;
						new_note.id = index;
						new_note.on_time = player.time;
						new_note.channel = 0;
						new_note.active = true;

						player.notes.emplace_back(new_note);
					}
					// Key has been pressed again during release phase
					else if (note_found->off_time > note_found->on_time) {
						note_found->on_time = player.time;
						note_found->active = true;
					}

					notes_mutex.unlock();
					break;
				}
				case SDL_KEYUP: {
					char key = (char) event.key.keysym.sym;
					int index = keys.find(key);
					if (index == std::string::npos)
						break;

					notes_mutex.lock();
					std::vector<cynth::Note<>>& notes = player.notes;

					auto note_found = std::find_if(notes.begin(), notes.end(),
						[&index](cynth::Note<> const& note) { return note.id == index; });

					// Key has been released, so switch off
					// Key is up, note is found in vector, and note is not off yet.
					if (note_found != notes.end() && note_found->off_time < note_found->on_time) {
						note_found->off_time = player.time;
					}

					notes_mutex.unlock();
					break;
				}

            }

			// std::wcout << "\rNotes:\t" << player.notes.size() << std::endl;
			// std::this_thread::sleep_for(5ms);
        }

        SDL_RenderPresent(renderer);
    }

	speaker.close();
    SDL_DestroyWindow(window);
    SDL_Quit();

	return 0;
}
