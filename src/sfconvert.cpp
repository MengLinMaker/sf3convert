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

#include <unistd.h>
#include <QtCore/QFile>
#include <QtCore/QElapsedTimer>
#include "sfont.h"

bool smallSf = false;

//---------------------------------------------------------
//   usage
//---------------------------------------------------------

static void usage(const char *pname)
{
	fprintf(stderr, "usage: %s [-flags] soundfont [outfile]\n", pname);
	fprintf(stderr, "   -z     compress sf\n");
	fprintf(stderr, "   -q qq  ogg quality\n");
	fprintf(stderr, "   -a nn  amplification in dB before ogg compression\n");
	fprintf(stderr, "   -c     c output\n");
	fprintf(stderr, "   -p nn  preset\n");
	fprintf(stderr, "   -d     dump presets\n");
	fprintf(stderr, "   -s     create small sf (one instrument/preset), pan to 0\n");
	fprintf(stderr, "   -S nn  ogg serial number\n");
}

//---------------------------------------------------------
//   main
//---------------------------------------------------------

int main(int argc, char *argv[])
{
	bool code = false;
	bool dump = false;
	bool compress = false;
	double oggQuality = 0.3;
	double oggAmp = -1.0;
	qint64 oggSerial = std::numeric_limits<qint64>::max();

	QList<int> presets;

	QElapsedTimer t;
	t.start();
	fprintf(stderr, "%s: convert sound file\n", argv[0]);

	int c;
	while ((c = getopt(argc, argv, "xcp:dS:szq:a:")) != EOF)
	{
		switch (c)
		{
		case 'c':
			code = true;
			break;
		case 'p':
			presets.append(atoi(optarg));
			break;
		case 'd':
			dump = true;
			break;
		case 'S':
			oggSerial = atoi(optarg);
			break;
		case 's':
			smallSf = true;
			break;
		case 'z':
			compress = true;
			break;
		case 'q':
			oggQuality = atof(optarg);
			break;
		case 'a':
			oggAmp = atof(optarg);
			break;
		default:
			usage(argv[0]);
			exit(1);
		}
	}
	const char *pname = argv[0];

	argc -= optind;
	argv += optind;
	if ((code && (argc != 1)) || (dump && argc != 1))
	{
		usage(pname);
		exit(3);
	}
	if (!code && !dump && !compress)
	{
		usage(pname);
		exit(4);
	}

	SoundFont sf(argv[0]);

	if (!sf.read())
	{
		fprintf(stderr, "sf read error\n");
		exit(3);
	}

	if (code)
	{
		if (presets.isEmpty())
			sf.writeCode();
		else
			sf.writeCode(presets);
	}
	else if (dump)
		sf.dumpPresets();
	else if (compress)
	{
		QFile fo(argv[1]);
		if (!fo.open(QIODevice::WriteOnly))
		{
			fprintf(stderr, "cannot open <%s>\n", argv[2]);
			exit(2);
		}
		sf.write(&fo, oggQuality, oggAmp, oggSerial);
		fo.close();
	}
	qDebug("Soundfont converted in: %d ms", (int)t.elapsed());
	return 0;
}
