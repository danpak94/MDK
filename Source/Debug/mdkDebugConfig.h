#include <iostream>

namespace mdk
{

//============================= Switch ON or OFF DEBUG settings ===================================================

#define MDK_DEBUG_MODE

//==================================================================================================================

#define  MDK_DebugCode(x)    x

//#define  mdkDebugCode(x) 

#define  MDK_Warning    std::cout << "Warning: "

#define  MDK_Error      std::cout << '\a' << '\a' << '\a' << "ERROR: !~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!: " << '\n'

/*
#define  MDK_Warning    std::cout << "Warning: ......" << '\n' \
                                  << "File: "__FILE__ << '\n' \
                                  << "Line: " << __LINE__ << '\n' \
                                  << "Message: "

#define  MDK_Error      std::cout << '\a' << '\a' << '\a' \
                                  << "ERROR: !~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!~!: " << '\n' \
                                  << "File: "__FILE__ << '\n' \
                                  << "Line: " << __LINE__ << '\n' \
                                  << "Message: "
*/
//-------------------------------mdk3DImage---------------------------------------------------------//


}// namespace mdk
