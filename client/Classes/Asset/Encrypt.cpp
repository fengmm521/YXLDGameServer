#include "Encrypt.h"
#include "blowfish.h"

NS_FM_BEGIN

uint8* Encrypt::GetFileData( const char* fileName, const char* mode, uint* dataSize )
{
	if( dataSize == NULL )
		return NULL;
	uint8* data = IOUtil::GetFileData( fileName, mode, dataSize );
	if( data == NULL )
		return NULL;
	if( ((*dataSize)%8) != 0 || (*dataSize) <= 8 )
	{
		delete []data;
		data = NULL;
		return NULL;
	}
	CBlowFish blowfish;
	unsigned char key[] = { 0x6a,0x69,0x6f,0x6e,0x67,0x78,0x69,0x79,0x6f,0x75 };
	blowfish.Initialize( key, sizeof(key)/sizeof(key[0]) );
	uint decodeDataSize = *dataSize;
	unsigned char* decodeData = new unsigned char[decodeDataSize];
	blowfish.Decode( data, decodeData, decodeDataSize );
	FM_SAFE_DELETE_ARRAY( data );
	// 最后8个字节表示原始的数据长度(uint64)
	uint64 originalSize = *((uint64*)(decodeData + (decodeDataSize-8)));
	if( originalSize > *dataSize )
	{
		FM_SAFE_DELETE_ARRAY( decodeData );
		return NULL;
	}
	//assert( originalSize > 0 );
	*dataSize = (uint)originalSize;
	return decodeData;
}

bool Encrypt::SaveFileData( const char* fileName, const uint8* data, uint dataSize )
{
	if( fileName == NULL || data == NULL || dataSize == 0 )
		return false;
	// 追加一个8字节表示原文件的大小
	uint fixDataSize = dataSize+8;
	// 原文件大小不是8的倍数,则进行补齐
	uint yushu = dataSize%8;
	if( yushu > 0 )
	{
		fixDataSize += 8-yushu;
	}
	uint8* fixData = new uint8[(uint)fixDataSize];
	memset( fixData, NULL, fixDataSize );
	memcpy( fixData, data, dataSize );
	*((uint64*)(fixData + (fixDataSize-8))) = (uint64)dataSize;

	CBlowFish blowfish;
	unsigned char key[] = { 0x6a,0x69,0x6f,0x6e,0x67,0x78,0x69,0x79,0x6f,0x75 };
	blowfish.Initialize( key, sizeof(key)/sizeof(key[0]) );
	uint saveDataSize = blowfish.GetOutputLength( (DWORD)fixDataSize );
	unsigned char* saveData = new unsigned char[saveDataSize];
	memset( saveData, NULL, saveDataSize );
	blowfish.Encode( (unsigned char*)fixData, saveData, (DWORD)fixDataSize );
	FILE* pf = ::fopen( fileName, "wb+" );
	if( pf )
	{
		::fwrite( saveData, 1, saveDataSize, pf );
		::fclose( pf );
	}
	FM_SAFE_DELETE_ARRAY( saveData );
	FM_SAFE_DELETE_ARRAY( fixData );
	return true;
}

bool Encrypt::EncryptFile( const char* fileName, const char* encryptFileName )
{
	uint dataSize = 0;
	uint8* data = IOUtil::GetFileData( fileName, "rb", &dataSize );
	if( data == NULL )
		return false;
	bool result = SaveFileData( encryptFileName, data, dataSize );
	FM_SAFE_DELETE_ARRAY( data );
	return result;
}

bool Encrypt::MaskFile( const char* fileName,const char* maskFileName )
{
	uint dataSize = 0;
	uint8* data = IOUtil::GetFileData( fileName, "rb", &dataSize );
	if( data == NULL )
		return false;
	bool result = SaveMaskData( maskFileName, data, dataSize );
	FM_SAFE_DELETE_ARRAY( data );
	return result;
}

bool Encrypt::SaveMaskData( const char* fileName, const uint8* data, uint dataSize )
{
	if( fileName == NULL || data == NULL || dataSize == 0 )
		return false;
	// 文件头加2个字节
	uint fixDataSize = dataSize+2;
	uint8* fixData = new uint8[(uint)fixDataSize];
	memset( fixData, NULL, fixDataSize );
	memcpy( fixData, "XX", 2 );
	memcpy( fixData+2, data, dataSize );

	FILE* pf = ::fopen( fileName, "wb+" );
	if( pf )
	{
		::fwrite( fixData, 1, fixDataSize, pf );
		::fclose( pf );
	}
	FM_SAFE_DELETE_ARRAY( fixData );
	return true;
}

NS_FM_END