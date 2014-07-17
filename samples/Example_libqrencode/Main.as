package {
import flash.display.Bitmap;
import flash.display.BitmapData;
import flash.events.Event;
import flash.text.TextField;

import sample.qrencode.CModule;

import flash.display.Sprite;

class Main extends Sprite {

    internal var uic:Sprite;
    internal var bm:Bitmap;

    private var srctext:TextField;

    public function Main() {
        addEventListener(Event.ADDED_TO_STAGE, appInit);
    }

    internal function appInit(event:Event):void {
        removeEventListener(Event.ADDED_TO_STAGE, appInit);

        CModule.rootSprite = this
        //CModule.vfs.console = this
        CModule.startAsync(this)

        uic = new Sprite()
        addChild(uic)

        runScript(null)
    }

    internal function runScript(event:Event):void {
        var qrcode:QRcode = new QRcode()
        qrcode.swigCPtr = QREncode.QRcode_encodeString(srctext.text, vers.value, ecc.value, QREncode.QR_MODE_8, 1)

        if (qrcode.swigCPtr == 0 || qrcode.width <= 0) {
            return
        }

        var bmd:BitmapData = new BitmapData(qrcode.width, qrcode.width, false, 0xAAAAAA)
        var d:int = qrcode.data
        for (var y:int = 0; y < qrcode.width; y++) {
            for (var x:int = 0; x < qrcode.width; x++) {
                bmd.setPixel(x, y, CModule.read8(d++) & 1 ? 0x0 : 0xFFFFFF)
            }
        }

        if (bm) {
            uic.removeChild(bm)
        }
        bm = new Bitmap(bmd)
        bm.smoothing = false
        bm.scaleX = bm.scaleY = 2
        uic.addChild(bm)
        // initTesting();
    }

    private function errorCorrectionCodeName(val:String):String {
        switch (val) {
            case "0":
                return "EC Level L"
            case "1":
                return "EC Level M"
            case "2":
                return "EC Level Q"
            case "3":
                return "EC Level H"
        }
        return null
    }

    private function versionTip(val:String):String {
        return "Version: " + val
    }

    public function output(s:String):void {
    }

    public function write(fd:int, buf:int, nbyte:int, errno_ptr:int):int {
        var str:String = CModule.readString(buf, nbyte);
        output(str);
        return nbyte;
    }

    public function read(fd:int, buf:int, nbyte:int, errno_ptr:int):int {
        return 0
    }
}
}