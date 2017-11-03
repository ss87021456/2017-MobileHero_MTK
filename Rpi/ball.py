import cv2
import cv2.cv as cv
from car import *
import numpy as np

car = myCar((12, 11), (32, 31))
kernel = np.ones((5,5),np.uint8)

# Take input from webcam
cap = cv2.VideoCapture(0)

# Reduce the size of video to 320x240 so rpi can process faster
cap.set(3,320)
cap.set(4,240)

def nothing(x):
    pass
# Creating a windows for later use
#cv2.namedWindow('HueComp')
#cv2.namedWindow('SatComp')
#cv2.namedWindow('ValComp')
#cv2.namedWindow('closing')
#cv2.namedWindow('tracking')


# Creating track bar for min and max for hue, saturation and value
# You can adjust the defaults as you like
#cv2.createTrackbar('hmin', 'HueComp',20,179,nothing)
#cv2.createTrackbar('hmax', 'HueComp',60,179,nothing)
##
#cv2.createTrackbar('smin', 'SatComp',70,255,nothing)
#cv2.createTrackbar('smax', 'SatComp',218,255,nothing)
##
#cv2.createTrackbar('vmin', 'ValComp',166,255,nothing)
#cv2.createTrackbar('vmax', 'ValComp',255,255,nothing)

# My experimental values
# hmn = 12
# hmx = 37
# smn = 145
# smx = 255
# vmn = 186
# vmx = 255


while(1):

    buzz = 0
    _, frame = cap.read()

    #converting to HSV
    hsv = cv2.cvtColor(frame,cv2.COLOR_BGR2HSV)
    hue,sat,val = cv2.split(hsv)

    # get info from track bar and appy to result
    #hmn = cv2.getTrackbarPos('hmin','HueComp')
    #hmx = cv2.getTrackbarPos('hmax','HueComp')
    hmn = 20
    hmx = 60
    smn = 70 
    smx = 218
    vmn = 166
    vmx = 255
    
    #smn = cv2.getTrackbarPos('smin','SatComp')
    #smx = cv2.getTrackbarPos('smax','SatComp')
    #vmn = cv2.getTrackbarPos('vmin','ValComp')
    #vmx = cv2.getTrackbarPos('vmax','ValComp')
#
    # Apply thresholding
    hthresh = cv2.inRange(np.array(hue),np.array(hmn),np.array(hmx))
    sthresh = cv2.inRange(np.array(sat),np.array(smn),np.array(smx))
    vthresh = cv2.inRange(np.array(val),np.array(vmn),np.array(vmx))

    # AND h s and v
    tracking = cv2.bitwise_and(hthresh,cv2.bitwise_and(sthresh,vthresh))

    # Some morpholigical filtering
    dilation = cv2.dilate(tracking,kernel,iterations = 1)
    closing = cv2.morphologyEx(dilation, cv2.MORPH_CLOSE, kernel)
    closing = cv2.GaussianBlur(closing,(5,5),0)

    # Detect circles using HoughCircles
    circles = cv2.HoughCircles(closing,cv.CV_HOUGH_GRADIENT,2,120,param1=120,param2=50,minRadius=10,maxRadius=0)
    # circles = np.uint16(np.around(circles))

    #print circles

    #Draw Circles
    if circles is not None:
        i = circles[0,0]
        x, y = (int(round(i[0])),int(round(i[1])))
        print "center: ",(int(round(i[0])),int(round(i[1])))
        # If the ball is far, draw it in green
        cv2.circle(frame,(int(round(i[0])),int(round(i[1]))),int(round(i[2])),(0,255,0),5)
        cv2.circle(frame,(int(round(i[0])),int(round(i[1]))),2,(0,255,0),10)
        # turn right
        if int(round(i[2])) > 30:
            idle(car)
            print "stop!!"
            #cv2.circle(frame,(int(round(i[0])),int(round(i[1]))),int(round(i[2])),(0,0,255),5)
            #cv2.circle(frame,(int(round(i[0])),int(round(i[1]))),2,(0,0,255),10)
            buzz = 1
        elif x < 125:
            turn_left(car)
            print "turn left!!"
            idle(car)
        # turn left
        elif x > 175:
            turn_right(car)
            print "turn right!!"
            idle(car)
        # forward
        elif int(round(i[2])) < 30:
            forward(car)
            print "forward!!"
            idle(car)
            #cv2.circle(frame,(int(round(i[0])),int(round(i[1]))),int(round(i[2])),(0,255,0),5)
            #cv2.circle(frame,(int(round(i[0])),int(round(i[1]))),2,(0,255,0),10)
        # else draw it in red
        # stop
    else:
        print "searching..."
        #rotation(car)
        #idle(car,1)'''



	#you can use the 'buzz' variable as a trigger to switch some GPIO lines on Rpi :)
    # print buzz                    
    # if buzz:
        # put your GPIO line here

    
    #Show the result in frames
    #cv2.imshow('HueComp',hthresh)
    #cv2.imshow('SatComp',sthresh)
    #cv2.imshow('ValComp',vthresh)
    #cv2.imshow('closing',closing)
    #cv2.imshow('tracking',frame)

    k = cv2.waitKey(5) & 0xFF
    if k == 27:
        break

GPIO.cleanup()
cap.release()

cv2.destroyAllWindows()
