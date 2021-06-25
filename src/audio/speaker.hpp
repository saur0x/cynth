namespace cynth::audio
{
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

		// Speaker(SDL_AudioCallback callback, uint16_t samples, void *userdata)
		Speaker(void *callback, uint16_t samples, void *userdata)
		{
			SDL_zero(specification);

			specification.freq = sample_rate;
			specification.format = AUDIO_F32;
			specification.channels = 1;
			specification.samples = samples;
			specification.callback = (SDL_AudioCallback) callback;
			specification.userdata = userdata;

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
}