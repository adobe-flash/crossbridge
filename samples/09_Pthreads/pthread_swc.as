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
  import flash.display.Sprite;
  import flash.text.TextField;
  import flash.events.Event;
  import flash.utils.setTimeout;
  import sample.pthread.CModule;
  import sample.pthread.vfs.ISpecialFile;
  import sample.pthread.spawnThread;

  public class pthread_swc extends Sprite implements ISpecialFile
  {
    public function pthread_swc()
    {
      addEventListener(Event.ADDED_TO_STAGE, initCode);
      addEventListener(Event.ENTER_FRAME, enterFrame);
      // wait 2s and spawn a thread via interface exposed by swc
      setTimeout(function():void {
        spawnThread();
      }, 2000);
    }
 
    public function initCode(e:Event):void
    {
      CModule.rootSprite = this

      if(CModule.runningAsWorker()) {
        return;
      }

      CModule.vfs.console = this;
      //CModule.startBackground(this);
      CModule.startAsync(this);
    }

    public function write(fd:int, buf:int, nbyte:int, errno_ptr:int):int
    {
      var str:String = CModule.readString(buf, nbyte);
      trace(str);
      return nbyte;
    }

    public function read(fd:int, buf:int, nbyte:int, errno_ptr:int):int
    {
      return 0
    }

    public function fcntl(fd:int, com:int, data:int, errnoPtr:int):int
    {
      return 0
    }

    public function ioctl(fd:int, com:int, data:int, errnoPtr:int):int
    {
      return 0
    }

    public function enterFrame(e:Event):void
    {
      CModule.serviceUIRequests();
    }
  }
}
