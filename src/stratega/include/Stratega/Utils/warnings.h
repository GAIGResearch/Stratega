#pragma once

#if defined(_MSC_VER)
	#define DISABLE_WARNING_PUSH           __pragma(warning( push ))
	#define DISABLE_WARNING_POP            __pragma(warning( pop )) 
	#define DISABLE_WARNING(warningNumber) __pragma(warning( disable : warningNumber ))

	#define DISABLE_WARNING_UNREFERENCED_FORMAL_PARAMETER    DISABLE_WARNING(4100)
	#define DISABLE_WARNING_UNREFERENCED_FUNCTION            DISABLE_WARNING(4505)
	#define DISABLE_WARNING_UNSAFE_CONVERSION                DISABLE_WARNING(4191)
// other warnings you want to deactivate...

#elif defined(__clang__)
	#define DO_PRAGMA(X) _Pragma(#X)
	#define DISABLE_WARNING_PUSH           DO_PRAGMA(GCC diagnostic push)
	#define DISABLE_WARNING_POP            DO_PRAGMA(GCC diagnostic pop) 
	#define DISABLE_WARNING(warningName)   DO_PRAGMA(GCC diagnostic ignored #warningName)

	#define DISABLE_WARNING_UNREFERENCED_FORMAL_PARAMETER    DISABLE_WARNING(-Wunused-parameter)
	#define DISABLE_WARNING_UNREFERENCED_FUNCTION            DISABLE_WARNING(-Wunused-function)
	#define DISABLE_WARNING_UNREFERENCED					 DISABLE_WARNING(-Wunused)
	#define DISABLE_WARNING_SIGN_CONVERSION					 DISABLE_WARNING(-Wsign-conversion)
	#define DISABLE_WARNING_FORMAT							 DISABLE_WARNING(-Wformat-security)
	#define DISABLE_WARNING_OLD_CAST						 DISABLE_WARNING(-Wold-style-cast)
	#define DISABLE_WARNING_USELESS_CAST					 DISABLE_WARNING(-Wuseless-cast)
	#define DISABLE_WARNING_FORMAT_NONLITERAL				 DISABLE_WARNING(-Wformat-nonliteral)
	#define DISABLE_WARNING_NULL_DEREFERENCE				 DISABLE_WARNING(-Wnull-dereference)
	// other warnings you want to deactivate... 


#elif defined(__GNUC__)
	#define DO_PRAGMA(X) _Pragma(#X)
	#define DISABLE_WARNING_PUSH           DO_PRAGMA(GCC diagnostic push)
	#define DISABLE_WARNING_POP            DO_PRAGMA(GCC diagnostic pop) 
	#define DISABLE_WARNING(warningName)   DO_PRAGMA(GCC diagnostic ignored #warningName)

	#define DISABLE_WARNING_UNREFERENCED_FORMAL_PARAMETER    DISABLE_WARNING(-Wunused-parameter)
	#define DISABLE_WARNING_UNREFERENCED_FUNCTION            DISABLE_WARNING(-Wunused-function)
	#define DISABLE_WARNING_UNREFERENCED					 DISABLE_WARNING(-Wunused)
	#define DISABLE_WARNING_USELESS_CAST					 DISABLE_WARNING(-Wuseless-cast)
	#define DISABLE_WARNING_SIGN_CONVERSION					 DISABLE_WARNING(-Wsign-conversion)
	#define DISABLE_WARNING_FORMAT							 DISABLE_WARNING(-Wformat-security)
	#define DISABLE_WARNING_OLD_CAST						 DISABLE_WARNING(-Wold-style-cast)
	#define DISABLE_WARNING_USELESS_CAST					 DISABLE_WARNING(-Wuseless-cast)
	#define DISABLE_WARNING_FORMAT_NONLITERAL				 DISABLE_WARNING(-Wformat-nonliteral)
	#define DISABLE_WARNING_NULL_DEREFERENCE				 DISABLE_WARNING(-Wnull-dereference)
	// other warnings you want to deactivate... 

#else
	#define DISABLE_WARNING_PUSH
	#define DISABLE_WARNING_POP
	#define DISABLE_WARNING_UNREFERENCED_FORMAL_PARAMETER
	#define DISABLE_WARNING_UNREFERENCED_FUNCTION
	// other warnings you want to deactivate... 

#endif