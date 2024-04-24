# bbAir [English](https://github.com/RealCorebb/bbLaser/blob/main/README_EN.md "English")  
![image](https://github.com/RealCorebb/bbLaser/blob/main/IMG/bbLaser.jpg?raw=true)
## 来点气泡

RELEASES DATE: 2022/11/18      Let's GOOOOO!!!  
HAPPY BIRTHDATE ⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐⭐

🔗[PCB工程文件地址](https://oshwhub.com/corebb/bblaser_pro "PCB工程文件地址")  
😄[3D模型、更详细的教程](https://afdian.net/a/kuruibb "3D模型、更详细的教程")  
🐧QQ群（仅供交流）：647186542  
🐦[Twitter](https://twitter.com/RealCorebb "@RealCorebb") @RealCorebb  
▶️视频(Video):[Youtube](https://www.youtube.com/watch?v=yFprzIGSGpM "Youtube")[ Bilibili](https://www.bilibili.com/video/BV1q14y1W7TJ/ " Bilibili")  
# 禁止搬运到Gitee  
![image](https://github.com/RealCorebb/bbLaser/blob/main/IMG/logo.png?raw=true)  
创意无止境
# 目录结构：
**FrontEnd** bbLaser前端控制台  
**bbLaser** 主程序，Arduino工程文件  
**test_utils** 一些制作过程中杂七杂八的测试东东  
# 关于前端控制台：  
可以前往http://bblaser.bbrealm.com  
（注意不是https）查看DEMO  
因为前端网页比较大，所有东西内嵌到ESP32的话会很慢  
所以，烧写固件到ESP32后，访问它的IP将会重定向至上面的网址并附上ESP32的IP，浏览器会根据?ip=XXXXXX连接WS，以此完成通讯。  
需要在浏览器设置（chrome://flags/#unsafely-treat-insecure-origin-as-secure）中添加上面的网址，以此允许在HTTP的条件下使用相机。

# 关于SD卡播放动画文件：  
在SD卡根目录创建一个文件夹叫做bbLaser  
将.ild （注意大小写）文件放入里面，会按顺序播放。  
可以点击背面的按钮切换/暂停。  
ild文件全称为 ILDA 文件，是一种激光投影的标准文件。  

