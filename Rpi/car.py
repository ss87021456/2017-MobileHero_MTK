import RPi.GPIO as GPIO
import time

class myCar:

	pwm_frequency = 200

	def __init__(self, (right1, right2), (left1, left2)):
		#GPIO.cleanup()
		GPIO.setmode(GPIO.BOARD)
		GPIO.setup(right1, GPIO.OUT)
		GPIO.setup(right2, GPIO.OUT)
		GPIO.setup(left1, GPIO.OUT)
		GPIO.setup(left2, GPIO.OUT)
		print right1, right2, left1, left2
		self.right1_pwm = GPIO.PWM(right1, self.pwm_frequency)
		self.right1_pwm.start(0.0)
		self.left1_pwm = GPIO.PWM(left1, self.pwm_frequency)
		self.left1_pwm.start(0.0)
		self.right2 = right2
		self.left2 = left2
		GPIO.output(self.right2, False)
		GPIO.output(self.left2, False)

	def rightWheel(self, value):
		if value >= 0:
			self.right1_pwm.ChangeDutyCycle(value)
			GPIO.output(self.right2, False)
		else:
			self.right1_pwm.ChangeDutyCycle(100.0 + value)
			GPIO.output(self.right2, True)

	def leftWheel(self, value):
		if value >= 0:
			self.left1_pwm.ChangeDutyCycle(value)
			GPIO.output(self.left2, False)
		else:
			self.left1_pwm.ChangeDutyCycle(100.0 + value)
			GPIO.output(self.left2, True)
#GPIO.cleanup()
#car = myCar((12, 11), (32, 31))
#GPIO.cleanup()

def forward(car):
	car.rightWheel(100.0)
	car.leftWheel(100.0)
	time.sleep(0.2)

def backward(car):
	car.rightWheel(-40.0)
	car.leftWheel(-40.0)
	time.sleep(0.5)

def idle(car,sleep=0.01):
	car.rightWheel(0.0)
	car.leftWheel(0.0)
	time.sleep(sleep)


def turn_right(car):
	car.rightWheel(60.0)
	car.leftWheel(100.0)
	time.sleep(0.05)

def turn_left(car):
	car.rightWheel(100.0)
	car.leftWheel(50.0)
	time.sleep(0.05)

def rotation(car):
	car.rightWheel(100)
	car.leftWheel(-100)
	time.sleep(0.1)

# Write Yout Function Here


#forward()
#backward()
#turn_right()
#turn_left()

#GPIO.cleanup()
