package 
{ 
    import flash.display.Loader; 
    import flash.display.Sprite; 
    import flash.events.*; 
    import flash.net.URLRequest; 
    import flash.system.ApplicationDomain; 
    import flash.system.LoaderContext; 
    
    public class loadMultipleSWFsMain extends Sprite 
    { 
        private var ldr1:Loader; 
        private var ldr2:Loader; 
        
        public function loadMultipleSWFsMain() 
        { 
            ldr1 = new Loader(); 
            var req1:URLRequest = new URLRequest("loadMultipleSWF1.swf"); 
            var ldrContext1:LoaderContext = new LoaderContext(false, ApplicationDomain.currentDomain); 
            ldr1.contentLoaderInfo.addEventListener(Event.COMPLETE, completeHandler); 
            ldr1.load(req1, ldrContext1);    
            ldr1.x = 0;
			
            addChild(ldr1);
			
            ldr2 = new Loader(); 
            var req2:URLRequest = new URLRequest("loadMultipleSWF2.swf"); 
            var ldrContext2:LoaderContext = new LoaderContext(false, ApplicationDomain.currentDomain); 
            ldr2.contentLoaderInfo.addEventListener(Event.COMPLETE, completeHandler); 
            ldr2.load(req2, ldrContext2);    
            ldr2.x = 100;
			
            addChild(ldr2);
        } 
        
        private function completeHandler(event:Event):void 
        { 
            trace("completeHandler"); 
        } 
    } 
}