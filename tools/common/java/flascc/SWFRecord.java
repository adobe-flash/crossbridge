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

class SWFRecord
{
	public final short tag;
	public final byte[] payload;
	
	public final int length;
	
	public static SWFRecord read(InputStream is) throws IOException
	{
		int tagAndLen = is.read() | (is.read() << 8);
		
		if(tagAndLen < 0)
			return null;
		
		short tag = (short)(tagAndLen >> 6);
		int length;
		
		if((tagAndLen & 63) == 63)
			length = is.read() | (is.read() << 8) | (is.read() << 16) | (is.read() << 24);
		else
			length = tagAndLen & 63;
		
		if(length < 0)
			return null;
		
		byte[] payload = new byte[length];
		int read = 0;
		
		while(read < payload.length)
		{
			int chunk = is.read(payload, read, payload.length-read);
			if(chunk <= 0)
				return null;
			read += chunk;
		}
		return new SWFRecord(tag, payload);
	}
	
	public void write(OutputStream os) throws IOException
	{
		int len = payload.length;		
		int tagAndLen = (tag << 6);
		
		if(len > 62)
			tagAndLen |= 63; // always use extended length
		else
			tagAndLen |= len;
		
		os.write(tagAndLen);
		os.write(tagAndLen >> 8);
		
		if(len > 62)
		{
			os.write(len);
			os.write(len >> 8);
			os.write(len >> 16);
			os.write(len >> 24);
		}
		
		os.write(payload);
	}
	
	public SWFRecord(short tag, byte[] payload)
	{
		this.tag = tag;
		this.payload = payload;
		length = ((payload.length > 62) ? 6 : 2) + payload.length;
	}
}
