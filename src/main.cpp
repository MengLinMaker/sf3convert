#include "sfont/sfont.h"
#include <getopt.h>

static void cliUsage(const char *pname)
{
	fprintf(stderr, "\nUsage: %s [-flags] soundfont [outfile]\n", pname);
	fprintf(stderr, "   -h     help\n");
	fprintf(stderr, "   -q qq  ogg quality\n");
	fprintf(stderr, "   -a nn  amplification in dB before ogg compression\n");
	fprintf(stderr, "   -d     dump presets\n");
	fprintf(stderr, "\n");
}

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
	if (argc <= 1)
	{
		fprintf(stderr, "\nNot enough arguments\n");
		cliUsage(argv[0]);
		exit(1);
	}

	double oggQuality = 0;
	double oggDbAmp = 0;
	char flag;
	while ((flag = getopt(argc, argv, "q:a:d")) != EOF)
	{
		switch (flag)
		{
		case 'q':
			oggQuality = atof(optarg);
			break;
		case 'a':
			oggDbAmp = atof(optarg);
			break;
		case 'd':
		{
			const char *soundFontPath = argv[2];
			SoundFont soundFont = readSoundFont(soundFontPath);
			soundFont.dumpPresets();
			exit(0);
		}
		default:
			fprintf(stderr, "\nSupported commands\n");
			cliUsage(argv[0]);
			exit(0);
		}
	}

	// Shift argv position to non-flag args
	argv += optind;

	const char *soundFontPath = argv[0];
	SoundFont soundFont = readSoundFont(soundFontPath);

	const char *newSoundFontPath = argv[1];
	std::fstream newSoundFont;
	newSoundFont.open(newSoundFontPath, std::fstream::out);
	if (!newSoundFont)
	{
		fprintf(stderr, "Failed to setup output SoundFont: %s\n", newSoundFontPath);
		exit(2);
	}
	soundFont.write(&newSoundFont, oggQuality, oggDbAmp);
	newSoundFont.close();
	exit(0);
}
