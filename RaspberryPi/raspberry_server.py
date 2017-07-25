#!/usr/bin/env python3

import xbox
from RF24 import *
import time

joy = xbox.Joystick()
radio = RF24(22, 0)

localAddr=1
destAddr=2

radio.begin()
radio.enableDynamicPayloads()
radio.setRetries(5,15)
radio.printDetails()

radio.openReadingPipe(1,localAddr)
radio.openWritingPipe(destAddr)

while not joy.Back():   
    x=joy.leftX()
    y=joy.leftY()   
    command='S'
    
    if x<-0.8:
		command='L'
    elif x>0.8:
		command='R'
    elif y<-0.8:
		command='B'
    elif y>0.8:
		command='F'
		
    if joy.B():
		command='b'
		
    if joy.A():
		command='a'
		
    if joy.X():
		command='x'
		
    if joy.Y():
		command='y'
		
    if command!='S':
		print(command)
		radio.write(bytearray(command))
		time.sleep(0.3)
        
joy.close()

  

