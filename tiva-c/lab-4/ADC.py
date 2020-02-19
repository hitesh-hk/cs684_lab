import serial

tiva = serial.Serial()
tiva.baudrate=115200
tiva.port='COM3'
def delay(x):
    while x!=0:
        i=1
        while i<700:
            i=i+1
        x=x-1

tiva.open()
y=1
while y<=10:
    tiva.write(b'1')
    rcv = tiva.read(4)
    t=repr(rcv)
    t=int(t[2:6])
    print(t)
    delay(10)
    tiva.write(b'0')
    rcv = tiva.read(4)
    t=repr(rcv)
    t=int(t[2:6])
    print(t)
    delay(10)
    y=y+1
tiva.close()
