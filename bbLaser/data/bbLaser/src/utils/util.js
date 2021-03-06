import {Buffer} from 'buffer';
export function makeStreamBuffer(pointData){
    var frameData = new Uint8Array()
        for (let point of pointData.points){
            point.x = parseInt((point.x - 0.5) * 65535)
            point.y = parseInt((point.y - 0.5)  * 65535)

            //My Code
            const coor = new Int16Array(2);
            coor[0] = parseInt(point.x);
            coor[1] = parseInt(point.y);
            const blanking = new Uint8Array(1);
            const color = new Uint8Array(1);
            
            if(point.r == 0 && point.g == 0 && point.b == 0){
                blanking[0] = 64; // Equal 0100 0000
                color[0] = 0;
            }
            else{
                blanking[0] = 0; 
                if(point.r == 1 && point.g == 0 && point.b == 0){  //RED
                    color[0] = 5
                }
                else if (point.r == 1 && point.g == 1 && point.b == 0){ //YELLOW
                    color[0] = 15
                }
                else if (point.r == 0 && point.g == 1 && point.b == 0){ //GREEN
                    color[0] = 25
                }
                else if (point.r == 0 && point.g == 1 && point.b == 1){ //CYAN
                    color[0] = 34
                }
                else if (point.r == 0 && point.g == 0 && point.b == 1){ //BLUE
                    color[0] = 42
                }
                else if (point.r == 1 && point.g == 0 && point.b == 1){ //MAGENTA
                    color[0] = 52
                }
                else if (point.r == 1 && point.g == 1 && point.b == 1){ //WHITE
                    color[0] = 63
                }
            }

            //combine coor and color    <- Github Copilot
            const data = new Uint8Array(4);
            data[0] = coor[0] >> 8;
            data[1] = coor[0] & 0xFF;
            data[2] = coor[1] >> 8;
            data[3] = coor[1] & 0xFF;
            frameData = Buffer.concat([frameData,data,color,blanking])	
        }
    return frameData
}

export function hexToILDAColor(color){
    let hex = color.replace('#','');
    let r = parseInt(hex.substring(0,2),16);
    let g = parseInt(hex.substring(2,4),16);
    let b = parseInt(hex.substring(4,6),16);
    //if rgb > 128 ,rgb is 1,else 0
    let r1 = r > 128 ? 1 : 0;
    let g1 = g > 128 ? 1 : 0;
    let b1 = b > 128 ? 1 : 0;
    return [r1,g1,b1]
}