//  Couple of functions to help you to get to your ILD files on your computer.
//  window.showOpenFilePicker supported on Chrome and edge. Not supported on all browsers
//  Apparently it is difficult to implement file access on a browser.
//  Use some File access library if you must. I will not accommodate the lack of a decent file API min browsers.

var myIldaData;
var ctx;
var axisResolution = 500;
var axisScale = 1;
var slide;

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
                // Now we have a file. Let's read from it.
                loadFile(file);
            });
        });
    });
}

function loadFile(file){
    let reader = new FileReader();
    reader.onload = (evt)=>{
        if (evt.target.readyState == FileReader.DONE) {
            let bytes = new Uint8Array(evt.target.result);
            myIldaData = ILDA.readAsArrayBuffer(evt.target.result);
            showDataOnPage(myIldaData);
        }
    }
    reader.readAsArrayBuffer(file);
}

function saveILDADataTofile(data, fileName){
    if (typeof(data)=='object'){
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

function showDataOnPage(ildaData){
    console.log(ildaData);
    slide.setAttribute('max',ildaData.frames.length - 1);
    slide.value = 0;
    if (myIldaData.frames.length > 0){
        paintFrame(slide.value);
    }
}

function ildaPointToCanvas(value){
    return (value * axisScale) + axisResolution;
}

function paintFrame(frameID){ 
    if (frameID < myIldaData.frames.length){
        let frame = myIldaData.frames[frameID];
        console.log(frame);
        let showPoints = document.getElementById('showpoints').checked;
        ctx.clearRect(0, 0, axisResolution * 2, axisResolution * 2);
        let lastPoint = frame.points[0];
        frame.points.forEach(point=>{
            ctx.beginPath();
            ctx.strokeStyle = "#" + ((1 << 24) + (point.color.r << 16) + (point.color.g << 8) + point.color.b).toString(16).slice(1);
            if (!point.blanking){
                ctx.moveTo(ildaPointToCanvas(lastPoint.x), ildaPointToCanvas(-lastPoint.y))
                ctx.lineTo(ildaPointToCanvas(point.x), ildaPointToCanvas(-point.y));
            }
            ctx.stroke();
            if (showPoints){
                ctx.strokeStyle = '#00ff00';
                ctx.beginPath();
                ctx.arc(ildaPointToCanvas(point.x), ildaPointToCanvas(-point.y), 5, 0, 2 * Math.PI);
                ctx.closePath();
                ctx.stroke();
            }
            lastPoint = point;            
        })
    }
}

window.addEventListener('load', ()=>{
    document.getElementById("upload").addEventListener("dragover", (evt)=>{
        evt.stopPropagation();
		evt.preventDefault();
	    evt.dataTransfer.dropEffect = 'copy';
    }, false);

    document.getElementById("upload").addEventListener("drop", (evt)=>{
		evt.stopPropagation();
		evt.preventDefault();
		for (let i=0; i<evt.dataTransfer.files.length; i++){
            loadFile(evt.dataTransfer.files[i]);
        };
    }, false);

    document.getElementById("load").addEventListener("click", (evt)=>{
        selectAndLoadFile();
    });

    document.getElementById("save").addEventListener("click", (evt)=>{
        //  only save if there is actual data
        if (myIldaData != null & myIldaData.frames.length > 0){
            saveILDADataTofile(myIldaData, 'myfile.ild');
        }
    });

    slide = document.getElementById("frameid");
    slide.addEventListener('input', (evt)=>{
        paintFrame(evt.target.value);
    })

    document.getElementById("showpoints").addEventListener('input', (evt)=>{
        paintFrame(slide.value);
    })   

    let can = document.getElementById("canvas");
    can.width = axisResolution * 2;
    can.height = axisResolution * 2;
    axisScale =  1/32768 * axisResolution;
    ctx = can.getContext('2d');
});

function spiral(startAngle=0, endAngle=7200, startRadius=500, endRadius=25000, pointCount=1000){
    data = {version:'ILDA5.JS', frames:[{owner:'dinther', project:'raster', index:0, total:1, pointTotal:1000, type:4, points:[]}]};
    startAngle *= (Math.PI/180);
    endAngle *= (Math.PI/180);
    let stepAngle = (endAngle - startAngle) / pointCount;
    let radiusStep = (endRadius - startRadius) / pointCount;
    let dist = 0;
    let angle = startAngle;
    for (let i=0; i<pointCount; i++){
        angle += stepAngle;
        dist += radiusStep;
        let radius = startRadius + dist;
        let point = {};
        point.x = Math.sin(angle) * radius;
        point.y = Math.cos(angle) * radius;
        point.z = 0;
        point.blanking = (i==0);
        point.last = (i == pointCount - 1);
        point.color = {r:255, g:255, b: 255};
        data.frames[0].points.push(point);
    }
    myIldaData = data;
    paintFrame(0);
    return data;
}
