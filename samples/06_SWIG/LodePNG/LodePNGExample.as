// Copyright (c) 2013 Adobe Systems Inc

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

package
{
    import sample.LodePNG.ram;
    
    import LodePNGLib;
    
    import sample.LodePNG.CModule;
    
    import flash.display.BitmapData;
    import flash.display.DisplayObject;
    import flash.display.Sprite;
    import flash.events.Event;
    import flash.events.MouseEvent;
    import flash.geom.Rectangle;
    import flash.net.FileReference;
    import flash.text.TextField;
    import flash.utils.ByteArray;
    import flash.utils.getTimer;
    
	/*
	 * LodePNGExample based on the LodePNG library from http://lodev.org/lodepng/
	 */
    public class LodePNGExample extends Sprite
    {
        private var canvas:Sprite = new Sprite();
        
        public function LodePNGExample()
        {
			// initialize the flascc library
            CModule.startAsync(this);
			
            drawShapes();
        }
        
        private function drawShapes():void 
        {
            addChild(canvas);
            canvas.x = 100;
            canvas.y = 100;
            
            canvas.graphics.beginFill(0x0000FF);
            canvas.graphics.drawRect(0, 0, 50, 50);
            canvas.graphics.endFill();
            
            canvas.graphics.beginFill(0xFF0000);
            canvas.graphics.drawRect(50, 50, 50, 50);
            canvas.graphics.endFill();
            
            canvas.graphics.beginFill(0x00FF00);
            canvas.graphics.drawRect(0, 50, 50, 50);
            canvas.graphics.endFill();
            
            canvas.graphics.beginFill(0x00FFFF);
            canvas.graphics.drawRect(50, 0, 50, 50);
            canvas.graphics.endFill();
            
            canvas.addEventListener(MouseEvent.CLICK, handleClick);
        }
        
        private function handleClick(e:MouseEvent):void 
        {
            // Take snapshot of some bitmap data
            var bd:BitmapData = capturePixels(canvas);
            var ba:ByteArray = convertToByteArray(bd);
            
            // Encode that data
            var png:ByteArray = encodePNG(ba, bd.width, bd.height);
            
            // Save it to disk
            var fr:FileReference = new FileReference();
            fr.save(png, "outputAS.png");
        }
        
        private function capturePixels(target:DisplayObject):BitmapData
        {
            var bd:BitmapData = new BitmapData(target.width, target.height);
            bd.draw(target);
            return bd;
        }
        
        private function convertToByteArray(bitmapData:BitmapData):ByteArray
        {
            // setup the bytearray of the pixels
            var pixels:Vector.<uint> = bitmapData.getVector(new Rectangle(0, 0, bitmapData.width, bitmapData.height));
            
            var ba:ByteArray = new ByteArray();
            
            // convert ARGB to RGBA
            for (var x:int = 0; x < pixels.length; x++){
                var pixel:uint = pixels[x];
                ba.writeUnsignedInt(convertARGBtoRGBA(pixel));
            }
            
            ba.position = 0;
            return ba;
        }
        
        private function convertARGBtoRGBA(argbValue:uint):uint
        {
            return (argbValue << 8) | (argbValue >>> (32-8));
        }
        
        private function encodePNG(bytes:ByteArray, width:int, height:int):ByteArray 
        {
            // setup the encoder pointer
            var encoderPtr:int = CModule.malloc(LodePNG_EncoderValue.size);
            var encoder:LodePNG_EncoderValue = new LodePNG_EncoderValue(sample.LodePNG.ram, encoderPtr);
            
            // setup the source image pointer
            var image:ByteArray = bytes;
            var imagePtr:int = CModule.malloc(image.length);
            CModule.writeBytes(imagePtr, image.length, image);
            
            // setup the output buffer pointers
            var bufferPtr:int = CModule.malloc(4);
            var buffersizePtr:int = CModule.malloc(4);
            
            // intialize the encoder
            LodePNGLib.LodePNG_Encoder_init(encoderPtr);
            encoder.settings.zlibsettings.windowSize = 2048;
            
            // add a comment
            LodePNGLib.LodePNG_Text_add(encoder.infoPng.text.addressOf(), "Comment", "Created with LodePNG");
            
            // encode the pixels
            LodePNGLib.LodePNG_Encoder_encode(encoderPtr, bufferPtr, buffersizePtr, imagePtr, width, height);
            
            // get the output
            var buffersize:int = CModule.read32(buffersizePtr);
            var buffer:ByteArray = new ByteArray();
            
            for (var i:int = 0; i < buffersize; i++){
                buffer.writeByte(CModule.read32(CModule.read32(bufferPtr) + i));
            }
            
            // cleanup malloc'd memory
            CModule.free(encoderPtr);
            CModule.free(bufferPtr);
            CModule.free(buffersizePtr);
            CModule.free(imagePtr);
            
            return buffer;
        }
    }
}
