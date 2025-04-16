import os
import signal
import socket
import subprocess
import sys
import time


def one_client_set_get():
    server_host = "127.0.0.1"
    server_port = 5001
    server = subprocess.Popen([sys.argv[1], str(server_port)])
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    time.sleep(1)
    client_socket.connect((server_host, server_port))
    client_socket.send('get foo'.encode())
    value = client_socket.recv(1024).decode()
    assert value == '\n'
    client_socket.send('set foo bar'.encode())
    time.sleep(1)
    client_socket.send('get foo'.encode())
    value = client_socket.recv(1024).decode()
    assert value == 'bar\n'
    client_socket.close()
    server.kill()

def two_client_set_get():
    server_host = "127.0.0.1"
    server_port = 5002
    server = subprocess.Popen([sys.argv[1], str(server_port)])
    set_client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    time.sleep(1)
    set_client_socket.connect((server_host, server_port))
    set_client_socket.send('set foo bar'.encode())
    time.sleep(1)
    get_client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    get_client_socket.connect((server_host, server_port))
    get_client_socket.send('get foo'.encode())
    value = get_client_socket.recv(1024).decode()
    assert value == 'bar\n'
    set_client_socket.close()
    get_client_socket.close()
    server.kill()

one_client_set_get()
two_client_set_get()
