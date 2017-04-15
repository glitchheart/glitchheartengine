#ifndef ERROR_H
#define ERROR_H

static void HandleError(char const *File, int32 LineNum, char const *msg)
{
    fprintf(stderr, "Error on in file %s on line %d\n", File, LineNum);
    fprintf(stderr, "%s\n", msg);
}

#endif