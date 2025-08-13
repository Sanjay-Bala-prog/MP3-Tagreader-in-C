#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "view.h"

static int syncsafe_int(const unsigned char *b)
{
    return ((b[0] & 0x7F) << 21) | ((b[1] & 0x7F) << 14) | ((b[2] & 0x7F) << 7) | (b[3] & 0x7F);
}

static int normal_int(const unsigned char *b) 
{
    return (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3];
}

Status read_validate_view(char *argv[], ViewTagInfo *view) 
{
    const char *filename = argv[2];
    return reader(filename, view);
}

Status do_view(ViewTagInfo *view) 
{
    display_tag_info(view);
    return m_success;
}

Status reader(const char *filename, ViewTagInfo *tag)
 {
    FILE *fp = fopen("vill.mp3", "rb");
    if (!fp)                                  //file opening
    {
        perror("Failed to open file");
        return m_failure;
    }

    unsigned char header[10];
    fread(header, 1, 10, fp);
    if (memcmp(header, "ID3", 3) != 0)    //checking the mp3 file or not
    {
        printf("No ID3 tag found.\n");
        fclose(fp);
        return m_failure;
    }

    int version = header[3];
    int tag_size = syncsafe_int(&header[6]);        //checking the version
    snprintf(tag->version, sizeof(tag->version), "ID3v2.%d", version);
    display_header(tag->version, tag_size);

    long pos = 10;
    while (pos < tag_size + 10)           //next 10 bytes is tag and tag size
    {
        unsigned char frame_header[10];
        fseek(fp, pos, SEEK_SET);
        if (fread(frame_header, 1, 10, fp) != 10 || frame_header[0] == 0)
            break;

        char frame_id[5] = {0};
        memcpy(frame_id, frame_header, 4);

        int frame_size = (version == 4)
            ? syncsafe_int(&frame_header[4])
            : normal_int(&frame_header[4]);

        if (frame_size <= 0 || frame_size > tag_size)
            break;

        char *data = malloc(frame_size + 1);
        if (!data) 
        {
            perror("Memory allocation failed");
            fclose(fp);
            return m_failure;
        }

        if (fread(data, 1, frame_size, fp) != frame_size) {
            free(data);
            break;
        }

        data[frame_size] = '\0';
        unsigned char encoding = data[0];
        char *text = data + 1;

        // Basic UTF-16 decoding fallback
        if (encoding == 0x01 || encoding == 0x02) {
            for (int i = 0, j = 0; i < frame_size - 1 && j < frame_size / 2; i += 2, j++) 
            {
                data[j] = text[i];
            }
            data[frame_size / 2] = '\0';
            text = data;
        }

        if (strncmp(frame_id, "COMM", 4) == 0)     //the comm tag function
        {
            int offset = 1 + 3;
            while (offset < frame_size && data[offset] != '\0') offset++;
            offset++;
            strncpy(tag->comment, data + offset, sizeof(tag->comment) - 1);
            tag->comment[sizeof(tag->comment) - 1] = '\0';
        } 
        else  //here all the tag function
        {
            if (strncmp(frame_id, "TIT2", 4) == 0)
                strncpy(tag->title, text, sizeof(tag->title) - 1);
            else if (strncmp(frame_id, "TPE1", 4) == 0)
                strncpy(tag->artist, text, sizeof(tag->artist) - 1);
            else if (strncmp(frame_id, "TALB", 4) == 0)
                strncpy(tag->album, text, sizeof(tag->album) - 1);
            else if (strncmp(frame_id, "TYER", 4) == 0 || strncmp(frame_id, "TDRC", 4) == 0)
                strncpy(tag->year, text, sizeof(tag->year) - 1);
        }

        free(data);
        pos += 10 + frame_size;
    }

    fclose(fp);
    return m_success;
}

void display_header(const char *version, int tag_size) {
    printf("=========== Tag Header ===========\n");
    printf("Version   : %s\n", version);
    printf("Tag Size  : %d bytes\n", tag_size);
}

void display_tag_info(ViewTagInfo *tag) {
    printf("\n--- TAG INFORMATION ---\n");
    printf("Title   : %s\n", tag->title);
    printf("Artist  : %s\n", tag->artist);
    printf("Album   : %s\n", tag->album);
    printf("Year    : %s\n", tag->year);
    printf("Comment : %s\n", tag->comment);
}
