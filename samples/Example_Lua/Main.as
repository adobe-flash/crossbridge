package {
import flash.display.SimpleButton;
import flash.display.Sprite;
import flash.events.Event;
import flash.text.TextField;
import flash.text.TextFormat;
import flash.utils.ByteArray;
import flash.utils.getTimer;

import sample.lua.CModule;
import sample.lua.vfs.ISpecialFile;

[SWF(width="800", height="600", backgroundColor="#333333", frameRate="60")] class Main extends Sprite {

    internal var luastate:int;

    private var inbox:TextField;
    private var outbox:TextField;
    private var runtimelabel:TextField;
    private var button:SimpleButton;

    public function Main() {
        addEventListener(Event.ADDED_TO_STAGE, appInit);
    }

    internal function appInit(event:Event):void {
        removeEventListener(Event.ADDED_TO_STAGE, appInit);

        runtimelabel = getTextField(0, 0, 800, 20);
        inbox = getTextField(0, 20, 800, 280);
        outbox = getTextField(0, 300, 800, 300);

        // TODO: button = new SimpleButton();

        CModule.rootSprite = this
        CModule.vfs.console = this
        CModule.startAsync(this)

        runScript(null)
    }

    private function getTextField(x:int, y:int, w:int, h:int):TextField {
        var result:TextField = new TextField();
        result.width = w;
        result.height = h;
        result.x = x;
        result.y = y;
        result.multiline = true;
        result.selectable = true;
        result.wordWrap = true;
        addChild(result);
        const tf:TextFormat = new TextFormat("Arial", 12, 0x000000);
        result.defaultTextFormat = tf;
        return result;
    }

    internal function runScript(event:Event):void {
        var err:int = 0
        outbox.text = ""
        luastate = Lua.luaL_newstate()

        Lua.luaL_openlibs(luastate)
        err = Lua.luaL_loadstring(luastate, inbox.text)
        if (err) {
            Lua.lua_settop(luastate, -2)
            Lua.lua_close(luastate)
            output("Can't parse script: " + err)
            return
        }

        var runtime:int = getTimer()
        err = Lua.lua_pcallk(luastate, 0, Lua.LUA_MULTRET, 0, 0, null)
        runtime = getTimer() - runtime
        runtimelabel.text = "Script time: " + runtime + "ms"
        /* + " final stack depth: " + Lua.lua_gettop(luastate) */

        if (err) {
            output("Failed to run script: " + Lua.lua_tolstring(luastate, -1, 0))
        } else {
            var result:Number = Lua.lua_tonumberx(luastate, -1, 0)
            output("Script returned: " + result)
        }

        Lua.lua_settop(luastate, -2)
        Lua.lua_close(luastate)
    }

    public function output(s:String):void {
        outbox.text += s
        trace(s)
    }

    /**
     * The PlayerKernel implementation will use this function to handle
     * C IO write requests to the file "/dev/tty" (e.g. output from
     * printf will pass through this function). See the ISpecialFile
     * documentation for more information about the arguments and return value.
     */
    public function write(fd:int, bufPtr:int, nbyte:int, errnoPtr:int):int {
        var str:String = CModule.readString(bufPtr, nbyte)
        output(str)
        return nbyte
    }

    /**
     * The PlayerKernel implementation will use this function to handle
     * C IO read requests to the file "/dev/tty" (e.g. reads from stdin
     * will expect this function to provide the data). See the ISpecialFile
     * documentation for more information about the arguments and return value.
     */
    public function read(fd:int, bufPtr:int, nbyte:int, errnoPtr:int):int {
        return 0
    }

    /**
     * The PlayerKernel implementation will use this function to handle
     * C fcntl requests to the file "/dev/tty"
     * See the ISpecialFile documentation for more information about the
     * arguments and return value.
     */
    public function fcntl(fd:int, com:int, data:int, errnoPtr:int):int {
        return 0
    }

    /**
     * The PlayerKernel implementation will use this function to handle
     * C ioctl requests to the file "/dev/tty"
     * See the ISpecialFile documentation for more information about the
     * arguments and return value.
     */
    public function ioctl(fd:int, com:int, data:int, errnoPtr:int):int {
        return 0;
    }
}
}
