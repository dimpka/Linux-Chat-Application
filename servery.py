import socket
import sys
import struct
import binascii
import time

# Here argument 1 is the Server name(ie. tux057.eng.auburn.edu)
#Arg1 = hostname of udp server
# And argument 2 is the port number (ie. 10031)
#Arg2 = port number
# Argument 3 is the request ID (a number from 0-127)
# Argument 4 to argument n is a hostname (ie. www.google.com)

if len(sys.argv) < 2:
	print "you must input argument hostname, followed by the port#, followed by a requestID number (0-127), then one more more strings of websites like www.google.com !"
	sys.exit()
	
myGID = 15
BUFFER_SIZE = 256
# UDP_IP = socket.gethostbyname(sys.argv[1])
UDP_IP = socket.gethostbyname(socket.gethostname())
# UDP_PORT = int(sys.argv[2]) + 10010 # Our GID + 10010
UDP_PORT = int(sys.argv[1]) # Our GID * 5 + 10010 = 10115
# requestID = int(sys.argv[3])
# hostNameArg = (sys.argv[4:]) # joins the string arguments with spaces???

startTime = time.time() # The start time

sock = socket.socket(socket.AF_INET, # Internet 
	socket.SOCK_DGRAM) # UDP
	
sock.bind((UDP_IP, UDP_PORT))

# Variables for the waiting client info
waitingIP = ""
waitingPort = 0

errorCode = 0 # Will be manipulated to be send back to client.
isWaiting = False;


while True:
	data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
	print "received message:", data
	# Joy!, Ph, Pl, GID(Client)
	#unpackedData = struct.unpack_from("!4sBBB", data)
	unpackedData = struct.unpack_from("!4sHB", data)
	print "len(data)" + str(len(data))
	print struct.unpack_from("!4sBBB", data) 
	print struct.unpack_from("!4sHB", data) 
	print addr
	print "Below is the port from addr in reversed Network Order."
	print struct.unpack_from("!H", str(addr[1]))
	print type(addr)
	print type(addr[0])
	print type(addr[1])
	# Do some error checking. Send back 0111 or 0100 or 0101 or 0011 or whatevers
	errorCode = 0
	if (unpackedData[0] != "Joy!"):
		# Magic Number is wrong
		errorCode = 1
	
	if (len(data) != 7):
		# Length is wrong
		errorCode = errorCode + 2
	
	if ((unpackedData[1] > (10010 + (unpackedData[2] * 5) + 4)) or (unpackedData[1] < (10010 + (unpackedData[2] * 5)))):
		# Port is out of range for client
		errorCode = errorCode + 4
	
	print "errorCode " + str(errorCode)
	if (errorCode > 0):
		print "There was an error with the request."
		# Error Packet Format: MagicNumber GID 00 XY(ErrorCode)
		# WTH is the GID to use here? The server's or the client's?
		errorPacket = struct.pack("!4sBBB", "Joy!", unpackedData[2], 0, errorCode)
		sock.sendto(errorPacket, addr)
	elif(not isWaiting): # The request is valid. If there's no one waiting.
		print "Returning info for the client to wait."
		waitInstrPacket = struct.pack("!4sBH", "Joy!", myGID, unpackedData[1])
		sock.sendto(waitInstrPacket, addr)
		waitingIP = addr[0]
		waitingPort = unpackedData[1]
		isWaiting = True
	else: # Below someone is waiting.
		# socket.inet_aton(ip_string)
		print "Someone is waiting, will send you the info now!"
		chatInfoPacket = struct.pack("!4s", "Joy!")
		chatInfoPacket = chatInfoPacket + socket.inet_aton(waitingIP) 
		chatInfoPacket = chatInfoPacket + struct.pack("!HB", waitingPort, myGID)
		sock.sendto(chatInfoPacket, addr)
		waitingIP = "" # Clear the infos.
		waitingPort = 0
		isWaiting = False
		break
		
	
