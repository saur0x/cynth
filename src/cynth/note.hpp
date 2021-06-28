namespace cynth
{
	template<typename T = float>
	struct Note
	{
		int id;
		T on_time;
		T off_time;
		bool active;
		int channel;

		Note()
		{
			id = 0;
			on_time = 0.0;
			off_time = 0.0;
			active = false;
			channel = 0;
		}

		T frequency(int offset = 0)
		{
			// pow(2.0, 1.0 / 12.0) = 1.059
			return 27.500 * pow(1.0594630943592952645618252949463, id + offset);
		}
	};
}