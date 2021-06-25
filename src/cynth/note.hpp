namespace cynth
{
	template<typename T = float>
	struct Note
	{
		// Storing id of the key on keyboard instead of the frequency.
		// Position in scale
		int id;
		T on_time;
		T off_time;
		bool active;

		// The instrument
		int channel;

		Note()
			: id(0), on_time(0.0), off_time(0.0), active(false), channel(0);
		{}
	}
}