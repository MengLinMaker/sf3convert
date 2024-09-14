<h1 align="center">sf3convert</h1>

<div flex align="center">
<img src="https://img.shields.io/github/v/release/MengLinMaker/sf3convert">
<img src="https://img.shields.io/github/actions/workflow/status/menglinmaker/sf3convert/CI.yml?label=CI">
</div>

Compress SoundFont, from `.sf2` to `.sf3`.

This is a fork of [MuseScore sftools](https://github.com/musescore/sftools) with more compliance with [RIFF](https://johnloomis.org/cpe102/asgn/asgn1/riff.html) format. The compressed SoundFont3 is non standard

## [Download](https://github.com/MengLinMaker/sftools/releases)

Precompiled binaries are available for:
- linux: `ubuntu-sf3convert.tar.xz`
- macos: `macos-sf3convert.tar.xz`

## Usage example:

List commands and flags:

```Bash
sf3convert -h
```

Compresses sample SoundFont from 28.9 mb to 5.5 mb:

```Bash
sf3convert convert -q 0 -a 0 test/sample.sf2 test/sample.sf3
```

Dump all SoundFont preset names:

```Bash
sf3convert dump test/sample.sf2
```

## Compilation
Ensure `make`, `cmake`, `ninja` and `conan` are installed beforehand.
1. Install dependencies `make install`.
2. Compile program `make prod`.
3. Test program `make test-prod`.
4. Generate doxygen doc `make doc`.


## Todo:
* Currently stereo samples are compressed as two single streams instead of compressing them as stereo ogg vorbis streams. This may be less optimal.
* Adhere to RIFF chunk size rules.
