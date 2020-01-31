/*
 * Util.cpp
 * Copyright (C) 2020 Vinay B. Y. Kumar <vinayby@iitb.ac.in>
 *
 * Distributed under terms of the - license.
 */

#include "Util.h"

#include <cstdio>
#include <cstring>
#include <stdlib.h>

//***************************************************************************************

void StrReplaceChar(char *Str, char ch_source, char ch_destination)
{
	unsigned i;

	for (i = 0;i < strlen(Str);i++)
	{
		if (Str[i] == ch_source)
			Str[i] = ch_destination;
	}
}

//***************************************************************************************

void ReadNonCommentFromFile(FILE* FileHeader, char* Str, const char* CommentSyntax)
{
	int  l;
	char ch;

	l = strlen(CommentSyntax);
	do {
		fscanf(FileHeader, "%s", Str);
		if (!memcmp(CommentSyntax, Str, l))
		{
			do ch = fgetc(FileHeader);
			while ((ch != '\n') & (!feof(FileHeader)));
		}
	} while ((!memcmp(CommentSyntax, Str, l)) & (!feof(FileHeader)));
}


