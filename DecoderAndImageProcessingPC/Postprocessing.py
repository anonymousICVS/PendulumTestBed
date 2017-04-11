import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

a = pd.read_csv("~/build-CodingPendulumReceiver-Desktop_Qt_5_7_0_GCC_64bit-Release/log_angles_coded.csv", header=0)

#Nulling time, normalizing it
a.loc[:,'Time'] = a.loc[:,'Time'] - a.loc[0,'Time']
a.loc[:,'Time'] = a.loc[:,'Time']/1000000

#Removing the first second and the last 0.8 seconds (initalizing and after fall)
idx = a.loc[:,'Time'] > 1
a = a.loc[idx,:]
a = a.reset_index(drop=True)

#Nulling again
a.loc[:,'Time'] = a.loc[:,'Time'] - a.loc[0,'Time']

#Removing the last 0.8 seconds
idx = a.loc[:,'Time'] < a.iloc[-1,0] - 0.0
a = a.loc[idx,:]
print "\n The pendulum was up for " + "{:.2f}".format(a.iloc[-1,0]) + " seconds"

#Statistics about failed measurements
idx = a.loc[:,'Measured Angle'] < 90
failed = (len(idx)-sum(idx) + 0.0)/len(idx)
print " Failed measurements: " + "{:.2f}".format(failed*100) + "%"

#Removing failed measurements from the first matrix, saving remainder into prediction matrix
predictions = a#.loc[[not i for i in idx],:]
a = a.loc[idx,:]
a = a.reset_index(drop=True)

#Compute the angle mean --> After its subtraction this should be very close to angle zero
meanAngle = a['Measured Angle'].mean()
print " Mean Measured Angle: "  + "{:.2f}".format(meanAngle) + " degrees"
a['Measured Angle'] = a['Measured Angle'] - meanAngle

#Computing the angle variance
angleVar = a['Measured Angle'].var()
print " Angle Variance: "  + "{:.2f}".format(angleVar) + " degrees"

#Angle samples per second: mean and variance
l = len(a.loc[:,'Measured Angle'])
b = a.loc[:,'Time'] - a.loc[:,'Time'].shift()
c = b.var()
d = 1/b.mean()
print " Angle samples per second: " + "{:.2f}".format(d) + ", with a period variance of " + "{:.5f}".format(c) + " seconds.\n"

#Plotting Angle and Frame times
#plt.plot(a.loc[:,'Time'],a.loc[:,'Angle'])
#plt.xlabel('Time [s]')
#plt.ylabel('Angle [deg]')
#plt.title('Pendulum behavior')
#plt.grid(True)
#plt.show()

fig, ax1 = plt.subplots()
ax1.plot(a.loc[:,'Time'],a.loc[:,'Measured Angle'], 'b-')
ax1.set_xlabel('Time [s]')
ax1.set_ylabel('Angle [deg]', color='b')
ax1.tick_params('y', colors='b')
plt.grid(True)

ax2 = ax1.twinx()
ax2.plot(a.loc[:,'Time'], b*1000, 'r-')
ax2.set_ylabel('Sample period [ms]', color='r')
ax2.tick_params('y', colors='r')

ax3 = ax1.twinx()
ax3.plot(a.loc[:,'Time'], a.loc[:,'Measured Angle Speed'], 'g-')
ax3.set_ylabel('Angle speed [deg/s]', color='g')
ax3.tick_params('y', colors='g')
ax3.spines['right'].set_position(('axes',1.05))

#ax4 = ax1.twinx()
#ax4.plot(predictions.loc[:,'Time'], predictions.loc[:,'PredictedAngle'], 'r-')
#ax4.set_ylabel('Predicted Angle [deg]', color='r')
#ax4.tick_params('y', colors='r')
#ax4.spines['right'].set_position(('axes',1.05))

#ax5 = ax1.twinx()
#ax5.plot(predictions.loc[:,'Time'], predictions.loc[:,'PredictedSpeed'], 'c-')
#ax5.set_ylabel('Predicted Angle speed [deg/s]', color='c')
#ax5.tick_params('y', colors='c')
#ax5.spines['right'].set_position(('axes',1.10))

fig.tight_layout()
plt.show()


#Additional plot for understanding the development of the predicted Angle

fig2, ax6 = plt.subplots()
ax6.plot(predictions.loc[:,'Time'], predictions.loc[:,'Predicted Angle'], 'b-')
ax6.set_ylabel('Predicted Angle [deg]', color='b')
ax6.tick_params('y', colors='b')

ax7 = ax6.twinx()
ax7.plot(predictions.loc[:,'Time'], predictions.loc[:,'Predicted Angle Speed'], 'g-')
ax7.set_ylabel('Predicted Angle speed [deg/s]', color='g')
ax7.tick_params('y', colors='g')

ax8 = ax6.twinx()
ax8.plot(predictions.loc[:,'Time'], predictions.loc[:,'Motor Speed'], 'r-')
ax8.set_ylabel('Motor speed', color='r')
ax8.tick_params('y', colors='r')
ax8.spines['right'].set_position(('axes',1.05))

ax9 = ax6.twinx()
ax9.plot(predictions.loc[:,'Time'], predictions.loc[:,'Motor Power'], 'c-')
ax9.set_ylabel('Power', color='c')
ax9.tick_params('y', colors='c')
ax9.spines['right'].set_position(('axes',1.10))

fig2.tight_layout()
plt.show()
