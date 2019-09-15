#include "pch.h"
#include "analyze_JPEG_util.h"

#include "jpeg_def.h"




bool analyze_JPEG::app_identify(const char* name, rsize_t name_size, JPEGSEGMENT_APPX* app, rsize_t bufsize)
{
	rsize_t data_size = bufsize - offsetof(JPEGSEGMENT_APPX, identifier);
	if (name_size < data_size)
		if (memcmp(app->identifier, name, name_size) == 0)
			return true;
	return false;
}
