﻿/* The MIT License (MIT) Copyright 2014 Andras Csizmadia (www.vpmedia.eu) Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions: The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */package  {    import flash.display.Sprite;    import flash.text.TextField;    import flash.events.*;    import performancetests.*;    import crossbridge.*;    import com.gskinner.utils.PerformanceTest;        [SWF(backgroundColor="0xFFFFFF", frameRate="60", width="800", height="600")]    public class Main extends Sprite {                private var textField:TextField;                public function Main() {            addEventListener(Event.ADDED_TO_STAGE, onAdded);        }                protected function onAdded(event:Event):void {            removeEventListener(Event.ADDED_TO_STAGE, onAdded);            textField = new TextField();            addChild(textField);            textField.width = 800;            textField.height = 600;            var perfTest:PerformanceTest = PerformanceTest.getInstance();            perfTest.out = onLogEvent;            perfTest.testSuite(new PassDataPT());            perfTest.testSuite(new FibonacciPT());            //perfTest.testFunction(test, 5, "testFunction", "this is a quick test of testFunction");        }                private function onLogEvent(message:*):void {            textField.appendText(message + "\n");            //textField.dispatchEvent(new Event(Event.CHANGE));            trace(message);        }                /*protected function test():void {            for (var i:int=0; i<100000; i++) {                Math.random();            }        }*/            }    }