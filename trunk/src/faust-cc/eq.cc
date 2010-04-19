namespace eq {
// generated from file '../src/faust/eq.dsp'

float 	fConst0;
float 	fConst1;
FAUSTFLOAT 	fslider0;
FAUSTFLOAT 	fslider1;
float 	fConst2;
float 	fConst3;
FAUSTFLOAT 	fslider2;
FAUSTFLOAT 	fslider3;
float 	fConst4;
float 	fConst5;
FAUSTFLOAT 	fslider4;
FAUSTFLOAT 	fslider5;
float 	fConst6;
float 	fConst7;
FAUSTFLOAT 	fslider6;
FAUSTFLOAT 	fslider7;
float 	fConst8;
float 	fConst9;
FAUSTFLOAT 	fslider8;
FAUSTFLOAT 	fslider9;
float 	fConst10;
float 	fConst11;
FAUSTFLOAT 	fslider10;
FAUSTFLOAT 	fslider11;
float 	fConst12;
float 	fConst13;
FAUSTFLOAT 	fslider12;
FAUSTFLOAT 	fslider13;
float 	fConst14;
float 	fConst15;
FAUSTFLOAT 	fslider14;
FAUSTFLOAT 	fslider15;
float 	fConst16;
float 	fConst17;
FAUSTFLOAT 	fslider16;
FAUSTFLOAT 	fslider17;
float 	fConst18;
float 	fConst19;
FAUSTFLOAT 	fslider18;
FAUSTFLOAT 	fslider19;
float 	fRec9[3];
float 	fRec8[3];
float 	fRec7[3];
float 	fRec6[3];
float 	fRec5[3];
float 	fRec4[3];
float 	fRec3[3];
float 	fRec2[3];
float 	fRec1[3];
float 	fRec0[3];
int	fSamplingFreq;

void init(int samplingFreq)
{
	fSamplingFreq = samplingFreq;
	fConst0 = tanf((50265.482457f / fSamplingFreq));
	fConst1 = (2 * ((fConst0 * fConst0) - 1));
	fConst2 = tanf((25132.741229f / fSamplingFreq));
	fConst3 = (2 * ((fConst2 * fConst2) - 1));
	fConst4 = tanf((12566.370614f / fSamplingFreq));
	fConst5 = (2 * ((fConst4 * fConst4) - 1));
	fConst6 = tanf((6283.185307f / fSamplingFreq));
	fConst7 = (2 * ((fConst6 * fConst6) - 1));
	fConst8 = tanf((3141.592654f / fSamplingFreq));
	fConst9 = (2 * ((fConst8 * fConst8) - 1));
	fConst10 = tanf((1570.796327f / fSamplingFreq));
	fConst11 = (2 * ((fConst10 * fConst10) - 1));
	fConst12 = tanf((785.398163f / fSamplingFreq));
	fConst13 = (2 * ((fConst12 * fConst12) - 1));
	fConst14 = tanf((392.699082f / fSamplingFreq));
	fConst15 = (2 * ((fConst14 * fConst14) - 1));
	fConst16 = tanf((196.349541f / fSamplingFreq));
	fConst17 = (2 * ((fConst16 * fConst16) - 1));
	fConst18 = tanf((98.17477f / fSamplingFreq));
	fConst19 = (2 * ((fConst18 * fConst18) - 1));
	for (int i=0; i<3; i++) fRec9[i] = 0;
	for (int i=0; i<3; i++) fRec8[i] = 0;
	for (int i=0; i<3; i++) fRec7[i] = 0;
	for (int i=0; i<3; i++) fRec6[i] = 0;
	for (int i=0; i<3; i++) fRec5[i] = 0;
	for (int i=0; i<3; i++) fRec4[i] = 0;
	for (int i=0; i<3; i++) fRec3[i] = 0;
	for (int i=0; i<3; i++) fRec2[i] = 0;
	for (int i=0; i<3; i++) fRec1[i] = 0;
	for (int i=0; i<3; i++) fRec0[i] = 0;
}

void compute(int count, float *input0, float *output0)
{
	float 	fSlow0 = fslider0;
	float 	fSlow1 = (powf(10,(5.000000e-02f * (0 - fslider1))) / fSlow0);
	float 	fSlow2 = (1 + (fConst0 * (fConst0 - fSlow1)));
	float 	fSlow3 = (1.0f / (1 + (fConst0 * (fConst0 + fSlow1))));
	float 	fSlow4 = fslider2;
	float 	fSlow5 = (powf(10,(5.000000e-02f * (0 - fslider3))) / fSlow4);
	float 	fSlow6 = (1 + (fConst2 * (fConst2 - fSlow5)));
	float 	fSlow7 = (1.0f / (1 + (fConst2 * (fConst2 + fSlow5))));
	float 	fSlow8 = fslider4;
	float 	fSlow9 = (powf(10,(5.000000e-02f * (0 - fslider5))) / fSlow8);
	float 	fSlow10 = (1 + (fConst4 * (fConst4 - fSlow9)));
	float 	fSlow11 = (1.0f / (1 + (fConst4 * (fConst4 + fSlow9))));
	float 	fSlow12 = fslider6;
	float 	fSlow13 = (powf(10,(5.000000e-02f * (0 - fslider7))) / fSlow12);
	float 	fSlow14 = (1 + (fConst6 * (fConst6 - fSlow13)));
	float 	fSlow15 = (1.0f / (1 + (fConst6 * (fConst6 + fSlow13))));
	float 	fSlow16 = fslider8;
	float 	fSlow17 = (powf(10,(5.000000e-02f * (0 - fslider9))) / fSlow16);
	float 	fSlow18 = (1 + (fConst8 * (fConst8 - fSlow17)));
	float 	fSlow19 = (1.0f / (1 + (fConst8 * (fConst8 + fSlow17))));
	float 	fSlow20 = fslider10;
	float 	fSlow21 = (powf(10,(5.000000e-02f * (0 - fslider11))) / fSlow20);
	float 	fSlow22 = (1 + (fConst10 * (fConst10 - fSlow21)));
	float 	fSlow23 = (1.0f / (1 + (fConst10 * (fConst10 + fSlow21))));
	float 	fSlow24 = fslider12;
	float 	fSlow25 = (powf(10,(5.000000e-02f * (0 - fslider13))) / fSlow24);
	float 	fSlow26 = (1 + (fConst12 * (fConst12 - fSlow25)));
	float 	fSlow27 = (1.0f / (1 + (fConst12 * (fConst12 + fSlow25))));
	float 	fSlow28 = fslider14;
	float 	fSlow29 = (powf(10,(5.000000e-02f * (0 - fslider15))) / fSlow28);
	float 	fSlow30 = (1 + (fConst14 * (fConst14 - fSlow29)));
	float 	fSlow31 = (1.0f / (1 + (fConst14 * (fConst14 + fSlow29))));
	float 	fSlow32 = fslider16;
	float 	fSlow33 = (powf(10,(5.000000e-02f * (0 - fslider17))) / fSlow32);
	float 	fSlow34 = (1 + (fConst16 * (fConst16 - fSlow33)));
	float 	fSlow35 = (1.0f / (1 + (fConst16 * (fConst16 + fSlow33))));
	float 	fSlow36 = fslider18;
	float 	fSlow37 = (powf(10,(5.000000e-02f * (0 - fslider19))) / fSlow36);
	float 	fSlow38 = (1 + (fConst18 * (fConst18 - fSlow37)));
	float 	fSlow39 = (1.0f / (1 + (fConst18 * (fConst18 + fSlow37))));
	float 	fSlow40 = (1.0f / fSlow36);
	float 	fSlow41 = (1 + (fConst18 * (fConst18 - fSlow40)));
	float 	fSlow42 = (1 + (fConst18 * (fConst18 + fSlow40)));
	float 	fSlow43 = (1.0f / fSlow32);
	float 	fSlow44 = (1 + (fConst16 * (fConst16 - fSlow43)));
	float 	fSlow45 = (1 + (fConst16 * (fConst16 + fSlow43)));
	float 	fSlow46 = (1.0f / fSlow28);
	float 	fSlow47 = (1 + (fConst14 * (fConst14 - fSlow46)));
	float 	fSlow48 = (1 + (fConst14 * (fConst14 + fSlow46)));
	float 	fSlow49 = (1.0f / fSlow24);
	float 	fSlow50 = (1 + (fConst12 * (fConst12 - fSlow49)));
	float 	fSlow51 = (1 + (fConst12 * (fConst12 + fSlow49)));
	float 	fSlow52 = (1.0f / fSlow20);
	float 	fSlow53 = (1 + (fConst10 * (fConst10 - fSlow52)));
	float 	fSlow54 = (1 + (fConst10 * (fConst10 + fSlow52)));
	float 	fSlow55 = (1.0f / fSlow16);
	float 	fSlow56 = (1 + (fConst8 * (fConst8 - fSlow55)));
	float 	fSlow57 = (1 + (fConst8 * (fConst8 + fSlow55)));
	float 	fSlow58 = (1.0f / fSlow12);
	float 	fSlow59 = (1 + (fConst6 * (fConst6 - fSlow58)));
	float 	fSlow60 = (1 + (fConst6 * (fConst6 + fSlow58)));
	float 	fSlow61 = (1.0f / fSlow8);
	float 	fSlow62 = (1 + (fConst4 * (fConst4 - fSlow61)));
	float 	fSlow63 = (1 + (fConst4 * (fConst4 + fSlow61)));
	float 	fSlow64 = (1.0f / fSlow4);
	float 	fSlow65 = (1 + (fConst2 * (fConst2 - fSlow64)));
	float 	fSlow66 = (1 + (fConst2 * (fConst2 + fSlow64)));
	float 	fSlow67 = (1.0f / fSlow0);
	float 	fSlow68 = (1 + (fConst0 * (fConst0 - fSlow67)));
	float 	fSlow69 = (1 + (fConst0 * (fConst0 + fSlow67)));
	for (int i=0; i<count; i++) {
		float fTemp0 = (fConst1 * fRec0[1]);
		float fTemp1 = (fConst3 * fRec1[1]);
		float fTemp2 = (fConst5 * fRec2[1]);
		float fTemp3 = (fConst7 * fRec3[1]);
		float fTemp4 = (fConst9 * fRec4[1]);
		float fTemp5 = (fConst11 * fRec5[1]);
		float fTemp6 = (fConst13 * fRec6[1]);
		float fTemp7 = (fConst15 * fRec7[1]);
		float fTemp8 = (fConst17 * fRec8[1]);
		float fTemp9 = (fConst19 * fRec9[1]);
		fRec9[0] = ((float)input0[i] - (fSlow39 * ((fSlow38 * fRec9[2]) + fTemp9)));
		fRec8[0] = ((fSlow39 * ((fTemp9 + (fSlow42 * fRec9[0])) + (fSlow41 * fRec9[2]))) - (fSlow35 * ((fSlow34 * fRec8[2]) + fTemp8)));
		fRec7[0] = ((fSlow35 * ((fTemp8 + (fSlow45 * fRec8[0])) + (fSlow44 * fRec8[2]))) - (fSlow31 * ((fSlow30 * fRec7[2]) + fTemp7)));
		fRec6[0] = ((fSlow31 * ((fTemp7 + (fSlow48 * fRec7[0])) + (fSlow47 * fRec7[2]))) - (fSlow27 * ((fSlow26 * fRec6[2]) + fTemp6)));
		fRec5[0] = ((fSlow27 * ((fTemp6 + (fSlow51 * fRec6[0])) + (fSlow50 * fRec6[2]))) - (fSlow23 * ((fSlow22 * fRec5[2]) + fTemp5)));
		fRec4[0] = ((fSlow23 * ((fTemp5 + (fSlow54 * fRec5[0])) + (fSlow53 * fRec5[2]))) - (fSlow19 * ((fSlow18 * fRec4[2]) + fTemp4)));
		fRec3[0] = ((fSlow19 * ((fTemp4 + (fSlow57 * fRec4[0])) + (fSlow56 * fRec4[2]))) - (fSlow15 * ((fSlow14 * fRec3[2]) + fTemp3)));
		fRec2[0] = ((fSlow15 * ((fTemp3 + (fSlow60 * fRec3[0])) + (fSlow59 * fRec3[2]))) - (fSlow11 * ((fSlow10 * fRec2[2]) + fTemp2)));
		fRec1[0] = ((fSlow11 * ((fTemp2 + (fSlow63 * fRec2[0])) + (fSlow62 * fRec2[2]))) - (fSlow7 * ((fSlow6 * fRec1[2]) + fTemp1)));
		fRec0[0] = ((fSlow7 * ((fTemp1 + (fSlow66 * fRec1[0])) + (fSlow65 * fRec1[2]))) - (fSlow3 * ((fSlow2 * fRec0[2]) + fTemp0)));
		output0[i] = (FAUSTFLOAT)(fSlow3 * ((fTemp0 + (fSlow69 * fRec0[0])) + (fSlow68 * fRec0[2])));
		// post processing
		fRec0[2] = fRec0[1]; fRec0[1] = fRec0[0];
		fRec1[2] = fRec1[1]; fRec1[1] = fRec1[0];
		fRec2[2] = fRec2[1]; fRec2[1] = fRec2[0];
		fRec3[2] = fRec3[1]; fRec3[1] = fRec3[0];
		fRec4[2] = fRec4[1]; fRec4[1] = fRec4[0];
		fRec5[2] = fRec5[1]; fRec5[1] = fRec5[0];
		fRec6[2] = fRec6[1]; fRec6[1] = fRec6[0];
		fRec7[2] = fRec7[1]; fRec7[1] = fRec7[0];
		fRec8[2] = fRec8[1]; fRec8[1] = fRec8[0];
		fRec9[2] = fRec9[1]; fRec9[1] = fRec9[0];
	}
}

static struct RegisterParams { RegisterParams(); } RegisterParams;
RegisterParams::RegisterParams()
{
	registerVar("eq.f250","","S","gain (dB) at 250 Hz",&fslider13, 0.0f, -50.0f, 10.0f, 0.1f);
	registerVar("eq.Q250","","S","",&fslider12, 1.0f, 1.0f, 100.0f, 1.0f);
	registerVar("eq.f500","","S","gain (dB) at 500 Hz",&fslider11, 0.0f, -50.0f, 10.0f, 0.1f);
	registerVar("eq.Q500","","S","",&fslider10, 1.0f, 1.0f, 100.0f, 1.0f);
	registerVar("eq.f62_5","","S","gain (dB) at 62.5 Hz",&fslider17, 0.0f, -50.0f, 10.0f, 0.1f);
	registerVar("eq.Q62_5","","S","",&fslider16, 1.0f, 1.0f, 100.0f, 1.0f);
	registerVar("eq.f125","","S","gain (dB) at 125 Hz",&fslider15, 0.0f, -50.0f, 10.0f, 0.1f);
	registerVar("eq.Q125","","S","",&fslider14, 1.0f, 1.0f, 100.0f, 1.0f);
	registerVar("eq.f31_25","","S","gain (dB) at 31.25 Hz",&fslider19, 0.0f, -50.0f, 10.0f, 0.1f);
	registerVar("eq.Q31_25","","S","",&fslider18, 1.0f, 1.0f, 100.0f, 1.0f);
	registerVar("eq.f2k","","S","gain (dB) at 2 kHz",&fslider7, 0.0f, -50.0f, 10.0f, 0.1f);
	registerVar("eq.Q2k","","S","",&fslider6, 1.0f, 1.0f, 100.0f, 1.0f);
	registerVar("eq.f4k","","S","gain (dB) at 4 kHz",&fslider5, 0.0f, -50.0f, 10.0f, 0.1f);
	registerVar("eq.Q4k","","S","",&fslider4, 1.0f, 1.0f, 100.0f, 1.0f);
	registerVar("eq.f8k","","S","gain (dB) at 8 kHz",&fslider3, 0.0f, -50.0f, 10.0f, 0.1f);
	registerVar("eq.Q8k","","S","",&fslider2, 1.0f, 1.0f, 100.0f, 1.0f);
	registerVar("eq.f16k","","S","gain (dB) at 16 kHz",&fslider1, 0.0f, -50.0f, 10.0f, 0.1f);
	registerVar("eq.Q16k","","S","",&fslider0, 1.0f, 1.0f, 100.0f, 1.0f);
	registerVar("eq.f1k","","S","gain (dB) at 1 kHz",&fslider9, 0.0f, -50.0f, 10.0f, 0.1f);
	registerVar("eq.Q1k","","S","",&fslider8, 1.0f, 1.0f, 100.0f, 1.0f);
	registerInit(init);
}

} // end namespace eq
