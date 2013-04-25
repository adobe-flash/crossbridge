/*
 * Copyright notice
 *
 * (c) 2005-2006 Darron Schall, Claus Wahlers.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

package c64.memory.io
{
	import core.memory.io.IOHandler;
	import core.memory.io.IOHandlerInfo;
	import core.misc.Convert;

	public dynamic class SID extends IOHandler
	{
		public var osc1Frq:uint;
		public var osc1PulseWidth:uint;
		public var osc1Control:uint;
		public var osc1AttackDecay:uint;
		public var osc1SustainRelease:uint;

		public var osc2Frq:uint;
		public var osc2PulseWidth:uint;
		public var osc2Control:uint;
		public var osc2AttackDecay:uint;
		public var osc2SustainRelease:uint;

		public var osc3Frq:uint;
		public var osc3PulseWidth:uint;
		public var osc3Control:uint;
		public var osc3AttackDecay:uint;
		public var osc3SustainRelease:uint;
		
		public var cutoff:uint;
		public var filtRes:uint;
		public var modVol:uint;
		
		public var osc3:uint;
		public var env3:uint;
		
		public function SID(debugFlag:Boolean = false) {
			super(debugFlag);
			osc1Frq = 0;
			osc1PulseWidth = 0;
			osc2Frq = 0;
			osc2PulseWidth = 0;
			osc3Frq = 0;
			osc3PulseWidth = 0;
		}

		// The first 24 registers are write-only if i read right
		
		// Voice 1
		
		private function setFreqControlLow1(index:int, value:int):void {
			if(debug) debugMessage("[SID] Osc1 F LO " + Convert.toHex(value));
			osc1Frq = (osc1Frq & 0xff00) | value;
			arr[index] = value;
		}
		
		private function setFreqControlHigh1(index:int, value:int):void {
			if(debug) debugMessage("[SID] Osc1 F HI " + Convert.toHex(value));
			osc1Frq = (osc1Frq & 0x00ff) | (value << 8);
			arr[index] = value;
		}
		
		private function setPulseWaveformLow1(index:int, value:int):void {
			if(debug) debugMessage("[SID] Osc1 PW LO " + Convert.toHex(value));
			osc1PulseWidth = (osc1PulseWidth & 0xff00) | value;
			arr[index] = value;
		}
		
		private function setPulseWaveformHigh1(index:int, value:int):void {
			if(debug) debugMessage("[SID] Osc1 PW HI " + Convert.toHex(value));
			osc1PulseWidth = (osc1PulseWidth & 0x00ff) | (value << 8);
			arr[index] = value;
		}
		
		private function setControl1(index:int, value:int):void {
			if(debug) debugMessage("[SID] Osc1 C " + Convert.toHex(value));
			osc1Control = value;
			arr[index] = value;
		}
		
		private function setAttackDecay1(index:int, value:int):void {
			if(debug) debugMessage("[SID] Osc1 AD " + Convert.toHex(value));
			osc1AttackDecay = value;
			arr[index] = value;
		}
		
		private function setSustainRelease1(index:int, value:int):void {
			if(debug) debugMessage("[SID] Osc1 SR " + Convert.toHex(value));
			osc1SustainRelease = value;
			arr[index] = value;
		}
		
		// Voice 2
		
		private function setFreqControlLow2(index:int, value:int):void {
			if(debug) debugMessage("[SID] Osc2 F LO " + Convert.toHex(value));
			osc2Frq = (osc2Frq & 0xff00) | value;
			arr[index] = value;
		}
		
		private function setFreqControlHigh2(index:int, value:int):void {
			if(debug) debugMessage("[SID] Osc2 F HI " + Convert.toHex(value));
			osc2Frq = (osc2Frq & 0x00ff) | (value << 8);			
			arr[index] = value;
		}
		
		private function setPulseWaveformLow2(index:int, value:int):void {
			if(debug) debugMessage("[SID] Osc2 PW LO " + Convert.toHex(value));
			osc2PulseWidth = (osc2PulseWidth & 0xff00) | value;
			arr[index] = value;
		}
		
		private function setPulseWaveformHigh2(index:int, value:int):void {
			if(debug) debugMessage("[SID] Osc2 PW HI " + Convert.toHex(value));
			osc2PulseWidth = (osc2PulseWidth & 0x00ff) | (value << 8);
			arr[index] = value;
		}
		
		private function setControl2(index:int, value:int):void {
			if(debug) debugMessage("[SID] Osc2 C " + Convert.toHex(value));
			osc2Control = value;
			arr[index] = value;
		}
		
		private function setAttackDecay2(index:int, value:int):void {
			if(debug) debugMessage("[SID] Osc2 AD " + Convert.toHex(value));
			osc2AttackDecay = value;
			arr[index] = value;
		}
		
		private function setSustainRelease2(index:int, value:int):void {
			if(debug) debugMessage("[SID] Osc2 SR " + Convert.toHex(value));
			osc2SustainRelease = value;
			arr[index] = value;
		}
		
		// Voice 3
		
		private function setFreqControlLow3(index:int, value:int):void {
			if(debug) debugMessage("[SID] Osc3 F LO " + Convert.toHex(value));
			osc3Frq = (osc3Frq & 0xff00) | value;			
			arr[index] = value;
		}
		
		private function setFreqControlHigh3(index:int, value:int):void {
			if(debug) debugMessage("[SID] Osc3 F HI " + Convert.toHex(value));
			osc3Frq = (osc3Frq & 0x00ff) | (value << 8);
			arr[index] = value;
		}
		
		private function setPulseWaveformLow3(index:int, value:int):void {
			if(debug) debugMessage("[SID] Osc3 PW LO " + Convert.toHex(value));
			osc3PulseWidth = (osc3PulseWidth & 0xff00) | value;
			arr[index] = value;
		}
		
		private function setPulseWaveformHigh3(index:int, value:int):void {
			if(debug) debugMessage("[SID] Osc3 PW HI " + Convert.toHex(value));
			osc3PulseWidth = (osc3PulseWidth & 0x00ff) | (value << 8);
			arr[index] = value;
		}
		
		private function setControl3(index:int, value:int):void {
			if(debug) debugMessage("[SID] Osc3 C " + Convert.toHex(value));
			osc3Control = value;
			arr[index] = value;
		}
		
		private function setAttackDecay3(index:int, value:int):void {
			if(debug) debugMessage("[SID] Osc3 AD " + Convert.toHex(value));
			osc3AttackDecay = value;
			arr[index] = value;
		}
		
		private function setSustainRelease3(index:int, value:int):void {
			if(debug) debugMessage("[SID] Osc3 SR " + Convert.toHex(value));
			osc3SustainRelease = value;
			arr[index] = value;
		}
		
		// etc
		
		private function setFreqCutoffLow(index:int, value:int):void {
			if(debug) debugMessage("[SID] FC LO " + Convert.toHex(value));
			arr[index] = value;
		}
		
		private function setFreqCutoffHigh(index:int, value:int):void {
			if(debug) debugMessage("[SID] FC HI " + Convert.toHex(value));
			arr[index] = value;
		}
		
		private function setResonanceControl(index:int, value:int):void {
			if(debug) debugMessage("[SID] RES " + Convert.toHex(value));
			filtRes = value;
			arr[index] = value;
		}
		
		private function setVolumeFilterSelect(index:int, value:int):void {
			if(debug) debugMessage("[SID] MODE / VOL " + Convert.toHex(value));
			modVol = value;
			arr[index] = value;
		}

		// I'm unsure if these must have setters too

		private function getGamePaddle1(index:int):int {
			return arr[index];
		}

		private function getGamePaddle2(index:int):int {
			return arr[index];
		}

		private function getOscillator3(index:int):int {
			if(debug) debugMessage("[SID] get OSC3 " + Convert.toHex(arr[index]));
			osc3 = Math.floor(Math.random() * 256);
			arr[index] = osc3;
			return arr[index];
		}

		private function getEnvelopeGenerator3(index:int):int {
			if(debug) debugMessage("[SID] get ENV 3 " + Convert.toHex(arr[index]));
			arr[index] = env3;
			return arr[index];
		}

		
		override protected function initHandlers():void {
			handlers = [
				// the SID chip has 32 registers:
				new IOHandlerInfo(getDefault, setFreqControlLow1), // 00
				new IOHandlerInfo(getDefault, setFreqControlHigh1), // 01
				new IOHandlerInfo(getDefault, setPulseWaveformLow1), // 02
				new IOHandlerInfo(getDefault, setPulseWaveformHigh1), // 03
				new IOHandlerInfo(getDefault, setControl1), // 04
				new IOHandlerInfo(getDefault, setAttackDecay1), // 05
				new IOHandlerInfo(getDefault, setSustainRelease1), // 06
				new IOHandlerInfo(getDefault, setFreqControlLow2), // 07
				new IOHandlerInfo(getDefault, setFreqControlHigh2), // 08
				new IOHandlerInfo(getDefault, setPulseWaveformLow2), // 09
				new IOHandlerInfo(getDefault, setPulseWaveformHigh2), // 0a
				new IOHandlerInfo(getDefault, setControl2), // 0b
				new IOHandlerInfo(getDefault, setAttackDecay2), // 0c
				new IOHandlerInfo(getDefault, setSustainRelease2), // 0d
				new IOHandlerInfo(getDefault, setFreqControlLow3), // 0e
				new IOHandlerInfo(getDefault, setFreqControlHigh3),  // 0f
				new IOHandlerInfo(getDefault, setPulseWaveformLow3), // 10
				new IOHandlerInfo(getDefault, setPulseWaveformHigh3), // 11
				new IOHandlerInfo(getDefault, setControl3), // 12
				new IOHandlerInfo(getDefault, setAttackDecay3), // 13
				new IOHandlerInfo(getDefault, setSustainRelease3), // 14
				new IOHandlerInfo(getDefault, setFreqCutoffLow), // 15
				new IOHandlerInfo(getDefault, setFreqCutoffHigh), // 16
				new IOHandlerInfo(getDefault, setResonanceControl), // 17
				new IOHandlerInfo(getDefault, setVolumeFilterSelect), // 18
				new IOHandlerInfo(getGamePaddle1, setDefault), // 19
				new IOHandlerInfo(getGamePaddle2, setDefault), // 1a
				new IOHandlerInfo(getOscillator3, setDefault), // 1b
				new IOHandlerInfo(getEnvelopeGenerator3, setDefault), // 1c
				new IOHandlerInfo(getDefault, setDefault), // n/c
				new IOHandlerInfo(getDefault, setDefault), // n/c
				new IOHandlerInfo(getDefault, setDefault)  // n/c
			];
		}
	}
}
