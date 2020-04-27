
'''
import serial
import numpy as np
import datetime
'''

import matplotlib.pyplot as plt
import matplotlib.animation as anim

'''
All changes before we get actual serial input need to be done in the txt file
readoutsLog = open('readouts_' + str(datetime.datetime.now()) + '.txt', 'a')

timeArray = []
pressureArray = []
temperatureArray = []
'''
#creating a figure with multiple subplots
fig = plt.figure()
temperatureAx = fig.add_subplot(2,1,1)
pressureAx = fig.add_subplot(2,1,2)

def animate(i):
    
    '''
    data = ser.readline
    
    and anything to do with physically input data needs to be done later
    '''
    timeArray = []
    pressureArray = []
    temperatureArray = []
    #populate into the subplots
    data = open("readouts.txt", "r").read()
    dataArray = data.split('\n')
    x,y,z = 0,0,0
    for line in dataArray:
        
        if len(line) > 1:
            x,y,z = line.split(',')
            timeArray.append(int(x))
            pressureArray.append(int(y))
            temperatureArray.append(int(z))
    
    temperatureAx.clear()
    temperatureAx.plot(timeArray, temperatureArray)
    
    
    pressureAx.clear()
    pressureAx.plot(timeArray, pressureArray)
    
    print(x)
    print(y)
    print(z)
    
ani = anim.FuncAnimation(fig, animate, interval = 1000)
plt.show()
            
            
    