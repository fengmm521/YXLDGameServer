#!/usr/bin/python
# -*- coding: utf-8 -*-
#创建SocketServerTCP服务器：

import socket
import sys

def client_program():
    host = socket.gethostname()  # as both code is running on same pc
    print('hostname',host)
    port = 9101  # socket server port number

    client_socket = socket.socket()  # instantiate
    client_socket.connect((host, port))  # connect to the server

    if hasattr(__builtins__, 'raw_input'): 
        inputstr = raw_input
    else:
        inputstr = input
    message = inputstr('prompt:')
    print(message)

    while message.lower().strip() != 'bye':
        client_socket.send(message.encode())  # send message
        data = client_socket.recv(1024).decode()  # receive response

        print('Received from server: ' + data)  # show in terminal

        message = inputstr(" -> ")  # again take input

    client_socket.close()  # close the connection


if __name__ == '__main__':
    client_program()