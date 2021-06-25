namespace cynth::wave
{
	constexpr float PI = 3.1415926535f;

	template<typename T = float>
	static inline T hertz_to_omega(T hertz)
	{
		return 2.0 * PI * hertz;
	}

	template<typename T = float>
	static inline T modulated_frequency(T frequency, T time, T LFOfrequency, T LFOamplitude)
	{
		return hertz_to_omega(frequency) * time
			+ LFOamplitude * frequency * sin(hertz_to_omega(LFOfrequency) * time);
	}

	enum class WaveType : uint8_t {
		Sine, Square, Triangle, Sawtooth, SawtoothAnalog, Noise
	};

	template<typename T = float>
	class Wave
	{
	public:
		virtual T oscillate(T frequency, T time, T LFOfrequency = 0.0, T LFOamplitude = 0.0)
		{
			return 0.0;
		}
	};

	template<typename T = float>
	class Sine : public Wave<T>
	{
	public:
		T oscillate(T frequency, T time, T LFOfrequency = 0.0, T LFOamplitude = 0.0)
		{
			// float sine_wave = sinf(
			// 	hertz_to_omega(frequency) * time
			// 	+ 0.01 * frequency * sinf(hertz_to_omega(5.0) * time));
			float sine_wave = sin(modulated_frequency(frequency, time, LFOfrequency, LFOamplitude));
			return sine_wave;
		}
	};

	template<typename T = float>
	class Square : public Wave<T>
	{
	public:
		T oscillate(T frequency, T time, T LFOfrequency = 0.0, T LFOamplitude = 0.0)
		{
			// float sine_wave = sinf(hertz_to_omega(frequency) * time);
			float sine_wave = sin(modulated_frequency(frequency, time, LFOfrequency, LFOamplitude));
			return (sine_wave >= 0.0) ? 1.0 : -1.0;
		}
	};

	template<typename T = float>
	class Triangle : public Wave<T>
	{
	public:
		T oscillate(T frequency, T time, T LFOfrequency = 0.0, T LFOamplitude = 0.0)
		{
			// float sine_wave = sinf(2.0 * PI * frequency * time);
			float sine_wave = sin(modulated_frequency(frequency, time, LFOfrequency, LFOamplitude));
			return asinf(sine_wave) * (2.0 / PI);
		}
	};

	template<typename T = float>
	class Sawtooth : public Wave<T>
	{
	public:
		T oscillate(T frequency, T time, T LFOfrequency = 0.0, T LFOamplitude = 0.0)
		{
			return 2.0 / PI * (frequency * PI * fmod(time, 1.0 / frequency) - (PI / 2.0));
		}
	};

	template<typename T = float>
	class SawtoothAnalog : public Wave<T>
	{
	public:
		T oscillate(T frequency, T time, T LFOfrequency = 0.0, T LFOamplitude = 0.0, T iterations = 50.0)
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

	template<typename T = float>
	class Noise : public Wave<T>
	{
	public:
		T oscillate(T frequency = 0.0, T time = 0.0)
		{
			return 2.0 * ((T) rand() / (T) RAND_MAX) - 1.0;
		}
	};	
}