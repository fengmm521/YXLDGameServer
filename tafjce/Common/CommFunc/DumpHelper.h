/*********************************************************
*
*	名称: DumpHelper.h
*	作者: feiwu
*	时间: 2011-04-11
*	描述: Dump 日志和core文件帮助类
*********************************************************/

#ifndef __DUMP_HELPER_H__
#define __DUMP_HELPER_H__

class DumpHelper
{
public:

	// constructor/destructor
	DumpHelper(bool bEnableCore);
	~DumpHelper();

	// 功能: 进程crash处理
	static void OnCrash(int nSigno);

	static void DumpGdbLog();

	static void DumpBackTrace();

	static int GetThreadCount();
};

#endif
