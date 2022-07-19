#include "Common.h"

Logger* getLogger()
{
    static Logger g_log;
    return &g_log;
}
