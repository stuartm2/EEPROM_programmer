import serial
from time import sleep

class Programmer():
    def __init__(self, port, speed=57600):
        self.port = port
        self.speed = speed
        self.ser = None

    def __enter__(self):
        self.connect()
        return self

    def __exit__(self, type, value, traceback):
        self.close()

    def write_data(self, addr, data):
        s = b'%04x:%02x\n' % (addr, data)
        self.ser.write(s)
        sleep(0.01)

    def connect(self):
        self.ser = serial.Serial(self.port, self.speed)
        print("Connected to port: " + self.port)

    def close(self):
        self.ser.close()

