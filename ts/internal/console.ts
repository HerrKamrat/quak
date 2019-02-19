class ConsoleClass implements Console {
    log(...args:any[]):void{
        const msg = args.join("");
        __sys.print(msg);
    }
}

global.console = new ConsoleClass;
