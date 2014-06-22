#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "token.h"

Token *scan_string(char **buf);
void scan_file(char *filename);

#ifdef __cplusplus
}
#endif
