#pragma once

HRESULT multibyte_to_widechar(char* srcBuf, int size, uint32_t codepage, agaliaString** dst);
HRESULT multibyte_to_widechar(char* srcBuf, int size, uint32_t codepage, std::wstringstream& dst);
