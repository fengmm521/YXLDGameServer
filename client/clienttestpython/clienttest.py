#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Date    : 2018-08-01 06:04:45
# @Author  : Your Name (you@example.org)
# @Link    : http://example.org
# @Version : $Id$
#创建SocketServerTCP服务器：


import os
import socket
import sys
import struct
import base64


import GameServer_pb2 as game
import GameServerExt_pb2 as gameExt
import RouterServer_pb2 as router


def packetForRouter(cmd,sbody):
    msg = router.CSMessage()
    msg.iCmd = cmd
    msg.strMsgBody = sbody.SerializeToString()
    data = msg.SerializeToString()
    fmat = 'H%ds'%(len(data))
    datalen = socket.htons(len(data)+2)
    print(type(datalen),datalen)
    print('packagedata:')
    print(sbody)
    bytesout = struct.pack(fmat,datalen,data)

    return bytesout

def package_cmd(cmd):
    msg = router.CSMessage()
    msg.iCmd = cmd
    data = msg.SerializeToString()
    fmat = 'H%ds'%(len(data))
    datalen = socket.htons(len(data)+2)
    print(type(datalen))
    bytesout = struct.pack(fmat,datalen,data)
    return bytesout

def unpacketFromServer(data):
    fmat = 'H%ds'%(len(data)-2)
    plen,pdata = struct.unpack(fmat,data)
    msg = game.SCMessage()
    msg.ParseFromString(pdata)
    cmd = msg.iCmd
    data = msg.strMsgBody
    # RouterServer_pb.SC_RS_Auth()
    print(msg)
    return cmd,data

NewRoleName = ''

def reciveDataFromServer(cmd,data):

    if cmd == router.RS_ROUTER_AUTH:            #服务器返回auth信息
        msg = router.SC_RS_Auth()
        msg.ParseFromString(data)
        print(msg)
    elif cmd == router.RS_ROUTER_WORLDLIST:     #获取服务器列表
        msg = router.SC_RS_WorldList()
        msg.ParseFromString(data)
        wlist = msg.worldList
        for w in wlist:
            print('iWorldID:',w.iWorldID)
            print('strWorldName:%s'%(w.strWorldName))
            print('iAtt:',w.iAtt)
            print('iOnlineStatus:',w.iOnlineStatus)
            print('iBusyStatus:',w.iBusyStatus)
            print('bHaveRole:',w.bHaveRole)
    elif cmd == game.CMD_ALOGIN:                #打通登陆协议
        msg = game.Cmd_Sc_Alogin()
        msg.ParseFromString(data)
        print(msg)
    elif cmd == game.CMD_NEW_ROLE:              #创建新角色
        msg = game.Cmd_Sc_NewRole()
        msg.ParseFromString(data)
        print(msg)
        global NewRoleName
        NewRoleName = msg.strRoleName
        
    elif cmd == game.CMD_GETROLE:               #得到服务器返回角色数据
        msg = game.Cmd_Sc_GetRole()
        msg.ParseFromString(data)
        print(msg)
        
    elif cmd == game.CMD_ERROR:                 #服务器出现错误
        msg = game.Cmd_Sc_Error()
        msg.ParseFromString(data)
        print('iType:',msg.iType)
        print('strErrorMsg:%s'%(msg.strErrorMsg))



def getSendAuth():
    authTab = router.CS_RS_Auth()
    md5password = base64.encodestring('KKWout6,')
    # authTab.strAccount = "KKWout6"
    # authTab.strMd5Passwd = "BgdLS1dvdXQwHCw="
    authTab.strAccount = "jjjjjjj"
    authTab.strMd5Passwd = ""

    outstr = packetForRouter(router.RS_ROUTER_AUTH, authTab)

    return outstr

def getWorldList():
    outstr = package_cmd(router.RS_ROUTER_WORLDLIST)
    return outstr


def getLoginProtobuf():
    aloginTab = game.Cmd_Cs_Alogin()
    aloginTab.iIsVersionCheck = 0
    aloginTab.iVersionType = 1
    aloginTab.iVersionMain = 1
    aloginTab.iVersionFeature = 1
    aloginTab.iVersionBuild = 6
    aloginTab.strChannelFlag = "2144"

    msg = game.CSMessage()
    msg.iCmd = game.CMD_ALOGIN
    msg.strMsgBody = aloginTab.SerializeToString()

    rsAloginTab = router.CS_RS_ALogin()
    rsAloginTab.iWorldID = 0
    rsAloginTab.strTransMsg = msg.SerializeToString()

    outstr = packetForRouter(router.RS_ROUTER_ALOGIN, rsAloginTab)
    return outstr

def getNewRoleProtobuf():
    newrole = game.Cmd_Cs_NewRole()
    newrole.strRoleName = 'pyuse3r8'
    outstr = packetForRouter(game.CMD_NEW_ROLE, newrole)
    return outstr

def getRole():
    outstr = package_cmd(game.CMD_GETROLE)
    return outstr

def client_program(setverip,sport):

    client_socket = socket.socket()  # instantiate
    client_socket.connect((setverip, sport))  # connect to the server

    if hasattr(__builtins__, 'raw_input'): 
        inputstr = raw_input
    else:
        inputstr = input
    message = inputstr('prompt:')
    print(message)

    while message.lower().strip() != 'bye':
        sendmsg = ''
        if message.lower().strip() == 'auth':
            sendmsg = getSendAuth()

        elif message.lower().strip() == 'worldlist':
            sendmsg = getWorldList()

        elif message.lower().strip() == 'login':
            sendmsg = getLoginProtobuf()


        elif message.lower().strip() == 'newrole':
            sendmsg = getNewRoleProtobuf()


        elif message.lower().strip() == 'getrole':
            sendmsg = getRole()

        else:
            message = inputstr(" -> ")  # again take input
            continue
        print('send_auth,len:%d'%(len(sendmsg)))
        client_socket.send(sendmsg)  # send message
        print('waite recv data')
        data = client_socket.recv(8*1024)  # receive response
        print('recv data len=',len(data))
        if len(data) > 1:
            cmd,servermsg = unpacketFromServer(data)
            print('Received from server:',len(data))  # show in terminal
            reciveDataFromServer(cmd, servermsg)
        else:
            print('recive data len too short')

        message = inputstr(" -> ")  # again take input

    client_socket.close()  # close the connection

serverip = '192.168.123.122'    #roter的IP地址
serverport = 22322              #roter的端口

#目前有三种请求可以使用
#auth     感觉没有什么用
#newrole  服务器会出错
#login    登陆这里感觉和sdk有关系，还是出错
#getrole  获取角色列表需要先调用login登陆

def main():
    client_program(serverip,serverport)


def test():
    prostr = getLoginProtobuf()
    newRole = game.Cmd_Cs_NewRole()
    newRole.ParseFromString(prostr)
    print(newRole.strRoleName)

#测试
if __name__ == '__main__':
    main()
    # args = sys.argv
    # fpth = ''
    # if len(args) == 2 :
    #     if os.path.exists(args[1]):
    #         fpth = args[1]
    #     else:
    #         print "请加上要转码的文件路径"
    # else:
    #     print "请加上要转码的文件路径"
    
