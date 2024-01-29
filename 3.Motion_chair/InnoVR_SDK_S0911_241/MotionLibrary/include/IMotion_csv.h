/********************************************************************************//**
\file      IMotion_csv.h
\brief     This header provides IMotion motion file loader function declarations.
\copyright Copyright (C) 2016-2017 InnoSimulation Co., Ltd. All rights reserved.
************************************************************************************/

#ifndef _IMOTION_CSV_H_
#define _IMOTION_CSV_H_

#include "IMotion.h"

#ifdef __cplusplus
extern "C"{
#endif	
	
#ifndef IM_DRIVER_STATIC_LIB
#	ifdef IM_DRIVER_EXPORTS
#		define IM_DRIVER_DLL_API __declspec(dllexport)
#	else
#		define IM_DRIVER_DLL_API __declspec(dllimport)
#	endif
#else
#  define IM_DRIVER_DLL_API
#endif
	
/**
 * This function loads motion data from the file name.
 * (Note, Encrypted file requires decryption key)
 */
IM_DRIVER_DLL_API int IMotion_LoadCSV(const char* filename, IM_FORMAT* format, uint8 ** motion_buf, uint32 * motion_len, const char* key);
	
/**
 * This function loads motion data from the file memory.
 */
IM_DRIVER_DLL_API int IMotion_LoadCSV_RAW(const void* data, int size, IM_FORMAT* format, uint8 ** motion_buf, uint32 * motion_len, const char* key);

/**
 * This function frees the loaded motion data memory.
 */
IM_DRIVER_DLL_API int IMotion_FreeCSV(uint8 * motion_buf);

/**
 * This function saves motion data as a file name.
 */
IM_DRIVER_DLL_API int IMotion_SaveCSV(const char* filename, const IM_FORMAT* format, const uint8 * motion_buf, uint32 motion_len, const char* key);

#ifdef __cplusplus
}
#endif

#endif // _IMOTION_CSV_H_