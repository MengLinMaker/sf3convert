//=============================================================================
//  MuseScore sftools
//
//  Copyright (C) 2004-2011 Werner Schweer
//
//  This work is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Library General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  See LICENCE for the licence text and disclaimer of warranty.
//=============================================================================

#include "sfont.h"
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
		QFile newSoundFontPath(argv[2]);
		if (!newSoundFontPath.open(QIODevice::WriteOnly))
		{
			fprintf(stderr, "Failed to setup SoundFont3: %s\n", argv[2]);
			exit(2);
		}
		soundFont.write(&newSoundFontPath, oggQuality, oggDbAmp);
		newSoundFontPath.close();
	}
	return 0;
}
