class Header:
	def __init__(self, opcode, s_addr, d_addr):
		self.opcode = opcode
		self.s_addr = s_addr
		self.d_addr = d_addr

class Signature:
	def __init__(self, e, s):
		self.e = e
		self.s = s

class Message:
	def __init__(self, opcode, s_addr, d_addr, \
				p=0, q=0, g=0, y=0, alpha=0, a=0, m="", \
				e=0, s=0, ver_status=False,dummy=0):
		self.hdr = Header(opcode, s_addr, d_addr)
		self.p = p
		self.q = q
		self.g = g
		self.y = y
		self.alpha = alpha
		self.a = a
		self.m= m
		self.sign = Signature(e, s)
		self.ver_status = ver_status
		self.dummy=dummy