# YXLDGameServer

用到的taf框架:

https://github.com/fengmm521/taf

因为taf框架使用了epoll和pthread，这个框架只能在linux下编译和部署，taf框架和游戏服务器都编译过了，但在运行后，客户端发送请求时出现了一些错误

客户端使用的是cocos2d-2.2版的lua实现.客户端只保留了lua程序和C++程序，资源文件没有上传.