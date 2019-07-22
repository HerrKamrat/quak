class Window {
    poll():void {};
}


interface Global {
    window:Window;
}

/** Declare global instance */
interface Global { window:Window; }
declare const window:Window

global.window = new Window
