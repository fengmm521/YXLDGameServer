/*=============================================================================
#
#     FileName: sig_check.h
#         Desc: 统一的签名验证类
#               oauth库: http://liboauth.sourceforge.net/index.html
#
#       Author: dantezhu
#        Email: zny2008@gmail.com
#     HomePage: http://www.vimer.cn
#
#      Created: 2011-09-05 19:22:47
#      Version: 0.0.1
#      History:
#               0.0.1 | dantezhu | 2011-09-05 19:22:47 | initialization
#
=============================================================================*/
/*
【开放平台第三方应用】签名参数sig的说明
sig:
将请求源串以及密钥根据一定签名方法生成的签名值，用来提高传输过程参数的防篡改性。
签名值的生成共有3个步骤：构造源串，构造密钥，生成签名值。详见下面的描述：

一.说明
    1.将请求的URI路径进行URL编码（URI不含host，如/user/info）。
    2.将除"sig"外的所有参数按key进行字典升序排列。
    3.将第2步中排序后的参数(key=value)用&拼接起来，并进行URL编码。
    4.将HTTP请求方式（GET或者POST）以及第1步和第3步中的字符串用&拼接起来。
        HTTP请求方式 & urlencode(uri) & urlencode(a=x&b=y&...)
    5.创建密钥（最后的&一定要有，这是为了和hmac-sha1算法统一，见url:http://tools.ietf.org/html/rfc5849#section-3.4.2）
        shared-secret&
    6.将得到源串与密钥进行HMAC-SHA1加密，得到的buf经过Base64编码后赋值给sig（如果想直接用即通的接口验证的话，要先做base64的decode，把解码后的buf给即通）

二.生成源串
    原始请求信息
        HTTP请求方式：GET
        shared-secret：123456
        请求的URI路径（不含HOST）：/user/info

    请求参数：
        openid=11111111111111111&openkey=2222222222222222

    下面开始生成sig： 
    1.将请求的URI路径进行URL编码，得到：
        %2Fuser%2Finfo
    2.将除"sig"外的所有参数按key进行字典升序排列，得到：
        openid, openkey
    3.将第2步中排序后的参数(key=value)用&拼接起来，并进行URL编码
        openid=11111111111111111&openkey=2222222222222222
    4.将HTTP请求方式，第1步以及第3步中的到的字符串用&拼接起来,得到源串：
        GET&%2Fuser%2Finfo&openid%3D11111111111111111%26openkey%3D2222222222222222

三.构造密钥
    1.将shared-secret末尾加上一个字节的: &，得到:
        123456&

四.生成签名
    1.使用HMAC-SHA1加密算法，将第二步中的到的源串以及第三步获取的密钥进行加密。
    2.然后将加密后的字符串经过Base64编码，即得到"sig"签名参数的值。
*/

#ifndef _SIG_CHECK_H_20110905192345_
#define _SIG_CHECK_H_20110905192345_
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <iostream>
#include <memory>
#include <sstream>
#include <algorithm>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>

#ifdef __cplusplus
extern "C"{
#endif
#include "oauth.h"

#ifdef __cplusplus
};
#endif

using namespace std;

#ifndef SIGCHECK_ERROR
#define SIGCHECK_ERROR(fmt, args...) \
    snprintf(m_szErrMsg, sizeof(m_szErrMsg), "[%s][%d][%s]"fmt, \
             __FILE__, __LINE__,__FUNCTION__, ##args)
#endif


namespace NS_SIG_CHECK
{

    struct StSigInfo
    {
        string method; // GET or POST
        string url_path;
        map<string, string> params;

        string share_secret;

        string desc()
        {
            stringstream ss;
            //ss << endl;
            ss << "share_secret:" << share_secret; //<< endl;
            ss << " method:" << method;// << endl;
            ss << " url_path:" << url_path ;//<< endl;

            ss << " params:" ;//<< endl;

            for(map<string, string>::iterator it = params.begin(); it != params.end(); ++it)
            {
                ss << it->first << ":" << it->second <<" ";//<< endl;
            }

            return ss.str();
        }
    };

    class CSigCheck
    {
    public:
        CSigCheck() {}
        virtual ~CSigCheck() {}

        const char* GetErrMsg() { return m_szErrMsg; }

        string Create(StSigInfo& data)
        {
            string source;

            //转化为大写
            transform(data.method.begin(), data.method.end(), data.method.begin(), ::toupper);

            source.append(data.method);
            source.append("&");
            source.append(url_encode(data.url_path));
            source.append("&");
            source.append(url_encode(join_params(data.params)));

            char* p_sig = oauth_sign_hmac_sha1_raw(
                source.c_str(),
                source.size(), 
                data.share_secret.c_str(),
                data.share_secret.size());

            if (p_sig == NULL)
            {
                return "";
            }

            string sig = p_sig;;

            delete [] p_sig;
            p_sig = NULL;

            return sig;
        }

        int Check(StSigInfo& data, const string& param_sig)
        {
            if (param_sig.empty())
            {
                SIGCHECK_ERROR("param_sig is empty");
                return -1;
            }

            string real_sig = Create(data);

            if (real_sig != param_sig)
            {
                SIGCHECK_ERROR("sig not match,param_sig[%s], real_sig[%s], sig_info[%s]", 
                               param_sig.c_str(), 
                               real_sig.c_str(),
                               data.desc().c_str()
                               );
                return -2;
            }

            return 0;
        }

    
        string url_encode(const string& src)
        {
            char* p_dest = oauth_url_escape(src.c_str());
            if (p_dest == NULL)
            {
                return "";
            }
            string str_dest = p_dest;

            delete [] p_dest;
            p_dest = NULL;

            str_dest = replace_str(str_dest, "~", "%7E");

            return str_dest;
        }

		
	private:
        string join_params(map<string,string> &params)
        {
            string source;
            for(map<string, string>::iterator it = params.begin(); it != params.end(); ++it)
            {
                if (it != params.begin())
                {
                    source.append("&");
                }
                source.append(it->first);
                source.append("=");
                source.append(it->second);
            }

            return source;
        }

        string replace_str(string src,const string& old_value,const string& new_value)
        {
            for(string::size_type pos(0); pos != string::npos; pos += new_value.size()) 
            {
                if((pos=src.find(old_value, pos)) != string::npos)
                {
                    src.replace(pos, old_value.size(), new_value);
                }
                else 
                {
                    break;
                }
            }
        
            return src;
        }

    private:
        char m_szErrMsg[512];
    };

} /* NS_SIG_CHECK */
#endif
