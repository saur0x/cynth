## TODO
- Remove base type as float template argument.

## Notes
- Octaves and semi-tones
- Frequency modulation
	- Texture to the sound
	- Carrier frequency and message frequency
- Polyphony
	- More than one notes at the same time.
	- An oscillator for every note on the keyboard constantly output even if zero.
	- or only active notes output its sound and has its own oscillator.
	- Store all active notes in a vector.
	- Another function reads from this vector, generates and mixes the frequencies.
-  There seems to be a little mix-up in terms. You clearly implement lfo that modulates the pitch of an osc; usually in fm synths the modulator frequencies are in audible range (and indeed most often multiples or harmonic overtones of the carrier frequency). but most of the fm synths do incorporate a lfo for the main carrier frequency and/or amplitude as well. the bell sounds are prime candidates to be recreated with fm sounds but they actually have dissonant overtones from the main frequency. also one of the most characteristic thing in adlib/sb synth was a feedback feature, that created very rough harmonics if cranked up to 11.
- Sound card | DAC | Digital to Analog Converter

## Library Search
- <https://stackoverflow.com/questions/10184956/sound-api-ubuntu-linux>

## Rust
- <https://github.com/RustAudio>
- <https://github.com/RustAudio/rust-portaudio>
- <https://github.com/RustAudio/synth>

## Resources
- <https://stackoverflow.com/questions/8777724/store-derived-class-objects-in-base-class-variables>
- [Add](https://theorie.ikp.physik.tu-darmstadt.de/qcd/moore/ph224/notes/lecture14.pdf)

## References
- [LIVESTREAM: Real-time audio programming in C++ from first principles](https://youtu.be/HVkcdhqWp5s "@Bartholomew")
- [Audio in Standard C++](https://youtu.be/UvRU25T_XOg "@Coding Tech")
- [Introduction to Sound Programming with ALSA](https://www.linuxjournal.com/article/6735)
- <https://isocpp.org/wiki/faq/templates#nondependent-name-lookup-members>