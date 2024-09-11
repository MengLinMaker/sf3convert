#pragma once
#include <fstream>
#include <QtCore/QFile>

//---------------------------------------------------------
//   sfVersionTag
//---------------------------------------------------------

struct sfVersionTag
{
	int major;
	int minor;
};

enum Modulator
{
};
enum Generator
{
	Gen_StartAddrOfs,
	Gen_EndAddrOfs,
	Gen_StartLoopAddrOfs,
	Gen_EndLoopAddrOfs,
	Gen_StartAddrCoarseOfs,
	Gen_ModLFO2Pitch,
	Gen_VibLFO2Pitch,
	Gen_ModEnv2Pitch,
	Gen_FilterFc,
	Gen_FilterQ,
	Gen_ModLFO2FilterFc,
	Gen_ModEnv2FilterFc,
	Gen_EndAddrCoarseOfs,
	Gen_ModLFO2Vol,
	Gen_Unused1,
	Gen_ChorusSend,
	Gen_ReverbSend,
	Gen_Pan,
	Gen_Unused2,
	Gen_Unused3,
	Gen_Unused4,
	Gen_ModLFODelay,
	Gen_ModLFOFreq,
	Gen_VibLFODelay,
	Gen_VibLFOFreq,
	Gen_ModEnvDelay,
	Gen_ModEnvAttack,
	Gen_ModEnvHold,
	Gen_ModEnvDecay,
	Gen_ModEnvSustain,
	Gen_ModEnvRelease,
	Gen_Key2ModEnvHold,
	Gen_Key2ModEnvDecay,
	Gen_VolEnvDelay,
	Gen_VolEnvAttack,
	Gen_VolEnvHold,
	Gen_VolEnvDecay,
	Gen_VolEnvSustain,
	Gen_VolEnvRelease,
	Gen_Key2VolEnvHold,
	Gen_Key2VolEnvDecay,
	Gen_Instrument,
	Gen_Reserved1,
	Gen_KeyRange,
	Gen_VelRange,
	Gen_StartLoopAddrCoarseOfs,
	Gen_Keynum,
	Gen_Velocity,
	Gen_Attenuation,
	Gen_Reserved2,
	Gen_EndLoopAddrCoarseOfs,
	Gen_CoarseTune,
	Gen_FineTune,
	Gen_SampleId,
	Gen_SampleModes,
	Gen_Reserved3,
	Gen_ScaleTune,
	Gen_ExclusiveClass,
	Gen_OverrideRootKey,
	Gen_Dummy
};

enum Transform
{
	Linear
};

//---------------------------------------------------------
//   ModulatorList
//---------------------------------------------------------

struct ModulatorList
{
	Modulator src;
	Generator dst;
	int amount;
	Modulator amtSrc;
	Transform transform;
};

//---------------------------------------------------------
//   GeneratorList
//---------------------------------------------------------

union GeneratorAmount
{
	short sword;
	ushort uword;
	struct
	{
		uchar lo, hi;
	};
};

struct GeneratorList
{
	Generator gen;
	GeneratorAmount amount;
};

//---------------------------------------------------------
//   Zone
//---------------------------------------------------------

struct Zone
{
	std::vector<GeneratorList *> generators;
	std::vector<ModulatorList *> modulators;
	int instrumentIndex;
};

//---------------------------------------------------------
//   Preset
//---------------------------------------------------------

struct Preset
{
	char *name{0};
	int preset{0};
	int bank{0};
	int presetBagNdx{0}; // used only for read
	int library{0};
	int genre{0};
	int morphology{0};
	std::vector<Zone *> zones;
};

//---------------------------------------------------------
//   Instrument
//---------------------------------------------------------

struct Instrument
{
	char *name;
	int index; // used only for read
	std::vector<Zone *> zones;

	Instrument();
	~Instrument();
};

//---------------------------------------------------------
//   Sample
//---------------------------------------------------------

struct Sample
{
	char *name;
	uint start;
	uint end;
	uint loopstart;
	uint loopend;
	uint samplerate;

	int origpitch;
	int pitchadj;
	int sampletype;

	Sample();
	~Sample();
};

//---------------------------------------------------------
//   SoundFont
//---------------------------------------------------------

class SoundFont
{
	std::string path;
	sfVersionTag version;
	char *engine;
	char *name;
	char *date;
	char *comment;
	char *tools;
	char *creator;
	char *product;
	char *copyright;

	int samplePos;
	int sampleLen;

	std::vector<Preset *> presets;
	std::vector<Instrument *> instruments;

	std::vector<Zone *> pZones;
	std::vector<Zone *> iZones;
	std::vector<Sample *> samples;

	std::fstream *file;
	FILE *f;

	double _oggQuality;
	double _oggAmp;

	unsigned readDword();
	int readWord();
	int readShort();
	int readByte();
	int readChar();
	int readFourcc(const char *);
	int readFourcc(char *);
	void readSignature(const char *signature);
	void readSignature(char *signature);
	void skip(int);
	void readSection(const char *fourcc, int len);
	void readVersion();
	char *readString(int);
	void readPhdr(int);
	void readBag(int, std::vector<Zone *> *);
	void readMod(int, std::vector<Zone *> *);
	void readGen(int, std::vector<Zone *> *);
	void readInst(int);
	void readShdr(int);

	void writeDword(int);
	void writeWord(unsigned short int);
	void writeByte(unsigned char);
	void writeChar(char);
	void writeShort(short);
	void write(const char *p, int n);
	bool writeSampleFile(Sample *, std::string);
	void writeSample(const Sample *);
	void writeStringSection(const char *fourcc, char *s);
	void writePreset(int zoneIdx, const Preset *);
	void writeModulator(const ModulatorList *);
	void writeGenerator(const GeneratorList *);
	void writeInstrument(int zoneIdx, const Instrument *);

	void writeIfil();
	void writeSmpl();
	void writePhdr();
	void writeBag(const char *fourcc, std::vector<Zone *> *);
	void writeMod(const char *fourcc, const std::vector<Zone *> *);
	void writeGen(const char *fourcc, std::vector<Zone *> *);
	void writeInst();
	void writeShdr();

	int writeCompressedSample(Sample *);
	bool writeCSample(Sample *, int);
	char *readCompressedSample(Sample *);

public:
	SoundFont(const std::string &);
	~SoundFont();
	bool read();
	bool write(std::fstream *, double oggQuality, double oggAmp);
	bool writeCode(std::vector<int>);
	bool writeCode();
	void dumpPresets();
};
