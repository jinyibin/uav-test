# uav-test
unmanned aerial vehicle----------for helis	 
created by Jin Yibin in 2015-09-01	
this is a test bench for uav project.	
you can use it to communicate with uav through RS232

if you want to send way point data ,please add way point data in the wp_init.csv,
you can set task field to 0x01 for test.

if you want to send modify way point data ,please add data in the wp_modify.csv,
modify_type: 0x01 insert
             0x02 modify
             0x04 delete
you can set task field to 0x01 for test.


