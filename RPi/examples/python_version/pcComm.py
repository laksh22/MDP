import socket
import sys
import time
#if port is already in use
#sudo netstat -tunap | grep :5005
#sudo kill -9 1375


class pcComm(object):
	
	def __init__(self):
	#any port number that is larger than 1024 should be ok
		self.ip_address = '192.168.4.1'
		self.port = 5005
		self.pc_is_connect = False

	def close_pc_socket(self):
		if self.conn:
			self.conn.close()
			print ('Closing server scoket...')
	
		if self.client:
			self.client.close()
			print ('Closing client socket...')

		self.pc_is_connect = False

	def pc_is_connected(self):
		return self.pc_is_connect

#this makes the server listen to requests coming from RPI
#only restrict to one connection for listen
	def init_pc_comm(self):
		while True:
			retry = False
			try:
				self.conn = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
				self.conn.bind((self.ip_address, self.port))
				self.conn.listen(1)
		
				print("Listening for incoming PC connection on " + self.ip_address + ':' + str(self.port) + "..." )
				self.client, self.addr = self.conn.accept()
				
				print ("PC connected! IP address : " + str(self.addr))
				self.pc_is_connect = True

				# uncomment if testing one-to-one communication
				"""
				while True:
					data = self.reading_from_PC()
					sendData = "PI : " + data
					self.writing_to_PC(sendData)
					retry = False 
					"""
				retry = False
		
			except Exception as e:
				print( 'PC connection failed: %s' % str(e))
				retry = True
			if(not retry):
				break
			print ('Reconnecting PC connection...')
			time.sleep(1)

	def reading_from_PC(self):
		try:
			pc_data = ""

			while len(pc_data) == 0:
				pc_data = str(self.client.recv(1024).decode('ascii'))
			
			return pc_data

		except Exception as e:
			print ('PC-Comm or PC-RECV Error: %s ' % str(e))
			if('Broken pipe' in str(e) or 'Connection reset by peer' in str(e)):
				self.close_pc_socket()
				print('PC Broken Pipe; trying to reconnect...')
				self.init_pc_comm()

	def writing_to_PC(self,message):
		try:
			
			if(not self.pc_is_connect):
				print('PC is not connected!')
				return
			self.client.sendto((message.encode('ascii')),self.addr)
			
		except Exception as e:
			print ('PC-Comm or PC-Send Error: %s' % str(e))
			if('Broken pipe' in str(e) or 'Connection reset by peer' in str(e)):
				self.close_pc_socket()
				print('PC Broken Pipe; trying to reconnect...')
				self.init_pc_comm()

# uncomment if testing one-to-one communication
"""
pcObj = pcComm()
pcObj.init_pc_comm()
"""