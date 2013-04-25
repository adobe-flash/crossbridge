// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

package FlogRayTracer {
public class Engine {
    public var options;
    
    
    public function Engine(options = null){
        if (options) {
          this.options = options;
        } else {
          this.options = {
                canvasHeight: 100,
                canvasWidth: 100
            };
        }

    }
} // class Engine

function renderScene(){
    var imageWidth = 100; // $F('imageWidth');
    var imageHeight = 100; // $F('imageHeight');

    var raytracer = new Engine(
        {
            canvasWidth: imageWidth,
            canvasHeight: imageHeight
        }
    );

    print(raytracer.options.canvasWidth);
}

renderScene();

} // package
