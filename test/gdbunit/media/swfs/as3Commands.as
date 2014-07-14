package
{
    public class ASFile
    {

        public static function getNumber(i:Number = 0):Number 
        {
            var k:Number = i;
		
            for (var j:Number = 0; j < 3; j++)
            {
                i += getNumberHelper(j, j);
            }
        
            return i;
        }

        private static function getNumberHelper(i:Number, j:Number):Number
        {
            return i + increment(j);
        }
        
        private static function increment(k:Number):Number 
        {
        	var incrementLocalVariable:String = "Go Canucks!";
        	var exactly80Characters:String = "01234567890123456789012345678901234567890123456789012345678901234567890123456789";
        	var longLocalVariableString:String = "This string is longer than eighty characters and should be properly truncated by gdb";
        	var anotherString:String = "test";
        	return k++;
        }
    }
    
}