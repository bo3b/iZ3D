/**************************************************/
/***
 * Copyright (C) 2004
 * NVIDIA Corporation
 **/
/**************************************************/
#ifndef PDHHELPER_H__
#define PDHHELPER_H__
/**************************************************/
/***
 * \file
 **/
/**************************************************/
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <vector>

#pragma message("  Note: Including libs for PDH\n")
#pragma comment( lib, "pdh.lib" )

/**************************************************/
#define PDHERROR(x) {if (x!=ERROR_SUCCESS)std::cout<<"PDH Error: "<<__FILE__<<":"<<__LINE__<<std::endl;}

#define PDH_ERROR_STRING(pdhError, pcString) \
	{switch(pdhError) {\
		case PDH_CSTATUS_BAD_COUNTERNAME: pcString = _T("The counter name path string could not be parsed or interpreted."); break;\
		case PDH_CSTATUS_NO_COUNTER: pcString = _T("The specified counter was not found."); break;\
		case PDH_CSTATUS_NO_COUNTERNAME: pcString = _T("An empty counter name path string was passed in."); break;\
		case PDH_CSTATUS_NO_MACHINE: pcString = _T("A computer entry could not be created."); break;\
		case PDH_CSTATUS_NO_OBJECT: pcString = _T("The specified object could not be found."); break;\
		case PDH_FUNCTION_NOT_FOUND: pcString = _T("The calculation function for this counter could not be determined."); break;\
		case PDH_INVALID_ARGUMENT: pcString = _T("One or more arguments are invalid."); break;\
		case PDH_INVALID_HANDLE: pcString = _T("The query handle is not valid."); break;\
		case PDH_MEMORY_ALLOCATION_FAILURE: pcString = _T("A memory buffer could not be allocated."); break;\
		default: pcString = _T("Unknown PDH Error.");break;}}

#define WINPDHERROR(x, context) \
	{PDH_STATUS status; if ((status = x) != ERROR_SUCCESS) {\
		TCHAR zBuffer[200]; \
		TCHAR *pcErrorString;\
		PDH_ERROR_STRING(status, pcErrorString);\
		_stprintf(zBuffer, _T("%s : %s"), context, pcErrorString); \
		MessageBox(NULL, zBuffer, _T("PDH Error"), MB_OK | MB_ICONWARNING);}}
/**************************************************/
/// CPDHHelper: 
/**
 * \class CPDHHelper pdhhelper.h "pdhhelper.h"
 **
 *
 **/
/**************************************************/
class CPDHHelper
{
public:
    typedef std::vector<PDH_HCOUNTER> Counter;
    typedef std::vector<PDH_STATUS> Status;
    typedef std::vector<PDH_FMT_COUNTERVALUE> Value;
public:
    CPDHHelper():_query(NULL){}
    ~CPDHHelper(){PdhCloseQuery(_query);}
    size_t nCounters()const                                 {return _counters.size();}
    PDH_STATUS status(const unsigned int i)const            {return _stati[i];}
    PDH_FMT_COUNTERVALUE value(const unsigned int i)const   {return _values[i];}
    PDH_STATUS add(const LPCTSTR counterStr)
    {
        if (_query==NULL)
        {
            PDH_STATUS status = PdhOpenQuery(0,0,&_query);
            if (ERROR_SUCCESS != status) return status;
	    }
	    _counters.push_back(NULL);
	    _stati.push_back(PdhAddCounter(_query,counterStr,0,&_counters.back()));
	    PDH_FMT_COUNTERVALUE value;
	    memset(&value,0,sizeof(PDH_FMT_COUNTERVALUE));
	    _values.push_back(value);
	    return _stati.back();
    }
    PDH_STATUS sample()
    {
        PDH_STATUS status = PdhCollectQueryData(_query);
	    Counter::iterator cit = _counters.begin();
	    Status::iterator sit = _stati.begin();
	    Value::iterator vit = _values.begin();
	    while (cit!=_counters.end()){
	        *sit++ = PdhGetFormattedCounterValue(*cit++,PDH_FMT_DOUBLE|PDH_FMT_NOCAP100|PDH_FMT_NOSCALE,0,&(*vit++));
	    }
        return status;
    }
protected:
    PDH_HQUERY _query;
    Counter _counters;
    Status _stati;
    Value _values;
private:
private:
    //  CPDHHelper();
    CPDHHelper(const CPDHHelper &that);
    const CPDHHelper &operator=(const CPDHHelper &that);
};
/**************************************************/
/**************************************************/
#endif
/**************************************************/
