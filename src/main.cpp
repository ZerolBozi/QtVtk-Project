#include "CanvasHandler.h"


int main(int argc, char **argv)
{
#ifdef __linux
	putenv((char *)"MESA_GL_VERSION_OVERRIDE=3.2");

	// Fixes decimal point issue in vtkSTLReader
	putenv((char *)"LC_NUMERIC=C");
#endif //LINUX

	CanvasHandler(argc, argv);

	return 0;
}
