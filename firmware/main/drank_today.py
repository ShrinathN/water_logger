#!/bin/python3

import sqlite3
from flask import Flask
import time


app = Flask(__name__)

@app.route('/')
def hello_world():
    db = sqlite3.connect("water_log.sqlite3")
    data = db.execute("SELECT * FROM data;").fetchall()
    db.close()

    total_sip = 0
    today_time_struct = list(time.localtime(time.time()))

    for addr,recv_time,timestamp,sip in data:
        current_record_struct = list(time.localtime(timestamp))
        if(today_time_struct[0] == current_record_struct[0] and today_time_struct[1] == current_record_struct[1] and today_time_struct[2] == current_record_struct[2]):
            # print(f'Record {timestamp} today')
            total_sip += sip
        else:
            # print(f'Record {timestamp} not today')
            pass

    return 'Drank ' + str(total_sip) + 'ml today'

if __name__ == '__main__':
   app.run()