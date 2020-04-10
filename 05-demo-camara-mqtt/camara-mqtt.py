#!/usr/bin/python3
import time
from picamera import PiCamera
from time import sleep
from ftplib import FTP
from datetime import datetime
import paho.mqtt.publish as publish

MQTT_SERVER = "m20.cloudmqtt.com"  #Write Server IP Address
MQTT_PATH = "Image"

ftp = FTP('aprendiendoarduino.com','user','pass')

camera = PiCamera()

camera.start_preview()
for i in range(360):	# 1 hora
    camera.capture('imagen.jpg')
    file = open('imagen.jpg','rb')	# file to send
    ftp.storbinary('STOR /html/servicios/imagen.jpg', file)	# send the file
    file.close()	# close file and FTP
    
    f=open("imagen.jpg", "rb") #in same folder
    fileContent = f.read()
    byteArr = bytearray(fileContent)
    key = dict(username="......", password="......")
    publish.single(MQTT_PATH, byteArr, hostname=MQTT_SERVER,port=10488,auth=key)
    f.close()

    print(datetime.now())
    time.sleep(10)	# 10 seconds
camera.stop_preview()
ftp.quit()