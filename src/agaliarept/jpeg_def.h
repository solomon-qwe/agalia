#pragma once

#include <stdint.h>	// for uint**_t 

namespace analyze_JPEG
{
	enum JPEG_Marker
	{
		SOF0 = 0xFFC0,	// Baseline DCT 
		SOF1 = 0xFFC1,	// Extended sequential DCT 
		SOF2 = 0xFFC2,	// Progressive DCT 
		SOF3 = 0xFFC3,	// Lossless (sequential) 
		SOF5 = 0xFFC5,	// Differential sequential DCT 
		SOF6 = 0xFFC6,	// Differential progressive DCT 
		SOF7 = 0xFFC7,	// Differential lossless (sequential) 
		JPG = 0xFFC8,	// Reserved for JPEG extensions 
		SOF9 = 0xFFC9,	// Extended sequential DCT 
		SOF10 = 0xFFCA,	// Progressive DCT 
		SOF11 = 0xFFCB,	// Lossless (sequential) 

		SOF13 = 0xFFCD,	// Differential sequential DCT 
		SOF14 = 0xFFCE,	// Differential progressive DCT 
		SOF15 = 0xFFCF,	// Differential lossless (sequential) 

		DHT = 0xFFC4,	// Define Huffman table(s) 
		DAC = 0xFFCC,	// Define arithmetic coding conditioning(s) 

		RST0 = 0xFFD0,	// Restart 
		RST1 = 0xFFD1,	// Restart 
		RST2 = 0xFFD2,	// Restart 
		RST3 = 0xFFD3,	// Restart 
		RST4 = 0xFFD4,	// Restart 
		RST5 = 0xFFD5,	// Restart 
		RST6 = 0xFFD6,	// Restart 
		RST7 = 0xFFD7,	// Restart 

		SOI = 0xFFD8,	// Start of image 
		EOI = 0xFFD9,	// End of image 
		SOS = 0xFFDA,	// Start of scan 
		DQT = 0xFFDB,	// Define quantization table(s) 
		DNL = 0xFFDC,	// Define number of lines 
		DRI = 0xFFDD,	// Define restart interval 
		DHP = 0xFFDE,	// Define hierarchical progression 
		EXP = 0xFFDF,	// Expand reference component(s) 
		APP0 = 0xFFE0,	// Reserved for application segments 
		APP1 = 0xFFE1,	// Reserved for application segments 
		APP2 = 0xFFE2,	// Reserved for application segments 
		APP3 = 0xFFE3,	// Reserved for application segments 
		APP4 = 0xFFE4,	// Reserved for application segments 
		APP5 = 0xFFE5,	// Reserved for application segments 
		APP6 = 0xFFE6,	// Reserved for application segments 
		APP7 = 0xFFE7,	// Reserved for application segments 
		APP8 = 0xFFE8,	// Reserved for application segments 
		APP9 = 0xFFE9,	// Reserved for application segments 
		APP10 = 0xFFEA,	// Reserved for application segments 
		APP11 = 0xFFEB,	// Reserved for application segments 
		APP12 = 0xFFEC,	// Reserved for application segments 
		APP13 = 0xFFED,	// Reserved for application segments 
		APP14 = 0xFFEE,	// Reserved for application segments 
		APP15 = 0xFFEF,	// Reserved for application segments 
		JPG0 = 0xFFF0,	// Reserved for JPEG extensions 
		JPG1 = 0xFFF1,	// Reserved for JPEG extensions 
		JPG2 = 0xFFF2,	// Reserved for JPEG extensions 
		JPG3 = 0xFFF3,	// Reserved for JPEG extensions 
		JPG4 = 0xFFF4,	// Reserved for JPEG extensions 
		JPG5 = 0xFFF5,	// Reserved for JPEG extensions 
		JPG6 = 0xFFF6,	// Reserved for JPEG extensions 
		JPG7 = 0xFFF7,	// Reserved for JPEG extensions 
		JPG8 = 0xFFF8,	// Reserved for JPEG extensions 
		JPG9 = 0xFFF9,	// Reserved for JPEG extensions 
		JPG10 = 0xFFFA,	// Reserved for JPEG extensions 
		JPG11 = 0xFFFB,	// Reserved for JPEG extensions 
		JPG12 = 0xFFFC,	// Reserved for JPEG extensions 
		JPG13 = 0xFFFD,	// Reserved for JPEG extensions 
		COM = 0xFFFE,	// Comment 
		TEM = 0xFF01,	// For temporary private use in arithmetic coding 
		RES_Start = 0xFF02,	// RES X'FF02' through X'FFBF' Reserved 
		RES_End = 0xFFBF
	};

#pragma pack(push, 1)
	struct JPEGSEGMENT
	{
		uint16_t marker;
	};

	struct JPEGSEGMENT_APP : public JPEGSEGMENT
	{
		uint16_t length;
	};

	struct JPEGSEGMENT_APP0_JFIF : public JPEGSEGMENT_APP
	{
		uint8_t identifier[5];
		uint8_t major_version;
		uint8_t minor_version;
		uint8_t units;
		uint16_t Xdensity;
		uint16_t Ydensity;
		uint8_t Xthumbnail;
		uint8_t Ythumbnail;
		//	(RGB)n
	};

	struct JPEGSEGMENT_APPX : public JPEGSEGMENT_APP
	{
		uint8_t identifier[1];
	};

	struct JPEGSEGMENT_DQT : public JPEGSEGMENT
	{
		uint16_t Lq;
	};

	struct JPEGSEGMENT_DQT_BODY
	{
		uint8_t Tq : 4;
		uint8_t Pq : 4;
		union {
			uint8_t Q8[64];
			uint16_t Q16[64];
		};
	};

	struct JPEGSEGMENT_SOF : public JPEGSEGMENT
	{
		uint16_t Lf;
		uint8_t P;
		uint16_t Y;
		uint16_t X;
		uint8_t Nf;
		struct {
			uint8_t C;
			uint8_t V : 4;
			uint8_t H : 4;
			uint8_t Tq;
		} comp[1];
	};

	struct JPEGSEGMENT_DHT : public JPEGSEGMENT
	{
		uint16_t Lh;
	};

	struct JPEGSEGMENT_DHT_BODY
	{
		uint8_t Th : 4;
		uint8_t Tc : 4;
		uint8_t L[16];
		uint8_t V[1];
	};

	struct JPEGSEGMENT_DAC : public JPEGSEGMENT
	{
		uint16_t La;
		struct {
			uint8_t Tb : 4;
			uint8_t Tc : 4;
			uint8_t Cs;
		} comp[1];
	};

	struct JPEGSEGMENT_SOS : public JPEGSEGMENT
	{
		uint16_t Ls;
		uint8_t Ns;
		struct {
			uint8_t Cs;
			uint8_t Ta : 4;
			uint8_t Td : 4;
		} comp[1];
	};

	struct JPEGSEGMENT_SOS_Suffix
	{
		uint8_t Ss;
		uint8_t Se;
		uint8_t Al : 4;
		uint8_t Ah : 4;
	};

	struct JPEGSEGMENT_COM : public JPEGSEGMENT
	{
		uint16_t Lc;
		uint8_t Cm[1];
	};

	struct JPEGSEGMENT_DRI : public JPEGSEGMENT
	{
		uint16_t Lr;
		uint16_t Ri;
	};
}

#pragma pack(pop)
