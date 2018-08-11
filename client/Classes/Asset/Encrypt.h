/********************************************************************
created:	2013-07-17
author:		Fish (于国平)
summary:	加密解密
*********************************************************************/
#pragma once

#include "FmIOUtil.h"

NS_FM_BEGIN

class Encrypt
{
public:
	// 读取加密文件
	static uint8* GetFileData( const char* fileName, const char* mode, uint* dataSize );

	// 保存加密数据
	static bool SaveFileData( const char* fileName, const uint8* data, uint dataSize );

	// 加密文件
	static bool EncryptFile( const char* fileName, const char* encryptFileName );



	//文件头添加字节0xcc，演示PNG文件
	static bool MaskFile(const char* fileName,const char* hidenFileName );

	// 保存被掩饰的文件
	static bool SaveMaskData( const char* fileName, const uint8* data, uint dataSize );

};

NS_FM_END