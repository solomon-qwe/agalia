#pragma once

class CCSVString : public CString
{
public:
	CCSVString() { isFirstItem = true; }
	virtual ~CCSVString() {}

	void Add(const wchar_t* psz)
	{
		size_t cntComma = 0;
		size_t cntCRLF = 0;
		size_t cntDQuote = 0;

		size_t length = 0;
		const wchar_t* p = psz;
		while (*p)
		{
			switch (*p)
			{
			case L',':
				cntComma++;
				break;
			case L'\n':
			case L'\r':
				cntCRLF++;
				break;
			case L'"':
				cntDQuote++;
				break;
			}
			p++;
			length++;
		}

		const wchar_t* buf = psz;
		if (cntComma != 0 || cntCRLF != 0 || cntDQuote != 0)
		{
			wchar_t* temp = new wchar_t[length + cntDQuote + 2 + 1];
			buf = temp;
			*temp++ = L'\"';
			for (size_t i = 0; i < length; i++)
			{
				*temp++ = psz[i];
				if (psz[i] == L'\"')
				{
					*temp++ = psz[i];
				}
			}
			*temp++ = L'\"';
			*temp = L'\0';
		}

		if (!isFirstItem)
		{
			Append(L",");
		}
		else
		{
			isFirstItem = false;
		}
		Append(buf);

		if (buf != psz) {
			delete[]buf;
		}
	}

	void Next(void)
	{
		Append(_T("\n"));
		isFirstItem = true;
	}

protected:
	bool isFirstItem;
};

class CTSVString : public CString
{
public:
	CTSVString() { isFirstItem = true; }
	virtual ~CTSVString() {}

	void Add(const wchar_t* psz)
	{
		size_t cntTab = 0;
		size_t cntCRLF = 0;
		size_t cntDQuote = 0;

		size_t length = 0;
		const wchar_t* p = psz;
		while (*p)
		{
			switch (*p)
			{
			case L'\t':
				cntTab++;
				break;
			case L'\n':
			case L'\r':
				cntCRLF++;
				break;
			case L'"':
				cntDQuote++;
				break;
			}
			p++;
			length++;
		}

		const wchar_t* buf = psz;
		if (cntTab != 0 || cntCRLF != 0 || cntDQuote != 0)
		{
			wchar_t* temp = new wchar_t[length + cntDQuote + 2 + 1];
			buf = temp;
			*temp++ = L'\"';
			for (size_t i = 0; i < length; i++)
			{
				*temp++ = psz[i];
				if (psz[i] == L'\"')
				{
					*temp++ = psz[i];
				}
			}
			*temp++ = L'\"';
			*temp = L'\0';
		}

		if (!isFirstItem)
		{
			Append(L"\t");
		}
		else
		{
			isFirstItem = false;
		}
		Append(buf);

		if (buf != psz) {
			delete[]buf;
		}
	}

	void Next(void)
	{
		Append(_T("\n"));
		isFirstItem = true;
	}

protected:
	bool isFirstItem;
};