class ILDACLASS{
    #Version = 'ILDA5.JS';
    #DefaultPalette = [{r:255,g:0,b:0},{r:255,g:17,b:0},{r:255,g:34,b:0},{r:255,g:51,b:0},{r:255,g:68,b:0},{r:255,g:85,b:0},{r:255,g:102,b:0},{r:255,g:119,b:0},{r:255,g:136,b:0},{r:255,g:153,b:0},{r:255,g:170,b:0},{r:255,g:187,b:0},{r:255,g:204,b:0},{r:255,g:221,b:0},{r:255,g:238,b:0},{r:255,g:255,b:0},{r:255,g:255,b:0},{r:238,g:255,b:0},{r:204,g:255,b:0},{r:170,g:255,b:0},{r:136,g:255,b:0},{r:102,g:255,b:0},{r:68,g:255,b:0},{r:34,g:255,b:0},{r:0,g:255,b:0},{r:0,g:255,b:34},{r:0,g:255,b:68},{r:0,g:255,b:102},{r:0,g:255,b:136},{r:0,g:255,b:170},{r:0,g:255,b:204},{r:0,g:255,b:238},{r:0,g:136,b:255},{r:0,g:119,b:255},{r:0,g:102,b:255},{r:0,g:102,b:255},{r:0,g:85,b:255},{r:0,g:68,b:255},{r:0,g:68,b:255},{r:0,g:34,b:255},{r:0,g:0,b:255},{r:34,g:0,b:255},{r:68,g:0,b:255},{r:102,g:0,b:255},{r:136,g:0,b:255},{r:170,g:0,b:255},{r:204,g:0,b:255},{r:238,g:0,b:255},{r:255,g:0,b:255},{r:255,g:34,b:255},{r:255,g:68,b:255},{r:255,g:102,b:255},{r:255,g:136,b:255},{r:255,g:170,b:255},{r:255,g:204,b:255},{r:255,g:238,b:255},{r:255,g:255,b:255},{r:255,g:238,b:238},{r:255,g:204,b:204},{r:255,g:170,b:170},{r:255,g:136,b:136},{r:255,g:102,b:102},{r:255,g:68,b:68},{r:0,g:34,b:34}];
    #currentPalette;
    #readBytes = [];
    #readPosition = 0;
    #writebytes = [];    
    #FrameTypes = {
        COORD_3D: 0,
        COORD_2D: 1,
        COLOR_TABLE: 2,
        COORD_3D_TRUE_COL: 4,
        COORD_2D_TRUE_COL: 5,
        UNKNOWN: 99
    };
    constructor(){
        this.#currentPalette = this.#DefaultPalette;
    }
    #seek = function(c) {
        this.#readPosition = c
    };
    #eof = function() {
        return this.#readPosition >= this.#readBytes.length;
    };
    #readString = function(count) {
        let text = '';
        for (let i = 0; i < count; i++) {
            let b = this.#readByte();
            0 < b && 127 > b && (text += String.fromCharCode(b))
        }
        return text.trim()
    };
    #readByte = function() {
        var b = this.#readBytes[this.#readPosition];
        this.#readPosition++;
        return b
    };
    #readShort = function() {
        var c = this.#readByte()
          , h = this.#readByte();
        return (c << 8) + h
    };
    #readSignedShort = function() {
        var c = this.#readByte()
          , h = this.#readByte()
          , c = (c << 8) + h;
        32768 < c && (c = -(65535 - c));
        return c
    };
    #writeByte = function(c) {
        this.#writebytes.push(c)
    };
    #writeShort = function(c) {
        this.#writeByte(c >> 8 & 255);
        this.#writeByte(c >> 0 & 255)
    };
    #writeSignedShort = function(c) {
        0 > c && (c = 65535 + c);
        this.#writeByte(c >> 8 & 255);
        this.#writeByte(c >> 0 & 255)
    };
    #writeString = function(c, h) {
        for (var b = 0; b < h; b++)
            b < c.length ? this.#writeByte(c.charCodeAt(b)) : this.#writeByte(0)
    };    

    readAsArrayBuffer(buffer) {
        this.#readBytes = new Uint8Array(buffer);
        this.#readPosition = 0;
        let frames = [];
        while(!this.#eof() && "ILDA" == this.#readString(4)) {
            let frame = {};
            this.#readByte();
            this.#readByte();
            this.#readByte();
            frame.type = this.#readByte();
            frame.project = this.#readString(8);
            frame.owner = this.#readString(8);
            switch (frame.type) {
                case this.#FrameTypes.COORD_3D: //  0
                    frame.pointTotal = this.#readShort();
                    frame.index = this.#readShort();
                    frame.total = this.#readShort();
                    frame.head = this.#readByte();
                    this.#readByte(); //reserved
                    frame.points=[];
                    for (let point, l, i = 0; i < frame.pointTotal; i++){
                        point = {};
                        point.x = this.#readSignedShort();
                        point.y = this.#readSignedShort();
                        point.z = this.#readSignedShort();
                        l = this.#readShort();
                        point.colorIndex = l >> 0 & 127;
                        point.color = this.#currentPalette[Math.min(this.#currentPalette.length - 1, point.colorIndex)];
                        point.blanking = 16384 == (l & 16384);
                        point.last = 32768 == (l & 32768);
                        frame.points.push(point);
                    }
                    break;
                case this.#FrameTypes.COORD_2D: //  1
                    frame.pointTotal = this.#readShort();
                    frame.index = this.#readShort();
                    frame.total = this.#readShort();
                    frame.head = this.#readByte();
                    this.#readByte(); //reserved
                    frame.points=[];
                    for (let point, l, i = 0; i < frame.pointTotal; i++){
                        point = {},
                        point.x = this.#readSignedShort(),
                        point.y = this.#readSignedShort(),
                        point.z = 0;
                        l = this.#readShort(),
                        point.colorIndex = l >> 0 & 127,
                        point.color = this.#currentPalette[Math.min(this.#currentPalette.length - 1, point.colorIndex)];
                        point.blanking = 16384 == (l & 16384),
                        point.last = 32768 == (l & 32768),
                        frame.points.push(point);
                    }
                    break;
                case this.#FrameTypes.COLOR_TABLE: //  2
                    frame.pointTotal = this.#readShort();  //number of records
                    if (frame.pointTotal<2 || frame.pointTotal>256) throw new Error('Incorrect palette size. Must be between 2 and 255.')
                    frame.paletteNumber = arrayReader.readShort();
                    arrayReader.#readByte(); //skip
                    arrayReader.#readByte(); //skip
                    frame.head = arrayReader.#readByte();
                    this.#readByte(); //reserved
                    frame.colors = [];
                    for (let color, i = 0; i < frame.pointTotal; i++){
                        color = {},
                        color.r = this.#readByte(),
                        color.g = this.#readByte(),
                        color.b = this.#readByte(),
                        frame.colors.push(color);
                    }
                    this.#currentPalette = frame.colors;
                    break;
                case this.#FrameTypes.COORD_3D_TRUE_COL: //  4
                    frame.pointTotal = this.#readShort();
                    frame.index = this.#readShort();
                    frame.total = this.#readShort();
                    frame.head = this.#readByte();
                    frame.points=[];
                    this.#readByte(); //reserved
                    for (let point, l, i = 0; i < frame.pointTotal; i++){
                        point = {};
                        point.x = this.#readSignedShort();
                        point.y = this.#readSignedShort();
                        point.z = this.#readSignedShort();
                        l = this.#readByte();
                        point.blanking = l>>6 & 1;
                        point.last = l>>7 & 1;
                        point.color = {},
                        point.color.b = this.#readByte();
                        point.color.g = this.#readByte();
                        point.color.r = this.#readByte();
                        frame.points.push(point)
                    }
                    break;
                case this.#FrameTypes.COORD_2D_TRUE_COL: //  5
                    frame.pointTotal = this.#readShort();
                    frame.index = this.#readShort();
                    frame.total = this.#readShort();
                    frame.head = this.#readByte();
                    frame.points=[];
                    this.#readByte(); //reserved
                    for (let point, l, i = 0; i < frame.pointTotal; i++){
                        point = {};
                        point.x = this.#readSignedShort();
                        point.y = this.#readSignedShort();
                        point.z = 0;
                        l = this.#readByte();
                        point.blanking = l>>6 & 1;
                        point.last = l>>7 & 1;
                        point.color = {},
                        point.color.b = this.#readByte();
                        point.color.g = this.#readByte();
                        point.color.r = this.#readByte();
                        frame.points.push(point)
                    }
                    break;
            }
            frames.push(frame);
        }
        return {frames: frames, version: this.#Version};
    }

    writeAsArrayBuffer(ildaData){
        ildaData.frames.forEach(frame=>{
            this.#writeString("ILDA", 4);
            this.#writeByte(0); //reserved
            this.#writeByte(0); //reserved
            this.#writeByte(0); //reserved
            this.#writeByte(frame.type);
            this.#writeString(frame.project, 8);
            this.#writeString(frame.owner, 8);
            this.#writeShort(frame.points.length);
            this.#writeShort(frame.index);
            switch (frame.type) {
            case this.#FrameTypes.COORD_3D: //  0
                this.#writeShort(frame.total); //Should this not be calculated internally?
                this.#writeByte(frame.head);
                this.#writeByte(0); //reserved
                frame.points.forEach(point=>{
                    this.#writeSignedShort(point.x);
                    this.#writeSignedShort(point.y);
                    this.#writeSignedShort(point.z);
                    let d = 0 | point.colorIndex & 127;
                    point.blanking && (d |= 16384);
                    if (point.last || point == frame.points[frame.points.length - 1]){
                        d |= 32768;
                    }
                    this.#writeShort(d)
                });
                break;
            case this.#FrameTypes.COORD_2D: //  1
                this.#writeShort(frame.total);
                this.#writeByte(frame.head);
                this.#writeByte(0); //reserved
                frame.points.forEach(point=>{
                    this.#writeSignedShort(point.x);
                    this.#writeSignedShort(point.y);
                    let d = 0 | point.colorIndex & 127;
                    point.blanking && (d |= 16384);
                    if (point.last || point == frame.points[frame.points.length - 1])
                        d |= 32768;
                    this.#writeShort(d)
                });
                break;
            case this.#FrameTypes.COLOR_TABLE: //  2
                this.#writeShort(0);
                this.#writeByte(frame.head);
                this.#writeByte(0); //reserved
                frame.colors.forEach(color=>{
                    this.#writeByte(color.r),
                    this.#writeByte(color.g),
                    this.#writeByte(color.b);
                });
                break;
            case this.#FrameTypes.COORD_3D_TRUE_COL: //  4
                this.#writeShort(frame.total);
                this.#writeByte(frame.head);
                this.#writeByte(0); //reserved
                frame.points.forEach(point=>{
                    this.#writeSignedShort(point.x);
                    this.#writeSignedShort(point.y);
                    this.#writeSignedShort(point.z);
                    var d = 0;
                    point.blanking && (d |= 64);
                    if (point.last || point == frame.points[frame.points.length - 1])
                        d |= 128;
                    this.#writeByte(d);
                    this.#writeByte(point.color.b); //Note reverse order b,g,r rather than r,g,b  such humor.
                    this.#writeByte(point.color.g);
                    this.#writeByte(point.color.r);
                });
                break;
            case FrameTypes.COORD_2D_TRUE_COL: //  5
                this.#writeShort(frame.total);
                this.#writeByte(frame.head);
                this.#writeByte(0); //reserved
                frame.points.forEach(point=>{
                    this.#writeSignedShort(point.x);
                    this.#writeSignedShort(point.y);
                    var d = 0;
                    point.blanking && (d |= 64);
                    if (point.last || point == frame.points[frame.points.length - 1])
                        d |= 128;
                    this.#writeByte(d);
                    this.#writeByte(point.color.b); //Note reverse order b,g,r rather than r,g,b  such humor.
                    this.#writeByte(point.color.g);
                    this.#writeByte(point.color.r);
                });
                break;    
            }
        });
        let data = new Uint8Array(this.#writebytes);
        return data.buffer;
    }
    get defaultPalette(){
        return this.#DefaultPalette;
    }
}

ILDA = new ILDACLASS();
