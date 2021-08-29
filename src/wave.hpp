#ifndef CYNTH_WAVE_H
#define CYNTH_WAVE_H


namespace cynth
{
	constexpr float PI = 3.1415926535f;

	static inline float hertz_to_omega(float hertz)
	{
		return 2.0 * PI * hertz;
	}

	static inline float modulated_frequency(float frequency, float time, float LFOfrequency, float LFOamplitude)
	{
		return hertz_to_omega(frequency) * time
			+ LFOamplitude * frequency * sin(hertz_to_omega(LFOfrequency) * time);
	}


	struct Wave
	{
		virtual float oscillate(float frequency, float time, float LFOfrequency = 0.0, float LFOamplitude = 0.0)
		{
			return 0.0;
		}
	};

	struct Sine : public Wave
	{
		float oscillate(float frequency, float time, float LFOfrequency = 0.0, float LFOamplitude = 0.0)
		{
			// float sine_wave = sinf(
			// 	hertz_to_omega(frequency) * time
			// 	+ 0.01 * frequency * sinf(hertz_to_omega(5.0) * time));
			float sine_wave = sin(modulated_frequency(frequency, time, LFOfrequency, LFOamplitude));
			return sine_wave;
		}
	};

	struct Square : public Wave
	{
		float oscillate(float frequency, float time, float LFOfrequency = 0.0, float LFOamplitude = 0.0)
		{
			// float sine_wave = sinf(hertz_to_omega(frequency) * time);
			float sine_wave = sin(modulated_frequency(frequency, time, LFOfrequency, LFOamplitude));
			return (sine_wave >= 0.0) ? 1.0 : -1.0;
		}
	};

	struct Triangle : public Wave
	{
		float oscillate(float frequency, float time, float LFOfrequency = 0.0, float LFOamplitude = 0.0)
		{
			// float sine_wave = sinf(2.0 * PI * frequency * time);
			float sine_wave = sin(modulated_frequency(frequency, time, LFOfrequency, LFOamplitude));
			return asinf(sine_wave) * (2.0 / PI);
		}
	};

	struct Sawtooth : public Wave
	{
		float oscillate(float frequency, float time, float LFOfrequency = 0.0, float LFOamplitude = 0.0)
		{
			return 2.0 / PI * (frequency * PI * fmod(time, 1.0 / frequency) - (PI / 2.0));
		}
	};

	struct SawtoothAnalog : public Wave
	{
		float oscillate(float frequency, float time, float LFOfrequency = 0.0, float LFOamplitude = 0.0, float iterations = 50.0)
		{
			float output = 0.0;

			// for (float i = 1.0; i < sawtooth_analog_iterations; ++i)
			// 	sine_wave += sinf(i * hertz_to_omega(frequency) * time) / i;

			float mfrequency = modulated_frequency(frequency, time, LFOfrequency, LFOamplitude);
			for (float i = 1.0; i < iterations; ++i)
				output += sinf(i * mfrequency) / i;

			return output;
		}
	};

	struct Noise
	{
		float oscillate()
		{
			return 2.0 * ((float) rand() / (float) RAND_MAX) - 1.0;
		}
	};
}


#endif /* CYNTH_WAVE_H */