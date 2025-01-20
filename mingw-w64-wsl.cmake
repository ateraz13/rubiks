set(CMAKE_SYSTEM_NAME Windows)

set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++) 

set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Add MinGW include directories
include_directories(/usr/share/mingw-w64/include)
include_directories(/usr/x86_64-w64-mingw32/include)

set(CMAKE_C_FLAGS "-static-libgcc")
set(CMAKE_CXX_FLAGS "-static-libstdc++ -static-libgcc")
