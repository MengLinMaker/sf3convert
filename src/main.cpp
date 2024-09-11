#include "sfont/sfont.h"
#include <unistd.h>

static void cliUsage(const char *pname)
{
	fprintf(stderr, "\nusage: %s [-flags] soundfont [outfile]\n", pname);
	fprintf(stderr, "   -h     help\n");
	fprintf(stderr, "   -q qq  ogg quality\n");
	fprintf(stderr, "   -a nn  amplification in dB before ogg compression\n");
	fprintf(stderr, "   -d     dump presets\n");
	fprintf(stderr, "\n");
}

int main(int argc, char *argv[])
{
	double oggQuality = 0;
	double oggDbAmp = 0;
	bool dump = false;
	int flag;
	while ((flag = getopt(argc, argv, "xcp:dS:szq:a:")) != EOF)
	{
		switch (flag)
		{
		case 'h':
			cliUsage(argv[0]);
			break;
		case 'q':
			oggQuality = atof(optarg);
			break;
		case 'a':
			oggDbAmp = atof(optarg);
			break;
		case 'd':
			dump = true;
			break;
		default:
			break;
		}
	}

	const char *soundFontPath = argv[1];
	SoundFont soundFont(soundFontPath);
	if (!soundFont.read())
	{
		fprintf(stderr, "Failed to read SoundFont2: %s\n", soundFontPath);
		exit(3);
	}
	else if (dump)
		soundFont.dumpPresets();
	else
	{
		const char *newSoundFontPath = argv[2];
		std::fstream newSoundFont(newSoundFontPath);
		if (!newSoundFont.is_open())
		{
			fprintf(stderr, "Failed to setup SoundFont3: %s\n", argv[2]);
			exit(2);
		}
		soundFont.write(&newSoundFont, oggQuality, oggDbAmp);
		newSoundFont.close();
	}
	return 0;
}
