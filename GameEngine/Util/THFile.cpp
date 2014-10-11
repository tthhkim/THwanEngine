#include "THFile.h"
#include <THPrivate.h>
#include <assert.h>
#include <string.h>

#if THPLATFORM==THPLATFORM_ANDROID
char* dataPath;
unsigned int dataPathLen;
#endif

FILE* newFile(const char* filename,const unsigned int nameLen,const char* mode)
{
#if THPLATFORM==THPLATFORM_ANDROID
	THLog("File Open : %s , %s",filename,mode?mode:"r+");
#ifndef NDEBUG
	const unsigned int cnl=strlen(filename);
	if(nameLen!=cnl)
	{
		THError("Param NameLen is different %s",filename);
		assert(0);
	}
	if(nameLen>MAX_FILE_NAME)
	{
		THError("filename %s Name is Too long to open",filename);
		assert(0);
	}
#endif
	memcpy(dataPath+dataPathLen,filename,nameLen);
	dataPath[dataPathLen+nameLen]='\0';

	FILE* file=fopen(dataPath,mode?mode:"r+");
	return file;
#endif
	return 0;
}