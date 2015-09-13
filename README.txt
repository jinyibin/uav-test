# uav-test
unmanned aerial vehicle----------for helis	 
created by Jin Yibin in 2015-09-01	
this is a test bench for uav project.	
you can use it to communicate with uav through RS232.

make sure use "exit" to exit the program every time,so that
you can save the flying status data received into the flying_status.csv and flying_status.raw.
or use "save" command to save flying status data

if you want to send way point data ,please add way point data in the wp_init.csv,
you can set task field to 0x01 for test.

if you want to send modify way point data ,please add data in the wp_modify.csv,
modify_type: 0x01 insert
             0x02 modify
             0x04 delete
you can set task field to 0x01 for test.

---------------------------command list------------------
exit       : save the data and exit the program 
stop       : stop the sending data,but not exit  
save       : save flying status data
help       : show you command list
servo test : send servo test command
link  test : send link test command
joystick   : send joystick data
version    : read version of app,kernel,board and FPGA etc
take off   : send plane take off command list
remote1    : set plane into remote mode1
remote2    : set plane into remote mode2
hover      : let the plane hover 
fly        : let the plane fly according to the way point
return     : let the plane fly back
land       : let the plane land
config     : configure the auto pilot system,like heli type,oil volume,max speed etc
para set1  : set the parameter1 of auto pilot system
para set2  : set the parameter2 of auto pilot system
wp modify  : modify the way point,add data in the wp_modify.csv
             format:m_type,id,v,Long,lat,h,task
wp init    : initialize the way point,add data in the wp_ini.csv
             format:id,v,Long,lat,h,task
firmware   : update the firmware
export     : export data
manual     : manual mode
ground ok  : ground check ok
------------------------------------------------------------------------------------------



