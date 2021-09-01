#ifndef CYNTH_PLAYER_H
#define CYNTH_PLAYER_H


#include "instrument.hpp"
#include "note.hpp"
#include <memory>
#include <vector>
#include <mutex>


namespace cynth::utils
{
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
}

namespace cynth
{
	using InstrumentVector = std::vector<std::unique_ptr<Instrument>>;

	class Player
	{
		int sample_rate;
		InstrumentVector& instruments;

	public:
		float time;
		std::vector<Note> notes;
		std::mutex &notes_mutex;

		Player(
			int sample_rate,
			InstrumentVector& instruments,
			std::mutex &notes_mutex)
			: instruments(instruments), notes_mutex(notes_mutex)
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
					if (!note.active)
						continue;

					bool note_finished = false;

					value += instruments[note.channel]->sound(note, time, note_finished);

					if (note_finished && note.off_time > note.on_time)
						note.active = false;
				}

				if (notes.size()) value /= notes.size();
				value = cynth::utils::clamp<float>(value, -1.0, 1.0);

				stream[i] = value;
				time += seconds_per_frame;
			}

			cynth::utils::erase_if(notes, [](Note const& note) { return !note.active; });
		}

		static inline void callback(void *userdata, float *stream, int len)
		{
			((Player *) userdata)->write_samples(stream, len);
		}
	};
}


#endif /* CYNTH_PLAYER_H */