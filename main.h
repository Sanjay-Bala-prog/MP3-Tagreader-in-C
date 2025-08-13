#ifndef MAIN_H
#define MAIN_H

typedef unsigned int uint;  // Alias for unsigned int

// Status codes for function returns
typedef enum 
{
    m_success,
    m_failure
} Status;

// Operation types for command-line arguments
typedef enum 
{
    m_view,
    m_edit,
    m_help,
    m_undefind
} OperationType;

#endif
