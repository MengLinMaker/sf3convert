# sf3convert
Compress SoundFont, from `.sf2` to `.sf3`:
* Achieve 10x lossy compression with OGG VORBIS.
* Used in [MuseScore](http://musescore.org).

This is a fork of [MuseScore sftools](https://github.com/musescore/sftools) with more compliance with [RIFF](https://johnloomis.org/cpe102/asgn/asgn1/riff.html) format.

## Compilation
Ensure `make`, `cmake`, `ninja` and `conan` are installed beforehand.
1. Install dependencies `make install`.
2. Compile program `make prod`.
3. Test program `make test-prod`.
4. Generate doxygen doc `make doc`.

## Usage Example:

This compresses the sample piano SoundFont from 21.8 MBytes to 1.8 MBytes.

```Bash
build.release/sf3convert convert test/sample.sf2 test/sample.sf3
```

The compressed SoundFont is non standard - mostly used by [MuseScore](http://musescore.org).

## Todo:
* Currently stereo samples are compressed as two single streams instead of compressing them as stereo ogg vorbis streams. This may be less optimal.
* Adhere to RIFF chunk size rules.
