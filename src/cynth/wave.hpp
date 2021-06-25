namespace cynth::wave
{
	constexpr float PI = 3.1415926535f;

	template<typename T = float>
	static inline T hertz_to_omega(T hertz)
	{
		return 2.0 * PI * hertz;
	}

	enum class WaveType : uint8_t {
		Sine, Square, Triangle, Sawtooth, SawtoothAnalog, Noise
	};

	template<typename T = float>
	class Wave
	{
	public:
		virtual T oscillate(T frequency, T time)
		{
			return 0.0;
		}
	};

	template<typename T = float>
	class Sine : public Wave<T>
	{
	public:
		T oscillate(T frequency, T time)
		{
			float sine_wave = sinf(2.0 * PI * frequency * time);
			return sine_wave;
		}
	};

	template<typename T = float>
	class Square : public Wave<T>
	{
	public:
		T oscillate(T frequency, T time)
		{
			float sine_wave = sinf(hertz_to_omega(frequency) * time);
			return (sine_wave >= 0.0) ? 1.0 : -1.0;
		}
	};

	template<typename T = float>
	class Triangle : public Wave<T>
	{
	public:
		T oscillate(T frequency, T time)
		{
			float sine_wave = sinf(2.0 * PI * frequency * time);
			return asinf(sine_wave) * 2.0 / PI;
		}
	};

	template<typename T = float>
	class Sawtooth : public Wave<T>
	{
	public:
		T oscillate(T frequency, T time)
		{
			return 2.0 / PI * (frequency * PI * fmod(time, 1.0 / frequency) - (PI / 2.0));
		}
	};

	template<typename T = float>
	class SawtoothAnalog : public Wave<T>
	{
	public:
		T oscillate(T frequency, T time)
		{
			static const float sawtooth_analog_iterations = 100.0;
			float sine_wave = sinf(2.0 * PI * frequency * time);

			for (float i = 2.0; i < sawtooth_analog_iterations; ++i)
				sine_wave += sinf(i * 2.0 * PI * frequency * time) / i;

			return sine_wave;
		}
	};

	template<typename T = float>
	class Noise : public Wave<T>
	{
	public:
		T oscillate(T frequency, T time)
		{
			return 2.0 * ((T) rand() / (T) RAND_MAX) - 1.0;
		}
	};	
}