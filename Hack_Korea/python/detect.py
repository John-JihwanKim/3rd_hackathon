import serial

ser = serial.Serial(
    port='/dev/cu.usbserial-14310',
    baudrate=115200,
)

while True:
    print("insert op :")
    op = input()
    ser.write(op.encode())