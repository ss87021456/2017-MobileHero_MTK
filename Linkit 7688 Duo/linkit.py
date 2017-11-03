import requests, json, time, serial

database_url = "https://linkittest1010.firebaseio.com/"
heartbeat_url = database_url+'Heart/'+'.json'
abnormal_heart_url = database_url+'Abnormal/Heart/'+'.json'
abnormal_fall_url = database_url+'Abnormal/Fall/'+'.json'

data_iteration = 1

s = None

def setup():
    global s
    s = serial.Serial("/dev/ttyS0", 57600)


def loop():
    global data_iteration
    timestamp = str(time.ctime())
    s.flush()
    data = []
    BPM = s.readline()[:-1]
    data.append(BPM)
    Accident = s.readline()[:-1]
    data.append(Accident)
    Falling = s.readline()[:-1]
    data.append(Falling)
    print data
    text = data[0] + "@" + data[1] + "@" + data[2]
    k = text.split('@')
    data_1 = {'beat': data[0]}
    data_2 = {'time': timestamp }
    data_3 = {'beat': data[0], 'time': timestamp}

    result = requests.patch(heartbeat_url, json=data_1)
    #result = requests.patch(heartbeat_url, json=data_2)
    hearthistory = database_url+'HeartHistory/'+str(data_iteration)+'/.json'
    requests.patch(hearthistory, json=data_3)
    data_iteration = data_iteration+1
    #print(result.status_code)

    if(k[1]=='1'):
        #print "Ya"
        result = requests.patch(abnormal_heart_url, json=data_2)
    if(k[2]=='1'):
        #print "Ya"
        result = requests.patch(abnormal_fall_url, json=data_2)

    print "Heart Beat per min: ", data[0], "\t Heart accident: ", data[1], "\t Falling: ", data[2]


if __name__ == '__main__':
    setup()
    while True:
        loop()