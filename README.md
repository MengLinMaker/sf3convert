# sf3convert
Compress SoundFont, from `.sf2` to `.sf3`:
* Achieve 10x lossy compression with OGG VORBIS.
* Used in [MuseScore](http://musescore.org).

This is a fork of [MuseScore sftools](https://github.com/musescore/sftools) with more compliance with [RIFF](https://johnloomis.org/cpe102/asgn/asgn1/riff.html) format.

## Compilation

Install dependencies:
* `Qt >= 6`
* `libsndfile >= 1.0.25`
* `libogg >= 1.3.3`
* `libvorbis >= 1.3.0`

Build binary:
```Bash
make release
```

## Usage Example:

This compresses the sample piano SoundFont from 21.8 MBytes to 1.8 MBytes.

```Bash
build.release/sf3convert -z test/piano.sf2 test/piano.sf3
```

The compressed SoundFont is non standard - mostly used by [MuseScore](http://musescore.org).

## Todo:
* Currently stereo samples are compressed as two single streams instead of compressing them as stereo ogg vorbis streams. This may be less optimal.
* Adhere to RIFF chunk size rules.
