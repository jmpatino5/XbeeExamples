from Libs.xbee import Xbee

import signal 													# Import Signals
import sys 	

radio = Xbee(port = '/dev/ttyUSB0', baudrate = 9600)

radio.setAdress([0x00,0x13,0xA2,0x00,0x40,0x61,0x4B,0x07])
radio.sendFrame("Hello World!", debug = 1)

def signalHandler(signal, frame):
    print 'You pressed Ctrl+C!'
    sys.exit(0)

while 1:
	if radio.frameAvailable():
		print "RecAdress: {}".format(radio.lastRecAdress())
		print "Data: {}".format(radio.getPayLoad())

	signal.signal(signal.SIGINT, signalHandler)
