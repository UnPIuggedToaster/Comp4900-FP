
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart

import socket
import sys 

import smtplib, ssl 

from email.mime.text import MIMEText


def sendEmail(msg): 
    
    sslPort = 587 # port for ssl 
    
    From = "finalprojectqnx@outlook.com"
    password = "" 

    # add email addresses to this section when testing. 
    to = ["email@gmail.com"]

    # message = MIMEText("This message is sent from your local python email server.")
    
    # server = smtplib.SMTP_SSL(smtpServer, sslPort)
    
    subject = "QNX pinpad notification"
    
    # format the email 
    message = 'Subject: {}\n\n{}'.format(subject, msg)
    
    try: 
        server = smtplib.SMTP('smtp.office365.com', sslPort)
        server.starttls() #tell the server we are using tls encryption 

        server.login(From, password)
        server.sendmail(From, to, message)
        server.close() 
        
        print("""Sent email from: %s to: %s""" % (From, to))
        
    except: 
        print ('Failed sending email.')  
    
    
    

def main_loop():    
    # host adress and port name, need to change the IP address to QNX vm's ip address 
    qhost = '192.168.150.128'
    qport = 8080

    # create a socket on the python email server 
    s = socket.socket(socket.AF_INET,
                    socket.SOCK_STREAM)
    
    # connect server to ip address and port address 
    s.connect((qhost, qport))

    s.send(b'HELLO FROM PYTHON')

    msg = s.recv (1024)

    # check if the server is sending messages 
    while msg: 
        print('Received: ' + msg.decode())
        sendEmail(msg.decode())  
        msg = s.recv(1024)
        s.send(b'HELLO FROM PYTHON')
        
    s.close()   
    
# dont stop the program until the python mail server cannot communicate with the server  
while 1: 
    main_loop()
