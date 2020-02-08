import serial 
import time

class arduinoComm(object):
	
	def __init__(self):
		#ls -1 /dev > dev.txt
		#ls -1 /dev > dev2.txt
		#diff dev.txt dev2.txt
		self.port = '/dev/ttyACM0'
		self.baurd_rate = 115200 
		self.arduino_connected = False

	def init_arduino(self):
		print ("Waiting for Arduino connection...")
		retry = False

		while True:
			try:
				self.ser = serial.Serial(self.port, self.baurd_rate)
				print('Connected to Arduino')
				self.arduino_connected = True
				

				# uncomment if testing one-to-one communication
				"""sendData = 'Aw'
				self.writing_to_arduino(sendData)
				self.reading_from_arduino()
				"""

				retry = False
			except Exception as e:
				print('Arudino-Conn Error: %s' % str(e))
				retry = True
			if(not retry):
				break

	def arduino_is_connected(self):
		return self.arduino_connected
	
	def disconnect_arduino(self):
		if self.ser:
			self.ser.close()
			self.arduino_connected = False

	def reading_from_arduino(self):
		try:
			self.ser.flush()
			get_data = self.ser.readline()
			return get_data.decode("ascii")

		except Exception as e:
			print('Arduino-Recv Error: %s' % str(e))
			if('Input/output error' in str(e)):
				self.disconnect_arduino()
				print('Trying to reconnect...')
				self.init_arduino()


	def writing_to_arduino(self,message):
		try:
			if(not self.arduino_connected):
				print('Arduino is not connected, unable to transmit')
				return
			message = (message.encode('ascii'))
			self.ser.write(message)
			self.ser.flush()

			
		except Exception as e:
			print ('Arduino-Send Error: ', str(e))


# uncomment if testing one-to-one communication
"""
arduinoObj = arduinoComm()
arduinoObj.init_arduino()
"""
