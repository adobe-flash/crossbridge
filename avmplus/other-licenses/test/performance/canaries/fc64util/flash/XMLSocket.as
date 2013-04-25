package flash.net
{
	import flash.events.EventDispatcher;

//
// XMLSocket
//

/**
 * Dispatched when the server closes the socket connection. 
 * The <code>close</code> event is dispatched only when the server
 * closes the connection; it is not dispatched when you call the <code>XMLSocket.close()</code> method.
 * @eventType flash.events.Event.CLOSE
 * @playerversion Flash 9
 * @langversion 3.0
 */
[Event(name="close", type="flash.events.Event")]



/**
 * Dispatched after a successful call to the <code>XMLSocket.connect()</code> method.
 * @eventType flash.events.Event.CONNECT
 * @playerversion Flash 9
 * @langversion 3.0
 */
[Event(name="connect", type="flash.events.Event")]



/**
 * Dispatched after raw data is sent or received.
 * @eventType flash.events.DataEvent.DATA
 * @playerversion Flash 9
 * @langversion 3.0
 */
[Event(name="data", type="flash.events.DataEvent")]



/**
 * Dispatched when an input/output error occurs that causes a send or receive operation to fail. 
 * @eventType flash.events.IOErrorEvent.IO_ERROR
 * @playerversion Flash 9
 * @langversion 3.0
 */
[Event(name="ioError", type="flash.events.IOErrorEvent")]



/**
 * Dispatched if a call to the <code>XMLSocket.connect()</code> method
 * attempts to connect either to a server outside the caller's security sandbox or to a port lower than 1024.
 * @see XMLSocket#connect()
 * @eventType flash.events.SecurityErrorEvent.SECURITY_ERROR
 * @playerversion Flash 9
 * @langversion 3.0
 */
[Event(name="securityError", type="flash.events.SecurityErrorEvent")]



/**
 *
 * The XMLSocket class implements client sockets that let the computer that is running Flash Player communicate
 * with a server computer identified by an IP address or domain name. The XMLSocket class is useful for
 * client-server applications that require low latency, such as real-time chat systems. A traditional 
 * HTTP-based chat solution frequently polls the server and downloads new messages using an HTTP 
 * request. In contrast, an XMLSocket chat solution maintains an open connection to the server, which
 * lets the server immediately send incoming messages without a request from the client.
 * To use the XMLSocket class, the server computer must run a daemon that understands the protocol used
 * by the XMLSocket class. The protocol is described in the following list: 
 * <ul>
 *   <li>XML messages are sent over a full-duplex TCP/IP stream socket connection.</li>
 *   <li>Each XML message is a complete XML document, terminated by a zero (0) byte.</li>
 *   <li>An unlimited number of XML messages can be sent and received over a single XMLSocket 
 * connection.</li>
 * </ul>
 * <p>The <code>XMLSocket.connect()</code> method can connect only to TCP port numbers greater than or equal to
 * 1024. One consequence of this restriction is that the server daemons that communicate with the 
 * XMLSocket object must also be assigned to port numbers greater than or equal to 1024. Port numbers
 * below 1024 are often used by system services such as FTP, Telnet, and HTTP, so XMLSocket objects are
 * barred from these ports for security reasons. The port number restriction limits the possibility that
 * these resources will be inappropriately accessed and abused.</p>
 * 
 * <p>Setting up a server to communicate with the XMLSocket object can be challenging. If your application
 * does not require real-time interactivity, use the URLLoader class instead of the XMLSocket class. 
 * To use the methods of the XMLSocket class, you must first use the constructor, <code>new XMLSocket</code>,
 * to create an XMLSocket object.</p>
 *
 * <p>When you use this class, consider the Flash Player security model:</p>
 * 
 * <ul>
 *   <li>Sending or receiving data is not allowed if the calling SWF file is in the local-with-file-system 
 * sandbox and the target resource is from a network sandbox.</li>
 *   <li>Sending or receiving data is also not allowed if the calling SWF file is from a network sandbox and the 
 * target resource is local.</li>
 *   <li>The calling SWF file and the network resource being accessed must be in 
 * exactly the same domain. For example, a SWF file at www.adobe.com can send or receive data only from 
 * sources that are also at www.adobe.com.</li>
 *   <li>Websites can permit cross-domain access to a resource through a cross-domain policy file.</li>
 * </ul>
 * 
 * <p>For more information, see the following:</p>
 * 
 * <ul>
 * 
 * <li class="flexonly">The <a target="mm_external" href="http://www.adobe.com/go/flex3_progAS3_security">security chapter</a>
 * in the <em>Programming ActionScript 3.0</em> book and the latest comments on LiveDocs</li>
 * <li class="flashonly">The <a target="mm_external" href="http://www.adobe.com/go/flashCS3_progAS3_security">security chapter</a>
 * in the <em>Programming ActionScript 3.0</em> book and the latest comments on LiveDocs</li>
 * 
 * <li>The <a target="mm_external" href="http://www.adobe.com/go/fp9_0_security">Flash Player 9 Security white paper</a></li>
 * <li>The Flash Player Developer Center Topic: <a href="http://www.adobe.com/devnet/flashplayer/security.html" target="external">Security</a></li>
 * </ul>
 * 
 * @includeExample examples\XMLSocketExample.as -noswf
 * 
 * @playerversion Flash 9
 * @langversion 3.0
 * 
 * @helpid
 * @refpath
 * @keyword XMLsocket, XMLsocket object, built-in class
 * @see flash.net.URLLoader#load()
 * @see flash.net.URLLoader
 */

public class XMLSocket extends EventDispatcher
{
	
	/**
	 * Creates a new XMLSocket object. The XMLSocket object is not initially connected to any server. You must call the
	 * <code>XMLSocket.connect()</code> method to connect the object to a server.
	 *
	 * @param host A fully qualified DNS domain name or an IP address in the form 
	 * <em>aaa.bbb.ccc.ddd</em>. You can also specify <code>null</code> to connect to the host server
	 * on which the SWF file resides. If the SWF file issuing this call is running in a web browser,
	 * <code>host</code> must be in the same domain as the SWF file.
	 * 
	 * @param port The TCP port number on the host used to establish a connection. The port
	 * number must be 1024 or greater, unless a policy file is being used.
     *
     * @see #connect()
	 * @playerversion Flash 9
     * @langversion 3.0
	 */
	public function XMLSocket(host:String=null, port:int=0)
    {
        if (host != null) {
            connect(host, port);
        }
    }



	/**
	 * Establishes a connection to the specified Internet host using the specified TCP port. By default
	 * you can only connect to port 1024 or higher, unless you are using a policy file.
	 *
	 * <p>If you specify <code>null</code> for the <code>host</code> parameter, the host 
	 * contacted is the one where the SWF file calling <code>XMLSocket.connect()</code> resides. 
	 * For example, if the SWF file was downloaded from www.adobe.com, specifying <code>null</code>
	 * for the host parameter is the same as entering the IP address for www.adobe.com.</p>
	 * 
	 * 
     * <p product="flash">In SWF files running in a version of the player earlier than Flash Player 7, 
	 * <code>host</code> must be in the same superdomain as the SWF file that is issuing this
	 * call. For example, a SWF file at www.adobe.com can send or receive variables from a SWF file at 
	 * store.adobe.com because both files are in the same superdomain of adobe.com.</p>
	 * 
	 * <p>In SWF files of any version running in Flash Player 7 or later,
	 * <code>host</code> must be in exactly the same domain. For example, a SWF file at www.adobe.com
	 * that is published for Flash Player 5, but is running in Flash Player 7 or later can send or receive 
	 * variables only from SWF files that are also at www.adobe.com. If you want to send or receive variables
	 * from a different domain, you can place a <em>cross-domain policy file</em> on the server hosting
	 * the SWF file that is being accessed.</p>
     * 
	 *
	 * <p product="flex">By default, the value you pass for <code>host</code> must be in exactly
	 * the same domain and the value you pass for <code>port</code> must be 1024 or above. 
     * For example, a SWF file at www.adobe.com can send or receive variables only from SWF files that are also 
     * at www.adobe.com. If you want to connect to a socket on a different host than the one from which 
     * the connecting SWF file was served, or if you want to connect to a port lower than 1024 on any host, 
     * you must obtain an <code>xmlsocket:</code> policy file from the host to which you are 
     * connecting. </p>
	 * 
	 * <p>You can prevent a SWF file from using this method by setting the 
	 * <code>allowNetworking</code> parameter of the the <code>object</code> and <code>embed</code> 
	 * tags in the HTML page that contains the SWF content.</p>
	 * 
	 * <p>For more information, see the following:</p>
     * <ul>
     * 
     * <li class="flexonly">The <a target="mm_external" href="http://www.adobe.com/go/flex3_progAS3_security">security chapter</a>
     * in the <em>Programming ActionScript 3.0</em> book and the latest comments on LiveDocs</li>
     * <li class="flashonly">The <a target="mm_external" href="http://www.adobe.com/go/flashCS3_progAS3_security">security chapter</a>
     * in the <em>Programming ActionScript 3.0</em> book and the latest comments on LiveDocs</li>
     * 
     * <li>The <a target="mm_external" href="http://www.adobe.com/go/fp9_0_security">Flash Player 9 Security white paper</a></li>
     * <li>The Flash Player Developer Center Topic: <a href="http://www.adobe.com/devnet/flashplayer/security.html" target="external">Security</a></li>
     * </ul>
	 * 
	 * @playerversion Flash 9
	 * @langversion 3.0
	 *
	 * @param host A fully qualified DNS domain name or an IP address in the form 
	 * <em>aaa.bbb.ccc.ddd</em>. You can also specify <code>null</code> to connect to the host server
	 * on which the SWF file resides. If the SWF file issuing this call is running in a web browser,
	 * <code>host</code> must be in the same domain as the SWF file.
	 * 
	 * @param port The TCP port number on the host used to establish a connection. The port
	 * number must be 1024 or greater, unless a policy file is being used.
	 *
	 * @oldexample The following example uses <code>XMLSocket.connect()</code> to connect to the host
	 * where the SWF file resides and uses <code>trace</code> to display the return value 
	 * indicating the success or failure of the connection:
	 * <pre>
	 * var socket:XMLSocket = new XMLSocket()
	 * socket.onConnect = function (success:Boolean) {
	 *   if (success) {
	 *     trace ("Connection succeeded!")
	 *   } else {
	 *     trace ("Connection failed!")
	 *   }
	 * }
	 * if (!socket.connect(null, 2000)) {
	 *   trace ("Connection failed!")
	 * }
	 * </pre>
	 *
	 * @see flash.events.Event#CONNECT
	 *
	 * @helpid
	 * @refpath
	 * @keyword xmlsocket.connect, connect
	 *
	 * @throws SecurityError Local untrusted SWF files may not communicate with
	 * the Internet. This may be worked around by reclassifying this SWF file
	 * as local-with-networking or trusted.
	 * @throws SecurityError You may not specify a socket port higher than
	 * 65535.
	 * @event securityError SecurityErrorEvent A connect operation attempted
	 * to connect to a server outside the caller's security sandbox, and/or
	 * to a port lower than 1024.  Either problem may be worked around using
	 * an XMLSocket policy file on the server.
	 * @event data DataEvent Dispatched when raw data has been received.
	 * @event connect Event Dispatched when network connection has been established. 
	 */
	public function connect(host:String, port:int):void {
      trace("XMLSocket.connect()");
    }



	/**
	 * Converts the XML object or data specified in the <code>object</code> parameter
	 * to a string and transmits it to the server, followed by a zero (0) byte. If <code>object</code> is an XML object, the string is 
	 * the XML textual representation of the XML object. The 
	 * send operation is asynchronous; it returns immediately, but the data may be transmitted at a 
	 * later time. The <code>XMLSocket.send()</code> method does not return a value indicating whether
	 * the data was successfully transmitted.
	 * 
	 * <p>If you do not connect the XMLSocket object to the server using 
	 * <code>XMLSocket.connect()</code>), the <code>XMLSocket.send()</code> 
	 * operation fails.</p>
	 *
	 * @playerversion Flash 9
	 * @langversion 3.0
	 *
	 *
	 * @param object An XML object or other data to transmit to the server.
	 *
	 * @throws IOError The XMLSocket object is not connected to the server.
	 *
	 * 
	 * @oldexample The following example shows how you could specify a user name and password to send
	 * the XML object <code>my_xml</code> to the server:
	 * <pre>
	 * <code>var myXMLSocket:XMLSocket = new XMLSocket();</code>
	 * <code>var my_xml:XML = new XML();</code>
	 * <code>var myLogin:XMLNode = my_xml.createElement("login");</code>
	 * <code>myLogin.attributes.username = usernameTextField;</code>
	 * <code>myLogin.attributes.password = passwordTextField;</code>
	 * <code>my_xml.appendChild(myLogin);</code>
	 * <code>myXMLSocket.send(my_xml);</code>
	 * </pre>
	 *
	 * @see #connect()
	 *
	 * @helpid
	 * @refpath
	 * @keyword xmlsocket.send, send
	 */
	public function send(object:*):void {
      trace("XMLSocket.send()");
    }



	/**
	 * Closes the connection specified by the XMLSocket object. 
	 * The <code>close</code> event is dispatched only when the server
	 * closes the connection; it is not dispatched when you call the <code>close()</code> method.
	 *
	 * @playerversion Flash 9
	 * @langversion 3.0
	 *
	 * @oldexample The following simple example creates an XMLSocket object, attempts to connect to the
	 * server, and then closes the connection.
	 * <pre>
	 * var socket:XMLSocket = new XMLSocket();
	 * socket.connect(null, 2000);
	 * socket.close();
	 * </pre>
	 * 
	 * @see #connect()
	 *
	 * @helpid
	 * @refpath
	 * @keyword xmlsocket.close, close
	 */
	public function close():void {
      trace("XMLSocket.close()");
    }



	/**
	 * Indicates whether this XMLSocket object is currently connected. You can also check 
     * whether the connection succeeded by registering for the <code>connect</code> 
     * event and <code>ioError</code> event.
	 * 
	 * @playerversion Flash 9
	 * @langversion 3.0
     * @see #event:connect
     * @see #event:ioError
	 */
	public function get connected():Boolean {
      trace("XMLSocket.get connected()");
    }
}

/*
 * [ggrossman 04/07/05] API SCRUB
 *
 * - Added a constructor, which has optional arguments for _host_
 *   and _port_.  If specified, _connect_ is called with these
 *   arguments.
 *
 * - The _port_ parameter of _connect_ is now type _uint_, not _int_.
 *
 * - Added _get connected_ accessor.
 *
 * - The _xml_ event has been removed.  Instead, the _data_ event alone will be
 *   dispatched.  (Originally, the _data_ event was dispatched, and that
 *   failing, the _xml_ event was dispatched.)
 *
 * - [srahim 04/05/05] Doc scrub
 */

}
