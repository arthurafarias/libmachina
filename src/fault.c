#include "fault.h"
#include <assert.h>
#if WIN32
	#include "windows.h"
#endif

/**
 * @brief Fault Handler
 * 
 * @param file
 * @param line 
 */
void fault_handler(const char* file, unsigned short line)
{
#if WIN32
	// If you hit this line, it means one of the ASSERT macros failed.
    DebugBreak();
#endif

	assert(0);
}