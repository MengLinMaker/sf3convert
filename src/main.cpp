#include "sfont/sfont.h"

#include <CLI/CLI.hpp>
#include <getopt.h>

SoundFont readSoundFont(const char *soundFontPath)
{
	SoundFont soundFont(soundFontPath);
	if (!soundFont.read())
	{
		fprintf(stderr, "Failed to read input SoundFont: %s\n", soundFontPath);
		exit(3);
	}
	return soundFont;
}

int main(int argc, char *argv[])
{
	CLI::App cli("SoundFont utilities");
	// Prefer detailed help flag over summary
	cli.set_help_flag("");
	cli.set_help_all_flag("-h", "Print this help message and exit");

	CLI::App *convertCli = cli.add_subcommand("convert", "Convert SoundFont2 to SoundFont3");
	{
		double oggQuality = 0;
		double oggDbAmp = 0;
		std::string inputSoundFontPath = "";
		std::string outputSoundFontPath = "";
		convertCli->add_option("-q", oggQuality, "Ogg quality")->check(CLI::Range(0.0, 1.0));
		convertCli->add_option("-a", oggDbAmp, "Amplify sample dB")->check(CLI::Range(-60.0, 60.0));
		convertCli->add_option("input-soundfont", inputSoundFontPath)->required();
		convertCli->add_option("output-soundfont", outputSoundFontPath)->required();
		convertCli->callback([&oggQuality, &oggDbAmp, &inputSoundFontPath, &outputSoundFontPath](){
			std::fstream newSoundFont;
			newSoundFont.open(outputSoundFontPath, std::fstream::out);
			if (!newSoundFont)
			{
				fprintf(stderr, "Failed to setup output SoundFont: %s\n", outputSoundFontPath.c_str());
				exit(2);
			}
			readSoundFont(inputSoundFontPath.c_str()).write(&newSoundFont, oggQuality, oggDbAmp);
			newSoundFont.close();
			exit(0);
		});
	}

	CLI::App *presetCli = cli.add_subcommand("preset", "Dump SoundFont preset names");
	{
		std::string inputSoundFontPath = "";
		presetCli->add_option("input-soundfont", inputSoundFontPath)->required();
		presetCli->callback([&inputSoundFontPath](){
			readSoundFont(inputSoundFontPath.c_str()).dumpPresets();
			exit(0);
		});
	}

	cli.require_subcommand();
	cli.parse(argc, argv);
	exit(0);
}
