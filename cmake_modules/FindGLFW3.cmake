SET(GLFW3_INCLUDE_DIR "C:/Users/mjpea/.cpplibs/glfw-3.4.bin.WIN32/include")
SET(GLFW3_LIBRARY_DIR "C:/Users/mjpea/.cpplibs/glfw-3.4.bin.WIN32/lib-vc2022")

# Check that the paths are valid
FIND_PATH(GLFW3_INCLUDE_DIR "GLFW/glfw3.h")
FIND_LIBRARY(GLFW3_LIBRARY "glfw3.lib")


INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(GLFW3 DEFAULT_MSG GLFW3_LIBRARY GLFW3_INCLUDE_DIR)