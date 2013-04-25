// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

package RayTrace {
	public interface Shape {
        function get material():Material;
    }

	public class Material {
		function Material() {}
	}

    public class Sphere implements Shape {
        public var _material:Material;
        public function Sphere () { _material = new Material() }
        public function get material():Material { return _material; }
    }

	function rayTrace (info:Shape) {
			print(info.material);
	}

	function testHarness() {
		var x:Sphere = new Sphere();
		rayTrace(x);
	}

	testHarness();
}
