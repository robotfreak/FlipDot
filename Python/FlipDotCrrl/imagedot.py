import time
import serial
from PIL import Image

ser = serial.Serial(
	port='COM9', #/dev/ttyUSB0',
	baudrate=19200,
	timeout=1,
	parity=serial.PARITY_NONE,
	stopbits=serial.STOPBITS_ONE,
	bytesize=serial.EIGHTBITS
)

frame = Image.open("abc.bmp")
nframes = 0

hexar1 = []
hexar2 = []
hexar3 = []

# Set index and start commands for matrix
startbit1 = [0x80, 0x85, 0x01]
startbit2 = [0x80, 0x85, 0x02]
startbit3 = [0x80, 0x85, 0x03]


# Set endbit for matrix
endbit = 0x8F

while frame:
	# load frame pixels
	pix = frame.load()
	
	# Init the two lists for holding matrix chars
	hexar1 = []
	hexar2 = []
	hexar3 = []

	# Loop through 28 pixels width
	for x in range(0, 28):
		row1 = ""
		row2 = ""
		row3 = ""
		# Loop seven pixels down, add to binary string
		for y in range(0, 7):
			thispix = int(pix[x, y])
			if (thispix > 0): thispix = 1
			row1 = str(thispix) + row1
			thispix = int(pix[x, (y+7)])
			if (thispix > 0): thispix = 1
			row2 = str(thispix) + row2
#		for y in range(0,1):	
#			thispix = int(pix[x, (y+14)])
#			if (thispix > 0): thispix = 1
#			row3 = str(thispix) + row3
		# Convert binary string to integer and add to output lists
		hexar1.append(int(row1, 2))
		hexar2.append(int(row2, 2))
#		hexar3.append(int(row3, 2))

	# Write to matrix one
	for x in startbit1: 
		ser.write(chr(x))
	for y in hexar1:
		ser.write(chr(y))

	ser.write(chr(endbit))

	# Write to matrix two
	for x in startbit2: 
		ser.write(chr(x))
	for y in hexar2:
		ser.write(chr(y))

	ser.write(chr(endbit))
	
	# Write to matrix three
#	for x in startbit3: 
##		ser.write(chr(x))
#	for y in hexar3:
#		ser.write(chr(y))

#	ser.write(chr(endbit))

	# Step up frame counter
	nframes += 1

	# Check for more frames in GIF
	try:
		frame.seek(nframes)
	except EOFError:
		# GIF is EOF (last frame done), start over
		frame.seek(0)
		nframes = 0

	# Wait to give matrix time to update
	line = ser.readline()
	print line
	time.sleep(.5)
	
