
/**
 * 
 * QQ第2代加密算法C++版
 * Created on 2005-10-26
 * @author admundliu. modify by billjiang
 * @version 1.0
 *
 **/
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include<netinet/in.h>
#include <time.h>

#define TEA_DEF_KEY "bcdfjjlmnprss123"
#define TEA_CRYPOR_CONTENT_MAX_LEN 10240
#define TEA_CRYPOR_BUFF_LEN 8
#define TEA_CRYPOR_KEY_LEN 16

class CTeaCryptor
{
    /**
     * 加密解密QQ消息的工具类. QQ消息的加密算法是一个16次的迭代过程，并且是反馈的，每一个加密单元是8字节，输出也是8字节，密钥是16字节
     * 我们以prePlain表示前一个明文块，plain表示当前明文块，crypt表示当前明文块加密得到的密文块，preCrypt表示前一个密文块
     * f表示加密算法，d表示解密算法 那么从plain得到crypt的过程是: m_iCrypt = f(m_szPlain &circ; m_iPreCrypt) &circ;
     * m_szPrePlain 所以，从crypt得到plain的过程自然是 m_szPlain = d(m_iCrypt &circ; m_szPrePlain) &circ;
     * m_iPreCrypt 此外，算法有它的填充机制，其会在明文前和明文后分别填充一定的字节数，以保证明文长度是8字节的倍数
     * 填充的字节数与原始明文长度有关，填充的方法是:
     * 
     * <pre>
     * <code>
     * 
     *      ------- 消息填充算法 ----------- 
     *      a = (明文长度 + 10) mod 8
     *      if(a 不等于 0) a = 8 - a;
     *      b = 随机数 &amp; 0xF8 | a;              这个的作用是把a的值保存了下来
     *      m_szPlain[0] = b;                     然后把b做为明文的第0个字节，这样第0个字节就保存了a的信息，这个信息在解密时就要用来找到真正明文的起始位置
     *      m_szPlain[1 至 a+2] = 随机数 &amp; 0xFF;    这里用随机数填充明文的第1到第a+2个字节
     *      m_szPlain[a+3 至 a+3+明文长度-1] = 明文; 从a+3字节开始才是真正的明文
     *      m_szPlain[a+3+明文长度, 最后] = 0;       在最后，填充0，填充到总长度为8的整数为止。到此为止，结束了，这就是最后得到的要加密的明文内容
     *      ------- 消息填充算法 ------------
     *   
     * </code>
     * </pre>
     * 
     * @author 马若劼
     * @author notXX
     */

private:
	
	char m_szPlain[8];	 	// 指向当前的明文块
	char m_szPrePlain[8];	// 这指向前面一个明文块
	int m_iCrypt;		// 当前加密的密文位置
	int m_iPreCrypt;	//上一次加密的密文块位置，他们相差8
	int m_iPos;     	 			// 当前处理的加密解密块的位置
	char m_szKey[TEA_CRYPOR_KEY_LEN+1];	// 密钥
	char m_szErrMsg[128];
	
	// 用于加密时，表示当前是否是第一个8字节块，因为加密算法是反馈的
	// 但是最开始的8个字节没有反馈可用，所有需要标明这种情况
	bool m_bIsHeader;
	
	// 这个表示当前解密开始的位置，之所以要这么一个变量是为了避免当解密到最后时
	// 后面已经没有数据，这时候就会出错，这个变量就是用来判断这种情况免得出错
	int m_iContextStart;

public:
	
	CTeaCryptor( char* key )
	{
		m_bIsHeader = true;
		
		memset( m_szPlain, 0, 8 );
		memset( m_szPrePlain, 0, 8 );
		memcpy( m_szKey, key, TEA_CRYPOR_KEY_LEN );
		memset( m_szErrMsg, 0, 128 );
	}
		
        /**
         * 解密
         * @param szIn 密文
         * @param iOffset 密文开始的位置
         * @param iLen 密文长度
         * @param m_szKey 密钥
         * @return 明文
         */
        /**
         * 把字节数组从offset开始的len个字节转换成一个unsigned int， 因为java里面没有unsigned，所以unsigned
         * int使用long表示的， 如果len大于8，则认为len等于8。如果len小于8，则高位填0 <br>
         * (edited by notxx) 改变了算法, 性能稍微好一点. 在我的机器上测试10000次, 原始算法花费18s, 这个算法花费12s.
         * 
         * @param szIn
         *                   字节数组.
         * @param iOffset
         *                   从哪里开始转换.
         * @param iLen
         *                   转换长度, 如果len超过8则忽略后面的
         * @return
         */
	unsigned int GetUnsignedInt( const char* szIn, int iOffset, int iLen )
	{
		unsigned int ret = 0;
		int end = 0;
		
		if ( iLen > 8 )
			end = iOffset + 8;
		else
			end = iOffset + iLen;
		
		for ( int i = iOffset; i < end; i++ )
		{
			ret <<= 8;
			ret |= szIn[i] & 0xff;
		}
		
		//ret =  ntohl(ret);

		return ret;
			
	}

	//=================================================================================
	/**
	 * @param szIn		需要加密的明文
	 * @param inLen 		明文长度
	 * @param m_szKey	密钥
	 * @return Message		密文
	 */
	bool Encrypt( char* szContent, int iContentLen, char* szEncryptedContent, int& iEncryptedLen )
	{
		if ( !szContent || !szEncryptedContent || iContentLen == 0 )
		{
			snprintf( m_szErrMsg, sizeof( m_szErrMsg ), "params invalid" );
			return false;
		}
	
		int iPadding = 0;
		
		iEncryptedLen = 0;
		m_iPreCrypt = 0;
		m_bIsHeader = true;

		// 计算头部填充字节数
		m_iPos = ( iContentLen + 0x0A ) % 8;
		if ( m_iPos != 0 )
		{
			m_iPos = 8 - m_iPos;
		}
		
		// 输出的密文长度不允许超过最大值
		if ( ( iContentLen + m_iPos + 10 ) > TEA_CRYPOR_CONTENT_MAX_LEN )
		{
			snprintf( m_szErrMsg, sizeof( m_szErrMsg ), "content len reach limit" );
			return false;
		}
		
		// 这里的操作把pos存到了plain的第一个字节里面
		// 0xF8后面三位是空的，正好留给pos，因为pos是0到7的值，表示文本开始的字节位置
		m_szPlain[0] = (char)( ( Rand() & 0xF8) | m_iPos );

		// 这里用随机产生的数填充plain[1]到plain[m_iPos]之间的内容
		for ( int i = 1; i <= m_iPos; i++ )
		{
			m_szPlain[i] = (char)( Rand() & 0xFF );
		}
		m_iPos ++;
		
		// 第一个8字节块当然没有prePlain，所以我们做一个全0的给第一个8字节块
		for ( int i = 0; i < 8; i++ )
			m_szPrePlain[i] = 0x0;

		// 继续填充2个字节的随机数，这个过程中如果满了8字节就加密之
		iPadding = 1;
		while ( iPadding <= 2 )
		{
			if ( m_iPos < 8 )
			{
				m_szPlain[m_iPos] = (char)( Rand() & 0xFF );
				m_iPos++;
				iPadding ++;
			}
			
			if ( m_iPos == 8 )
			{
				DoEncrypt8Bytes( szEncryptedContent, iEncryptedLen );
			}
		}

		// ===========================================
		// 头部填充完了，这里开始填真正的明文了
		// 也是满了8字节就加密，一直到明文读完
		int i = 0;
		while ( iContentLen > 0 )
		{
			if ( m_iPos < 8 )
			{
				m_szPlain[m_iPos] = szContent[i++];
				m_iPos++;
				iContentLen --;
			}

			if ( m_iPos == 8 )
			{
				DoEncrypt8Bytes( szEncryptedContent, iEncryptedLen );
			}
		}
		// ===========================================

		// 最后填上0，以保证是8字节的倍数
		iPadding = 1;
		while ( iPadding <= 7 )
		{
			if ( m_iPos < 8 )
			{
				m_szPlain[m_iPos++] = 0x0;
				iPadding++;
			}
			if ( m_iPos == 8 )
				DoEncrypt8Bytes( szEncryptedContent, iEncryptedLen );
		}

		return true;
		
	}


        /**
         * @param szIn		需要被解密的密文
         * @param inLen		密文长度
         * @return Message 	已解密的消息
         */	
        bool Decrypt( const char* szContent, int iContentLen, char* szDecryptedContent, int& iDecryptedLen )
	{
		if ( !szContent || !szDecryptedContent || iContentLen == 0 )
		{
			snprintf( m_szErrMsg, sizeof( m_szErrMsg ), "params invalid" );
			return false;
		}
	
		iDecryptedLen = 0;
		m_iCrypt = 0;
		m_iPreCrypt = 0;
		int iCount = 0;

		// 因为QQ消息加密之后至少是16字节，并且肯定是8的倍数，这里检查这种情况
		if ( ( iContentLen % 8 != 0 ) || ( iContentLen < 16 ) )
		{
			return false;
		}
			
		// 得到消息的头部，关键是得到真正明文开始的位置，这个信息存在第一个字节里面，所以其用解密得到的第一个字节与7做与
		Decipher( szContent, m_szPrePlain );
		
		m_iPos = m_szPrePlain[0] & 0x7;

		// 得到真正明文的长度
		iCount = iContentLen - m_iPos - 10;
		if ( iCount < 0 || iCount > TEA_CRYPOR_CONTENT_MAX_LEN )
		{
			return false;
		}

		
		// 这个是临时的preCrypt，和加密时第一个8字节块没有prePlain一样，解密时
		// 第一个8字节块也没有preCrypt，所有这里建一个全0的
		char szBuff[TEA_CRYPOR_BUFF_LEN] = { 0 };
		const char* pContent = szBuff;
		
		// 设置preCrypt的位置等于0，注意目前的preCrypt位置是指向m_szBuff的，因为java没有指针，所以我们在后面要控制当前密文buf的引用
		m_iPreCrypt = 0;
		
		// 当前的密文位置，为什么是8不是0呢？注意前面我们已经解密了头部信息了，现在当然该8了
		m_iCrypt = 8;
		m_iContextStart = 8;
		
		// 加1，和加密算法是对应的
		m_iPos ++;

			
		// 开始跳过头部，如果在这个过程中满了8字节，则解密下一块
		// 因为是解密下一块，所以我们有一个语句 m = in，下一块当然有preCrypt了，我们不再用m了
		// 但是如果不满8，这说明了什么？说明了头8个字节的密文是包含了明文信息的，当然还是要用m把明文弄出来
		// 所以，很显然，满了8的话，说明了头8个字节的密文除了一个长度信息有用之外，其他都是无用的填充
		int iPadding = 1;
		while( iPadding <= 2 )
		{
			if ( m_iPos < 8 )
			{
				m_iPos++;
				iPadding++;
			}
			if ( m_iPos == 8 )
			{
				pContent = szContent;
				if (!DoDecrypt8Bytes( szContent, iContentLen ) )
				{
					return false;
				}
			}
		}

			
		// 这里是解密的重要阶段，这个时候头部的填充都已经跳过了，开始解密
		// 注意如果上面一个while没有满8，这里第一个if里面用的就是原始的m，否则这个m就是in了
		while( iCount != 0 )
		{
			if ( m_iPos < 8 )
			{
				szDecryptedContent[iDecryptedLen] = (char)( pContent[m_iPreCrypt + m_iPos] ^ m_szPrePlain[m_iPos] );
				
				iDecryptedLen ++;
				iCount --;
				m_iPos ++;
			}

			if ( m_iPos == 8 )
			{
				pContent = szContent;
				m_iPreCrypt = m_iCrypt - 8;
				if ( !DoDecrypt8Bytes( szContent, iContentLen ) )
				{
					return false;
				}
			}
		}

		// 最后的解密部分，上面一个while已经把明文都解出来了，到了这里还剩下什么？对了，还剩下尾部的填充，应该全是0
		// 所以这里有检查是否解密了之后是0，如果不是的话那肯定出错了，所以返回null
		for ( iPadding = 1; iPadding < 8; iPadding++ )
		{
			if ( m_iPos < 8 )
			{
				if ( ( pContent[m_iPreCrypt + m_iPos] ^ m_szPrePlain[m_iPos] ) != 0 )
				{
					return false;
				}

				
				m_iPos++;
			}
			if ( m_iPos == 8 ) 
			{
				pContent = szContent;
				m_iPreCrypt = m_iCrypt;
				if ( !DoDecrypt8Bytes( szContent, iContentLen ) )
				{
					return false;
				}

			}
		}

		return true;
		
	}

		
private:
	
	//=================================================================================	
        /**
         * 加密8字节 
         */
	void DoEncrypt8Bytes( char* szEncryptedContent, int& iEncryptedLen )
	{
		// 这部分完成我上面所说的 m_szPlain ^ preCrypt，注意这里判断了是不是第一个8字节块，如果是的话，那个prePlain就当作preCrypt用
		for ( int i = 0; i < 8; i++ )
		{
			if ( m_bIsHeader )
				m_szPlain[i] ^= m_szPrePlain[i];
			else
				m_szPlain[i] ^= szEncryptedContent[m_iPreCrypt + i];
		}

		char szCrypted[8] = { 0 };
		
		// 这个完成到了我上面说的 f(m_szPlain ^ m_iPreCrypt)
		Encipher( m_szPlain, szCrypted );

		memcpy( szEncryptedContent + iEncryptedLen, szCrypted, 8 );

		// 这个就是完成到了 f(m_szPlain ^ m_iPreCrypt) ^ prePlain
		for ( int i = 0; i < 8; i++ )
		{
			szEncryptedContent[iEncryptedLen + i] ^= m_szPrePlain[i];
		}

		memcpy( m_szPrePlain, m_szPlain, 8 );
	

		// 完成了加密，现在是调整crypt，preCrypt等等东西的时候了
		m_iPreCrypt = iEncryptedLen;
		iEncryptedLen += 8;      
		m_iPos = 0;
		m_bIsHeader = false;
		
	}

        /**
         * 解密8个字节
         * @param szIn		密文字节数组
         * @param iOffset		从何处开始解密
         * @param iLen		密文的长度
         * @return true		表示解密成功
         */
	bool DoDecrypt8Bytes( const char* szIn , int iLen )
	{
		// 这里第一步就是判断后面还有没有数据，没有就返回，如果有，就执行 m_iCrypt ^ m_szPrePlain
		for ( int i = 0; i < 8; i++ )
		{
			if ( m_iContextStart + i >= iLen )
				return true;
			
			m_szPrePlain[i] ^= szIn[m_iCrypt + i];
		}

		// 好，这里执行到了 d(m_iCrypt ^ m_szPrePlain)
		Decipher( m_szPrePlain, m_szPrePlain );
		
		// 解密完成，wait，没完成哦，最后一步没做哦？ 
		// 这里最后一步放到decrypt里面去做了，因为解密的步骤毕竟还是不太一样嘛
		// 调整这些变量的值先
		m_iContextStart += 8;
		m_iCrypt += 8;
		m_iPos = 0;
		
		return true;
	
	}
        

	//=================================================================================
	/**
	* 加密一个8字节块
	* @param szIn	明文字节数组
	* @return		密文字节数组
	*/
	void Encipher( char* szIn, char* szEncrypted )
	{
		// 迭代次数，16次
		int iLoop = 0x10;
		
		// 得到明文和密钥的各个部分 
		// 这部值得考虑是否需要转换字节序
		// ==================================
		unsigned int y = GetUnsignedInt( szIn, 0, 4 );
		unsigned int z = GetUnsignedInt( szIn, 4, 4 );
		unsigned int a = GetUnsignedInt( m_szKey, 0, 4 );
		unsigned int b = GetUnsignedInt( m_szKey, 4, 4 );
		unsigned int c = GetUnsignedInt( m_szKey, 8, 4 );
		unsigned int d = GetUnsignedInt( m_szKey, 12, 4 );
		// ==================================
	
		
		// 这是算法的一些控制变量，为什么delta是0x9E3779B9呢？
		// 这个数是TEA算法的delta，实际是就是sqr(5)-1 * 2^31
		unsigned int sum = 0;
		unsigned int delta = 0x9E3779B9;
	
		// 开始迭代了，乱七八糟的，我也看不懂，反正和DES之类的差不多，都是这样倒来倒去
		while ( iLoop-- > 0 )
		{
			sum += delta;
					y += ((z << 4) + a) ^ (z + sum) ^ ((z >> 5) + b);
					z += ((y << 4) + c) ^ (y + sum) ^ ((y >> 5) + d);
		}
	
		szEncrypted[3] = (char) y;
		szEncrypted[2] = (char)( y >> 8 );
		szEncrypted[1] = (char)( y >> 16 );
		szEncrypted[0] = (char)( y >> 24 );
		szEncrypted[7] = (char) z;
		szEncrypted[6] = (char)( z >> 8 );
		szEncrypted[5] = (char)( z >> 16 );
		szEncrypted[4] = (char)( z >> 24 );
	
	}
	
	/**
	* 解密从offset开始的8字节密文
	* @param szIn		密文字节数组
	* @return			明文
	*/
	void Decipher( const char*szIn, char* szDecrypted )
	{
		// 迭代次数，16次
		int iLoop = 0x10;
	
		// 得到明文和密钥的各个部分 
		// 这部值得考虑是否需要转换字节序
		// ==================================
		unsigned int y = GetUnsignedInt( szIn, 0, 4 );
		unsigned int z = GetUnsignedInt( szIn, 4, 4 );
		unsigned int a = GetUnsignedInt( m_szKey, 0, 4 );
		unsigned int b = GetUnsignedInt( m_szKey, 4, 4 );
		unsigned int c = GetUnsignedInt( m_szKey, 8, 4 );
		unsigned int d = GetUnsignedInt( m_szKey, 12, 4 );
		// ==================================
	
		
		// 算法的一些控制变量，为什么sum在这里也有数了呢，这个sum嘛就是和迭代次数有关系了
		// 因为delta是这么多，所以sum如果是这么多的话，迭代的时候减减减，减16次，最后
		// 得到什么？ Yeah，得到0。反正这就是为了得到和加密时相反顺序的控制变量，这样才能解密呀
		unsigned int sum = 0xE3779B90;
		unsigned int delta = 0x9E3779B9;
	
		// 迭代开始了， #_#
		while ( iLoop-- > 0 )
		{
					z -= ((y << 4) + c) ^ (y + sum) ^ ((y >> 5) + d);
					y -= ((z << 4) + a) ^ (z + sum) ^ ((z >> 5) + b);
					sum -= delta;
		}
	
		szDecrypted[3] = (char) y;
		szDecrypted[2] = (char)( y >> 8 );
		szDecrypted[1] = (char)( y >> 16 );
		szDecrypted[0] = (char)( y >> 24 );
		szDecrypted[7] = (char) z;
		szDecrypted[6] = (char)( z >> 8 );
		szDecrypted[5] = (char)( z >> 16 );
		szDecrypted[4] = (char)( z >> 24 );
	
	}
	   

		
        /**
         * 这是个随机因子产生器，用来填充头部的，如果为了调试，可以用一个固定值
         * 随机因子可以使相同的明文每次加密出来的密文都不一样
         * 
         * @return	随机因子
         */
        int Rand()
        {
		//srand(time(NULL)); 
		//return rand();
		return 1000;
        }
    
    
    
};

