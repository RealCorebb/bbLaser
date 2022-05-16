import collections
import math
import select
import socket
import struct
import threading
import time
import pyautogui

def gen_frame():
    screen_x = pyautogui.size()[0]
    screen_y = pyautogui.size()[1]
    mouse_x = pyautogui.position()[0]
    mouse_y = pyautogui.position()[1]
    laser_x = 4095 - int((mouse_x*4095)/screen_x)
    laser_y = 4095 - int((mouse_y*4095)/screen_y)
    print(laser_x, laser_y)

    frame = []

    for i in range(256):
        try:
            frame.append(struct.pack('<HHHHH',laser_x,laser_y,100,100,100)) # 100,100,100 = RGB intensity, max value 4095 on each color.
        except:
            pass

    return frame
    
while True:
    print(gen_frame())
    break