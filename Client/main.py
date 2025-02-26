import socket
import ssl
import threading
import sys
import time
import json


def create_connection(ip_adres, port, client_id):
    context = ssl.create_default_context()
    context.check_hostname = False
    context.verify_mode = ssl.CERT_NONE

    write_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    wrapped_write_socket = context.wrap_socket(write_socket,server_hostname=ip_adres)


    wrapped_write_socket.connect((ip_adres, port))
    wrapped_write_socket.send(str.encode(client_id))

    message = wrapped_write_socket.recv(10);

    read_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)


    wrapped_read_socket = context.wrap_socket(read_socket,server_hostname=ip_adres)


    wrapped_read_socket.connect((ip_adres, port))
    wrapped_read_socket.send(str.encode(client_id))
    message = wrapped_read_socket.recv(10)
    return (wrapped_write_socket, wrapped_read_socket)


def send_message(socket, recepiepient, message, client_id):
    data = {}
    data['seen'] = 0
    data['to'] = recepiepient
    data['content'] = message
    data['type'] = 'message'
    data['from'] = client_id
    data['timestamp'] = time.time()

    parsed_data = json.dumps(data)

    size = len(parsed_data)
    if (size > 1024):
        print("za długa wiadomosc")
    else:
        padded_message = parsed_data + ' ' *(1024-size)  

        socket.send(str.encode(padded_message))

def full_receive_message(socket, size) -> str:
    
    message = ''
    while(len(message)<1024):
        bytes = socket.recv(1024 - len(message))
        message += bytes.decode("utf-8")
    
    message = message.strip()
    return message




