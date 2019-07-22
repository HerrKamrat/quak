class Console {
    log(...args:any[]):void{
        const msg = args.join("");
        __sys.print(msg);
    }
}


/** Declare global instance */
interface Global { console:Console; }
declare const console:Console

global.console = new Console;
