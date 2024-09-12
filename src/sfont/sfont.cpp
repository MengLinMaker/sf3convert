#include "sfont.h"

#include "vorbis/vorbisenc.h"

#include <math.h>
#include <stdexcept>
#include <bit>
#include <cstring>
#include <string>

#define BE_SHORT(x) ((((x) & 0xFF) << 8) | (((x) >> 8) & 0xFF))
#define BE_LONG(x) ((((x) & 0xFF) << 24) |    \
					(((x) & 0xFF00) << 8) |   \
					(((x) & 0xFF0000) >> 8) | \
					(((x) >> 24) & 0xFF))
#define FOURCC(a, b, c, d) a << 24 | b << 16 | c << 8 | d
#define BLOCK_SIZE 1024

static const bool writeCompressed = true;

//---------------------------------------------------------
//   Sample
//---------------------------------------------------------

Sample::Sample()
{
	name = 0;
}

Sample::~Sample()
{
	free(name);
}

//---------------------------------------------------------
//   Instrument
//---------------------------------------------------------

Instrument::Instrument()
{
	name = 0;
}

Instrument::~Instrument()
{
	free(name);
}

//---------------------------------------------------------
//   SoundFont
//---------------------------------------------------------

SoundFont::SoundFont(const std::string &s)
{
	path = s;
	engine = 0;
	name = 0;
	date = 0;
	comment = 0;
	tools = 0;
	creator = 0;
	product = 0;
	copyright = 0;
}

SoundFont::~SoundFont()
{
	free(engine);
	free(name);
	free(date);
	free(comment);
	free(tools);
	free(creator);
	free(product);
	free(copyright);
}

//---------------------------------------------------------
//   read
//---------------------------------------------------------

bool SoundFont::read()
{
	file = new std::fstream(path);
	if (!file->is_open())
	{
		fprintf(stderr, "cannot open <%s>\n", path.c_str());
		delete file;
		return false;
	}
	try
	{
		int len = readFourcc("RIFF");
		readSignature("sfbk");
		len -= 4;
		while (len)
		{
			int len2 = readFourcc("LIST");
			len -= (len2 + 8);
			char fourcc[5];
			fourcc[0] = 0;
			readSignature(fourcc);
			fourcc[4] = 0;
			len2 -= 4;
			while (len2)
			{
				fourcc[0] = 0;
				int len3 = readFourcc(fourcc);
				fourcc[4] = 0;
				len2 -= (len3 + 8);
				readSection(fourcc, len3);
			}
		}
	}
	catch (std::string s)
	{
		printf("read sf file failed: %s\n", s.c_str());
		delete file;
		return false;
	}
	delete file;
	return true;
}

//---------------------------------------------------------
//   skip
//---------------------------------------------------------

void SoundFont::skip(int n)
{
	int pos = file->tellg();
	if (!file->seekg(pos + n))
		throw(std::string("unexpected end of file\n"));
}

//---------------------------------------------------------
//   readFourcc
//---------------------------------------------------------

int SoundFont::readFourcc(char *signature)
{
	readSignature(signature);
	return readDword();
}

int SoundFont::readFourcc(const char *signature)
{
	readSignature(signature);
	return readDword();
}

//---------------------------------------------------------
//   readSignature
//---------------------------------------------------------

void SoundFont::readSignature(const char *signature)
{
	char fourcc[4];
	readSignature(fourcc);
	if (memcmp(fourcc, signature, 4) != 0)
		throw std::runtime_error("fourcc " + std::string(signature) + " expected");
}

void SoundFont::readSignature(char *signature)
{
	if (file->read(signature, 4).fail())
		throw(std::string("unexpected end of file\n"));
}

//---------------------------------------------------------
//   readDword
//---------------------------------------------------------

unsigned SoundFont::readDword()
{
	unsigned format;
	if (file->read((char *)&format, 4).fail())
		throw(std::string("unexpected end of file\n"));
	if (std::endian::native == std::endian::big)
		return BE_LONG(format);
	else
		return format;
}

//---------------------------------------------------------
//   writeDword
//---------------------------------------------------------

void SoundFont::writeDword(int val)
{
	if (std::endian::native == std::endian::big)
		val = BE_LONG(val);
	write((char *)&val, 4);
}

//---------------------------------------------------------
//   writeWord
//---------------------------------------------------------

void SoundFont::writeWord(unsigned short int val)
{
	if (std::endian::native == std::endian::big)
		val = BE_SHORT(val);
	write((char *)&val, 2);
}

//---------------------------------------------------------
//   writeByte
//---------------------------------------------------------

void SoundFont::writeByte(unsigned char val)
{
	write((char *)&val, 1);
}

//---------------------------------------------------------
//   writeChar
//---------------------------------------------------------

void SoundFont::writeChar(char val)
{
	write((char *)&val, 1);
}

//---------------------------------------------------------
//   writeShort
//---------------------------------------------------------

void SoundFont::writeShort(short val)
{
	if (std::endian::native == std::endian::big)
		val = BE_SHORT(val);
	write((char *)&val, 2);
}

//---------------------------------------------------------
//   readWord
//---------------------------------------------------------

int SoundFont::readWord()
{
	unsigned short format;
	if (file->read((char *)&format, 2).fail())
		throw(std::string("unexpected end of file\n"));
	if (std::endian::native == std::endian::big)
		return BE_SHORT(format);
	else
		return format;
}

//---------------------------------------------------------
//   readShort
//---------------------------------------------------------

int SoundFont::readShort()
{
	short format;
	if (file->read((char *)&format, 2).fail())
		throw(std::string("unexpected end of file\n"));
	if (std::endian::native == std::endian::big)
		return BE_SHORT(format);
	else
		return format;
}

//---------------------------------------------------------
//   readByte
//---------------------------------------------------------

int SoundFont::readByte()
{
	unsigned char val;
	if (file->read((char *)&val, 1).fail())
		throw(std::string("unexpected end of file\n"));
	return val;
}

//---------------------------------------------------------
//   readChar
//---------------------------------------------------------

int SoundFont::readChar()
{
	char val;
	if (file->read(&val, 1).fail())
		throw(std::string("unexpected end of file\n"));
	return val;
}

//---------------------------------------------------------
//   readVersion
//---------------------------------------------------------

void SoundFont::readVersion()
{
	unsigned char data[4];
	if (file->read((char *)data, 4).fail())
		throw(std::string("unexpected end of file\n"));
	version.major = data[0] + (data[1] << 8);
	version.minor = data[2] + (data[3] << 8);
}

//---------------------------------------------------------
//   readString
//---------------------------------------------------------

char *SoundFont::readString(int n)
{
	char data[2500];
	if (file->read((char *)data, n).fail())
		throw(std::string("unexpected end of file\n"));
	if (data[n - 1] != 0)
		data[n] = 0;
	return strdup(data);
}

//---------------------------------------------------------
//   readSection
//---------------------------------------------------------

void SoundFont::readSection(const char *fourcc, int len)
{
	printf("readSection <%s> len %d\n", fourcc, len);

	switch (FOURCC(fourcc[0], fourcc[1], fourcc[2], fourcc[3]))
	{
	case FOURCC('i', 'f', 'i', 'l'): // version
		readVersion();
		break;
	case FOURCC('I', 'N', 'A', 'M'): // sound font name
		name = readString(len);
		break;
	case FOURCC('i', 's', 'n', 'g'): // target render engine
		engine = readString(len);
		break;
	case FOURCC('I', 'P', 'R', 'D'): // product for which the bank was intended
		product = readString(len);
		break;
	case FOURCC('I', 'E', 'N', 'G'): // sound designers and engineers for the bank
		creator = readString(len);
		break;
	case FOURCC('I', 'S', 'F', 'T'): // SoundFont tools used to create and alter the bank
		tools = readString(len);
		break;
	case FOURCC('I', 'C', 'R', 'D'): // date of creation of the bank
		date = readString(len);
		break;
	case FOURCC('I', 'C', 'M', 'T'): // comments on the bank
		comment = readString(len);
		break;
	case FOURCC('I', 'C', 'O', 'P'): // copyright message
		copyright = readString(len);
		break;
	case FOURCC('s', 'm', 'p', 'l'): // the digital audio samples
		samplePos = file->tellg();
		sampleLen = len;
		skip(len);
		break;
	case FOURCC('p', 'h', 'd', 'r'): // preset headers
		readPhdr(len);
		break;
	case FOURCC('p', 'b', 'a', 'g'): // preset index list
		readBag(len, &pZones);
		break;
	case FOURCC('p', 'm', 'o', 'd'): // preset modulator list
		readMod(len, &pZones);
		break;
	case FOURCC('p', 'g', 'e', 'n'): // preset generator list
		readGen(len, &pZones);
		break;
	case FOURCC('i', 'n', 's', 't'): // instrument names and indices
		readInst(len);
		break;
	case FOURCC('i', 'b', 'a', 'g'): // instrument index list
		readBag(len, &iZones);
		break;
	case FOURCC('i', 'm', 'o', 'd'): // instrument modulator list
		readMod(len, &iZones);
		break;
	case FOURCC('i', 'g', 'e', 'n'): // instrument generator list
		readGen(len, &iZones);
		break;
	case FOURCC('s', 'h', 'd', 'r'): // sample headers
		readShdr(len);
		break;
	case FOURCC('i', 'r', 'o', 'm'): // sample rom
	case FOURCC('i', 'v', 'e', 'r'): // sample rom version
	default:
		skip(len);
		throw std::runtime_error("unknown fourcc " + std::string(fourcc));
		break;
	}
}

//---------------------------------------------------------
//   readPhdr
//---------------------------------------------------------

void SoundFont::readPhdr(int len)
{
	if (len < (38 * 2))
		throw(std::string("phdr too short"));
	if (len % 38)
		throw(std::string("phdr not a multiple of 38"));
	int n = len / 38;
	if (n <= 1)
	{
		printf("no presets\n");
		skip(len);
		return;
	}
	int index1 = 0, index2;
	for (int i = 0; i < n; ++i)
	{
		Preset *preset = new Preset;
		preset->name = readString(20);
		preset->preset = readWord();
		preset->bank = readWord();
		index2 = readWord();
		preset->library = readDword();
		preset->genre = readDword();
		preset->morphology = readDword();
		if (index2 < index1)
			throw("preset header indices not monotonic");
		if (i > 0)
		{
			int n = index2 - index1;
			while (n--)
			{
				Zone *z = new Zone;
				presets.back()->zones.push_back(z);
				pZones.push_back(z);
			}
		}
		index1 = index2;
		presets.push_back(preset);
	}
	presets.pop_back();
}

//---------------------------------------------------------
//   readBag
//---------------------------------------------------------

void SoundFont::readBag(int len, std::vector<Zone *> *zones)
{
	if (len % 4)
		throw(std::string("bag size not a multiple of 4"));
	int gIndex2, mIndex2;
	int gIndex1 = readWord();
	int mIndex1 = readWord();
	len -= 4;
	for (Zone *zone : *zones)
	{
		gIndex2 = readWord();
		mIndex2 = readWord();
		len -= 4;
		if (len < 0)
			throw(std::string("bag size too small"));
		if (gIndex2 < gIndex1)
			throw("generator indices not monotonic");
		if (mIndex2 < mIndex1)
			throw("modulator indices not monotonic");
		int n = mIndex2 - mIndex1;
		while (n--)
			zone->modulators.push_back(new ModulatorList);
		n = gIndex2 - gIndex1;
		while (n--)
			zone->generators.push_back(new GeneratorList);
		gIndex1 = gIndex2;
		mIndex1 = mIndex2;
	}
}

//---------------------------------------------------------
//   readMod
//---------------------------------------------------------

void SoundFont::readMod(int size, std::vector<Zone *> *zones)
{
	for (Zone *zone : *zones)
	{
		for (ModulatorList *m : zone->modulators)
		{
			size -= 10;
			if (size < 0)
				throw(std::string("pmod size mismatch"));
			m->src = static_cast<Modulator>(readWord());
			m->dst = static_cast<Generator>(readWord());
			m->amount = readShort();
			m->amtSrc = static_cast<Modulator>(readWord());
			m->transform = static_cast<Transform>(readWord());
		}
	}
	if (size != 10)
		throw(std::string("modulator list size mismatch"));
	skip(10);
}

//---------------------------------------------------------
//   readGen
//---------------------------------------------------------

void SoundFont::readGen(int size, std::vector<Zone *> *zones)
{
	if (size % 4)
		throw(std::string("bad generator list size"));
	for (Zone *zone : *zones)
	{
		size -= (zone->generators.size() * 4);
		if (size < 0)
			break;

		for (GeneratorList *gen : zone->generators)
		{
			gen->gen = static_cast<Generator>(readWord());
			if (gen->gen == Gen_KeyRange || gen->gen == Gen_VelRange)
			{
				gen->amount.lo = readByte();
				gen->amount.hi = readByte();
			}
			else if (gen->gen == Gen_Instrument)
				gen->amount.uword = readWord();
			else
				gen->amount.sword = readWord();
		}
	}
	if (size != 4)
		throw std::runtime_error("generator list size mismatch " + std::to_string(size) + " != 4");
	skip(size);
}

//---------------------------------------------------------
//   readInst
//---------------------------------------------------------

void SoundFont::readInst(int size)
{
	int n = size / 22;
	int index1 = 0, index2;
	for (int i = 0; i < n; ++i)
	{
		Instrument *instrument = new Instrument;
		instrument->name = readString(20);
		index2 = readWord();
		if (index2 < index1)
			throw("instrument header indices not monotonic");
		if (i > 0)
		{
			int n = index2 - index1;
			while (n--)
			{
				Zone *z = new Zone;
				instruments.back()->zones.push_back(z);
				iZones.push_back(z);
			}
		}
		index1 = index2;
		instruments.push_back(instrument);
	}
	instruments.pop_back();
}

//---------------------------------------------------------
//   readShdr
//---------------------------------------------------------

void SoundFont::readShdr(int size)
{
	int n = size / 46;
	for (int i = 0; i < n - 1; ++i)
	{
		Sample *s = new Sample;
		s->name = readString(20);
		s->start = readDword();
		s->end = readDword();
		s->loopstart = readDword();
		s->loopend = readDword();
		s->samplerate = readDword();
		s->origpitch = readByte();
		s->pitchadj = readChar();
		readWord(); // sampleLink
		s->sampletype = readWord();

		s->loopstart -= s->start;
		s->loopend -= s->start;
		// printf("readFontHeader %d %d   %d %d\n", s->start, s->end, s->loopstart, s->loopend);
		samples.push_back(s);
	}
	skip(46); // trailing record
}

static const char *generatorNames[] = {
	"StartAddrOfs", "EndAddrOfs", "StartLoopAddrOfs",
	"EndLoopAddrOfs", "StartAddrCoarseOfs", "ModLFO2Pitch",
	"VibLFO2Pitch", "ModEnv2Pitch", "FilterFc", "FilterQ",
	"ModLFO2FilterFc", "ModEnv2FilterFc", "EndAddrCoarseOfs",
	"ModLFO2Vol", "Unused1", "ChorusSend", "ReverbSend", "Pan",
	"Unused2", "Unused3", "Unused4",
	"ModLFODelay", "ModLFOFreq", "VibLFODelay", "VibLFOFreq",
	"ModEnvDelay", "ModEnvAttack", "ModEnvHold", "ModEnvDecay",
	"ModEnvSustain", "ModEnvRelease", "Key2ModEnvHold",
	"Key2ModEnvDecay", "VolEnvDelay", "VolEnvAttack",
	"VolEnvHold", "VolEnvDecay", "VolEnvSustain", "VolEnvRelease",
	"Key2VolEnvHold", "Key2VolEnvDecay", "Instrument",
	"Reserved1", "KeyRange", "VelRange",
	"StartLoopAddrCoarseOfs", "Keynum", "Velocity",
	"Attenuation", "Reserved2", "EndLoopAddrCoarseOfs",
	"CoarseTune", "FineTune", "SampleId", "SampleModes",
	"Reserved3", "ScaleTune", "ExclusiveClass", "OverrideRootKey",
	"Dummy"};

//---------------------------------------------------------
//   write
//---------------------------------------------------------

bool SoundFont::write(std::fstream *f, double oggQuality, double oggAmp)
{
	file = f;
	_oggQuality = oggQuality;
	_oggAmp = oggAmp;
	int riffLenPos;
	int listLenPos;
	try
	{
		file->write("RIFF", 4);
		riffLenPos = file->tellg();
		writeDword(0);
		file->write("sfbk", 4);

		file->write("LIST", 4);
		listLenPos = file->tellg();
		writeDword(0);
		file->write("INFO", 4);

		writeIfil();
		if (name)
			writeStringSection("INAM", name);
		if (engine)
			writeStringSection("isng", engine);
		if (product)
			writeStringSection("IPRD", product);
		if (creator)
			writeStringSection("IENG", creator);
		if (tools)
			writeStringSection("ISFT", tools);
		if (date)
			writeStringSection("ICRD", date);
		if (comment)
			writeStringSection("ICMT", comment);
		if (copyright)
			writeStringSection("ICOP", copyright);

		int pos = file->tellg();
		file->seekg(listLenPos);
		writeDword(pos - listLenPos - 4);
		file->seekg(pos);

		file->write("LIST", 4);
		listLenPos = file->tellg();
		writeDword(0);
		file->write("sdta", 4);
		writeSmpl();
		pos = file->tellg();
		file->seekg(listLenPos);
		writeDword(pos - listLenPos - 4);
		file->seekg(pos);

		file->write("LIST", 4);
		listLenPos = file->tellg();
		writeDword(0);
		file->write("pdta", 4);

		writePhdr();
		writeBag("pbag", &pZones);
		writeMod("pmod", &pZones);
		writeGen("pgen", &pZones);
		writeInst();
		writeBag("ibag", &iZones);
		writeMod("imod", &iZones);
		writeGen("igen", &iZones);
		writeShdr();

		pos = file->tellg();
		file->seekg(listLenPos);
		writeDword(pos - listLenPos - 4);
		file->seekg(pos);

		int endPos = file->tellg();
		file->seekg(riffLenPos);
		writeDword(endPos - riffLenPos - 4);
	}
	catch (std::string s)
	{
		printf("write sf file failed: %s\n", s.c_str());
		return false;
	}
	return true;
}

//---------------------------------------------------------
//   write
//---------------------------------------------------------

void SoundFont::write(const char *p, int n)
{
	if (file->write(p, n).fail())
		throw(std::string("write error"));
}

//---------------------------------------------------------
//   writeStringSection
//---------------------------------------------------------

void SoundFont::writeStringSection(const char *fourcc, char *s)
{
	write(fourcc, 4);
	int nn = strlen(s) + 1;
	int n = ((nn + 1) / 2) * 2;
	writeDword(n);
	write(s, nn);
	if (n - nn)
	{
		char c = 0;
		write(&c, 1);
	}
}

//---------------------------------------------------------
//   writeIfil
//---------------------------------------------------------

void SoundFont::writeIfil()
{
	write("ifil", 4);
	writeDword(4);
	unsigned char data[4];
	if (writeCompressed)
		version.major = 3;
	data[0] = version.major;
	data[1] = version.major >> 8;
	data[2] = version.minor;
	data[3] = version.minor >> 8;
	write((char *)data, 4);
}

//---------------------------------------------------------
//   writeSmpl
//---------------------------------------------------------

void SoundFont::writeSmpl()
{
	write("smpl", 4);

	int pos = file->tellg();
	writeDword(0);
	int sampleLen = 0;
	if (writeCompressed)
	{
		for (Sample *s : samples)
		{
			s->sampletype |= 0x10;
			int len = writeCompressedSample(s);
			s->start = sampleLen;
			sampleLen += len;
			s->end = sampleLen;
		}
	}
	else
	{
		char *buffer = new char[sampleLen];
		std::fstream f(path);
		if (!f.is_open())
			throw std::runtime_error("cannot open " + path);
		for (Sample *s : samples)
		{
			f.seekg(samplePos + s->start * sizeof(short));

			int len = (s->end - s->start) * sizeof(short);
			f.read(buffer, len);
			write(buffer, len);
			s->start = sampleLen / sizeof(short);
			sampleLen += len;
			s->end = sampleLen / sizeof(short);
			s->loopstart += s->start;
			s->loopend += s->start;
		}
		f.close();
		delete[] buffer;
	}
	int npos = file->tellg();
	file->seekg(pos);
	writeDword(npos - pos - 4);
	file->seekg(npos);
}

//---------------------------------------------------------
//   writePhdr
//---------------------------------------------------------

void SoundFont::writePhdr()
{
	write("phdr", 4);
	int n = presets.size();
	writeDword((n + 1) * 38);
	int zoneIdx = 0;
	for (const Preset *p : presets)
	{
		writePreset(zoneIdx, p);
		zoneIdx += p->zones.size();
	}
	Preset p;
	memset(&p, 0, sizeof(p));
	writePreset(zoneIdx, &p);
}

//---------------------------------------------------------
//   writePreset
//---------------------------------------------------------

void SoundFont::writePreset(int zoneIdx, const Preset *preset)
{
	if (preset->name)
		write(preset->name, 20);
	else
		// End of preset message teminates "phdr" chunk
		write("EOP", 20);
	writeWord(preset->preset);
	writeWord(preset->bank);
	writeWord(zoneIdx);
	writeDword(preset->library);
	writeDword(preset->genre);
	writeDword(preset->morphology);
}

//---------------------------------------------------------
//   writeBag
//---------------------------------------------------------

void SoundFont::writeBag(const char *fourcc, std::vector<Zone *> *zones)
{
	write(fourcc, 4);
	int n = zones->size();
	writeDword((n + 1) * 4);
	int gIndex = 0;
	int pIndex = 0;
	for (const Zone *z : *zones)
	{
		writeWord(gIndex);
		writeWord(pIndex);
		gIndex += z->generators.size();
		pIndex += z->modulators.size();
	}
	writeWord(gIndex);
	writeWord(pIndex);
}

//---------------------------------------------------------
//   writeMod
//---------------------------------------------------------

void SoundFont::writeMod(const char *fourcc, const std::vector<Zone *> *zones)
{
	write(fourcc, 4);
	int n = 0;
	for (const Zone *z : *zones)
		n += z->modulators.size();
	writeDword((n + 1) * 10);

	for (const Zone *zone : *zones)
	{
		for (const ModulatorList *m : zone->modulators)
			writeModulator(m);
	}
	ModulatorList mod;
	memset(&mod, 0, sizeof(mod));
	writeModulator(&mod);
}

//---------------------------------------------------------
//   writeModulator
//---------------------------------------------------------

void SoundFont::writeModulator(const ModulatorList *m)
{
	writeWord(m->src);
	writeWord(m->dst);
	writeShort(m->amount);
	writeWord(m->amtSrc);
	writeWord(m->transform);
}

//---------------------------------------------------------
//   writeGen
//---------------------------------------------------------

void SoundFont::writeGen(const char *fourcc, std::vector<Zone *> *zones)
{
	write(fourcc, 4);
	int n = 0;
	for (const Zone *z : *zones)
		n += z->generators.size();
	writeDword((n + 1) * 4);

	for (const Zone *zone : *zones)
	{
		for (const GeneratorList *g : zone->generators)
			writeGenerator(g);
	}
	GeneratorList gen;
	memset(&gen, 0, sizeof(gen));
	writeGenerator(&gen);
}

//---------------------------------------------------------
//   writeGenerator
//---------------------------------------------------------

void SoundFont::writeGenerator(const GeneratorList *g)
{
	writeWord(g->gen);
	if (g->gen == Gen_KeyRange || g->gen == Gen_VelRange)
	{
		writeByte(g->amount.lo);
		writeByte(g->amount.hi);
	}
	else if (g->gen == Gen_Instrument)
		writeWord(g->amount.uword);
	else
		writeWord(g->amount.sword);
}

//---------------------------------------------------------
//   writeInst
//---------------------------------------------------------

void SoundFont::writeInst()
{
	write("inst", 4);
	int n = instruments.size();
	writeDword((n + 1) * 22);
	int zoneIdx = 0;
	for (const Instrument *p : instruments)
	{
		writeInstrument(zoneIdx, p);
		zoneIdx += p->zones.size();
	}
	Instrument p;
	memset(&p, 0, sizeof(p));
	writeInstrument(zoneIdx, &p);
}

//---------------------------------------------------------
//   writeInstrument
//---------------------------------------------------------

void SoundFont::writeInstrument(int zoneIdx, const Instrument *instrument)
{
	if (instrument->name)
		write(instrument->name, 20);
	else
		// End of instrument message teminates "inst" chunk
		write("EOI", 20);
	writeWord(zoneIdx);
}

//---------------------------------------------------------
//   writeShdr
//---------------------------------------------------------

void SoundFont::writeShdr()
{
	write("shdr", 4);
	writeDword(46 * (samples.size() + 1));
	for (const Sample *s : samples)
		writeSample(s);
	Sample s;
	memset(&s, 0, sizeof(s));
	writeSample(&s);
}

//---------------------------------------------------------
//   writeSample
//---------------------------------------------------------

void SoundFont::writeSample(const Sample *s)
{
	if (s->name)
		write(s->name, 20);
	else
		// End of sample message teminates "shdr" chunk
		write("EOS", 20);
	writeDword(s->start);
	writeDword(s->end);
	writeDword(s->loopstart);
	writeDword(s->loopend);
	writeDword(s->samplerate);
	writeByte(s->origpitch);
	writeChar(s->pitchadj);
	writeWord(0);
	writeWord(s->sampletype);
}

//---------------------------------------------------------
//   writeCompressedSample
//---------------------------------------------------------

int SoundFont::writeCompressedSample(Sample *s)
{
	std::fstream f(path);
	if (!f.is_open())
	{
		fprintf(stderr, "cannot open <%s>\n", path.c_str());
		return 0;
	}
	f.seekg(samplePos + s->start * sizeof(short));
	int samples = s->end - s->start;
	short ibuffer[samples];
	f.read((char *)ibuffer, samples * sizeof(short));
	f.close();

	ogg_stream_state os;
	ogg_page og;
	ogg_packet op;
	vorbis_info vi;
	vorbis_dsp_state vd;
	vorbis_block vb;
	vorbis_comment vc;

	vorbis_info_init(&vi);
	int ret = vorbis_encode_init_vbr(&vi, 1, s->samplerate, _oggQuality);
	if (ret)
	{
		printf("vorbis init failed\n");
		return false;
	}
	vorbis_comment_init(&vc);
	vorbis_analysis_init(&vd, &vi);
	vorbis_block_init(&vd, &vb);

	srand(time(NULL));
	const int oggSerial = rand();
	ogg_stream_init(&os, oggSerial);

	ogg_packet header;
	ogg_packet header_comm;
	ogg_packet header_code;

	vorbis_analysis_headerout(&vd, &vc, &header, &header_comm, &header_code);
	ogg_stream_packetin(&os, &header);
	ogg_stream_packetin(&os, &header_comm);
	ogg_stream_packetin(&os, &header_code);

	char obuf[1024 * 1024];
	char *p = obuf;

	for (;;)
	{
		int result = ogg_stream_flush(&os, &og);
		if (result == 0)
			break;
		memcpy(p, og.header, og.header_len);
		p += og.header_len;
		memcpy(p, og.body, og.body_len);
		p += og.body_len;
	}

	long i;
	int page = 0;
	double linearAmp = pow(10.0, _oggAmp / 20.0);
	for (;;)
	{
		int bufflength = std::min(BLOCK_SIZE, samples - page * BLOCK_SIZE);
		float **buffer = vorbis_analysis_buffer(&vd, bufflength);
		int j = 0;
		int max = std::min((page + 1) * BLOCK_SIZE, samples);
		for (i = page * BLOCK_SIZE; i < max; i++)
		{
			buffer[0][j] = (ibuffer[i] / 32768.f) * linearAmp;
			j++;
		}

		vorbis_analysis_wrote(&vd, bufflength);

		while (vorbis_analysis_blockout(&vd, &vb) == 1)
		{
			vorbis_analysis(&vb, 0);
			vorbis_bitrate_addblock(&vb);

			while (vorbis_bitrate_flushpacket(&vd, &op))
			{
				ogg_stream_packetin(&os, &op);

				for (;;)
				{
					int result = ogg_stream_pageout(&os, &og);
					if (result == 0)
						break;
					memcpy(p, og.header, og.header_len);
					p += og.header_len;
					memcpy(p, og.body, og.body_len);
					p += og.body_len;
				}
			}
		}
		page++;
		if ((max == samples) || !((samples - page * BLOCK_SIZE) > 0))
			break;
	}

	vorbis_analysis_wrote(&vd, 0);

	while (vorbis_analysis_blockout(&vd, &vb) == 1)
	{
		vorbis_analysis(&vb, 0);
		vorbis_bitrate_addblock(&vb);

		while (vorbis_bitrate_flushpacket(&vd, &op))
		{
			ogg_stream_packetin(&os, &op);

			for (;;)
			{
				int result = ogg_stream_pageout(&os, &og);
				if (result == 0)
					break;
				memcpy(p, og.header, og.header_len);
				p += og.header_len;
				memcpy(p, og.body, og.body_len);
				p += og.body_len;
			}
		}
	}

	ogg_stream_clear(&os);
	vorbis_block_clear(&vb);
	vorbis_dsp_clear(&vd);
	vorbis_comment_clear(&vc);
	vorbis_info_clear(&vi);

	int n = p - obuf;
	write(obuf, n);

	return n;
}

//---------------------------------------------------------
//   readCompressedSample
//---------------------------------------------------------

char *SoundFont::readCompressedSample(Sample *s)
{
	return 0;
}

//---------------------------------------------------------
//   writeCSample
//---------------------------------------------------------

bool SoundFont::writeCSample(Sample *s, int idx)
{
	std::fstream fi(path);
	if (!fi.is_open())
	{
		fprintf(stderr, "cannot open <%s>\n", path.c_str());
		return false;
	}
	fi.seekg(samplePos + s->start * sizeof(short));
	int samples = s->end - s->start;
	short ibuffer[samples];
	fi.read((char *)ibuffer, samples * sizeof(short));
	fi.close();

	fprintf(f, "static const short wave%d[] = {\n      ", idx);
	int n = 0;
	for (int row = 0;; ++row)
	{
		for (int col = 0; col < 16; ++col)
		{
			if (n >= samples)
				break;
			if (col != 0)
				fprintf(f, ", ");
			fprintf(f, "%d", ibuffer[n]);
			++n;
		}
		if (n >= samples)
			break;
		fprintf(f, ",\n      ");
	}
	fprintf(f, "\n      };\n");
	return true;
}

//---------------------------------------------------------
//   checkInstrument
//---------------------------------------------------------

static bool checkInstrument(std::vector<int> pnums, std::vector<Preset *> presets, int instrIdx)
{
	for (int idx : pnums)
	{
		Preset *p = presets[idx];
		for (Zone *z : p->zones)
		{
			for (GeneratorList *g : z->generators)
			{
				if (g->gen == Gen_Instrument)
				{
					if (instrIdx == g->amount.uword)
						return true;
				}
			}
		}
	}
	return false;
}

static bool checkInstrument(std::vector<Preset *> presets, int instrIdx)
{
	bool result = false;
	for (int i = 0; i < presets.size(); i++)
	{
		Preset *p = presets[i];
		Zone *z = p->zones[0];
		for (GeneratorList *g : z->generators)
		{
			if (g->gen == Gen_Instrument)
			{
				if (instrIdx == g->amount.uword)
					return true;
			}
		}
	}
	return false;
}

//---------------------------------------------------------
//   checkSample
//---------------------------------------------------------

static bool checkSample(std::vector<int> pnums, std::vector<Preset *> presets, std::vector<Instrument *> instruments,
						int sampleIdx)
{
	int idx = 0;
	for (Instrument *instrument : instruments)
	{
		if (!checkInstrument(pnums, presets, idx))
		{
			++idx;
			continue;
		}
		int zones = instrument->zones.size();
		for (Zone *z : instrument->zones)
		{
			std::vector<GeneratorList *> gl;
			for (GeneratorList *g : z->generators)
			{
				if (g->gen == Gen_SampleId)
				{
					if (sampleIdx == g->amount.uword)
						return true;
				}
			}
		}
		++idx;
	}
	return false;
}

//---------------------------------------------------------
//   checkSample
//---------------------------------------------------------

static bool checkSample(std::vector<Preset *> presets, std::vector<Instrument *> instruments,
						int sampleIdx)
{
	int idx = 0;
	for (Instrument *instrument : instruments)
	{
		if (!checkInstrument(presets, idx))
		{
			++idx;
			continue;
		}
		int zones = instrument->zones.size();
		for (Zone *z : instrument->zones)
		{
			std::vector<GeneratorList *> gl;
			for (GeneratorList *g : z->generators)
			{
				if (g->gen == Gen_SampleId)
				{
					if (sampleIdx == g->amount.uword)
						return true;
				}
			}
		}
		++idx;
	}
	return false;
}

//---------------------------------------------------------
//   writeCode
//---------------------------------------------------------

bool SoundFont::writeCode()
{
	int segments = 8;
	int n = samples.size() / segments;
	for (int i = 0; i < segments; ++i)
	{
		char buffer[16];
		snprintf(buffer, 7, "sf%d.cpp", i + 1);
		f = fopen(buffer, "w+");
		fprintf(f, "//\n//      this is generated code, do not change\n//\n");
		fprintf(f, "#include \"sfont.h\"\n\n");
		fprintf(f, "namespace FluidS {\n\n");

		//
		// dump wave samples
		//
		int end;
		if (i + 1 == segments)
			end = samples.size();
		else
			end = (i + 1) * n;
		std::vector<int> sampleIdx;
		for (int idx = i * n; idx < end; ++idx)
		{
			Sample *s = samples[idx];
			writeCSample(s, idx);
			sampleIdx.push_back(idx);
		}

		//
		// dump Sample[]
		//
		for (int idx : sampleIdx)
		{
			Sample *s = samples[idx];
			fprintf(f, "Sample sample%d(%d, %d, %d, %d, %d, %d, %d, %d, wave%d);\n",
					idx,
					0,
					s->end - (s->start + 1),
					s->loopstart, //  - s->start,
					s->loopend,	  //  - s->start,
					s->samplerate, s->origpitch, s->pitchadj, s->sampletype,
					idx);
		}
		fprintf(f, "} // end namespace\n");
		fclose(f);
	}

	f = fopen("sf.cpp", "w+");
	fprintf(f, "//\n//      this is generated code, do not change\n//\n");
	fprintf(f, "#include \"sfont.h\"\n\n");
	fprintf(f, "namespace FluidS {\n\n");

	fprintf(f, "extern Sample ");
	for (int i = 0; i < samples.size(); ++i)
	{
		if (i)
		{
			if ((i % 8) == 0)
				fprintf(f, ";\nextern Sample ");
			else
				fprintf(f, ", ");
		}
		fprintf(f, "sample%d", i);
	}
	fprintf(f, ";\n");

	//
	// dump Instrument[]
	//
	int idx2;
	int idx = 0;
	for (Instrument *instrument : instruments)
	{
		int zones = instrument->zones.size();
		idx2 = 0;
		for (Zone *z : instrument->zones)
		{
			int keyLo = 0;
			int keyHi = 127;
			int veloLo = 0;
			int veloHi = 127;
			int sampleIdx = -1;

			std::vector<GeneratorList *> gl;
			for (GeneratorList *g : z->generators)
			{
				const char *name = generatorNames[g->gen];
				if (g->gen == Gen_KeyRange)
				{
					keyLo = g->amount.lo;
					keyHi = g->amount.hi;
				}
				else if (g->gen == Gen_VelRange)
				{
					veloLo = g->amount.lo;
					veloHi = g->amount.hi;
				}
				else if (g->gen == Gen_SampleId)
					sampleIdx = g->amount.uword;
				else
					gl.push_back(g);
			}
			int idx3 = 0;
			for (GeneratorList *g : gl)
			{
				if ((idx3 % 8) == 0)
				{
					if (idx3)
						fprintf(f, ";\n");
					fprintf(f, "static Generator ");
				}
				else
					fprintf(f, ", ");
				fprintf(f, "ge_%d_%d_%d(%d, %d)",
						idx, idx2, idx3, int(g->gen), g->amount.sword);
				++idx3;
			}
			fprintf(f, ";\n");
			int n = gl.size();
			if (n)
			{
				fprintf(f, "static Generator* geList_%d_%d[%d] = {\n      ", idx, idx2, n);
				for (int i = 0; i < n; ++i)
				{
					if (i)
					{
						fprintf(f, ", ");
						if ((i % 8) == 0)
							fprintf(f, "\n      ");
					}
					fprintf(f, "&ge_%d_%d_%d", idx, idx2, i);
				}
				fprintf(f, "\n      };\n");
			}
			if (sampleIdx == -1)
				fprintf(f, "static IZone iz%d_%d(%d, %d, %d, %d, 0, 0, 0);\n", idx, idx2, keyLo, keyHi, veloLo, veloHi);
			else if (n)
				fprintf(f, "static IZone iz%d_%d(%d, %d, %d, %d, &sample%d, %d, geList_%d_%d);  // %s\n",
						idx, idx2, keyLo, keyHi, veloLo, veloHi, sampleIdx, n, idx, idx2,
						samples[sampleIdx]->name);
			else
				fprintf(f, "static IZone iz%d_%d(%d, %d, %d, %d, &sample%d, %d, 0);  // %s\n",
						idx, idx2, keyLo, keyHi, veloLo, veloHi, sampleIdx, n,
						samples[sampleIdx]->name);

			++idx2;
		}

		fprintf(f, "static IZone* izones%d[%d] = {\n", idx, zones);
		idx2 = 0;
		for (int i = 0; i < zones; ++i)
			fprintf(f, "      &iz%d_%d,\n", idx, idx2++);
		fprintf(f, "      };\n");
		fprintf(f, "static Instrument instr%d(0, %d, izones%d);\n\n", idx, zones, idx);
		++idx;
	}
	//
	// dump Preset[]
	//
	idx = 0;
	for (Preset *p : presets)
	{
		idx2 = 0;
		int zones = p->zones.size();
		for (Zone *z : p->zones)
		{
			int keyLo = 0;
			int keyHi = 127;
			int veloLo = 0;
			int veloHi = 127;
			int instrIdx = -1;

			for (GeneratorList *g : z->generators)
			{
				const char *name = generatorNames[g->gen];
				if (g->gen == Gen_KeyRange)
				{
					keyLo = g->amount.lo;
					keyHi = g->amount.hi;
				}
				else if (g->gen == Gen_VelRange)
				{
					veloLo = g->amount.lo;
					veloHi = g->amount.hi;
				}
				else if (g->gen == Gen_Instrument)
					instrIdx = g->amount.uword;
			}
			if (keyLo == 0 && keyHi == 127 && veloLo == 0 && veloHi == 127)
			{
				if (instrIdx == -1)
					fprintf(f, "static PZone pz%d_%d(0);\n", idx, idx2);
				else
					fprintf(f, "static PZone pz%d_%d(&instr%d);\n", idx, idx2, instrIdx);
			}
			else
			{
				if (instrIdx == -1)
					fprintf(f, "static PZone pz%d_%d(%d, %d, %d, %d, 0);\n", idx, idx2, keyLo, keyHi, veloLo, veloHi);
				else
					fprintf(f, "static PZone pz%d_%d(%d, %d, %d, %d, &instr%d);\n", idx, idx2, keyLo, keyHi, veloLo, veloHi, instrIdx);
			}
			++idx2;
		}
		fprintf(f, "static PZone* pzones%d[%d] = {\n", idx, zones);
		idx2 = 0;
		for (int i = 0; i < zones; ++i)
		{
			fprintf(f, "      &pz%d_%d,\n", idx, idx2);
			++idx2;
		}
		fprintf(f, "      };\n");

		fprintf(f, "static Preset preset%d(%d, %d, 0, %d, pzones%d);  // %s\n\n",
				idx, p->preset, p->bank, zones, idx, p->name);
		++idx;
	}

	fprintf(f, "static Preset* sfPresets[%d] = {\n", (int)presets.size());
	for (int idx = 0; idx < presets.size(); ++idx)
		fprintf(f, "      &preset%d,   // %s\n", idx, presets[idx]->name);
	fprintf(f, "      };\n");

	fprintf(f, "static SFont _buildinSf(%d, sfPresets);\n", (int)presets.size());
	fprintf(f, "SFont* createSf() { return &_buildinSf; }\n");
	fprintf(f, "} // end namespace\n");
	fclose(f);
	return true;
}

//---------------------------------------------------------
//   writeCode
//---------------------------------------------------------

bool SoundFont::writeCode(std::vector<int> pnums)
{
	printf("write code\n");

	int n = samples.size();
	f = fopen("sf.cpp", "w+");
	fprintf(f, "//\n//      this is generated code, do not change\n//\n");
	fprintf(f, "#include \"sfont.h\"\n\n");
	fprintf(f, "namespace FluidS {\n\n");

	//
	// dump wave samples
	//
	std::vector<int> sampleIdx;
	for (int idx = 0; idx < n; ++idx)
	{
		if (!checkSample(pnums, presets, instruments, idx))
			continue;
		Sample *s = samples[idx];
		writeCSample(s, idx);
		sampleIdx.push_back(idx);
	}

	//
	// dump Sample[]
	//
	for (int idx : sampleIdx)
	{
		Sample *s = samples[idx];
		fprintf(f, "Sample sample%d(%d, %d, %d, %d, %d, %d, %d, %d, wave%d);\n",
				idx,
				0,				   // sample start
				s->end - s->start, // samples
				s->loopstart - s->start,
				s->loopend - s->start,
				s->samplerate,
				s->origpitch, s->pitchadj, s->sampletype,
				idx);
	}

	//
	// dump Instrument[]
	//
	int idx2;
	int idx = 0;
	for (Instrument *instrument : instruments)
	{
		if (!checkInstrument(pnums, presets, idx))
		{
			++idx;
			continue;
		}
		int zones = instrument->zones.size();
		idx2 = 0;
		for (Zone *z : instrument->zones)
		{
			int keyLo = 0;
			int keyHi = 127;
			int veloLo = 0;
			int veloHi = 127;
			int sampleIdx = -1;

			std::vector<GeneratorList *> gl;

			for (GeneratorList *g : z->generators)
			{
				const char *name = generatorNames[g->gen];
				if (g->gen == Gen_KeyRange)
				{
					keyLo = g->amount.lo;
					keyHi = g->amount.hi;
				}
				else if (g->gen == Gen_VelRange)
				{
					veloLo = g->amount.lo;
					veloHi = g->amount.hi;
				}
				else if (g->gen == Gen_SampleId)
					sampleIdx = g->amount.uword;
				else
					gl.push_back(g);
			}
			int idx3 = 0;
			for (GeneratorList *g : gl)
			{
				if ((idx3 % 8) == 0)
				{
					if (idx3)
						fprintf(f, ";\n");
					fprintf(f, "static Generator ");
				}
				else
					fprintf(f, ", ");
				fprintf(f, "ge_%d_%d_%d(%d, %d)",
						idx, idx2, idx3, int(g->gen), g->amount.sword);
				++idx3;
			}
			fprintf(f, ";\n");
			int n = gl.size();
			if (n)
			{
				fprintf(f, "static Generator* geList_%d_%d[%d] = {\n      ", idx, idx2, n);
				for (int i = 0; i < n; ++i)
				{
					if (i)
					{
						fprintf(f, ", ");
						if ((i % 8) == 0)
							fprintf(f, "\n      ");
					}
					fprintf(f, "&ge_%d_%d_%d", idx, idx2, i);
				}
				fprintf(f, "\n      };\n");
			}
			if (sampleIdx == -1)
				fprintf(f, "static IZone iz%d_%d(%d, %d, %d, %d, 0, 0, 0);\n", idx, idx2, keyLo, keyHi, veloLo, veloHi);
			else
				fprintf(f, "static IZone iz%d_%d(%d, %d, %d, %d, &sample%d, %d, geList_%d_%d);\n",
						idx, idx2, keyLo, keyHi, veloLo, veloHi, sampleIdx, n, idx, idx2);
			++idx2;
		}

		fprintf(f, "static IZone* izones%d[%d] = {\n", idx, zones);
		idx2 = 0;
		for (int i = 0; i < zones; ++i)
			fprintf(f, "      &iz%d_%d,\n", idx, idx2++);
		fprintf(f, "      };\n");
		fprintf(f, "static Instrument instr%d(0, %d, izones%d);\n\n", idx, zones, idx);
		++idx;
	}
	//
	// dump Preset[]
	//

	for (int idx : pnums)
	{
		Preset *p = presets[idx];
		idx2 = 0;
		int zones = p->zones.size();
		for (Zone *z : p->zones)
		{
			int keyLo = 0;
			int keyHi = 127;
			int veloLo = 0;
			int veloHi = 127;
			int instrIdx = -1;
			for (GeneratorList *g : z->generators)
			{
				const char *name = generatorNames[g->gen];
				if (g->gen == Gen_KeyRange)
				{
					keyLo = g->amount.lo;
					keyHi = g->amount.hi;
				}
				else if (g->gen == Gen_VelRange)
				{
					veloLo = g->amount.lo;
					veloHi = g->amount.hi;
				}
				else if (g->gen == Gen_Instrument)
					instrIdx = g->amount.uword;
			}
			if (keyLo == 0 && keyHi == 127 && veloLo == 0 && veloHi == 127)
			{
				if (instrIdx == -1)
					fprintf(f, "static PZone pz%d_%d(0);\n", idx, idx2);
				else
					fprintf(f, "static PZone pz%d_%d(&instr%d);\n", idx, idx2, instrIdx);
			}
			else
			{
				if (instrIdx == -1)
					fprintf(f, "static PZone pz%d_%d(%d, %d, %d, %d, 0);\n", idx, idx2, keyLo, keyHi, veloLo, veloHi);
				else
					fprintf(f, "static PZone pz%d_%d(%d, %d, %d, %d, &instr%d);\n", idx, idx2, keyLo, keyHi, veloLo, veloHi, instrIdx);
			}
			++idx2;
		}
		fprintf(f, "static PZone* pzones%d[%d] = {\n", idx, zones);
		idx2 = 0;
		for (int i = 0; i < zones; ++i)
		{
			fprintf(f, "      &pz%d_%d,\n", idx, idx2);
			++idx2;
		}
		fprintf(f, "      };\n");

		fprintf(f, "static Preset preset%d(%d, %d, 0, %d, pzones%d);    // %s\n\n",
				idx, p->preset, p->bank, (int)p->zones.size(), idx, p->name);
		++idx;
	}

	fprintf(f, "static Preset* sfPresets[%d] = {\n", (int)pnums.size());
	for (int idx : pnums)
		fprintf(f, "      &preset%d,   // %s\n", idx, presets[idx]->name);
	fprintf(f, "      };\n");

	fprintf(f, "static SFont _buildinSf(%d, sfPresets);\n", (int)pnums.size());
	fprintf(f, "SFont* createSf() { return &_buildinSf; }\n");
	fprintf(f, "} // end namespace\n");
	fclose(f);
	return true;
}

//---------------------------------------------------------
//   dumpPresets
//---------------------------------------------------------

void SoundFont::dumpPresets()
{
	int idx = 0;
	for (const Preset *p : presets)
	{
		printf("%03d %04x-%02x %s\n", idx, p->bank, p->preset, p->name);
		++idx;
	}
}
