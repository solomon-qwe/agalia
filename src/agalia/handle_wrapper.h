#pragma once

#include <WinBase.h>	// for HANDLE 

class handle_wrapper
{
public:
	// �R���X�g���N�^ 
	handle_wrapper() {
		h = err = NULL;
	}
	handle_wrapper(HANDLE err) {
		this->h = this->err = err;
	}
	handle_wrapper(HANDLE h, HANDLE err) {
		this->h = h;
		this->err = err;
	}

	// �f�X�g���N�^ 
	virtual ~handle_wrapper() {
		close();
	}

	operator HANDLE() { return h; }
	HANDLE operator =(HANDLE hRight) { this->h = hRight; return hRight; }

	bool isErr(void) const { return h == err; }

	BOOL close(void)
	{
		BOOL ret = FALSE;
		if (h != err)
		{
			// �n���h������� 
			HANDLE temp = h;
			h = err;
			ret = ::CloseHandle(temp);
		}
		return ret;
	}

	HANDLE h;
	HANDLE err;
};
