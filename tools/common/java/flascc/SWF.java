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
import java.util.ArrayList;
import java.util.List;

class SWF
{
	public final SWFHeader header;
	public final SWFRecord[] records;
	
	public final int length;

	public static SWF read(InputStream is) throws IOException
	{
		SWFHeader.SWFHeaderAndInputStream pair = SWFHeader.read(is);
		
		if(pair == null)
			return null;
		
		SWFHeader header = pair.swfHeader;
		is = pair.inputStream;
		
		List<SWFRecord> recordList = new ArrayList<SWFRecord>();
		
		for(;;)
		{
			SWFRecord record = SWFRecord.read(is);
			
			if(record == null)
				break;
			recordList.add(record);
		}
		return new SWF(header, recordList.toArray(new SWFRecord[0]));
	}
		
	public OutputStream write(OutputStream os, boolean compress) throws IOException
	{
		os = header.write(os, length, compress);
		for(SWFRecord record: records)
			record.write(os);
		return os;
	}
	
	public SWF(SWFHeader header, SWFRecord[] records)
	{
		this.header = header;
		this.records = records;
		int length = header.length;
		for(SWFRecord record: records)
			length += record.length;
		this.length = length;
	}
}