# Comp4900-FP

Author: UnPluggedToaster

This project (school) is an implementation of the back-end of a security lock on a door, etc. 

The project consists of the following files: 
  
  - server.c:


    A C application that receives iov_t messages from the client and parses through it. Checks the entries against the master password. Sends results to python server. 
    
  - client.c:
  
    A C application that takes user input and sends iov_t messages to the server, also sends pulse_codes as part of the multipart message. 
    
  - mailServer.py:
  
    Python mail server that gets data from server.c using TCP protocols, uses the smtp library to send emails to a preconfigured mailing list. Results take around 5-15   seconds to send. Uses an outlook account to send the emails. 
    
    
To run the program, first you must run server.c, then mailServer.py, and then client.c

The QNX framework is required to run this program, as is python3. 
