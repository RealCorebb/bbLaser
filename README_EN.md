# bbLaser [中文](https://github.com/RealCorebb/bbLaser "中文")  
![image](https://github.com/RealCorebb/bbLaser/blob/main/IMG/bbLaser.jpg?raw=true)
## Let's have fun with Laser⚡

RELEASES DATE: 2022/11/18      Let's GOOOOO!!!  
HAPPY BIRTHDATE ⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐

🔗[PCB (Chinese version of EasyEDA)](https://oshwhub.com/corebb/bblaser_pro "PCB工程文件地址")  
😄[3D Models & Tutorials](https://afdian.net/a/kuruibb "3D模型、更详细的教程")  
🐧QQ Group ：647186542  
🐦[Twitter](https://twitter.com/RealCorebb "@RealCorebb") @RealCorebb  
▶️Video:[Youtube](https://www.youtube.com/watch?v=yFprzIGSGpM "Youtube")[ Bilibili](https://www.bilibili.com/video/BV1q14y1W7TJ/ " Bilibili")   
![image](https://github.com/RealCorebb/bbLaser/blob/main/IMG/logo.png?raw=true)  
Keep Creative!
# Folder：
**FrontEnd** bbLaser Front End Control Pannel  
**bbLaser** Arduino for ESP32 Project (Arduino IDE)
**test_utils** Somethings I used when I developed it 
# About Fron End Pannel：  
You can goto http://bblaser.bbrealm.com  
（NOT https）to see the demo.
Because it is too huge to put it in the ESP32.
So, After flashing the firmware to ESP32, we use browser and input the IP of the ESP32, and then it will redirect to http://bblaser.bbrealm.com?ip=192.168.X.XXX.
So the front end page knows what's the ip of your ESP32, and your mobile & PC will connect it by WebSockets.
Need to goto （chrome://flags/#unsafely-treat-insecure-origin-as-secure） first, and add http://bblaser.bbrealm.com . So you can use your webcam in HTTP Protocal.
