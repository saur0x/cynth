#include <cstdint>
#include <iostream>
#include <atomic>
#include <mutex>
#include <vector>
#include <algorithm>
#include <memory>

#include <SDL2/SDL.h>


#include "envelope.hpp"
#include "wave.hpp"
#include "note.hpp"
#include "instrument.hpp"
#include "speaker.hpp"
#include "player.hpp"

#include <unistd.h>
#include "midi_parser/midi_parser.h"


#define REAL_TIME


std::mutex notes_mutex;


void show_keyboard(uint8_t *notes, size_t size, FILE *output)
{
    for (size_t i = 0; i <= 108 - 21; ++i) {
        putc(notes[i] ? 'H' : '.', output);
    }
    putc('\n', output);
}


int main(int argc, char **argv)
{
    FILE
    *midi = stdin,
    *output = stderr;

    switch (argc) {
    case 3:
        output = fopen(argv[2], "wb");
    case 2:
        midi = fopen(argv[1], "rb");
    }


    SDL_Init(SDL_INIT_AUDIO);

	constexpr int samples = 256;
	constexpr int sample_rate = 44100;

	std::vector<std::unique_ptr<cynth::Instrument>> instruments;
	instruments.emplace_back(std::make_unique<cynth::Piano>());
	// instruments.emplace_back(std::make_unique<cynth::Harmonica>());
	// instruments.emplace_back(std::make_unique<cynth::Bell>());

	cynth::Player player(sample_rate, instruments, notes_mutex);
	cynth::Speaker speaker(sample_rate, samples, (void *) cynth::Player::callback, &player);

	struct midi_parser *parser = midi_parser_new(NULL, midi);
	struct midi_event event;
    uint8_t index, event_on, notes[128] = { 0 };


	speaker.play();

	for (; !parser->end_of_file; parser->timestamp += parser->dtime) {
		midi_parser_next(parser, midi, &event);

		switch (MIDI_EVENT_TYPE(&event)) {
			case EventNoteOn: {
				index = event.midi_data[0] - 21;
				event_on = event.midi_data[1] != 0;
				notes[index] = event_on;

				notes_mutex.lock();

				std::vector<cynth::Note>& notes = player.notes;

				auto note_found = std::find_if(notes.begin(), notes.end(),
					[&index](cynth::Note const& note) { return note.id == index; });

				if (event_on) {
					if (note_found == notes.end()) {
						cynth::Note new_note;
						new_note.id = index;
						new_note.on_time = player.time;
						new_note.channel = 0;
						new_note.active = true;

						player.notes.emplace_back(new_note);
					} else if (note_found->off_time > note_found->on_time) {
						note_found->on_time = player.time;
						note_found->active = true;
					}
				} else {
					if (note_found != notes.end() && note_found->off_time < note_found->on_time) {
						note_found->off_time = player.time;
					}
				}

				notes_mutex.unlock();
				break;
			}

			case EventNoteOff: {
				index = event.midi_data[0] - 21;
				event_on = 0;
				notes[index] = event_on;

				notes_mutex.lock();

				std::vector<cynth::Note>& notes = player.notes;

				auto note_found = std::find_if(notes.begin(), notes.end(),
					[&index](cynth::Note const& note) { return note.id == index; });

				if (note_found != notes.end() && note_found->off_time < note_found->on_time) {
					note_found->off_time = player.time;
				}

				notes_mutex.unlock();
				break;
			}
		}

        #ifdef SHOW_KEYBOARD
            show_keyboard(notes, 128, stderr);
        #endif
        #ifdef REAL_TIME
            usleep((useconds_t) MIDI_DELAY(parser));
        #endif
	}

	free(parser);

    fclose(midi);
    fclose(output);

	speaker.close();
    SDL_Quit();

    return 0;
}
