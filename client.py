import hashlib
import socket
import random
import math
import struct
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


#primility test as per miller rabin algorithm
def checkPrimeNumber(power,oddNum,PrimeNum):
	PrimeNum = max(PrimeNum, 4)
	randomNum=random.randint(2,PrimeNum-2)
	Exponent=fastExponentiation(randomNum,oddNum,PrimeNum)
	if Exponent==1 or Exponent==PrimeNum-1:
		return True
	while oddNum<PrimeNum-2:
		Exponent=(Exponent*Exponent)%PrimeNum
		if Exponent==1:
			return False
		if Exponent==PrimeNum-1:
			return True
		oddNum*=2
	return False



#------------------------------------------------------

#MillerRabin algorithm
def checkMillerRabin(PrimeNum):
	oddNum=PrimeNum-1
	power=0
	while oddNum%2==0:
		oddNum=oddNum/2
		power+=1
	conclusion=checkPrimeNumber(power,oddNum,PrimeNum)
	if conclusion==True:
		for i in range(15):
			conclusion=checkPrimeNumber(power,oddNum,PrimeNum)
			if conclusion==True:
				continue
			else:
				return False
	return conclusion


#creating a socket to connect with server
def connServer(Ip, Port):
	client = socket.socket()
	client.connect((Ip, int(Port)))
	return client





#----------------------------------------------------------------------
if __name__ == "__main__":
	argc=len(sys.argv)
	if(argc<3):
		print "ERROR!! Provide both Ip and Port."
		sys.exit()
	Ip   = sys.argv[1]
	Port = sys.argv[2]

	client=connServer(Ip,Port)
	p=2
	q=2
	g=2
	alpha=0
	while True:
		flag=0
		p=random.randint(10001,1000000)
		if p%2==0 or p<4:
			continue
		conclusion=checkMillerRabin(p)
		if conclusion==True:
			break

	for i in range(int(math.sqrt(p))-1,2,-1):
		qPrime=checkMillerRabin(i)
		if qPrime==True:
			if (p-1)%i==0:
				q=i
				break
	while True:
		g=random.randint(2,p-1)
		val=(p-1)/q
		alpha=fastExponentiation(g,val,p)
		if(alpha>1):
			break

	# "a" Private_key
	a=random.randint(1,q)

	#computing Y public key
	y=fastExponentiation(alpha,a,p)
	print "Key Generation Phase:\n"
	print "#=====================================#"
	print "Prime number p: ",p
	print "Prime number q: ",q
	print "Private key  a: ",a
	print "Public key   y: ",y
	print "Public key alpha: ",alpha
	print "#=====================================#\n\n"



	opcode = 10
	s_ip = socket.gethostbyname(socket.gethostname())
	s_port = Port

	structuredMsg = struct.Message(opcode, (s_ip, s_port), (Ip, Port), \
				  p=p, q=q,y=y,alpha = alpha,a=a)


	delim="##"
	pubKey=str(structuredMsg.p)\
			+delim + str(structuredMsg.q)\
			+delim + str(structuredMsg.alpha)\
			+delim + str(structuredMsg.y)
	client.send(pubKey)

	#random secret integer
	k=random.randint(1,q-1)

	#computing r
	r=fastExponentiation(alpha,k,p)
	print "Signature Generation Phase:"
	print "#=========================================#"
	print "Value of r: ",r

	#random generated message
	m=random.randint(100,10000)
	print "Message m: ",m

 	#converting message in bbinary form 
	mb=bin(m)
	print "Message m in binary: ",mb

	concatenate_msg=str(mb)+str(r)
	print "concatenated msg : ",concatenate_msg

	#computing H(m||r)
	hashE=hashlib.sha1(concatenate_msg).hexdigest()
	print "e=H(m||r) :",hashE

	#calculating e
	e=int(hashE,16)
	print "value of e : ",e

	#calculating s
	s=(a*e+k)%q
	print "Value of s : ",s

	#computing u where 1<u<q
	u=random.randint(1,q)
	print "Value of u : ",u

	#computing v where 1<v<q
	v=random.randint(1,q)
	print "Value of v : ",v

	# alpha to the power -u
	#first getting alpha inverse using extended euclidian methin then used Fast exponentiation
	alphaInverseT =extendedGcd(alpha,p)
	if alphaInverseT<0:
		alphaInverseT+=p
	alphaInverse  =fastExponentiation(alphaInverseT,u,p) %p

	# y to the power v
	yPowerV=fastExponentiation(y,v,p)

	#rPrime value =r*(alpha^-u)*(y^v)
	rPrime=((((r%p)*(alphaInverse%p))%p)*yPowerV)%p
	print "Vlaue of rPrime : ",rPrime

	# H(m||rPrime)
	mCon_rPrime=str(mb)+str(rPrime)
	print "H(m||rPrime) : ",mCon_rPrime

	#compute ePrime
	ePrimeHex=hashlib.sha1(mCon_rPrime).hexdigest()

	ePrime=int(ePrimeHex,16)
	print "Value of ePrime: ",ePrime
	#print "Value of e-v: ",e-v

	sPrime=s-u
	print "Value of sPrime: ",sPrime
	print "#=========================================\n\n#"



	# Forming the message
	opcode = 20
	s_ip = socket.gethostbyname(socket.gethostname())
	s_port = Port

	structuredMsg = struct.Message(opcode, (s_ip, s_port), (Ip, Port), \
				  m=mb, e=ePrime,s=sPrime)


	#Format to send message to server
	signMsg=str(structuredMsg.m)+delim\
			+str(structuredMsg.sign.e)+delim\
			+str(structuredMsg.sign.s)

	
	#sending message to server
	client.send(signMsg)


	#receiving status from server side
	ver_status=client.recv(1024)
	print "\n\n===================Status=====================\n\n"
	print "ver_status : ",ver_status
	if ver_status == "True":
		print "Signature verified"
	else:
		print "Invalid Signature"
	print "\n\n===================Status=====================\n"
	client.close()
	##--Signature Generation computations:
