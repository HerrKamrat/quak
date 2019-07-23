console.log("Running main!");



const cmds = new Uint8Array(5 * 4 * 6);
const view = new DataView(cmds.buffer); 

const events = new Uint8Array(5 * 3);
const eview = new DataView(events.buffer); 

let offset = 0;

function clear(){       
    view.setUint8(offset, 1);
    offset += 5 * 4;
}

function drawRect(x:number, y:number, w:number, h:number){       
    view.setUint8(offset, 2);
    view.setInt32(offset + 4 * 1, x, true);
    view.setInt32(offset + 4 * 2, y, true);
    view.setInt32(offset + 4 * 3, w, true);
    view.setInt32(offset + 4 * 4, h, true);

    offset += 5 * 4;
}

function setColor(c:number){       
    view.setUint8(offset, 3);
    view.setInt32(offset + 4 * 4, c, true);
    offset += 5 * 4;
}

function commit(){
    if(offset >= view.byteLength){
        return;
    }

    view.setUint8(offset, 3);
    offset += 5 * 4;
}

clear();
setColor(0xffff00ffff)
drawRect(10,10,100,200);
setColor(0xff00ffffff)
//drawRect(100,100,100,200);


for(let i = 0; i<cmds.length; ++i){
    console.log("cmds: " + cmds[i])
}

let wait = false;
while(__gfx.poll(events, wait) != 1){
    console.log("tick:" + new Date);
    __gfx.commit(cmds, true);
}


/*
	int flag = 0;
    while ((flag = window.poll()) != 1) {
        if (flag == 2)
            window.commit(cmds);
        }
*/

throw new Error("ERR:D");


/*

"use strict";
__sys.print(":D" + global.console);
/*__sys.print("fs" + Object.keys(fs));
var v = __sys.print(1, 2.2, "tre")

__sys.print("print: " + v)

v = fs.resolve("/a/c/../s.xj")
__sys.print("resolve: " + v)

try{
    v = fs.read(v)
    __sys.print("read: " + v)
        fs.read(1, 2.2, "tre")
}catch(e){
    __sys.print("err: " + e)
}
var x = 10;
var y = 20;
var z = x * y;
z;


__sys.resolve = function (moduleId, parentId) {
    if (!moduleId.endsWith(".js")) {
        moduleId += ".js"
    }
    var parts = parentId.split("/");
    parts.splice(-1, 1, moduleId);
    var path = parts.join("/");

    var resolvedId = fs.resolve(path);
    __sys.print("resolve, moduleId: " + moduleId + ", parentId: " + parentId + ", resolvedId: " + resolvedId + ", path: " + path);
    return resolvedId;
}
__sys.load = function (moduleId, b, c) {
    __sys.print("load, moduleId: " + moduleId + ", b: " + Object.keys(b) + ", c: " + Object.keys(c));
    var moduleSource = fs.read(moduleId);
    return moduleSource;
}

    (function () {
        return {
            resolve: function (moduleId, parentId) {
                if (!moduleId.endsWith(".js")) {
                    moduleId += ".js"
                }
                var parts = parentId.split("/");
                parts.splice(-1, 1, moduleId);
                var path = parts.join("/");

                var resolvedId = fs.resolve(path);
                __sys.print("resolve, moduleId: " + moduleId + ", parentId: " + parentId + ", resolvedId: " + resolvedId + ", path: " + path);
                return resolvedId;
            },
            load: function (moduleId, b, c) {
                __sys.print("load, moduleId: " + moduleId + ", b: " + Object.keys(b) + ", c: " + Object.keys(c));
                var moduleSource = fs.read(moduleId);
                return moduleSource;
            }
        }
    })();



var cmds = new Uint32Array(5 * 3);
var t16 = new Uint16Array(cmds.buffer); 
var t8 = new Uint8Array(cmds.buffer); 
console.log(cmds[0]);
__sys.test(cmds)
console.log(cmds[0]);

while(__gfx.poll(cmds, 1) != 1){
    console.log("tick:" + t8[0]);
    __gfx.commit(cmds, 0, true);
}



	int flag = 0;
    while ((flag = window.poll()) != 1) {
        if (flag == 2)
            window.commit(cmds);
        }


throw new Error("ERR:D");



*/