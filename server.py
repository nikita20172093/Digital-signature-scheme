import hashlib
import socket
import struct
import math
import sys

#Fast Exponentiaion method
def fastExponentiation(a,q,n):
	result=1
	while q>0:
		if q>0:
			result=((result%n)*(a%n))%n
		a=((a%n)*(a%n))%n
		q>>=1
	return result

#Extended Euclidian Method
def extendedGcd(a, b):
	x0, x1, y0, y1 = 1, 0, 0, 1
	while(b != 0):
		q, a, b = a // b, b, a % b
		x0, x1 = x1, x0 - q * x1
		y0, y1 = y1, y0 - q * y1    
	return x0

#this function is used to verify the signature
def SignatureVerify(basic,advanced):
	#in order to get rPrime
	#first we need to compute y^(-1), for this we used extended euclidian function
	yInverse = extendedGcd(int(basic[3]),int(basic[0]))
	if yInverse<0:
		yInverse+=int(basic[0])


	#after getting y inverse we need to calculate y^(-u)=== (y^(-1))^u
	yInverseE= fastExponentiation(int(yInverse),int(advanced[1]),int(basic[0]))
	aPowerS  = fastExponentiation(int(basic[2]),int(advanced[2]),int(basic[0]))

	#computing r*
	rStar    = (int(yInverseE)*int(aPowerS))%int(basic[0])
	print "rStar -> " + str(rStar)


	hashedMsg  = advanced[0]+str(rStar)
	H_HexValue = hashlib.sha1(hashedMsg).hexdigest()
	HValue     = int(H_HexValue,16)


	#compting H(m||r*) 
	print "Hash Value :   " + str(HValue)
	print "e'         :   " + str(advanced[1])

	if HValue==int(advanced[1]):
		return True
	else:
		return False
	return True



#----------------------------main---------------#
if __name__ == "__main__":
	argc=len(sys.argv)
	if argc<2:
		print "ERROR!! Port not given."
		sys.exit()
	port=sys.argv[1]
	server=socket.socket()
	server.bind(('',int(port)))
	server.listen(5)
	while True:
		accSocket,add=server.accept()

		recvdMessge=accSocket.recv(1024)

		msg=recvdMessge.split("##")
		print "Received key=========#"
		print "#====================================#\n"
		print "Prime number p: ",msg[0]
		print "Prime number q: ",msg[1]
		print "Value of alpha: ",msg[2]
		print "Value of y    : ",msg[3]
		print "#====================================#\n"

		msg2=accSocket.recv(1024)
		#accSocket.send("hello")
		SignMsg=msg2.split("##")
		print "Signed message========#"
		print "#====================================#\n"
		print "Message: ",SignMsg[0]
		print "ePrime : ",SignMsg[1]
		print "sPrime : ",SignMsg[2]
		print "#====================================#\n"
		#accSocket.send("hello")


		opcode = 30
		sip = socket.gethostbyname(socket.gethostname())
		s_port = 8989

		Conclusion=SignatureVerify(msg,SignMsg)


		print "\n\n===================Status=====================\n\n"
		if Conclusion==True:
			print "Verified\n"
			structVer=struct.Message(opcode, (sip, s_port), (add[0], add[1]), \
		 								ver_status=True)
		else:
			print "Invalid\n"
			structVer=struct.Message(opcode, (sip, s_port), (add[0], add[1]), \
		 								ver_status=False)

			print "\n\n===================Status=====================\n\n"
		accSocket.send(str(structVer.ver_status))
		accSocket.close()
	server.close()