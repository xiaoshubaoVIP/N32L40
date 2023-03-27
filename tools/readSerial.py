import threading,time
import serial

def serial_callback(s:serial.Serial):
    save = ""
    num = 0
    cache=False
    while(True):
        try:
            r = ser.readline().decode()
        except:
            continue

        if "co calibration setp 1 start" in r:
            print("get")
            cache = True

        if "flash ok" in r:
            print("fin:" + str(num) + ".txt")
            with open(str(num) + ".txt", "w") as f:
                f.write(save)
            save=""
            cache=False
            num+=1

        if cache:
            save += r
    # return

ser = serial.Serial("COM30", 115200)

t = threading.Thread(target = serial_callback, args = (ser,))
t.start()

while(True):
    time.sleep(2)
    ser.write("flash 0x8011800 10000\n".encode())