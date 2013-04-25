package adobe.utils
{

/**
 * The XMLUI class enables communication with SWF files that are used as custom user interfaces for the Flash 
 * authoring tool's extensibility features. 
 *
 * <p>Macromedia Flash MX 2004 and Macromedia Flash MX Professional 2004 and later versions come with several extensibility 
 * features including Behaviors, Commands (JavaScript API), Effects, and Tools. With these features, advanced 
 * users can extend or automate the authoring tool's capabilities. The XML-to-UI engine works with each of 
 * these extensibility features to create dialog boxes that the user sees if the extension either requires or 
 * accepts parameters. You can define dialog boxes by using XML tags or by creating a SWF file to display. 
 * The XMLUI object provides a mechanism by which an advanced user can communicate with a SWF file used in 
 * such a manner.</p>
 * 
 * @internal includeExample examples\XMLUIExample.as -noswf
 *
 * @playerversion Flash 9
 * @langversion 3.0
 * @helpid
 * @refpath 
 * @keyword XMLUI
 */

public final class XMLUI
{
	/**
	*
	* Retrieves the value of the specified property of the current XMLUI dialog box.
	*
	* @param name The name of the XMLUI property to retrieve.
	* 
	* @return The value of the property.
	*
	* @playerversion Flash 9
	* @langversion 3.0
	* @helpid
	* @refpath 
	* @keyword XMLUI, XMLUI.getProperty(), getProperty()
	*/
    public static function getProperty(name:String):String {
      trace("XMLUI.getProperty()");
    }
	/**
	*
	* Modifies the value of the specified property of the current XMLUI dialog. 
	* 
	* @param name The name of the XMLUI property to modify.
	* @param value The value to which the specified property will be set.
	*
	* @playerversion Flash 9
	* @langversion 3.0
	* @helpid
	* @refpath 
	* @keyword XMLUI, XMLUI.setProperty(), setProperty()
	*/
    public static function setProperty(name:String,value:String):void {
      trace("XMLUI.setProperty()");
    }
	/**
	*
	* Makes the current XMLUI dialog box close with an "accept" state. This is identical to the user 
	* clicking the OK button. 
	* 
	*
	* @playerversion Flash 9
	* @langversion 3.0
	* @helpid
	* @refpath 
	* @keyword XMLUI, XMLUI.accept(), accept()
	*/
    public static function accept():void {
      trace("XMLUI.accept()");
    }
	/**
	*
	* Makes the current XMLUI dialog box close with a "cancel" state. This is identical to 
	* the user clicking the Cancel button.
	* 
	* @playerversion Flash 9
	* @langversion 3.0
	* @helpid
	* @refpath 
	* @keyword XMLUI, XMLUI.cancel(), cancel()
	*/
    public static function cancel():void {
      trace("XMLUI.cancel()");
    }
}

/*
 * [ggrossman 04/07/05] API SCRUB
 * - This class is only available to the Flash Player when executing
 *   within a Macromedia tools product.
 * - This class is now marked _final_.
 * - Renamed _XMLUI.get_ -> _XMLUI.getProperty_
 * - Renamed _XMLUI.set_ -> _XMLUI.setProperty_
 * - The name _XMLUI_ is odd, but this class is only available
 *   to the player when embedded in an MM tool, and so doesn't merit
 *   changing.
 *
 * - [srahim 04/05/05] Doc scrub
 */

}


