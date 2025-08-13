#ifndef VIEW_H
#define VIEW_H

#include "main.h"

// Structure to hold extracted tag information
typedef struct 
{
    char title[100];       // Song title
    char artist[100];      // Artist name
    char album[100];       // Album name
    char year[10];         // Year (e.g., "2025")
    char comment[200];     // Comment or description
    char version[10];      // ID3 version string (e.g., "ID3v2.4")
} ViewTagInfo;

// Validate and prepare view operation
Status read_validate_view(char *argv[], ViewTagInfo *view);

// Display parsed tag information
Status do_view(ViewTagInfo *view);

// Read and parse ID3 tag from MP3 file
Status reader(const char *filename, ViewTagInfo *tag);

// Display tag content
void display_tag_info(ViewTagInfo *tag);

// Display tag header (version and size)
void display_header(const char *version, int tag_size);

#endif
