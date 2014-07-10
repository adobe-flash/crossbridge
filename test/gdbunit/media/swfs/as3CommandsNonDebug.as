package
{
    public class ASFileNonDebug
    {

        public static function getNumberNonDebug(i:Number = 0):Number 
        {
            var k:Number = i;
		
            for (var j:Number = 0; j < 3; j++)
            {
                i += getNumberHelperNonDebug(j, j);
            }
        
            return i;
        }

        private static function getNumberHelperNonDebug(i:Number, j:Number):Number
        {
            return i + incrementNonDebug(j);
        }
        
        private static function incrementNonDebug(k:Number):Number 
        {
        	var incrementLocalVariable:String = "Go Canucks!";
        	return k++;
        }
    }
    
}