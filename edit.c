#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "edit.h"

// Map command-line flag to field type
EditFieldType get_edit_field(char *arg)
{
    if (strcmp(arg, "-t") == 0) 
              return EDIT_TITLE;
    if (strcmp(arg, "-a") == 0) 
              return EDIT_ARTIST;
    if (strcmp(arg, "-A") == 0) 
              return EDIT_ALBUM;
    if (strcmp(arg, "-y") == 0) 
              return EDIT_YEAR;
    if (strcmp(arg, "-c") == 0) 
               return EDIT_COMMENT;
    return EDIT_UNKNOWN;
}

// Map field type to ID3 frame ID
const char* get_frame_id(EditFieldType field)
{
    switch (field)
    {
        case EDIT_TITLE: 
                     return "TIT2";
        case EDIT_ARTIST: 
                     return "TPE1";
        case EDIT_ALBUM: 
                     return "TALB";
        case EDIT_YEAR: 
                     return "TYER"; // fallback to TDRC for v2.4
        case EDIT_COMMENT: 
                     return "COMM";
        default: return NULL;
    }
}

int read_validate_edit(char *argv[], EditTagInfo *edit)
{
    edit->field = get_edit_field(argv[2]);           //validateing the command-line input
    if (edit->field == EDIT_UNKNOWN)
    {
        fprintf(stderr, "Unknown tag field: %s\n", argv[2]);
        return -1;
    }
    edit->new_value = argv[3];
    edit->filename = argv[4];
    return 0;
}


int do_edit(EditTagInfo *edit)         // Edit or create the tag frame
{
    FILE *fp = fopen(edit->filename, "r+b");       //file opening
    if (!fp)
    {
        perror("Failed to open file");
        return -1;
    }

    unsigned char header[10];       //checking version
    fread(header, 1, 10, fp);
    if (memcmp(header, "ID3", 3) != 0)
    {
        printf("No ID3 tag found.\n");
        fclose(fp);
        return -1;
    }

    int version_major = header[3];       //finding the size
    int tag_size = ((header[6] & 0x7F) << 21) |
                   ((header[7] & 0x7F) << 14) |
                   ((header[8] & 0x7F) << 7)  |
                   (header[9] & 0x7F);

    const char *target_frame = get_frame_id(edit->field);
    if (!target_frame)              //tag fram is empty return false
    {
        fclose(fp);
        return -1;
    }

    int pos = 10;
    while (pos < tag_size + 10)      //next 10 bytes is tag and tag size
    {
        unsigned char frame_header[10];
        fseek(fp, pos, SEEK_SET);
        fread(frame_header, 1, 10, fp);

        if (frame_header[0] == 0)
        {
            break; // Padding reached
        }

        char frame_id[5] = {0};
        memcpy(frame_id, frame_header, 4);

        int frame_size = (version_major == 4)
            ? ((frame_header[4] & 0x7F) << 21) |
              ((frame_header[5] & 0x7F) << 14) |
              ((frame_header[6] & 0x7F) << 7)  |
              (frame_header[7] & 0x7F)                    //finding the frame size decoding part
            : (frame_header[4] << 24) |
              (frame_header[5] << 16) |
              (frame_header[6] << 8)  |
              (frame_header[7]);

        if ((edit->field == EDIT_YEAR &&
             (strncmp(frame_id, "TYER", 4) == 0 || strncmp(frame_id, "TDRC", 4) == 0)) ||
            strncmp(frame_id, target_frame, 4) == 0)
        {
            int new_size = strlen(edit->new_value) + 1;
            fseek(fp, pos + 4, SEEK_SET);                  //the give data is encoding 

            if (version_major == 4)
            {
                fputc((new_size >> 21) & 0x7F, fp);
                fputc((new_size >> 14) & 0x7F, fp);
                fputc((new_size >> 7) & 0x7F, fp);
                fputc(new_size & 0x7F, fp);
            }
            else
            {
                fputc((new_size >> 24) & 0xFF, fp);
                fputc((new_size >> 16) & 0xFF, fp);
                fputc((new_size >> 8) & 0xFF, fp);
                fputc(new_size & 0xFF, fp);
            }

            fseek(fp, pos + 10, SEEK_SET);
            fputc(version_major == 4 ? 0x03 : 0x00, fp);
            fwrite(edit->new_value, strlen(edit->new_value), 1, fp);
            fclose(fp);
            printf("Tag '%s' updated successfully.\n", frame_id);
            return 0;
        }

        pos += 10 + frame_size;
    }

    
    int new_size = strlen(edit->new_value) + 1;
    fseek(fp, pos, SEEK_SET);
                                                          // Frame not found — create it at the end
    fwrite(target_frame, 4, 1, fp);
    if (version_major == 4)
    {
        fputc((new_size >> 21) & 0x7F, fp);
        fputc((new_size >> 14) & 0x7F, fp);
        fputc((new_size >> 7) & 0x7F, fp);
        fputc(new_size & 0x7F, fp);
    }
    else
    {
        fputc((new_size >> 24) & 0xFF, fp);
        fputc((new_size >> 16) & 0xFF, fp);
        fputc((new_size >> 8) & 0xFF, fp);
        fputc(new_size & 0xFF, fp);
    }

    fputc(0x00, fp); // Flags byte 1
    fputc(0x00, fp); // Flags byte 2

    fputc(version_major == 4 ? 0x03 : 0x00, fp);
    fwrite(edit->new_value, strlen(edit->new_value), 1, fp);

    fclose(fp);
    printf("Tag '%s' created and updated successfully.\n", target_frame);
    return 0;
}
