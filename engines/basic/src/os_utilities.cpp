#include "../include/os_utilities.h"

#include "../include/path.h"

#include <SDL2/SDL.h>

dbasic::Path dbasic::GetModulePath() {
    // Query SDL for the path to our executable
    char * path = SDL_GetBasePath();
    if (path == nullptr) return ".";

    // Return it but don't forget we own 'path'
    Path parentPath(path);
    SDL_free(path);
    return parentPath;
}
