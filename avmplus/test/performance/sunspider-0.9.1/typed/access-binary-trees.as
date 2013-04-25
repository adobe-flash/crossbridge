/* The Great Computer Language Shootout
   http://shootout.alioth.debian.org/
   contributed by Isaac Gouy */
package {

    public class TreeNode {
        private var left:TreeNode
        private var right:TreeNode;
        private var item:uint;

        public function TreeNode(left:TreeNode, right:TreeNode, item:uint){
            this.left = left;
            this.right = right;
            this.item = item;
        }

        public function itemCheck():uint {
            if (this.left==null) return this.item;
            else return this.item + this.left.itemCheck() - this.right.itemCheck();
        }

        public static function bottomUpTree(item:uint,depth:uint):TreeNode {
            if (depth>0){
                return new TreeNode(
                    bottomUpTree(2*item-1, depth-1)
                    ,bottomUpTree(2*item, depth-1)
                    ,item
                );
            }
            else {
                return new TreeNode(null,null,item);
            }
        }
    }
 
// main entry point for running testcase
public function runTest():void{
var ret:uint;

for ( var n:uint = 4; n <= 7; n += 1 ) {
    var minDepth:uint = 4;
    var maxDepth:uint = Math.max(minDepth + 2, n);
    var stretchDepth:uint = maxDepth + 1;
    
    var check:uint = TreeNode.bottomUpTree(0,stretchDepth).itemCheck();
    
    var longLivedTree:TreeNode = TreeNode.bottomUpTree(0,maxDepth);
    for (var depth:uint=minDepth; depth<=maxDepth; depth+=2){
        var iterations:uint = 1 << (maxDepth - depth + minDepth);

        check = 0;
        for (var i:uint=1; i<=iterations; i++){
            check += TreeNode.bottomUpTree(i,depth).itemCheck();
            check += TreeNode.bottomUpTree(-i,depth).itemCheck();
        }
    }

    ret = longLivedTree.itemCheck();
}
} //runTest()

// warm up run of testcase
runTest();
var startTime:uint = new Date().getTime();
runTest();
var time:uint = new Date().getTime() - startTime;
print("metric time " + time);

}