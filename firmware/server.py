#!/bin/python3
import sqlite3
# import flask
import socket
import struct
import time
import sys
import os

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEPORT, 1)
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s.bind(("0.0.0.0", 8123))

#creating the database
db = sqlite3.connect("water_log.sqlite3")

try:
	db.execute("CREATE TABLE data(addr VARCHAR, recv_time INT, timestamp INT, sip INT);")
	db.commit()
except sqlite3.OperationalError:
	pass

while(True):
    try:
        s.listen(2)
        print('Listening...', end='')
        sys.stdout.flush()
        a, addr = s.accept()
        print(f'Accepted from {addr[0]}!')
        data = a.recv(1000)
        a.shutdown(socket.SHUT_RDWR)
        a.close()
        print(data)
        unpacked_data = struct.unpack(str(len(data) // 4) + "I", data)

        #now we show the data
        for i in range(0, len(data) // 4, 2):
            sip = unpacked_data[i]
            timestamp = unpacked_data[i + 1]
            print(f'Drank {str(sip)}ml of water at {time.ctime(timestamp)}')
            db.execute(f'INSERT INTO data VALUES("{addr}", {str(int(time.time()))}, {str(timestamp)}, {str(sip)});')
            db.commit()
        
    except KeyboardInterrupt:
        s.shutdown(socket.SHUT_RDWR)
        s.close()
        a.shutdown(socket.SHUT_RDWR)
        a.close()
        db.commit()
        db.close()
        exit()

        exit(0)