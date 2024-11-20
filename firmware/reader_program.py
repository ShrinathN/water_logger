#!/bin/python3

import tkinter as tk
import threading
import socket

def mainthread():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEPORT, 1)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(('0.0.0.0', 8123))
    while(True):
        try:        
            s.listen(2)
            a, addr = s.accept()
            data = a.recv(1000)
            weight['text'] = str(data, 'utf-8')
        except KeyboardInterrupt:
            s.shutdown(socket.SHUT_RDWR)
            a.shutdown(socket.SHUT_RDWR)
            s.close()
            a.close()
            root.destroy()
            exit(0)

root = tk.Tk()
weight = tk.Label(root, fg="red", font=('',100), text="Hello world")
weight.pack()


threading.Thread(target=mainthread).start()
root.mainloop()