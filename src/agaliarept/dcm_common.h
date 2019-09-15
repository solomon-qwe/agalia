#pragma once

#include <iostream>
#include <unordered_map>

struct dcm_dic_elem
{
	std::string vr;
	std::wstring name;
};

struct dcm_dic_uid
{
	std::wstring name;
};

extern std::unordered_map<std::wstring, dcm_dic_elem>* g_dcm_dic_elem;
typedef std::unordered_map<std::wstring, dcm_dic_elem>::iterator dcm_dic_elem_iterator;
bool findVR(dcm_dic_elem_iterator& it, uint16_t group, uint16_t element);

extern std::unordered_map<std::string, dcm_dic_uid>* g_dcm_dic_uid;
typedef std::unordered_map<std::string, dcm_dic_uid>::iterator dcm_dic_uid_iterator;
bool findUID(dcm_dic_uid_iterator& it, const char* uid);

void init_dcm_dictionary(_In_opt_ const wchar_t* lang);
void clear_dcm_dictionary(void);
