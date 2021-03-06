#ifndef CYNTH_SPEAKER_H
#define CYNTH_SPEAKER_H


#include <SDL2/SDL.h>
#include <iostream>


namespace cynth
{
	class Speaker
	{
		SDL_AudioDeviceID device;
		SDL_AudioSpec specification;

	public:
		Speaker(int sample_rate = 44100, uint16_t samples = 256, void *callback = NULL, void *userdata = NULL)
		{
			SDL_zero(specification);

			specification.freq = sample_rate;
			specification.format = AUDIO_F32;
			specification.channels = 1;
			specification.samples = samples;
			specification.callback = (SDL_AudioCallback) callback;
			specification.userdata = userdata;

			device = SDL_OpenAudioDevice(NULL, 0, &specification, NULL, 0);

			if (!device) {
				std::cerr << "Couldn't open audio: " << SDL_GetError() << std::endl;
				exit(-1);
			}
		}

		~Speaker()
		{
			close();
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
}


#endif /* CYNTH_SPEAKER_H */