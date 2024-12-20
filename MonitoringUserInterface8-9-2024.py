# RMS Graphical User Interface Code
# Ryan Anderson
# 7/25/2024

# Import Functions
import customtkinter #user interface
from CTkMessagebox import CTkMessagebox
import serial.tools.list_ports
import functools
import tkinter

#Define arduino ports
ports = serial.tools.list_ports.comports()
serialInst = serial.Serial()
portList = []

# Define Interface Appearance 
customtkinter.set_appearance_mode("dark")
customtkinter.set_default_color_theme("green")

#Text Sizing
titlesize = 24
subtitlesize = 18
textsize = 18

#define root (main page)
root = customtkinter.CTk()
root.geometry("1920x1200")

#define FFUID input variable
FFUID_var = customtkinter.StringVar()

#character minimum for FFUID
FFUIDspecific = 5
FFUIDgeneral = 4

### Arduino Flood Data ###
for onePort in ports:
    portList.append(str(onePort))
    print(str(onePort))

#user input for arduino COM
val = input("select Port: COM: ")

for x in range(0,len(portList)):
    if portList[x].startswith("COM" + str(val)):
        portVar = "COM" + str(val)
        print(portVar + " Selected")

#arduino sampling rate/declaration
serialInst.baudrate = 115200 
serialInst.port = portVar
serialInst.open()

#main menu interface
def mainmenu():

    #Framing Application
    frame = customtkinter.CTkFrame(master=root)
    frame.pack(pady=10, padx=100, fill="both", expand=True)

    #Title/Subtitle of Application
    title = customtkinter.CTkLabel(master=frame, text="FFU Remote Monitoring System", text_color=("lightgreen"), font=("Courier New", titlesize))
    title.pack(pady=1, padx=1)

    subtitle = customtkinter.CTkLabel(master=frame, text="Global Foundries", text_color=("orange"), font=("Courier New", subtitlesize))
    subtitle.pack(pady=1, padx=1)

    subtitle = customtkinter.CTkLabel(master=frame, text="Contamination Control Team", text_color=("orange"), font=("Courier New", subtitlesize))
    subtitle.pack(pady=1, padx=1)

    #Flood Monitoring Button
    flood = customtkinter.CTkButton(master=frame, text="Open Flood Monitor", text_color=("black"), font=("Courier New", textsize),
                                    command=openflood)
    flood.pack(pady=50, padx=10)

    #Search for Specific FFU
    entry1 = customtkinter.CTkEntry(master=frame, textvariable = FFUID_var, placeholder_text="FFU ID", text_color=("orange"), font=("Courier New", textsize))
    entry1.pack(pady=12, padx=10)

    #Search Button
    searchbutton = customtkinter.CTkButton(master=frame, text="Search", text_color=("black"), font=("Courier New", textsize),
                                            command=search)
    searchbutton.pack(pady=12, padx=10)

    #Exit Application button
    quitbutton = customtkinter.CTkButton(master=frame, text="Exit Application", text_color=("black"), font=("Courier New", textsize),
                                            command=quit)
    quitbutton.pack(pady=10, padx=10)

#Validity function/checking
def search():
    #setting variable
    FFUID=FFUID_var.get()
    FFUID_var.set("")

    #statements to verify proper ID
    if len(FFUID) == FFUIDspecific:
        FANID = str(FFUID[4])
        print(FANID)
        result = FANID.isalpha()
        if result is False:
            invalid()
    if len(FFUID) != FFUIDspecific and len(FFUID) != FFUIDgeneral:       
        invalid()

    if len(FFUID) == FFUIDspecific and result is True:
        searchspecific(FFUID)

    if len(FFUID) == FFUIDgeneral:
        searchgeneral(FFUID)

### This section begins the code for flood monitoring section and specific/general search ### 

#function to open the flood monitor
def openflood():

    #Framing Application
    floodwindow = customtkinter.CTkScrollableFrame(root, orientation="vertical",width=1200,height=600)
    floodwindow.pack(pady=40)
    
    customtkinter.CTkLabel(floodwindow, text = "Flood Monitoring System", text_color=("lightgreen"), font=("Courier New", titlesize)).pack()

    #quitbutton = customtkinter.CTkButton(floodwindow, text="Stop Viewing Flood Monitor", text_color=("black"), font=("Courier New", textsize),
                                        #command=floodwindow.destroy)
    #quitbutton.pack(pady=10, padx=10)

    while True:
        floodwindow.update()
        if serialInst.in_waiting:
            packet = serialInst.readline()
            packetstring = packet.decode('utf').rstrip('\n')
            if packetstring[len(packetstring) - 2] != "l":
                print(packetstring)
                customtkinter.CTkLabel(floodwindow,text = packetstring, text_color=("orange"), font=("Courier New", subtitlesize)).pack()
                
            if packetstring[len(packetstring) - 2] == "l":
                print(packetstring)
                customtkinter.CTkLabel(floodwindow,text = packetstring, text_color=("white"), font=("Courier New", subtitlesize)).pack()



def searchspecific(FFUID):
    FFUID = str.upper(FFUID)
    #Framing Application
    ffuwindow = customtkinter.CTkToplevel()
    ffuwindow.title("Specific Search for " + FFUID)
    ffuwindow.geometry("1200x225")
    customtkinter.CTkLabel(ffuwindow, text = "Current Status for Tool: " + FFUID[0:4] + "\nFFU: " + FFUID[4], text_color=("lightgreen"), font=("Courier New", titlesize)).pack()

    quitbutton = customtkinter.CTkButton(master=ffuwindow, text="Stop Viewing " + FFUID, text_color=("black"), font=("Courier New", textsize),
                                        command=ffuwindow.destroy)
    quitbutton.pack(pady=10, padx=10)

    while True:
        ffuwindow.update()
        if serialInst.in_waiting:
            packet = serialInst.readline()
            packetstring = packet.decode('utf').rstrip('\n')
            if str(packetstring[5:10]) == str(FFUID):
                if packetstring[len(packetstring) - 2] != "l":
                    print(packetstring)
                    customtkinter.CTkLabel(ffuwindow,text = packetstring, text_color=("orange"), font=("Courier New", subtitlesize)).pack()
            
                if packetstring[len(packetstring) - 2] == "l":
                    print(packetstring)
                    customtkinter.CTkLabel(ffuwindow,text = packetstring, text_color=("white"), font=("Courier New", subtitlesize)).pack()



def searchgeneral(FFUID):
    FFUID = str.upper(FFUID)
    #Framing Application
    ffuwindow = customtkinter.CTkToplevel()
    ffuwindow.title("Specific Search for " + FFUID)
    ffuwindow.geometry("1200x225")
    customtkinter.CTkLabel(ffuwindow, text = "Current Status for Tool: " + FFUID[0:4], text_color=("lightgreen"), font=("Courier New", titlesize)).pack()

    quitbutton = customtkinter.CTkButton(ffuwindow, text="Stop Viewing " + str.upper(FFUID), text_color=("black"), font=("Courier New", textsize),
                                        command=ffuwindow.destroy)
    quitbutton.pack(pady=10, padx=10)

    while True:
        ffuwindow.update()
        if serialInst.in_waiting:
            packet = serialInst.readline()
            packetstring = packet.decode('utf').rstrip('\n')
            if str(packetstring[5:9]) == str(FFUID):
                if packetstring[len(packetstring) - 2] != "l":
                    print(packetstring)
                    customtkinter.CTkLabel(ffuwindow,text = packetstring, text_color=("orange"), font=("Courier New", subtitlesize)).pack()
            
                if packetstring[len(packetstring) - 2] == "l":
                    print(packetstring)
                    customtkinter.CTkLabel(ffuwindow,text = packetstring, text_color=("white"), font=("Courier New", subtitlesize)).pack()


### This Ends the section for flood monitoring and specific/general search ###

def invalid():
        #Framing Application
        CTkMessagebox(title="Error",message="INVALID INPUT\n\nInput proper FFUID",icon="cancel", option_1="Retry", text_color=("white"), font=("Courier New", subtitlesize))

        print("invalid")

def quit():
     root.destroy()

mainmenu()
root.mainloop()
