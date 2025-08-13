#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "view.h"
#include "edit.h"

OperationType check_operation(char *argv[]) 
{
    if (strcmp(argv[1], "-v") == 0) 
            return m_view;
    if (strcmp(argv[1], "-e") == 0) 
            return m_edit;
    if (strcmp(argv[1], "--h") == 0) 
             return m_help;
    return m_help;
}

int main(int argc, char *argv[]) 
{
    if (argc < 2) 
    {
        printf("Insufficient arguments. Use --h for help.\n");
        return m_failure;
    }

    switch (check_operation(argv)) 
    {
        case m_help:
            printf("========================= Help =========================\n");
            printf("Usage:\n");
            printf("  -v           View MP3 file tags\n");
            printf("  -e [flag]    Edit MP3 file tags\n");
            printf("    -t         Title\n");
            printf("    -a         Artist\n");
            printf("    -A         Album\n");
            printf("    -y         Year\n");
            printf("    -c         Comment\n");
            printf("  --h          Display help\n");
            printf("========================================================\n");
            break;

        case m_view: 
        {
            ViewTagInfo view;
            printf("Selected View Option\n");
            if (read_validate_view(argv, &view) == m_success) 
            {
                do_view(&view);
            } else 
            {
                printf("View validation failed.\n");
                return m_failure;
            }
            break;
        }

        case m_edit: 
        {
            EditTagInfo edit;
            printf("Selected Edit Option\n");
            if (read_validate_edit(argv, &edit) == m_success)
            {
                do_edit(&edit);
            } 
            else 
            {
                printf("Edit validation failed.\n");
                return m_failure;
            }
            break;
        }

        default:
            printf("Invalid operation. Use --h for help.\n");
            break;
    }

    return 0;
}
