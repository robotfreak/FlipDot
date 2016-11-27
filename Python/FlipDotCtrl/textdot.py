import time
import serial

import fiveBySevenFont

# Init startbits and endbits for matrix
startbit1 = [0x80, 0x85, 0x01] 
startbit2 = [0x80, 0x85, 0x02]
endbit = 0x8F

# Init Serial port
ser = serial.Serial(
	port= 'COM9', #'/dev/ttyUSB0',
	baudrate=19200,
	timeout=1,
	parity=serial.PARITY_NONE,
	stopbits=serial.STOPBITS_ONE,
	bytesize=serial.EIGHTBITS
)

# fillBoard board subroutine
def fillBoard(c): 
	for x in startbit1: 
		ser.write(chr(x))

	for x in range(0, 28):
		ser.write(chr(c))

	ser.write(chr(endbit))

	for x in startbit2: 
		ser.write(chr(x))

	for x in range(0, 28):
		ser.write(chr(c))

	ser.write(chr(endbit))


# Define string to scroll - this can be any length really. Begin with five blank spaces to create illusion of wrapping
outstr = "     Welcome"

# Init list for holding chars to matrix
outlist = []

# Start with clearing the matrix
#while (1 == 1):
fillBoard(0)
line = ser.readline()
print line
time.sleep(0.5)
fillBoard(255)
line = ser.readline()
print line
time.sleep(0.5)

# Loop through characters in message string
for x in outstr:
	# Get binary string from fiveBySeven-font
	thisbin = fiveBySevenFont.getBinaryFromChar(x)
	for y in thisbin:
		# Add char number to outlist
		outlist.append(int(y, 2))
	outlist.append(0)

framecount = 0

while (1 == 1):
	# Loop through 28 cols of matrix
	for x in range(0, 28):
		if (framecount > len(outlist)): framecount = 0
		framelist = outlist[framecount:] # Get 28 chars from outlist, starting with current frame

		# Write starting bits
		for z in startbit1: 
			ser.write(chr(z))
		# Write to first unit
		for y in framelist:
			ser.write(chr(y))

		ser.write(chr(endbit))

		for z in startbit2:
			ser.write(chr(z))
		# Write silly binary animation to second unit, just because. :)
		for p in range(0, 28):
			ser.write(chr(x))

		ser.write(chr(endbit))
					
		# Wait to give matrix time to update, then count up
		line = ser.readline()
		print line
		time.sleep(0.5)
		framecount+= 1


