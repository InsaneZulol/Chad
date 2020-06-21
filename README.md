# Chad 

#### Change your default system audio device from console.


If you often switch your default audio device with mmsys.cpl this little tool might make your life a little easier.

This is Windows only application.

### Requirements
Windows Vista and later.

### How to set up
Set up an environmental variable for the .exe:
- Drop chad.exe on your drive
- Launch sysdm.cpl -> advanced tab -> Environment Variables... 
- In "User variables %username%" click new -> [type "chad" in name; in value insert the path to chad.exe](https://i.imgur.com/8xB3yHv.png)
- Click OK

### How to use
Launch program with parameters following the .exe file:
- __help__ - to list available commands
- __list, li, l, print__  - to list available audio devices  
additional parameters available:  
- - __ren, render, play, playback__ to list only audio rendering devices;  
- - __cap, capture, rec, recording, mic__ to list only audio capturing devices

![list image](https://i.imgur.com/AyRf2ef.png)

- __set, change, select sel__ ***[n]*** - to set device number *[n]* as your system default audio device.

![set image2](https://i.imgur.com/dVIkfP2.png)

- __savedef__ ***[n]*** - save device number *[n]* as designated system device in the config.ini file. 

![savedef image3](https://i.imgur.com/FYIuVtB.png)

- __reset__ - loads devices from config.ini and selects them as default system devices.

![reset image4](https://i.imgur.com/NwoutBM.png)
