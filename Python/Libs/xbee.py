#Import external libs
from serial import Serial
from time import sleep, time

#Import libs from local
from util import merge, split

class Xbee(Serial):
	def __init__(self,port='/dev/ttyUSB0',baudrate= 115200):
		Serial.__init__(self,port,baudrate)
		self.__outBuffer=list()
		self.__destAdress=[0,0,0,0,0,0,0,0]
		self.__readyToSend = False
		self.__inpFrame = {'Payload':list()}
		self.__availableFrame = False
		self.flush()

	def toBuffer(self,msg=[], debug = False):
		if type(msg) == type([]) or type(msg) == type(""):
			for i in msg:
				self.__outBuffer.append(i)
		else:
			self.__outBuffer.append(msg)

		if debug:
			print "outBuffer: {}".format(self.__outBuffer)


	def fromBuffer(self,lenght = 1):
		output = list()
		for i in xrange(lenght):
			output.append(self.__inpFrame['Payload'].pop(0))
		return output

	def setAdress(self,adress):
		for i in range(8):
			self.__destAdress[i] = adress[i] 

	def inBuffer(self):
		return len(self.__inpBuffer)

	def __chkSum(self,data):
		chkSum = 0
		for i in data[3:]:
			chkSum += i
		chkSum = split(0xFF-chkSum)[1]
		return chkSum

	def __mkFrame(self,frameType=0x10,frameId=0x01,
					destAdress16=[0xFF,0xFF],broadCastRadio=0x00,
					option=0x00):
		destAdress = self.__destAdress
		header = [0x7E,0x00,0x00,frameType,frameId]	
		if frameType == 0x10:
			frame = header
			frame += destAdress + destAdress16
			frame.append(broadCastRadio)
			frame.append(option)
			for i in self.__outBuffer:
				if type(i) == int:
					frame.append(i)
				else: 
					frame.append(ord(i))
			frame[1],frame[2] = split(len(frame[3:]))
			frame.append(self.__chkSum(frame))
			return frame

	def __getResponse(self):
		inp_stream = list()
		timeout = time() + 0.1
		while self.inWaiting()==0:
			now = time()
			if now > timeout and self.inWaiting()==0:
				return False
		timeout = 0;
		while self.read()!='~':
			if timeout == 100:
				return False
			timeout+=1

		inp_stream.append(0x7e)
		MSB = ord(self.read())
		LSB = ord(self.read())

		lenght = merge(MSB,LSB)
		inp_stream.append(MSB)
		inp_stream.append(LSB)
		for i in xrange(lenght+1):
			inp_stream.append(ord(self.read()))

		#debug
		#print "Xbee.__getResponse() = {}".format(inp_stream)

		return inp_stream

	def __parceFrame(self,stream):
		inp = stream
		if not stream:
			return False
		inp.pop(0)
		if len(inp) > 0:
			MSB = inp.pop(0)
			LSB = inp.pop(0)
			lenght = merge(MSB,LSB)
		else:
			return False

		self.__inpFrame['FrameType'] = inp.pop(0)

		if self.__inpFrame['FrameType'] == 0x90:
			self.__inpFrame['InputAdress64'] = inp[:8]
			self.__inpFrame['InputAdress16'] = inp[8:10]
			self.__inpFrame['Status'] = inp[10]
			self.__inpFrame['Payload'] = inp[11:-1]
			self.__availableFrame = True

			#debug
			#print "Xbee.__parceFrame() = FrameType {}".format(hex(self.__inpFrame['FrameType']))

			return True
		
		elif self.__inpFrame['FrameType'] == 0x8B:
			self.__inpFrame['FrameId'] = inp.pop(0)
			self.__inpFrame['InputAdress16'] = inp[0:2]
			self.__inpFrame['TransmitRetry'] = inp.pop(2)
			self.__inpFrame['Status'] = inp.pop(2)
			self.__inpFrame['DiscoverStatus'] = inp.pop(2)
			if self.__inpFrame['Status'] == 0x00:

				#debug
				#print "Xbee.__parceFrame() = FrameType {}".format(hex(self.__inpFrame['FrameType']))

				return True
			else:
				print "Status : {}".format(self.__inpFrame['Status'])
				print "DiscoverStatus : {}".format(self.__inpFrame['DiscoverStatus'])
				return False
		else:
			return False

	def sendFrame(self,package = False , debug = False):
		if(package):
			self.__outBuffer = package
		frame = self.__mkFrame()
		if debug:
			print "Frame: {}".format(frame)
		for i in frame:
			self.write(chr(i))
		self.__outBuffer = []
		stream = self.__getResponse()
		out = self.__parceFrame(stream)

		#debug
		#print "Xbee.sendFrame() = {}".format(out)

		return out

	def frameAvailable(self):
		stream = self.__getResponse()
		if stream:
			try:
				return self.__parceFrame(stream)
			except:
				return False
		return False

	def getPayLoad(self,type_= 'list'):
		if self.__availableFrame == False or self.__inpFrame['FrameType'] != 0x90:
			return None
		if type_== 'string':
			output = ''
			for i in self.__inpFrame['Payload']:
				output+=chr(i)
				self.__availableFrame = False
				return output
		else:
			self.__availableFrame = False
			return self.__inpFrame['Payload']

	def lastRecAdress(self):
		if self.__availableFrame == False or self.__inpFrame['FrameType'] != 0x90:
			return None
		return self.__inpFrame['InputAdress64']
