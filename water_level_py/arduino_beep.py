
import serial 
import pygame 

pygame.mixer.init()

beep_sound=pygame.mixer.Sound("beep.wav")

try:
    ser=serial.Serial('COM6',9600,timeout=1)

    while True:
     if ser.in_waiting > 0:
        data=ser.readline().decode('utf-8').strip()
        print(f"recived:{data}")

        if data == "BEEP":
              beep_sound.play()

except KeyboardInterrupt:
    
    ser.close()

