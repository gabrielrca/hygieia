import threading
import serial
import binascii
import time
from datetime import datetime

connected = False
port = '/dev/ttyUSB0'
baud = 9600

serial_port = serial.Serial(port, baud)

frames_arrived=[]
current_frame=0

	

def read_from_port(ser):
	while True:
		try: 
			reading = ser.read(3)
			#print binascii.hexlify(reading)
			frames_arrived.append(binascii.hexlify(reading))
		except KeyboardInterrupt:
			break


start_time=time.time()

thread = threading.Thread(target=read_from_port, args=(serial_port,))
thread.start()

whole_data=[]
outlier_data=[]
nonoutlier_data=[]

f1_nonoutlier=0
f2_nonoutlier=0
f3_nonoutlier=0

f1_outlier=0
f2_outlier=0
f3_outlier=0

average_without_quality=[]
average_nonoutlier_data=[]
average_outlier_data=[]

time_to_run=60
timeout = time.time() + 60*time_to_run

while (time.time() < timeout):
		#if(len(frames_arrived) >= 1):
		#	if (current_frame <= len(frames_arrived)):
		#		print frames_arrived[-1]
		#		current_frame = current_frame + 1
	
	if(len(frames_arrived) > current_frame):
		frame = frames_arrived[current_frame]
		frame_fields=[]
		while frame:
			frame_fields.append(frame[:2])
			frame = frame[2:]
		#print frame_fields
		
		'''
		print "Node ID:" + str(frame_fields[0])
		print "Is outlier: " + str(frame_fields[1])
		print "Datum: " + str(int(frame_fields[2],16))
		'''
		
		whole_data.append(int(frame_fields[2],16))
		if(frame_fields[1] == '00'):
				#Is not outlier
				nonoutlier_data.append(int(frame_fields[2],16))
				
				if(frame_fields[0] == "f1"):
					f1_nonoutlier = f1_nonoutlier +1
				elif(frame_fields[0] == "f2"):
					f2_nonoutlier = f2_nonoutlier +1
				elif(frame_fields[0] == "f3"):
					f3_nonoutlier = f3_nonoutlier +1
					
		elif(frame_fields[1] == 'ff'):
				#is Outlier
				outlier_data.append(int(frame_fields[2],16))
				
				if(frame_fields[0] == "f1"):
					f1_outlier = f1_outlier +1
				elif(frame_fields[0] == "f2"):
					f2_outlier = f2_outlier +1
				elif(frame_fields[0] == "f3"):
					f3_outlier = f3_outlier +1
		
		if(len(whole_data) == 3):
			acc=0
			for datum in whole_data:
				acc = acc + int(datum)
			average_without_quality.append(acc/len(whole_data))
			whole_data=[]
		
		if(len(nonoutlier_data) == 3):
			acc=0
			for datum in nonoutlier_data:
				acc = acc + int(datum)
			average_nonoutlier_data.append(acc/len(nonoutlier_data))
			nonoutlier_data=[]

		if(len(outlier_data) == 3):
			acc=0
			for datum in outlier_data:
				acc = acc + int(datum)
			average_outlier_data.append(acc/len(outlier_data))
			outlier_data=[]

		print "Summary: " + str(datetime.now())
		print "Elapsed Time: " + str(time.time() - start_time)
		print "Quantity of non outlier (F1, F2, F3): " + str(f1_nonoutlier) + " " + str(f2_nonoutlier) + " " + str(f3_nonoutlier) 
		print "Quantity of outlier (F1, F2, F3): " + str(f1_outlier ) + " " + str(f2_outlier) + " " + str(f3_outlier)
		print "Quantity of frames arrived: " + str(len(frames_arrived))
		print "All Frames: " + str(frames_arrived)
		print "MAF No quality:  " + str(average_without_quality)
		print "MAF Non outlier: " + str(average_nonoutlier_data)
		print "MAF Outlier:     " + str(average_outlier_data)
			
		current_frame = current_frame + 1



