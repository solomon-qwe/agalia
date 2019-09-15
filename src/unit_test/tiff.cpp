#include "pch.h"
#include "CppUnitTest.h"

#include <Windows.h>
#include "../inc/agaliarept.h"
#include "util.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace unittest
{
	TEST_CLASS(TIFF)
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

		TEST_METHOD(Classic_Parse)
		{
			std::wstring asset_dir;
			get_asset_dir(&asset_dir);

			parse_to_log((asset_dir + L"TestData\\BigTIFF\\Classic.tif").c_str());
		}



		void entry(const agaliaItem * item, const wchar_t* tag, const wchar_t* type, const wchar_t* count, const wchar_t* value)
		{
			HRESULT hr;
			std::wstring err_str;
			agaliaStringPtr temp;

			hr = item->getItemPropValue(1, &temp);	// 1 = tag
			Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
			Assert::AreEqual(tag, temp);

			hr = item->getItemPropValue(3, &temp);	// 3 = type 
			Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
			Assert::AreEqual(type, temp);

			hr = item->getItemPropValue(4, &temp);	// 4 = count 
			Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
			Assert::AreEqual(count, temp);

			hr = item->getItemPropValue(6, &temp);	// 6 = value 
			Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
			Assert::AreEqual(value, temp);
		}



		TEST_METHOD(Classic_Verify)
		{
			std::wstring asset_dir;
			get_asset_dir(&asset_dir);

			HRESULT hr;

			std::wstring err_str;
			std::wstringstream expected_data;
			agaliaStringPtr temp;

			agaliaPtr<agaliaItem> item;
			agaliaPtr<agaliaItem> next;

			agaliaPtr<agaliaContainer> image;
			hr = getAgaliaImage(&image, (asset_dir + L"TestData\\BigTIFF\\Classic.tif").c_str(), 0, 0);
			Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));

			hr = image->getRootItem(&item);
			Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));

			{
				uint32_t index = 0;

				hr = item->getItemPropValue(index++, &temp);
				Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
				Assert::AreEqual(L"II", temp);	// Intel Byte Order 

				hr = item->getItemPropValue(index++, &temp);
				Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
				Assert::AreEqual(L"002A", temp);	// Classic TIFF 

				hr = item->getItemPropValue(index++, &temp);
				Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
				Assert::AreEqual(L"12302", temp);	// IFD Offset 
			}

			hr = item->getNextItem(&next);
			Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
			item = next.detach();

			{
				uint32_t index = 1;

				hr = item->getItemPropValue(index++, &temp);
				Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
				Assert::AreEqual(L"8", temp);	// count 

				hr = item->getItemPropValue(index++, &temp);
				Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
				Assert::AreEqual(L"0", temp);	// next offset 
			}

			hr = item->getChildItem(0, &next);
			Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
			item = next.detach();

			entry(item, L"0100", L"SHORT", L"1", L"64");	// ImageWidth 

			hr = item->getNextItem(&next);
			Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
			item = next.detach();

			entry(item, L"0101", L"SHORT", L"1", L"64");	// ImageLength 

			hr = item->getNextItem(&next);
			Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
			item = next.detach();

			entry(item, L"0102", L"SHORT", L"3", L"8, 8, 8");	// BitsPerSample 

			hr = item->getNextItem(&next);
			Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
			item = next.detach();

			entry(item, L"0106", L"SHORT", L"1", L"2");	// PhotometricInterpretation, RGB(2) 

			hr = item->getNextItem(&next);
			Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
			item = next.detach();

			entry(item, L"0111", L"SHORT", L"1", L"8");	// StripOffsets 

			hr = item->getNextItem(&next);
			Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
			item = next.detach();

			entry(item, L"0115", L"SHORT", L"1", L"3");	// SamplesPerPixel 

			hr = item->getNextItem(&next);
			Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
			item = next.detach();

			entry(item, L"0116", L"SHORT", L"1", L"64");	// RowsPerStrip 

			hr = item->getNextItem(&next);
			Assert::AreEqual(S_OK, hr, GetErrorMessage(hr, err_str));
			item = next.detach();

			entry(item, L"0117", L"SHORT", L"1", L"12288");	// StripByteCounts 
		}
	};
}
