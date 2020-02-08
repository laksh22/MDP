import sys
sys.path.append('/home/pi/ImageRec')
import time
import queue
import threading 
import json

from arduinoComm import *
from bluetoothComm import *
from pcComm import *
from queue import Queue
#from camera import *





#from Queue import queue
import cv2
import numpy as np

from picamera.array import PiRGBArray
from picamera import PiCamera

#python2.7 -m pip install <package-name>
class main(threading.Thread):
	def __init__(self):
		threading.Thread.__init__(self)
		self.debug = False
		

		#PiCamera initialization
		self.camera = PiCamera()
		self.camera.resolution = (1024, 768)
		self.camera.vflip = True
		self.camera.hflip = True
		self.camera.framerate = 30
		self.rawCapture = PiRGBArray(self.camera, size=(1024, 768))
		self.imgTaken = 0
		
		#initialize all the thread to each component class file
		self.pc_thread = pcComm()
		self.arduino_thread = arduinoComm()
		self.bluetooth_thread = bluetoothComm()

		#creating a thread
		#When you create a Thread, you pass it a function and a list containing the arguments to that function
		pcInitThread = threading.Thread(target = self.pc_thread.init_pc_comm, name = "PC_INIT_THREAD")
		arInitThread = threading.Thread(target = self.arduino_thread.init_arduino, name = "AR_INIT_THREAD")
		btInitThread = threading.Thread(target = self.bluetooth_thread.init_bluetooth_comm, name = "BT_INIT_THREAD")

		#A daemon thread will shut down immediately when the program exits
		pcInitThread.daemon = True
		arInitThread.daemon = True
		btInitThread.daemon = True
		
		#start the thread
		pcInitThread.start()
		arInitThread.start()
		btInitThread.start()

		time.sleep(1)
		#
		while not (self.pc_thread.pc_is_connected()  and self.bluetooth_thread.bluetooth_is_connected() and self.arduino_thread.arduino_is_connected()): 
			time.sleep(0.1)


	
	def writingToPC(self,messageToPC):
		self.pc_thread.writing_to_PC(messageToPC)
		print("Data transmitted to PC: %s \r\n" % messageToPC.rstrip())

	def readingFromPC(self):
		try:
			while True:

				readPCMessage = self.pc_thread.reading_from_PC()
				if (readPCMessage is None):
					continue
				readPCMessage = readPCMessage.lstrip()
				messageLineArr = readPCMessage.splitlines()

				for i in range(len(messageLineArr)):
					readPCMessage = messageLineArr[i].rstrip()
					#self.processMsg(readPCMessage)

					if(len(readPCMessage) == 0):
						continue
					
					print(readPCMessage)
					
					if(readPCMessage[0].upper() == 'B'):
						print("Data passed from PC to Bluetooth: %s" % readPCMessage[1:].rstrip())
						self.writingToBluetooth(readPCMessage[1:])
						time.sleep(0.1)
					
					elif(readPCMessage[0].upper() == 'A'):
						print("Data passed from PC to Arduino: %s" % readPCMessage[1:].rstrip())
						self.writingToArduino(readPCMessage[1:])
						time.sleep(0.1)

					elif(readPCMessage[0].upper() == 'I'):
						print("image need to take")
						img = self.takePicture()
						

					else:
						print(readPCMessage)
						print("Incorrect header from PC (expecting 'B' for Android, 'A' for Arduino or 'I' for Image Recog.): [%s]" % readPCMessage[0])
				
		except Exception as e:
			print("main/PC-Recv Error: %s" % str(e))


	def processMsg(self,readPCMessage):
		if(readPCMessage is None):
			return
		readPCMessage = readPCMessage.lstrip()
		if(len(readPCMessage) == 0):
			return
		
		if(readPCMessage[0].upper() == 'B'):
			print("Data passed from PC to Bluetooth: %s" % readPCMessage[1:].rstrip())
			self.writingToBluetooth(readPCMessage[1:])
		
		elif(readPCMessage[0].upper() == 'A'):
			print("Data passed from PC to Arduino: %s" % readPCMessage[1:].rstrip())
			self.writingToArduino(readPCMessage[1:])
		
		#msgSplit[0] is X corrdinate and msgSplit[1] is Y coordinate & msgSplit[2] is robot facing direction
		elif(readPCMessage[0].upper() == 'I'):
			img = self.takePicture()
			
		

	def takePicture(self):
		
		self.imgTaken += 1
		# grab an image from the camera
		self.camera.capture(self.rawCapture, format = "bgr", use_video_port = True)
		time.sleep(0.1)
		img = self.rawCapture.array

		path = '/home/pi/ImageRec/'
		imag = "image"+str(self.imgTaken)+".png"
		cv2.imwrite(path+imag,img)
		self.writingToPC(imag + "\r\n")
		print('Image sent')
		self.rawCapture.truncate(0)
			
	#writing to bluetooth
	def writingToBluetooth(self,messageToBT):
		self.bluetooth_thread.writing_to_bluetooth(messageToBT)
		print("Data transmitted to Bluetooth: %s" % messageToBT.rstrip())
	
	#reading from bluetooth
	def readingFromBluetooth(self):
		while True:
			retry = False
			try:
				while True:
					readBTMessage = self.bluetooth_thread.reading_from_bluetooth()
					if(readBTMessage is None):
						continue

					#returns a new string with leading whitespace removed, or removing whitespaces from the left side of the string
					readBTMessage = readBTMessage.lstrip()
					messageLineArr = readBTMessage.splitlines()

					if(len(readBTMessage) == 0):
						continue
					
					messageLineArr = readBTMessage.splitlines()

					for i in range(len(messageLineArr)):
						readBTMessage = messageLineArr[i].rstrip()

						if(len(readBTMessage) == 0):
							continue
						
						#reading the message from index 1 to the end
						if(readBTMessage[0].upper() == 'X'):
							print("Data Passed From Bluetooth To PC: %s" % readBTMessage[1:].rstrip())
							self.writingToPC(readBTMessage[1:] + "\r\n")						

						elif (readBTMessage[0].upper() == 'A'):
							print("Data Passed from Bluetooth to Arduino: %s " % readBTMessage[1:].rstrip())
							self.writingToArduino(readBTMessage[1:] + "\r\n")

						
						else: 
							print("Incorrect header from bluetooth ( 'A' from Arduino, 'X' from PC): [%s]" %readBTMessage[0])
			
			except Exception as e:
				print("MAIN/BT-RECV Error: %s" % str(e))
				retry = true
			if(not retry):
				break
		

	#write to Arduino
	def writingToArduino(self, messageToAr):
		self.arduino_thread.writing_to_arduino(messageToAr)
		print("Data transmiteed to Arduino: %s \r\n" % messageToAr.rstrip())

	def readingFromArduino(self):
		try:
			while True:
				readArMessage = self.arduino_thread.reading_from_arduino()	
				if(readArMessage is None):
					continue
				
				readArMessage = readArMessage.lstrip()
				if(len(readArMessage) == 0):
					continue
	
				if(readArMessage[0].upper() == 'X'):
					print("Data passed from Arduino to PC: %s" %readArMessage[1:].rstrip())
					self.writingToPC(readArMessage[1:] + "\r\n")
					time.sleep(0.1)
				elif(readArMessage[0].upper() == 'B'):
					print("Data passed from Arduino to Bluetooth: %s" % readArMessage[1:].rstrip())
					self.writingToBluetooth(readArMessage[1:])
		except socket.error as e:
			print("Arduino is Disconnected!")

	def initialize_threads(self):
		self.readPCThread = threading.Thread(target = self.readingFromPC, name = "pc_read_thread")
		self.readARThread = threading.Thread(target = self.readingFromArduino, name = "ar_read_thread")
		self.readBTThread = threading.Thread(target = self.readingFromBluetooth, name = "bt_read_thread")
		#self.imgRecogThread = threading.Thread(target = self.imgIDRecognition, name = "img_recog_thread")
	

		self.readPCThread.daemon = True
		self.readARThread.daemon = True
		self.readBTThread.daemon = True
		#self.imgRecogThread.daemon = True
		print("All threads (daemon) are initialised successfully")
	
		self.readPCThread.start()
		self.readARThread.start()
		self.readBTThread.start()
		#self.imgRecogThread.start()
		print("All thread (daemon) started successfully")

	def close_sockets(self):
		pc_thread.close_sockets()
		arduion_thread.close_sockets()
		bluetooth_thread.close_sockets()
		print("All threads being killed")
	
	def keep_alive(self):
		while(1):
			if not(self.readPCThread.is_alive()):
				print("PC thread is not running")
			if not(self.readARThread.is_alive()):
			  	print("Arduino thread is not running")
			if not (self.readBTThread.is_alive()):
				print("Bluetooth thread is not running")
			time.sleep(1)


if __name__ == "__main__":
	mainThread = main()
	mainThread.initialize_threads()
	mainThread.keep_alive()
	mainThread.close_sockets()

