import socket
import ssl
import threading
import sys
import time
import json





def create_connection(ip_adres, port, client_id):
    write_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    write_socket.connect((ip_adres, port))
    write_socket.send(str.encode(client_id))

    message = write_socket.recv(10);

    read_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    read_socket.connect((ip_adres, port))
    read_socket.send(str.encode(client_id))
    message = read_socket.recv(10)
    return (write_socket,read_socket)


def send_message(socket, recepiepient, message):

    data = {}
    data['to']= recepiepient
    data['content'] = message
    data['type'] = 'message'
    

    
    parsed_data = json.dumps(data)

    socket.send(str.encode(parsed_data))



# ws1,rs1 = create_connection('192.168.1.31',1234,'1560221111')
#
# ws2,rs2 = create_connection('192.168.1.31',1234,'1560221112')
#
# ws3,rs3 = create_connection('192.168.1.31',1234,'1560221113')
#
#
#
#
# send_message(ws1,'1560221112',"abecd")
#
# message = rs2.recv(1000);
# print(message)
#
# send_message(ws2,'1560221113',"wiadomosc")
#
#
# message = rs3.recv(1000);
# print(message)



