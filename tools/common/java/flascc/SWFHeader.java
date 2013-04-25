// Copyright (c) 2013 Adobe Systems Inc

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

package flascc;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.zip.DeflaterOutputStream;
import java.util.zip.InflaterInputStream;

class SWFHeader
{
	public final byte version;
	public final byte[] rect;
	public final short frameRate;
	public final short frameCount;
	
	public final int length;
	
	public static byte[] readRect(InputStream is) throws IOException
	{
		int b0 = is.read();

		if(b0 < 0)
			return null;

		int bpc = b0 >> 3; // high 5 bits indicate bits per component for xMin, xMax, yMin, yMax
		int bitCount = 5 + bpc * 4; // total bitcount for the rect (including component width bits)
		int byteCount = (bitCount + 7) / 8; // round up for total byte count
		byte[] result = new byte[byteCount];

		result[0] = (byte)b0;
		if(is.read(result, 1, result.length-1) != result.length-1)
			return null;
		
		return result;
	}
	
	public static class SWFHeaderAndInputStream
	{
		public final SWFHeader swfHeader;
		public final InputStream inputStream;
	
		public SWFHeaderAndInputStream(SWFHeader swfHeader, InputStream inputStream)
		{
			this.swfHeader = swfHeader;
			this.inputStream = inputStream;
		}
	}
	
	public static SWFHeaderAndInputStream read(InputStream is) throws IOException
	{
		// header
		boolean compressed;
		
		switch(is.read())
		{
			default:
				return null;
			case 'F':
				compressed = false;
				break;
			case 'C':
				compressed = true;
				break;
		}
		if(is.read() != 'W')
			return null;
		if(is.read() != 'S')
			return null;
		
		// version
		int ver = is.read();
		
		if(ver < 0)
			return null;
		
		// file size (ignored)
		if(is.skip(4) != 4)
			return null;
		
		if(compressed)
			is = new InflaterInputStream(is);

		byte[] rect = readRect(is);
		
		if(rect == null)
			return null;
		
		int frameRate = is.read() | (is.read() << 8);
		
		if(frameRate < 0)
			return null;
		
		int frameCount = is.read() | (is.read() << 8);
		
		if(frameCount < 0)
			return null;
		
		return new SWFHeaderAndInputStream(new SWFHeader((byte)ver, rect, (short)frameRate, (short)frameCount), is);
	}
	
	public OutputStream write(OutputStream os, int swfLength, boolean compress) throws IOException
	{
		compress &= (version >= 6);
		
		os.write(compress ? 'C' : 'F');
		os.write('W');
		os.write('S');
		os.write(version);
		
		os.write(swfLength);
		os.write(swfLength >> 8);
		os.write(swfLength >> 16);
		os.write(swfLength >> 24);
		
		if(compress)
		{
			os.flush();
			os = new DeflaterOutputStream(os);
		}

		os.write(rect);
		os.write(frameRate);
		os.write(frameRate >> 8);
		os.write(frameCount);
		os.write(frameCount >> 8);

		return os;
	}
	
	public SWFHeader(byte version, byte[] rect, short frameRate, short frameCount)
	{
		this.version = version;
		this.rect = rect;
		this.frameRate = frameRate;
		this.frameCount = frameCount;
		length = 4 + 4 + rect.length + 2 + 2;
	}
}