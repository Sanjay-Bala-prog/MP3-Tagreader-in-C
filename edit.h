#ifndef EDIT_H
#define EDIT_H

#include <stdio.h>

// Enum for supported tag fields
typedef enum
{
    EDIT_TITLE,
    EDIT_ARTIST,
    EDIT_ALBUM,
    EDIT_YEAR,
    EDIT_COMMENT,
    EDIT_UNKNOWN
} EditFieldType;

// Struct to hold edit operation details
typedef struct
{
    char *filename;        // MP3 file name
    EditFieldType field;   // Field to edit (title, artist, etc.)
    char *new_value;       // New value for the field
} EditTagInfo;

// Function declarations
int read_validate_edit(char *argv[], EditTagInfo *edit);
int do_edit(EditTagInfo *edit);
EditFieldType get_edit_field(char *arg);
const char* get_frame_id(EditFieldType field);

#endif
