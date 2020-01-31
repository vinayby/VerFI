/*
 * Util.h
 */

#ifndef UTIL_H
#define UTIL_H

#include <cstdio>
void StrReplaceChar(char *Str, char ch_source, char ch_destination);
void ReadNonCommentFromFile(FILE* FileHeader, char* Str, const char* CommentSyntax);

#endif /* !UTIL_H */
