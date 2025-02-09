#include "pch.h"
#include "CppUnitTest.h"

#include <Windows.h>
#include "../inc/agaliarept.h"
#include "util.h"

#include <iomanip>

void get_expected_str_zigzagscan(std::wstringstream& dst, const int table[8][8])
{
	struct {
		int x;
		int y;
	} conv[] = {
		{0, 0}, {1, 0}, {0, 1}, {0, 2}, {1, 1}, {2, 0}, {3, 0}, {2, 1},	// 7
		{1, 2}, {0, 3}, {0, 4}, {1, 3}, {2, 2}, {3, 1}, {4, 0}, {5, 0},	// 15
		{4, 1}, {3, 2}, {2, 3}, {1, 4}, {0, 5}, {0, 6}, {1, 5}, {2, 4},	// 23
		{3, 3}, {4, 2}, {5, 1}, {6, 0}, {7, 0}, {6, 1}, {5, 2}, {4, 3},	// 31
		{3, 4}, {2, 5}, {1, 6}, {0, 7}, {1, 7}, {2, 6}, {3, 5}, {4, 4}, // 39
		{5, 3}, {6, 2}, {7, 1}, {7, 2}, {6, 3}, {5, 4}, {4, 5}, {3, 6}, // 47
		{2, 7}, {3, 7}, {4, 6}, {5, 5}, {6, 4}, {7, 3}, {7, 4}, {6, 5}, // 55
		{5, 6}, {4, 7}, {5, 7}, {6, 6}, {7, 5}, {7, 6}, {6, 7}, {7, 7}	// 63
	};

	dst.str(L"");
	for (int i = 0; i < _countof(conv); i++)
	{
		if (i != 0) dst << L",";
		dst << table[conv[i].y][conv[i].x];
	}
}


inline void get_expected_str_lowerhexlist(std::wstringstream& dst, std::initializer_list<int> v)
{
	dst.str(L"");
	for (auto item = v.begin(); item != v.end(); item++)
	{
		if (item != v.begin()) dst << L",";
		dst << std::hex << std::setw(2) << std::setfill(L'0') << *item;
	}
}


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace unittest
{
	TEST_CLASS(JPEG)
	{
	public:
		err_mgr em;

		TEST_METHOD_INITIALIZE(MethodInitialize)
		{
			em.crt_memchk_init();
		}

		TEST_METHOD_CLEANUP(MethodCleanup)
		{
			em.crt_memchk();
		}



		TEST_METHOD(P1_A1_Parse)
		{
			std::wstring asset_dir;
			get_asset_dir(&asset_dir);

			parse_to_log((asset_dir + L"ITU-T\\T083v1_0\\T83_process1\\A1.JPG").c_str());
		}



		TEST_METHOD(P1_A1_Verify)
		{
			std::wstring asset_dir;
			get_asset_dir(&asset_dir);

			HRESULT hr;

			std::wstring err_str;
			std::wstringstream expected_data;
			agaliaStringPtr temp;

			agaliaPtr<agaliaElement> item;
			agaliaPtr<agaliaElement> next;

			agaliaPtr<agaliaContainer> image;
			hr = getAgaliaImage(&image, (asset_dir + L"ITU-T\\T083v1_0\\T83_process1\\A1.JPG").c_str(), 0, 0);
			Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));

			hr = image->getRootElement(&item);
			Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));

			{	// SOI 
				Assert::AreEqual(0ULL, item->getOffset());

				uint32_t index = 2;

				hr = item->getPropValue(index++, &temp);
				Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
				Assert::AreEqual(L"SOI", temp);
			}

			hr = item->getNext(&next);
			Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
			item = next.detach();

			{	// COM 
				Assert::AreEqual(2ULL, item->getOffset());

				uint32_t index = 2;

				hr = item->getPropValue(index++, &temp);
				Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
				Assert::AreEqual(L"COM", temp);

				hr = item->getPropValue(index++, &temp);	// Lc
				Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
				Assert::AreEqual(L"368", temp);

				const wchar_t* comment = L"Compressed test data stream A1 for ITU-T Rec. T.83 | ISO/IEC 10918-2:1994 (JPEG). Interchange format. Validated Nov. 1993 by Birger Niss (AutoGraph International, Denmark) and Chris Hepburn (Microstar Software Ltd., Canada). Revised and validated May 1994 by William B. Pennebaker, Chris Hepburn (Microstar Software Ltd., Canada), and Joan Mitchell (IBM Corp., USA).";

				hr = item->getPropValue(index++, &temp);	// Cm
				Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
				Assert::AreEqual(comment, temp);
			}

			hr = item->getNext(&next);
			Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
			item = next.detach();

			{	// DQT 
				Assert::AreEqual(372ULL, item->getOffset());

				uint32_t index = 2;

				hr = item->getPropValue(index++, &temp);
				Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
				Assert::AreEqual(L"DQT", temp);

				hr = item->getPropValue(index++, &temp);	// Lq
				Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
				Assert::AreEqual(L"262", temp);

				{
					hr = item->getPropValue(index++, &temp);	// Pq[0] = 0, 1byte
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"0", temp);

					hr = item->getPropValue(index++, &temp);	// Tq[0] = 0
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"0", temp);

					const int P1_A1_Q0[8][8] = {
						{8, 6, 5, 8, 12, 20, 26, 30},
						{6, 6, 7, 10, 13, 29, 30, 28},
						{7, 7, 8, 12, 20, 29, 35, 28},
						{7, 9, 11, 15, 26, 44, 40, 31},
						{9, 11, 19, 28, 34, 55, 52, 39},
						{12, 18, 28, 32, 41, 52, 57, 46},
						{25, 32, 39, 44, 52, 61, 60, 51},
						{36, 46, 48, 49, 56, 50, 52, 50}
					};
					get_expected_str_zigzagscan(expected_data, P1_A1_Q0);

					hr = item->getPropValue(index++, &temp);	// Qk
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);
				}

				{
					hr = item->getPropValue(index++, &temp);	// Pq[1] = 0, 1byte
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"0", temp);

					hr = item->getPropValue(index++, &temp);	// Tq[1] = 1
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"1", temp);

					const int P1_A1_Q1[8][8] = {
						{9, 9, 12, 24, 50, 50, 50, 50},
						{9, 11, 13, 33, 50, 50, 50, 50},
						{12, 13, 28, 50, 50, 50, 50, 50},
						{24, 33, 50, 50, 50, 50, 50, 50},
						{50, 50, 50, 50, 50, 50, 50, 50},
						{50, 50, 50, 50, 50, 50, 50, 50},
						{50, 50, 50, 50, 50, 50, 50, 50},
						{50, 50, 50, 50, 50, 50, 50, 50}
					};
					get_expected_str_zigzagscan(expected_data, P1_A1_Q1);

					hr = item->getPropValue(index++, &temp);	// Qk
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);
				}

				{
					hr = item->getPropValue(index++, &temp);	// Pq[2] = 0, 1byte
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"0", temp);

					hr = item->getPropValue(index++, &temp);	// Tq[2] = 2
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"2", temp);

					const int P1_A1_Q2[8][8] = {
						{16, 17, 18, 19, 20, 21, 22, 23},
						{17, 18, 19, 20, 21, 22, 23, 24},
						{18, 19, 20, 21, 22, 23, 24, 25},
						{19, 20, 21, 22, 23, 24, 25, 26},
						{20, 21, 22, 23, 24, 25, 26, 27},
						{21, 22, 23, 24, 25, 26, 27, 28},
						{22, 23, 24, 25, 26, 27, 28, 29},
						{23, 24, 25, 26, 27, 28, 29, 30}
					};
					get_expected_str_zigzagscan(expected_data, P1_A1_Q2);

					hr = item->getPropValue(index++, &temp);	// Qk
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);
				}

				{
					hr = item->getPropValue(index++, &temp);	// Pq[3] = 0, 1byte
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"0", temp);

					hr = item->getPropValue(index++, &temp);	// Tq[3] = 3
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"3", temp);

					const int P1_A1_Q3[8][8] = {
						{16, 16, 19, 22, 26, 27, 29, 34},
						{16, 16, 22, 24, 27, 29, 34, 37},
						{19, 22, 26, 27, 29, 34, 34, 38},
						{22, 22, 26, 27, 29, 34, 37, 40},
						{22, 26, 27, 29, 32, 35, 40, 48},
						{26, 27, 29, 32, 35, 40, 48, 58},
						{26, 27, 29, 34, 38, 46, 56, 69},
						{27, 29, 35, 38, 46, 56, 69, 83}
					};
					get_expected_str_zigzagscan(expected_data, P1_A1_Q3);

					hr = item->getPropValue(index++, &temp);	// Qk
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);
				}
			}

			hr = item->getNext(&next);
			Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
			item = next.detach();

			{	// DRI 
				Assert::AreEqual(636ULL, item->getOffset());

				uint32_t index = 2;

				hr = item->getPropValue(index++, &temp);
				Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
				Assert::AreEqual(L"DRI", temp);

				hr = item->getPropValue(index++, &temp);	// Lr
				Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
				Assert::AreEqual(L"4", temp);

				hr = item->getPropValue(index++, &temp);	// Ri
				Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
				Assert::AreEqual(L"5", temp);
			}

			hr = item->getNext(&next);
			Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
			item = next.detach();

			{	// SOF 
				Assert::AreEqual(642ULL, item->getOffset());

				uint32_t index = 2;

				hr = item->getPropValue(index++, &temp);
				Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
				Assert::AreEqual(L"SOF₀", temp);

				hr = item->getPropValue(index++, &temp);	// Lf
				Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
				Assert::AreEqual(L"20", temp);

				hr = item->getPropValue(index++, &temp);	// P
				Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
				Assert::AreEqual(L"8", temp);

				hr = item->getPropValue(index++, &temp);	// Y
				Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
				Assert::AreEqual(L"257", temp);

				hr = item->getPropValue(index++, &temp);	// X
				Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
				Assert::AreEqual(L"255", temp);

				hr = item->getPropValue(index++, &temp);	// Nf
				Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
				Assert::AreEqual(L"4", temp);

				{
					hr = item->getPropValue(index++, &temp);	// C
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"200", temp);

					hr = item->getPropValue(index++, &temp);	// H
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"1", temp);

					hr = item->getPropValue(index++, &temp);	// V
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"1", temp);

					hr = item->getPropValue(index++, &temp);	// Tq
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"0", temp);
				}

				{
					hr = item->getPropValue(index++, &temp);	// C
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"150", temp);

					hr = item->getPropValue(index++, &temp);	// H
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"1", temp);

					hr = item->getPropValue(index++, &temp);	// V
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"2", temp);

					hr = item->getPropValue(index++, &temp);	// Tq
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"1", temp);
				}

				{
					hr = item->getPropValue(index++, &temp);	// C
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"100", temp);

					hr = item->getPropValue(index++, &temp);	// H
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"3", temp);

					hr = item->getPropValue(index++, &temp);	// V
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"1", temp);

					hr = item->getPropValue(index++, &temp);	// Tq
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"2", temp);
				}

				{
					hr = item->getPropValue(index++, &temp);	// C
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"50", temp);

					hr = item->getPropValue(index++, &temp);	// H
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"1", temp);

					hr = item->getPropValue(index++, &temp);	// V
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"4", temp);

					hr = item->getPropValue(index++, &temp);	// Tq
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"3", temp);
				}
			}

			hr = item->getNext(&next);
			Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
			item = next.detach();

			{	// DHT 
				Assert::AreEqual(664ULL, item->getOffset());

				uint32_t index = 2;

				hr = item->getPropValue(index++, &temp);
				Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
				Assert::AreEqual(L"DHT", temp);

				hr = item->getPropValue(index++, &temp);	// Lh
				Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
				Assert::AreEqual(L"418", temp);

				{
					hr = item->getPropValue(index++, &temp);	// Tc = 0, DC
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"0", temp);

					hr = item->getPropValue(index++, &temp);	// Th = 0
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"0", temp);

					get_expected_str_lowerhexlist(expected_data, { 0x00,0x01,0x05,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00 });

					hr = item->getPropValue(index++, &temp);	// L
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0x00 });

					hr = item->getPropValue(index++, &temp);	// V L=2
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0x01,0x02,0x03,0x04,0x05 });

					hr = item->getPropValue(index++, &temp);	// V L=3
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0x06 });

					hr = item->getPropValue(index++, &temp);	// V L=4
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0x07 });

					hr = item->getPropValue(index++, &temp);	// V L=5
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0x08 });

					hr = item->getPropValue(index++, &temp);	// V L=6
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0x09 });

					hr = item->getPropValue(index++, &temp);	// V L=7
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0x0A });

					hr = item->getPropValue(index++, &temp);	// V L=8
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0x0B });

					hr = item->getPropValue(index++, &temp);	// V L=9
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);
				}

				{
					hr = item->getPropValue(index++, &temp);	// Tc = 1, AC
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"1", temp);

					hr = item->getPropValue(index++, &temp);	// Th = 0
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"0", temp);

					get_expected_str_lowerhexlist(expected_data, { 0x00,0x02,0x01,0x03,0x03,0x02,0x04,0x03,0x05,0x05,0x04,0x04,0x00,0x00,0x01,0x7d });

					hr = item->getPropValue(index++, &temp);	// L
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0x01,0x02 });

					hr = item->getPropValue(index++, &temp);	// V L=2
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0x03 });

					hr = item->getPropValue(index++, &temp);	// V L=3
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0x00, 0x04, 0x11 });

					hr = item->getPropValue(index++, &temp);	// V L=4
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0x05, 0x12, 0x21 });

					hr = item->getPropValue(index++, &temp);	// V L=5
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0x31, 0x41 });

					hr = item->getPropValue(index++, &temp);	// V L=6
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0x06, 0x13, 0x51, 0x61 });

					hr = item->getPropValue(index++, &temp);	// V L=7
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0x07, 0x22, 0x71 });

					hr = item->getPropValue(index++, &temp);	// V L=8
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0x14, 0x32, 0x81, 0x91, 0xa1 });

					hr = item->getPropValue(index++, &temp);	// V L=9
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0x08, 0x23, 0x42, 0xb1, 0xc1 });

					hr = item->getPropValue(index++, &temp);	// V L=10
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0x15, 0x52, 0xd1, 0xf0 });

					hr = item->getPropValue(index++, &temp);	// V L=11
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0x24, 0x33, 0x62, 0x72 });

					hr = item->getPropValue(index++, &temp);	// V L=12
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0x82 });

					hr = item->getPropValue(index++, &temp);	// V L=15
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0x09, 0x0a, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x34, 0x35, 0x36,0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x53, 0x54, 0x55, 0x56,0x57, 0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76,0x77, 0x78, 0x79, 0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95,0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3,0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca,0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,0xe8, 0xe9, 0xea, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa });

					hr = item->getPropValue(index++, &temp);	// V L=16
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);
				}

				{
					hr = item->getPropValue(index++, &temp);	// Tc = 0, DC
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"0", temp);

					hr = item->getPropValue(index++, &temp);	// Th = 1
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"1", temp);

					get_expected_str_lowerhexlist(expected_data, { 0x00,0x03,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00 });

					hr = item->getPropValue(index++, &temp);	// L
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0x00,0x01,0x02 });

					hr = item->getPropValue(index++, &temp);	// V L=2
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0x03 });

					hr = item->getPropValue(index++, &temp);	// V L=3
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0x04 });

					hr = item->getPropValue(index++, &temp);	// V L=4
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0x05 });

					hr = item->getPropValue(index++, &temp);	// V L=5
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0x06 });

					hr = item->getPropValue(index++, &temp);	// V L=6
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0x07 });

					hr = item->getPropValue(index++, &temp);	// V L=7
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0x08 });

					hr = item->getPropValue(index++, &temp);	// V L=8
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0x09 });

					hr = item->getPropValue(index++, &temp);	// V L=9
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0x0a });

					hr = item->getPropValue(index++, &temp);	// V L=10
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0x0b });

					hr = item->getPropValue(index++, &temp);	// V L=11
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);
				}

				{
					hr = item->getPropValue(index++, &temp);	// Tc = 1, AC
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"1", temp);

					hr = item->getPropValue(index++, &temp);	// Th = 1
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"1", temp);

					get_expected_str_lowerhexlist(expected_data, { 0x00,0x02,0x01,0x02,0x04,0x04,0x03,0x04,0x07,0x05,0x04,0x04,0x00,0x01,0x02,0x77 });

					hr = item->getPropValue(index++, &temp);	// L
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0x00,0x01 });

					hr = item->getPropValue(index++, &temp);	// V L=2
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0x02 });

					hr = item->getPropValue(index++, &temp);	// V L=3
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0x03,0x11 });

					hr = item->getPropValue(index++, &temp);	// V L=4
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0x04,0x05,0x21,0x31 });

					hr = item->getPropValue(index++, &temp);	// V L=5
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0x06,0x12,0x41,0x51 });

					hr = item->getPropValue(index++, &temp);	// V L=6
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0x07,0x61,0x71 });

					hr = item->getPropValue(index++, &temp);	// V L=7
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0x13,0x22,0x32,0x81 });

					hr = item->getPropValue(index++, &temp);	// V L=8
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0x08,0x14,0x42,0x91,0xa1,0xb1,0xc1 });

					hr = item->getPropValue(index++, &temp);	// V L=9
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0x09,0x23,0x33,0x52,0xf0 });

					hr = item->getPropValue(index++, &temp);	// V L=10
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0x15,0x62,0x72,0xd1 });

					hr = item->getPropValue(index++, &temp);	// V L=11
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0x0a,0x16,0x24,0x34 });

					hr = item->getPropValue(index++, &temp);	// V L=12
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0xe1 });

					hr = item->getPropValue(index++, &temp);	// V L=14
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0x25,0xf1 });

					hr = item->getPropValue(index++, &temp);	// V L=15
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);

					get_expected_str_lowerhexlist(expected_data, { 0x17,0x18,0x19,0x1a,0x26,0x27,0x28,0x29,0x2a,0x35,0x36,0x37,0x38,0x39,0x3a,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa });

					hr = item->getPropValue(index++, &temp);	// V L=16
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(expected_data.str().c_str(), temp);
				}
			}

			hr = item->getNext(&next);
			Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
			item = next.detach();

			{	// APP0 
				Assert::AreEqual(1084ULL, item->getOffset());

				uint32_t index = 2;

				hr = item->getPropValue(index++, &temp);
				Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
				Assert::AreEqual(L"APP₀", temp);

				hr = item->getPropValue(index++, &temp);	// Lp	
				Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
				Assert::AreEqual(L"46", temp);

				hr = item->getPropValue(index++, &temp);
				Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
				Assert::AreEqual(L"This field contains a simulated APP0 segment", temp);
			}

			hr = item->getNext(&next);
			Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
			item = next.detach();

			{	// SOS 
				Assert::AreEqual(1132ULL, item->getOffset());

				uint32_t index = 2;

				hr = item->getPropValue(index++, &temp);
				Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
				Assert::AreEqual(L"SOS", temp);

				hr = item->getPropValue(index++, &temp);	// Ls
				Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
				Assert::AreEqual(L"14", temp);

				hr = item->getPropValue(index++, &temp);	// Ns
				Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
				Assert::AreEqual(L"4", temp);

				{
					hr = item->getPropValue(index++, &temp);	// Cs
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"200", temp);

					hr = item->getPropValue(index++, &temp);	// Td
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"0", temp);

					hr = item->getPropValue(index++, &temp);	// Ta
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"0", temp);
				}

				{
					hr = item->getPropValue(index++, &temp);	// Cs
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"150", temp);

					hr = item->getPropValue(index++, &temp);	// Td
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"0", temp);

					hr = item->getPropValue(index++, &temp);	// Ta
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"0", temp);
				}

				{
					hr = item->getPropValue(index++, &temp);	// Cs
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"100", temp);

					hr = item->getPropValue(index++, &temp);	// Td
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"1", temp);

					hr = item->getPropValue(index++, &temp);	// Ta
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"1", temp);
				}

				{
					hr = item->getPropValue(index++, &temp);	// Cs
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"50", temp);

					hr = item->getPropValue(index++, &temp);	// Td
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"1", temp);

					hr = item->getPropValue(index++, &temp);	// Ta
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"1", temp);
				}

				{
					hr = item->getPropValue(index++, &temp);	// Ss
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"0", temp);

					hr = item->getPropValue(index++, &temp);	// Se
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"63", temp);

					hr = item->getPropValue(index++, &temp);	// Ah
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"0", temp);

					hr = item->getPropValue(index++, &temp);	// Al
					Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
					Assert::AreEqual(L"0", temp);
				}
			}

			hr = item->getNext(&next);
			Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
			item = next.detach();

			{	// EOI 

				//Assert::AreEqual(1148ULL, item->getOffset());

				uint32_t index = 2;

				hr = item->getPropValue(index++, &temp);
				Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
				Assert::AreEqual(L"EOI", temp);
			}
		}
	};
}
