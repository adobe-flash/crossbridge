
var array_rows:int;
var TestArray:Vector.<int>;
var size:int;
var datasizes:Vector.<int> = new Vector.<int>(4);
        

if (CONFIG::desktop) {
    var start = new Date();
    JGFrun(0);
    var elapsed = new Date() - start;
}
else { // mobile
    var start = getTimer();
    JGFrun(3);
    var elapsed = getTimer() - start;
}
if (JGFvalidate())
    print("metric time "+elapsed);
else
    print("validation failed");

/**
 * @author ayermolo
 */
function JGFrun(sizelocal:int):void
{
                datasizes[0]= 1000000;
                datasizes[1]= 5000000;
                datasizes[2]=25000000;
        datasizes[3]= 250000;
                
                JGFsetsize(sizelocal);
                JGFinitialise();
                NumHeapSort();
                JGFvalidate();

}

function JGFvalidate():Boolean {
        var error:Boolean;

        error = false;
        for (var i:int = 1; i < array_rows; i++) {
                error = (TestArray[i] < TestArray[i - 1]);
                if (error) {
                        print("Validation failed");
                        print("Item " + i + " = " + TestArray[i]);
                        print("Item " + (i - 1) + " = " + TestArray[i - 1]);
                        break;
                }
        }
        return !error;
}

function JGFsetsize(sizelocal:int):void
{
        size = sizelocal;
        
}

function JGFinitialise():void
{
        array_rows = datasizes[size];
        buildTestData();
}

function buildTestData():void
{
        TestArray = new Vector.<int>(array_rows);

        //var rndnum = new Random(1729); // Same seed every time.

        // Fill first TestArray with pseudo-random values.

        for (var i:int = 0; i < array_rows; i++)
        {
                var randomNumber:int = Math.floor( Math.random() * 1001 );
                TestArray[i]= randomNumber;
        }
}

/*
 * NumHeapSort
 *
 * Sorts one of the int arrays in the array of arrays. This routine performs
 * a heap sort on that array.
 */

function NumHeapSort():void {
        var temp:int; // Used to exchange elements.
        var top:int = array_rows - 1; // Last index in array. First is zero.

        // First, build a heap in the array. Sifting moves larger
        // values toward bottom of array.

        for (var i:int = Math.floor( top / 2 ); i > 0; --i)
        {
                NumSift(i, top);
        }
        // Repeatedly extract maximum from heap and place it at the
        // end of the array. When we get done, we'll have a sorted
        // array.

        for (var i:int = top; i > 0; --i)
        {
                NumSift(0, i);

                // Exchange bottom element with descending top.

                temp = TestArray[0];
                TestArray[0] = TestArray[i];
                TestArray[i] = temp;
        }
}
/*
 * NumSift
 *
 * Performs the sift operation on a numeric array, constructing a heap in
 * the array. Instructions from strsift: Pass this function: 1. The string
 * array # being sorted 2. Offsets within which to sort This performs the
 * core sort of the Heapsort algorithm
 */

function NumSift(min:int, max:int):void // Sort range offsets.
{
        var k:int; // Used for index arithmetic.
        var temp:int; // Used for exchange.

        while ((min + min) <= max) {
                k = min + min;
                if (k < max)
                if (TestArray[k] < TestArray[k + 1])
                                ++k;
                if (TestArray[min] < TestArray[k]) {
                        temp = TestArray[k];
                        TestArray[k] = TestArray[min];
                        TestArray[min] = temp;
                        min = k;
                } else
                        min = max + 1;
        }
}


