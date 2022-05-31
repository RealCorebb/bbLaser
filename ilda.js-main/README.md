# ilda.js

Based on the repository from https://github.com/possan/ilda.js but completely re-done and made readable.

ilda.js is a javascript class that can read and write ILDA data from a binary .ild file.
It supports all the ilda formats defined by the International [Laser Display Association (ILDA)](ilda.com).
The specification document [can be found here](https://www.ilda.com/resources/StandardsDocs/ILDA_IDTF14_rev011.pdf)

This class has zero dependencies so you don't need to download 40 thousand node modules to find out 6 days later after trying 7 package managers that it don't work.
I can tell you that right now ;-). Nah kidding. I am pretty sure this code has bugs and I will update this as I use this class myself in my BangOn Laser Show editor that is not published yet.

The following formats are supported:

- Format 0 – 3D Coordinates with Indexed Color
- Format 1 – 2D Coordinates with Indexed Color
- Format 2 – Color Palette for Indexed Color Frames
- Format 3 - Not implemented as it is not a formal format
- Format 4 – 3D Coordinates with True Color
- Format 5 – 2D Coordinates with True Color

# demo
![image](https://user-images.githubusercontent.com/1192916/147404880-2b100c17-208d-400a-8d69-da161e641927.png)

[Click here to explore a demo page](https://dinther.github.io/ilda.js/) complete with example code how to access the ilda files on your local computer.

# structure

The precise data [can be found here](https://www.ilda.com/resources/StandardsDocs/ILDA_IDTF14_rev011.pdf) but a quick birds eye overview.
An ILDA file is a binary file. It holds one or many frames which specify how the points held in that frame are defined.

Depending on the Format type a Frame may hold a color palette but also copy right information for that frame.
Initially it may seem strange that each frame holds this data but often ILDA data from various sources are sequentially combined into a laser show.

Points may be 2D or 3D and their color might be from a color palette or directly defined.
Typically the first point is blanked. This means that the laser should be off while moving to that point. Of course this all depends on the designer of the ilda artwork. There is an element of skill involved to create a good looking accurate representation by the laser projector.

# install
I don't do NPM or fantasy, coffee, tea or Coke script so you're on your own there. I just chuck it in the script tag.
```
<script src="ilda.js" type="text/javascript"></script>
```

# usage

```
function selectAndLoadFile(){
    window.showOpenFilePicker({
      types: [
        {description: 'ILD files', accept: {'vector/*': ['.ild']}},
      ],
      excludeAcceptAllOption: true,
      multiple: false
    }).then((fileHandles)=>{
        fileHandles.forEach(fileHandle => {
            fileHandle.getFile().then(file=>{
                // Now we have a file handle. FINALLY
                let reader = new FileReader();
                reader.onload = (evt)=>{
                    if (evt.target.readyState == FileReader.DONE) {
                        let bytes = new Uint8Array(evt.target.result);
                        myIldaData = ILDA.readAsArrayBuffer(evt.target.result);
                        showDataOnPage(myIldaData);
                    }
                }
                reader.readAsArrayBuffer(file);
            });
        })
    });
}
```

The ILDA data is an array of frames each holding an array of points. Other properties such as colors vary depending on the ILDA format type.

![image](https://user-images.githubusercontent.com/1192916/147405044-57b6c805-c9c1-4745-b6af-669192f8fb3e.png)


You can change the data object as you wish and save it back to file. for example like:

```
function saveILDADataTofile(data, fileName){
    if (typeof(data='object')){
        var buffer = ILDA.writeAsArrayBuffer(data);
        if (buffer.length == 0) throw new Error('data.length is zero. Something was not right in the data');
        let a = document.createElement("a");
        a.href = window.URL.createObjectURL(new Blob([buffer], {type: 'octet-stream'}));
        a.download = fileName +'.ild';
        a.click();
        return true;
    } else {
        return false;
    }
}
  ```

  # consider inertia in your design

In order to create good quality ILDA data it would help to understand a little of the physics involved with laser projectors. A laser projector doesn't display a raster like a normal LCD projector. It is no good for watching TV but they are brilliant for drawing rich colored beams of light on surfaces or in air. Lasers are so bright that tiny particles in the air light up enough to form a beam of light.

The laser beam is steered in the X and Y axis by two small mirrors that each turn on a single axis. These things are called Galvos or Galvometer. Think of them as a very fast voltmeters with a mirror instead of a needle.

![image](https://user-images.githubusercontent.com/1192916/147404947-35413e4b-b6d8-4aec-9cb5-caa2d614a0d1.png)

Here is a picture of a set.

The line art drawn by a laser projector can look fairly complex and yet it is drawn many times per second in order to make it appear like a continuous line. These galvos need to move incredibly fast to achieve this but the laws of inertia put a limit to how quickly they can change direction.

The capability of galvos is expressed in points per second they can handle within a certain mirror deflection range (ILDA specifies 8 degrees angle). Typically a galvo is said capable to handle around 30000 (thirty thousand) points per second. This suggests that an image made up of 1000 points can be displayed 30 times per second. Points per second is a rather unfortunate way to specify the capability of a galvo as it fails to take geometry into account.

Considering inertia, it stands to reason that drawing a curve is less demanding than drawing a right angle or worse an acute angle. In fact, it is physically impossible. A right angle drawn by a laser projector is always a curve. The better the galvo the tinier the radius but a curve it is.

Designers use various techniques by placing multiple points along a straight line and closer together when approaching a sharp turn. This has the effect of slowing the galvo down anticipating the turn similar to how you would drive a car. Often multiple points are placed on the very corner with the laser switched off to avoid a visible hotspot.

It is important to be aware these physical limitations as you may otherwise be dissapointed with the results but more importantly, because it is possible to burnout the expensive galvos in your laser projector. Use your ears. When your galvos are screaming they are in pain. Turn it off. When they smoke, they are not having a chill night, they are overheating and burning out.

My other software project BangOn will be dealing with these limitations in new and unique ways and hopefully squeeze even more performance out of existing laser projector equipment.

# warning

![image](https://user-images.githubusercontent.com/1192916/147405087-8a45e95b-e5c9-471e-ae22-c06c8c804898.png)

Be careful when working with lasers, even the tiny laser pointers are dangerous. A direct hit on an eye WILL at a minimum damage it but most likely destroy the eye. Never ever look directly at the beam of a laser. This warning simply can not be repeated enough.


