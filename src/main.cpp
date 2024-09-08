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

//---------------------------------------------------------
//   usage
//---------------------------------------------------------

static void usage(const char *pname)
{
	fprintf(stderr, "\nusage: %s [-flags] soundfont [outfile]\n", pname);
	fprintf(stderr, "   -h     help\n");
	fprintf(stderr, "   -q qq  ogg quality\n");
	fprintf(stderr, "   -a nn  amplification in dB before ogg compression\n");
	fprintf(stderr, "   -d     dump presets\n");
	fprintf(stderr, "\n");
}

//---------------------------------------------------------
//   main
//---------------------------------------------------------

int main(int argc, char *argv[])
{
	bool dump = false;
	double oggQuality = 0;
	double oggAmp = 0;

	QList<int> presets;

	fprintf(stderr, "%s: convert sound file\n", argv[0]);

	int c;
	while ((c = getopt(argc, argv, "xcp:dS:szq:a:")) != EOF)
	{
		switch (c)
		{
		case 'h':
			usage(argv[0]);
			break;
		case 'q':
			oggQuality = atof(optarg);
			break;
		case 'a':
			oggAmp = atof(optarg);
			break;
		case 'd':
			dump = true;
			break;
		default:
			break;
		}
	}
	const char *pname = argv[0];

	argc -= optind;
	argv += optind;

	SoundFont sf(argv[0]);

	if (!sf.read())
	{
		fprintf(stderr, "sf read error\n");
		exit(3);
	}
	else if (dump)
		sf.dumpPresets();
	else
	{
		QFile fo(argv[1]);
		if (!fo.open(QIODevice::WriteOnly))
		{
			fprintf(stderr, "cannot open <%s>\n", argv[2]);
			exit(2);
		}
		sf.write(&fo, oggQuality, oggAmp);
		fo.close();
	}
	return 0;
}
