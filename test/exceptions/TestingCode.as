import flash.events.*;
import flash.net.*;
import flash.system.*;
import flash.utils.*;

private var qaLoader:URLLoader = null;
private var qaPerfString:String = "PERF_";
private var oldIrrelevantNumber:Number = 0;
private var newIrrelevantNumber:Number = 0;

// If/when we test on mobile, these will need to be set dynamically.
// Maybe we can generate this file on the fly right before compiling,
// filled in with the needed values, like we did for Flex.
private var server:String = "localhost";
private var port:String = "31671";


public function sayHello():void{
	trace("hello");
}

/**
* Sends info. to the server.  Pass in an array of objects.  Each object
* needs to have a field called "key" and a field called "value".
**/
public function qaSendInfo( key:String, value:String, done:Boolean ):void{

	// Only on Windows, we have to add something irrelevant, but different, to the URL or else the
	// player won't send it.
	// Sometimes the number is the same (e.g. after uploading the last result, getting a complete
	// event, then sending another request telling the server everything is done) so loop until it's 
	// different.
	do{
		newIrrelevantNumber = new Date().getTime();
	}while( newIrrelevantNumber == oldIrrelevantNumber );
	
	var req:URLRequest = new URLRequest( "http://" + server + ":" + port + "?irrelevantNumber=" + newIrrelevantNumber.toString() );
 	var headers:Array = new Array();    
 	var i:int;

	if( qaLoader == null ){
		qaLoader = new URLLoader();
		qaLoader.addEventListener( flash.events.IOErrorEvent.IO_ERROR, QAHandleError );
		qaLoader.addEventListener( flash.events.SecurityErrorEvent.SECURITY_ERROR, QAHandleError );
		qaLoader.addEventListener( flash.events.Event.COMPLETE, qaHandleComplete );
    	}

	// When the final data has been successfully sent, we'll send a final "complete" event to the server.
	if( done ){
		qaLoader.addEventListener( flash.events.Event.COMPLETE, qaHandleCompleteDone );
	}

	// Create the array of URLRequestHeaders to send.
	var key:String = escape( key );
        var value:String = value;
        headers.push( new URLRequestHeader( key, value ) );
    
	req.requestHeaders = headers;           
	req.method = "POST";
	req.data="";
	trace( "request url: " + req.url + ", key: " + key + ", value: " + value );
	qaLoader.load( req );
	oldIrrelevantNumber = newIrrelevantNumber;
}

private function QAHandleError( e:Event ):void{
	trace("QAHandleError");
}

private function qaHandleComplete( e:Event ):void{
	trace("QAHandleComplete");
}

// When the final data has been successfully sent, we'll send a final "complete" event to the server.
private function qaHandleCompleteDone( e:Event ):void{
    qaSendInfo( [ {key:qaPerfString + "TestingComplete", value:1}], true );
	qaLoader.removeEventListener( flash.events.Event.COMPLETE, qaHandleCompleteDone );
}
