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


import java.io.*;
import java.net.*;
import java.util.*;
import java.util.concurrent.*;

import flash.tools.debugger.*;
import flash.tools.debugger.events.*;
import flash.tools.debugger.concrete.DValue;

// gdb<=>fdb<=>as3 helper bridge for flascc
// inferior itself handles much of the work via gdb packet delegation
public class AlcDB {
	private Session _sess;
	private GDBServer _gdbServer;
	private Value _dbgHelper = null;
    private Map<Integer, Value> _dbgHelpers = new HashMap<Integer, Value>();
    private int _currentIsolateId = 1;
    private List<Location> _breakpoints = new ArrayList<Location>();
    private List<Integer> _brokenIsolates = new ArrayList<Integer>();
    private static boolean _sendAcks = true;
    private static boolean _nonStop = false;
    private Map<Integer, Boolean> _runningIsolates = 
        new HashMap<Integer, Boolean>();
    private List<Long> _fakeBreakpointMaps = new ArrayList<Long>();
    private Set<Location> _clearedBreakpoints = new HashSet<Location>();
    private String _as3ns;
    private Map<Integer, Boolean> _infRunning = new HashMap<Integer, Boolean>();
    private Map<Integer, String> _lastException = new HashMap<Integer, String>();
    private Map<Integer, Boolean> _breakingFromCaught = new HashMap<Integer, Boolean>();
    private ThreadNotifier _notifier = this.new ThreadNotifier();
    private PerformanceLog _packetLog = new PerformanceLog("Packet");
    private PerformanceLog _cmdLog = new PerformanceLog("Helper Command");
    private PerformanceLog _eventLog = new PerformanceLog("Event");

    private String functionNS() {
    	if(_as3ns.equals("com.adobe.flascc"))
    		return "C_Run";
    	else
    		return _as3ns;
    }

    // When running in all-stop mode, there are two isolates that are
    // interesting to the debugger: the primordial isolate and the isolate
    // running the main C thread. The primordial isolate's id is always 1,
    // and this variable stores the id of the main C thread's isolate.
    private int _mainCThreadIsolate = -1;

	// notification of a helper thread having exited
	private static class HelperThreadExitEvent extends DebugEvent
	{
		private Thread _t;
		private Exception _e;
		
		public HelperThreadExitEvent(Thread t, Exception e)
		{
			_t = t;
			_e = e;
		}
		
		public Thread getThread()
		{
			return _t;
		}
		
		public Exception getException()
		{
			return _e;
		}
	}
	
	// pump DebugEvents from the session to the Queue
	private static class SessDebugEventHelper extends Thread
	{
		private Session _sess;
		private Queue<DebugEvent> _q;
		
		public SessDebugEventHelper(Session sess, Queue<DebugEvent> q)
		{
			_sess = sess;
			_q = q;
		}
		
		@Override
		public void run()
		{
			try
			{
				for(;;)
				{
					_sess.waitForEvent();
					_q.add(_sess.nextEvent());
				}
			}
			catch(Exception e)
			{
				_q.add(new HelperThreadExitEvent(this, e));
			}
		}
	}
	
	// notification of gdb having connected to us
	private static class GDBConnectEvent extends DebugEvent
	{
	}
	
	// notification of gdb having requested a break
	private static class GDBBreakEvent extends DebugEvent
	{
	}
	
	// notification of gdb having sent us a packet
	private static class GDBPacketEvent extends DebugEvent
	{
		private byte[] _payload;
		
		public GDBPacketEvent(byte[] payload)
		{
			_payload = payload;
			this.information = "[" + new String(payload) + "]";
		}
		
		public byte[] getPayload()
		{
			return _payload;
		}
	}
	
	// TODO i really have to roll my own?
	private static class ReentrantMutex
	{
		private Semaphore _sem = new Semaphore(1);
		private Thread _owner;
		private int _reentCount;
		
		public boolean tryAcquire()
		{
			if(acquired())
			{
				_reentCount++;
				return true;
			}
			if(_sem.tryAcquire())
			{
				_owner = Thread.currentThread();
				_reentCount++;
				return true;
			}
			return false;
		}
		
		public boolean acquired()
		{
			return _owner == Thread.currentThread();
		}
		
		public void acquire() throws InterruptedException
		{
			if(!acquired())
			{
				Thread self = Thread.currentThread();

				for(;;)
				{
					_sem.acquire();
					if(_owner == null || _owner == self)
					{
						_owner = self;
						break;
					}
					_sem.release();
				}
			}
			_reentCount++;
		}
		
		public void release()
		{
			if(acquired())
			{
				if(0 == --_reentCount)
				{
					_owner = null;
					_sem.release();
				}
			}
		}
		
	}
	
	// implements a simple gdb server that queues received packets
	// and provides packet sending facilities
	private static class GDBServer extends Thread
	{
		private int _port;
		private Queue<DebugEvent> _q;
		private Socket _sock;
		
		// bytes being read by _sockMutex holder (via lockedReadByte)
		private BlockingQueue<Byte> _sockByteQ = new LinkedBlockingQueue<Byte>();
		// semaphore indicating number of bytes _sockMutex holder wants to read from _sockByteQ
		private Semaphore _sockByteSemaphore = new Semaphore(Integer.MAX_VALUE);
		// socket mutex -- acquire to do exclusive reads (via lockedReadByte) or writes on _sock
		private ReentrantMutex _sockMutex = new ReentrantMutex();
		
		public GDBServer(int port, Queue<DebugEvent> q) throws InterruptedException
		{
			_port = port;
			_q = q;

			_sockByteSemaphore.drainPermits();
		}
		
		private void acquireSock() throws InterruptedException
		{
			_sockMutex.acquire();
		}
		
		private void releaseSock()
		{
			_sockMutex.release();
		}
		
		private int lockedSockReadByte() throws InterruptedException
		{
			if(!_sockMutex.acquired())
				return -1;
			
			_sockByteSemaphore.release();
			try
			{
				return (byte)(_sockByteQ.take());
			}
			catch(InterruptedException e)
			{
				_sockByteSemaphore.acquire(); // keep queue and sema balanced on error
				throw e;
			}
		}

        public void putPacket(byte[] packet, boolean notif) throws IOException, InterruptedException
        {
			int csum = 0;
			
			for(byte b: packet)
				csum += b;
			
			OutputStream os = _sock.getOutputStream();
			
			try
			{
				acquireSock();
				
				// adapted from gdb/i386-stub.c:putpacket
				do
				{
                    if (notif)
                    {
                        os.write('%');
                    }
                    else
                    {
                        os.write('$');
                    }
					os.write(packet);
					os.write('#');
					os.write(Integer.toHexString((csum >> 4) & 0xf).getBytes());
					os.write(Integer.toHexString(csum & 0xf).getBytes());
				}
				while(_sendAcks && lockedSockReadByte() != '+');
			}
			finally
			{
				releaseSock();
			}
        }
		
		// thread safe! -- send a packet to gdb
		public void putPacket(byte[] packet) throws IOException, InterruptedException
		{	
            putPacket(packet, false);
		}
		
		public void putPacket(String s) throws IOException, InterruptedException
		{
			putPacket(s.getBytes());
		}
		
		// run loop byte reader helper -- when it returns, the run loop owns the socket mutex
		private int runLockReadByte() throws InterruptedException, Exception
		{
			int ch;
			getchar:
			do
			{
				ch = _sock.getInputStream().read(); // read a char
				if(ch != -1 && !_sockMutex.tryAcquire()) // someone else has the mutex it -- try to feed the char to them
				{
					int ms = 1;
					
					for(;;) // spin until we either forward the byte, or are able to acquire the mutex
					{
						if(_sockByteSemaphore.tryAcquire(ms, TimeUnit.MILLISECONDS)) // request space in _sockByteQ
						{
							try
							{
								_sockByteQ.add((byte)ch); // queue up the char
							}
							catch(Exception e)
							{
								_sockByteSemaphore.release(); // keep queue and sema balanced on error
								throw e;
							}
							continue getchar; // queued! next char
						}
						else if(_sockMutex.tryAcquire()) // whoever had the mutex gave it up? we can move ahead
							break;
						if((ms *= 2) > 1000)
							throw new Error("Internal error: gdb server run loop couldn't delegate data or acquire socket mutex");
						// around again
					}
				}
			}
			while(false);
			return ch;
		}
		
		@Override
		public void run()
		{
			try
			{
				ServerSocket ssock = new ServerSocket(_port, 0);
				
				_sock = ssock.accept();
				
				ssock.close();
				
				_q.add(new GDBConnectEvent());
				
				InputStream is = _sock.getInputStream();
				OutputStream os = _sock.getOutputStream();
				
				// adapted from gdb/i386-stub.c:getpacket (sort of goofy flow control!)
				byte[] buffer = new byte[10 * 1024];		
				int checksum;
				int xmitcsum;
				int count;
				
				// main loop reads bytes and tries to recognize+queue packets or delegate bytes to
				// someone who has acquired the socket via acquireSocket
				for(;;)
				{
					try
					{
						int ch;

						do
						{
							_sockMutex.release(); // if we acquired the mutex last trip through the loop, release it here (extra releases are ok)
							ch = runLockReadByte(); // read a char
							if(ch == 3)
								_q.add(new GDBBreakEvent());
						}
						while(ch != '$' && ch != -1);
						// have the _sockMutex at this point
						if(ch == -1) // eof -- bail
							break;
						retry:
						do
						{
							checksum = 0;
							xmitcsum = -1;
							count = 0;
		
							while(count < buffer.length)
							{
								ch = is.read();
								if(ch == '$')
									break retry;
								if(ch == '#' || ch == -1)
									break;
								checksum += ch;
								buffer[count] = (byte)ch;
								count++;
							}
						} while(false);
						if(ch == '#')
						{
							ch = is.read();
							xmitcsum = Integer.parseInt(new Character((char)ch).toString(), 16) << 4;
							ch = is.read();
							xmitcsum += Integer.parseInt(new Character((char)ch).toString(), 16);
							if(_sendAcks && (checksum & 0xff) != (xmitcsum & 0xff))
                                os.write('-'); // failed checksum
							else
							{
                                if (_sendAcks)
								    os.write('+'); // successful transfer
								if(count > 2 && buffer[2] == ':') // sequence number
								{
									os.write(buffer[0]);
									os.write(buffer[1]);
									
									byte[] packet = new byte[count-3];
									
									System.arraycopy(buffer, 3, packet, 0, count - 3);
									_q.add(new GDBPacketEvent(packet));
                                    dbg("from gdb: " + new String(packet));
								}
								else
								{
									byte[] packet = new byte[count];
									
									System.arraycopy(buffer, 0, packet, 0, count);
									_q.add(new GDBPacketEvent(packet));
                                    dbg("from gdb: " + new String(packet));
								}
							}
						}
					}
					finally
					{
						_sockMutex.release(); // always release it (extra releases are ok)
					}
				}
			}
			catch(Exception e)
			{
				_q.add(new HelperThreadExitEvent(this, e));
			}
			_q.add(new HelperThreadExitEvent(this, null)); // normal eof on socket
		}
	}

    // Handles notification packets in non-stop mode, for details see:
    // http://sourceware.org/gdb/download/onlinedocs/gdb/Remote-Non_002dStop.html
    private class ThreadNotifier
    {
        private Queue<String> _pendingNotifications;

        public ThreadNotifier()
        {
            _pendingNotifications = new LinkedList<String>();
        }

        public void reportThreadStop(int tid, String notification)
            throws IOException, InterruptedException
        {
            if (_pendingNotifications.isEmpty())
            {
                _gdbServer.putPacket(notification.getBytes(), true);
            }
            _pendingNotifications.add(notification);
        }

        public void sendNextNotification() throws IOException, 
                InterruptedException
        {
            String resp = "OK";
            if (!_pendingNotifications.isEmpty())
            {
                _pendingNotifications.remove();
            }
            if (!_pendingNotifications.isEmpty()) 
            {
                // Remove the "Stop:" prefix because this is not an
                // asynchronous notification
                resp = _pendingNotifications.element().substring(5);
            }
            _gdbServer.putPacket(resp.getBytes());
        }

        public void resendAllNotifications() throws IOException,
                InterruptedException, NotConnectedException
        {
            String resp = "OK";
            for (Integer tid : _dbgHelpers.keySet())
            {
                if (_sess.getWorkerSession(tid).isSuspended())
                {
                    _pendingNotifications.add(getStopReplyForThread(tid));
                }
            }
            if (!_pendingNotifications.isEmpty())
            {
                resp = _pendingNotifications.remove();
            }
            _gdbServer.putPacket(resp.getBytes());
        }

        // TODO: duplicated in AlcDbgHelper.as
        private String getStopReplyForThread(int tid)
        {
            //return "Stop:T05thread:" + tid + ";";
            return "T05thread:" + tid + ";";
        }
    }
	
    
    // Stores a simple log of operations and their running time. 
    private static class PerformanceLog
    {
        private static class PerformanceLogEntry 
                            implements Comparable<PerformanceLogEntry>
        {
            private Long _millis;
            private Long _invokes;
            private String _desc;

            public PerformanceLogEntry(String desc, long millis)
            {
                _desc = desc;
                _millis = millis;
                _invokes = Long.valueOf(1);
            }

            public void add(long millis)
            {
                _millis += millis;
                _invokes++;
            }

            public int compareTo(PerformanceLogEntry e)
            {
                return _millis.compareTo(e._millis);
            }
        }

        private HashMap<String, PerformanceLogEntry> _entries;
        private String _category;

        public PerformanceLog(String category)
        {
            _category = category;
            _entries = new HashMap<String, PerformanceLogEntry>();
        }

        public void add(String desc, long millis)
        {
            if (_entries.containsKey(desc))
            {
                _entries.get(desc).add(millis);
            } else {
                _entries.put(desc, new PerformanceLogEntry(desc, millis));
            }
        }

        public String toString()
        {
            StringBuilder str = new StringBuilder();
            String catSep = (_category.length() > 7) ? "\t\t\t" : "\t\t\t\t";
            str.append(_category);
            str.append(catSep + "Total Time\tInvoke Count\tAverage Time\n");
            str.append(
" --------------------------------------------------------------------------- "
            + "\n");
            ArrayList<PerformanceLogEntry> sorted = 
                                        new ArrayList<PerformanceLogEntry>();
            sorted.addAll(_entries.values());
            Collections.sort(sorted, Collections.reverseOrder());

            int totalTime = 0;
            int totalInvokes = 0;;
            for (PerformanceLogEntry e : sorted)
            {
                int nTabs = 4 - e._desc.length() / 8;
                str.append(e._desc);
                for (int i = 0; i < nTabs; i++)
                {
                    str.append("\t");
                }
                str.append(e._millis + "\t\t" + e._invokes
                            + "\t\t" + (e._millis / e._invokes) + "\n");
            }

            return str.toString();
        }
    }
	
	private AlcDB(Session sess)
	{
		_sess = sess;
	}

    // Get the current thread, as reported to gdb
    private int getCurrentGDBThread()
    {
        return (_nonStop) ? _currentIsolateId : 1;
    }

    // Get the current isolate
    private int getCurrentIsolate()
    {
        if (_nonStop)
        {
            return _currentIsolateId;
        }
        else if (_mainCThreadIsolate > 0)
        {
            return _mainCThreadIsolate;
        }
        else
        {
            return 1;
        }
    }

    private boolean isolateAppearsToBeTerminating(String topFrame, 
                                                  String except)
    {
        if (except == null)
        {
            except = "";
        }

        return topFrame.startsWith("flash.system::Worker/terminate") ||
                topFrame.startsWith("flash.system::System$/exit") ||
                except.indexOf("::GoingAsync@") >= 0; 
    }

    private Value valueForPrimitive(Object o, int tid)
    {
        return DValue.forPrimitive(o, tid);
    }
	
	private Value[] getStackThisArray(int tid) throws NotConnectedException
	{
		Frame[] frames = _sess.getWorkerSession(tid).getFrames();
		Value[] r = (frames == null) ? null : new Value[frames.length];
		
		if(r != null)
		{
			int i = 0;
			for(Frame f: frames)
			{
				try {
					r[i] = f.getThis(_sess).getValue();
				} catch (Exception e) {
					r[i] = valueForPrimitive(Value.UNDEFINED, tid);
				}
				i++;
			}
		}
		return r;
	}
	
	private int[] getStackLocationArray(int tid) throws NotConnectedException
	{
        Frame[] frames = _sess.getWorkerSession(tid).getFrames();
        dbg("doing getStackLocationList for thread: " + tid);
		int[] r = (frames == null) ? null : new int[frames.length*2];
		
		if(r != null)
		{
			int i = 0;
			for(Frame f: frames)
			{
				r[i] = -1; // abcID defaults to -1, lineNo to 0
				try {
					Location l = f.getLocation();
					r[i+1] = l.getLine();
					r[i] = l.getFile().getId();
				} catch (Exception e) { // leave as is
				}
				i+=2;
			}
		}
		return r;
	}
	
	private int[] getStackLocalCountArray(int tid) throws NotConnectedException
	{
		Frame[] frames = _sess.getWorkerSession(tid).getFrames();
		int[] r = (frames == null) ? null : new int[frames.length];
		
		if(r != null)
		{
			int i = 0;
			for(Frame f: frames)
			{
				try {
					r[i] = f.getLocals(_sess).length;
				} catch (Exception e) { // leave 0
				}
				i++;
			}
		}
		return r;
	}
	
	private Value[] getStackSpecificLocalArray(String[] names, int tid) throws NotConnectedException, NoResponseException, NotSuspendedException
	{
        dbg("getStackSpecificLocalArray: " + tid);
		Frame[] frames = _sess.getWorkerSession(tid).getFrames();
		Value[] r = (frames == null) ? null : new Value[frames.length*names.length];
		
		if(r != null)
		{
			int i = 0;
			for(Frame f: frames)
			{
				Variable[] locals = f.getLocals(_sess);

				for(String name: names)
				{
					r[i] = valueForPrimitive(Value.UNDEFINED, tid);
					try {
						for(Variable local: locals)
							if(local.getName().equals(name))
							{
								r[i] = local.getValue();
                                dbg(name + ": " + 
                                    r[i].getValueAsString());
								break;
							}
					} catch (Exception e) { // leave undefined
					}
					i++;
				}
			}
		}
        dbg("DONE");
		return r;	
	}
	
	private String[] getFrameLocalNameArray(int frameNo, int tid) throws NotConnectedException, NoResponseException, NotSuspendedException
	{
		Frame frame = _sess.getWorkerSession(tid).getFrames()[frameNo];
		Variable[] locals = frame.getLocals(_sess);
		return getVarsNameArray(locals);
	}
	
	private String[] getFrameArgumentNameArray(int frameNo, int tid) throws NotConnectedException, NoResponseException, NotSuspendedException
	{
		Frame frame = _sess.getWorkerSession(tid).getFrames()[frameNo];
		Variable[] args = frame.getArguments(_sess);
		return getVarsNameArray(args);
	}
	
	private String[] getVarsNameArray(Variable[] vars) throws NotConnectedException, NoResponseException, NotSuspendedException
	{
		String[] r = new String[vars.length];
		int i = 0;
		
		for(Variable var: vars)
			r[i++] = var.getQualifiedName();
		return r;
	}
	
	private String[] getFrameLocalTypeArray(int frameNo, String[] names, int tid) throws NotConnectedException, NoResponseException, NotSuspendedException
	{
		Frame frame = _sess.getWorkerSession(tid).getFrames()[frameNo];
		Variable[] locals = frame.getLocals(_sess);
		return getVarsTypeArray(locals, names);
	}

	private String[] getFrameArgumentTypeArray(int frameNo, String[] names, int tid) throws NotConnectedException, NoResponseException, NotSuspendedException
	{
		Frame frame = _sess.getWorkerSession(tid).getFrames()[frameNo];
		Variable[] args = frame.getArguments(_sess);
		return getVarsTypeArray(args, names);
	}

	private String[] getVarsTypeArray(Variable[] vars, String[] names) throws NotConnectedException, NoResponseException, NotSuspendedException
	{
		String[] r = new String[names.length];
		int i = 0;
		
		for(String name: names)
		{
			r[i] = "";
			try {
				for(Variable var: vars)
					if(var.getName().equals(name))
					{
						r[i] = var.getValue().getTypeName();
						break;
					}
			} catch (Exception e) { // leave empty
			}
			i++;
		}
		return r;
	}

	private Value[] getFrameLocalValueArray(int frameNo, String[] names, int tid) throws NotConnectedException, NoResponseException, NotSuspendedException
	{
		Frame frame = _sess.getWorkerSession(tid).getFrames()[frameNo];
		Variable[] locals = frame.getLocals(_sess);
		return getVarsValueArray(locals, names, tid);
	}
	
	private Value[] getFrameArgumentValueArray(int frameNo, String[] names, int tid) throws NotConnectedException, NoResponseException, NotSuspendedException
	{
		Frame frame = _sess.getWorkerSession(tid).getFrames()[frameNo];
		Variable[] args = frame.getArguments(_sess);
		return getVarsValueArray(args, names, tid);
	}
	
	private Value[] getVarsValueArray(Variable[] vars, String[] names, int tid) throws NotConnectedException, NoResponseException, NotSuspendedException
	{
		Value[] r = new Value[names.length];
		int i = 0;
		
		for(String name: names)
		{
			r[i] = valueForPrimitive(Value.UNDEFINED, tid);
			try {
				for(Variable var: vars)
					if(var.getName().equals(name))
					{
						r[i] = var.getValue();
						break;
					}
			} catch (Exception e) { // leave undefined
			}
			i++;
		}
		return r;
	}

	private String[] intArrayToStringArray(int[] a)
	{
		String[] r = new String[a.length];
		int i = 0;
		
		for(int n: a)
			r[i++] = String.valueOf(n);
		return r;
	}

    private String getFunctionFromLoc(int fileId, int line) throws NoResponseException, InProgressException
    {
        SwfInfo[] swfs = _sess.getSwfs();
        for (SwfInfo swf : swfs) {
            SourceFile[] files = swf.getSourceList(_sess);
            for (SourceFile file : files) {
                if (fileId == file.getId()) {
                    return file.getFunctionNameForLine(_sess, line); 
                }
            }
        }
        return null;
    }

    private String hexDecode(String hex)
    {
        ByteArrayOutputStream os = new ByteArrayOutputStream();
        for (int i = 0; i < hex.length(); i += 2) {
            os.write(Integer.parseInt(hex.substring(i, i + 2), 16));
        }

        try {
            return os.toString("UTF-8");
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
            return new String();
        }
    }

    private String hexEncode(String str)
    {
        Formatter formatter = new Formatter();
        try {
            for (byte b : str.getBytes("UTF-8"))
            {
                formatter.format("%02x", b);
            }
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
            return new String();
        }

        return formatter.toString();
    }

	private Value helperQuery(String name, Value[] args, int tid) throws PlayerDebugException, IOException, InterruptedException
    {
        long startTime = 0;
        if (s_perf_log_level > 0)
        {
            startTime = System.currentTimeMillis();
        }
        Value ret = runHelperQuery(name, args, tid);
        if (s_perf_log_level > 0 && startTime != 0)
        {
            _cmdLog.add(name, System.currentTimeMillis() - startTime);
        }
        return ret;
    }

	// answer a query for an AS3 helper function
	private Value runHelperQuery(String name, Value[] args, int tid) throws PlayerDebugException, IOException, InterruptedException
	{
        dbg("helperQuery: " + name);
		if(name.equals("getVersion"))
			return valueForPrimitive(VERSION_STRING, tid);
		else if(name.equals("gdbPacket"))
		{
            dbg("sent gdb: " + args[0].getValueAsString());
			_gdbServer.putPacket(valueToByteArray(args[0], tid));
			return valueForPrimitive(Value.UNDEFINED, tid);
		}
        else if(name.equals("gdbNotify"))
        {
            dbg("sending notification: " + 
                    args[0].getValueAsString());
            _notifier.reportThreadStop(tid, new String(valueToByteArray(
                args[0], tid)));
			return valueForPrimitive(Value.UNDEFINED, tid);
        }
		else if(name.equals("setBreakpoint"))
		{
			int fileID = Integer.valueOf(args[0].getValueAsString());
			int lineNum = Integer.valueOf(args[1].getValueAsString());
			// if we can't set the breakpoint here, look ahead a few lines (default 100)
			int lookAhead = (args.length >= 3) ? Integer.valueOf(args[2].getValueAsString()) : 100;
			Location l;

            dbg("break req at " + fileID + ":" + lineNum);
			
			do l = _sess.getWorkerSession(tid).setBreakpoint(fileID, lineNum++);
			while(l == null && lookAhead-- > 0);

            dbg("trying to set breakpoint");
			
			if(l == null)
				return valueForPrimitive(Value.UNDEFINED, tid);
			SourceFile sf = l.getFile();
			fileID = (sf == null) ? -1 : sf.getId();
			lineNum = l.getLine();
            for (Isolate i : _sess.getWorkers()) {
                int id = i.getId();
                Location res;
                if (!_sess.getWorkerSession(id).isSuspended()) {
                    dbg("skipping breakpoints for thread " + id);
                    continue;
                }
                res = _sess.getWorkerSession(id).setBreakpoint(fileID, lineNum);
            }
            _breakpoints.add(l);

            /* Note: this isn't as simple as calling remove on 
             * _clearedBreakpoints because Location does not implement
             * equals in the way that we need. 
             */
            List<Location> clearedBreakpointsToRemove = new
                ArrayList<Location>();
            for (Location loc : _clearedBreakpoints) {
                if (locationsEqual(l, loc)) {
                    clearedBreakpointsToRemove.add(loc);
                }
            }
            for (Location loc : clearedBreakpointsToRemove) {
                _clearedBreakpoints.remove(loc);
            }

			// return actual location set
			return valueForPrimitive(Integer.toString(fileID) + "," + Integer.toString(lineNum), tid);
		}
		else if(name.equals("clearBreakpoint"))
		{
			int fileID = Integer.valueOf(args[0].getValueAsString());
			int lineNum = Integer.valueOf(args[1].getValueAsString());
			Location l = _sess.setBreakpoint(fileID, lineNum); // do this just to get a Location... breakpoints aren't RC-ed so it should be ok, if non-ideal
			if(l == null)
				return valueForPrimitive(Value.UNDEFINED, tid);
			l = _sess.clearBreakpoint(l);
			if(l == null)
				return valueForPrimitive(Value.UNDEFINED, tid);

            _clearedBreakpoints.add(l);
			SourceFile sf = l.getFile();
			fileID = (sf == null) ? -1 : sf.getId();
			lineNum = l.getLine();
			// return actual location cleared
			return valueForPrimitive(Integer.toString(fileID) + "," + Integer.toString(lineNum), tid);
		}
		else if(name.equals("getBreakpointList"))
		{
			Location l[] = _sess.getBreakpointList();
			int il[] = new int[l.length * 2];
			int i = 0;
			
			for(Location cl: l)
			{
				SourceFile sf = cl.getFile();
				il[i++] = (sf == null) ? -1 : sf.getId();
				il[i++] = cl.getLine();
			}
			return valueForPrimitive(stringArrayToList(intArrayToStringArray(il)), tid);
		}
		else if(name.equals("getStackThisArray"))
			return valueArrayToArrayValue(getStackThisArray(tid), tid);
		else if(name.equals("getStackLocationList"))
			return valueForPrimitive(stringArrayToList(intArrayToStringArray(getStackLocationArray(tid))), tid);
		else if(name.equals("getStackLocalCountList"))
			return valueForPrimitive(stringArrayToList(intArrayToStringArray(getStackLocalCountArray(tid))), tid);
		else if(name.equals("getStackSpecificLocalArray"))
			return valueArrayToArrayValue(getStackSpecificLocalArray(listToStringArray(args[0].getValueAsString()), tid), tid);
		else if(name.equals("getFrameLocalNameList"))
		{
			int frame = (int)(double)(Double)(args[0].getValueAsObject());
			return valueForPrimitive(stringArrayToList(getFrameLocalNameArray(frame, tid)), tid);
		}
		else if(name.equals("getFrameLocalTypeList"))
		{
			int frame = (int)(double)(Double)(args[0].getValueAsObject());
			return valueForPrimitive(stringArrayToList(getFrameLocalTypeArray(
					frame, listToStringArray(args[1].getValueAsString()), tid)),
                    tid);
		}
		else if(name.equals("getFrameLocalValueArray"))
		{
			int frame = (int)(double)(Double)(args[0].getValueAsObject());
			return valueArrayToArrayValue(getFrameLocalValueArray(
					frame, listToStringArray(args[1].getValueAsString()), tid),
                    tid);
		}
		else if(name.equals("getFrameArgumentNameList"))
		{
			int frame = (int)(double)(Double)(args[0].getValueAsObject());
			return valueForPrimitive(stringArrayToList(getFrameArgumentNameArray(frame, tid)), tid);
		}
		else if(name.equals("getFrameArgumentTypeList"))
		{
			int frame = (int)(double)(Double)(args[0].getValueAsObject());
			return valueForPrimitive(stringArrayToList(getFrameArgumentTypeArray(
					frame, listToStringArray(args[1].getValueAsString()), tid)),
                    tid);
		}
		else if(name.equals("getFrameArgumentValueArray"))
		{
			int frame = (int)(double)(Double)(args[0].getValueAsObject());
			return valueArrayToArrayValue(getFrameArgumentValueArray(
					frame, listToStringArray(args[1].getValueAsString()), tid),
                    tid);
		}
		else if(name.equals("getFileIdPath"))
		{
			try
			{
				int id = (int)(double)(Double)(args[0].getValueAsObject());
				if(id < 0)
					return valueForPrimitive(Value.UNDEFINED, tid);
				for(SwfInfo info: _sess.getSwfs())
					for(SourceFile sf: info.getSourceList(_sess))
						if(sf.getId() == id)
							return valueForPrimitive(sf.getFullPath(), tid);
			} catch(Exception e) {}
			warn("didn't find path for file id");
		}
        else if(name.equals("setNoAckMode"))
        {
            _sendAcks = false;
        }
        else if(name.equals("setNonStopMode"))
        {
            _nonStop = true;
            dbg("turned on non-stop mode");
        }
        else if(name.equals("getNonStopMode"))
        {
            return valueForPrimitive(_nonStop, tid);
        }
        else if (name.equals("callInferiorFunc"))
        {
            int file = (int)(double)(Double)(args[0].getValueAsObject());
            int line = (int)(double)(Double)(args[1].getValueAsObject());
            int lookahead = 0;
            String funcName = null;
            while (funcName == null && lookahead < 100) {
                funcName = getFunctionFromLoc(file, line + lookahead++);
            }

            Value thiz = _sess.getWorkerSession(tid).getGlobal("this");
			Value response = _sess.getWorkerSession(tid).
                callFunction(thiz, functionNS() + "::" + funcName, new Value[0]);

            // ask stub to deal with the break event
            Value dbgHelper = getHelperForThread(tid);
            callHelper(dbgHelper, HELPER_GDB_BREAK_HANDLER_NAME, new Value[] { }, 0, tid);
        }
        else if (name.equals("setThread"))
        {
            int threadId = (int)(double)(Double)(args[0].getValueAsObject());
            dbg("setThread: " + threadId);
            if (threadId > 0) 
            {
                _currentIsolateId = threadId;
            }
        } 
        else if (name.equals("getThisThread")) 
        {
            dbg("getThisThread: " + tid);
            return valueForPrimitive(Integer.toString(tid), tid);
        } 
        else if (name.equals("getThread")) 
        {
            dbg("returning tid: " + getCurrentGDBThread());
            return valueForPrimitive(Integer.toString(getCurrentGDBThread()), 
                                        tid);
        }
        else if (name.equals("recordBreakpointAddress"))
        {
            long gdbAddr = (long)(double)(Double)(args[0].getValueAsObject());
            long actualAddr = (long)(double)(Double)(
                                                args[1].getValueAsObject());
            _fakeBreakpointMaps.add(gdbAddr);
            _fakeBreakpointMaps.add(actualAddr);
        }
        else if (name.equals("clearBreakpointAddress"))
        {
            long actualAddr = (long)(double)(Double)(
                                            args[0].getValueAsObject());
            int i;
            while ((i = _fakeBreakpointMaps.indexOf(actualAddr)) >= 0) 
            {
                _fakeBreakpointMaps.remove(i);
                _fakeBreakpointMaps.remove(i - 1);
            }
        }
        else if (name.equals("getBreakpointAddresses"))
        {
            Value[] vals = new Value[_fakeBreakpointMaps.size()];
            for (int i = 0; i < _fakeBreakpointMaps.size(); i++) 
            {
                vals[i] = valueForPrimitive(Long.toString(
                    _fakeBreakpointMaps.get(i)), tid);
            }
            return valueArrayToArrayValue(vals, tid);
        }
		else
			warn("unrecognized helper query: " + name);
		return valueForPrimitive(Value.UNDEFINED, tid);
	}
	
	// call an AS3 helper function
	// if the helper function returns a continuation, we make requested queries,
	// and then execute the continuation with the results (which may return another continuation!) 
	private Value callHelper(Value helper, String name, Value[] args, int defResumeCode, int tid) throws PlayerDebugException, IOException, InterruptedException
	{
		Value response;
		int resumeCode = defResumeCode;
		
		for(;;)
		{
			// call the function
            dbg("calling " + name + ", thread " + tid);
            dbg("helper: " + helper.getValueAsString());
            dbg("args: " + args);
            dbg("suspended? " + _sess.getWorkerSession(tid).isSuspended());
			response = _sess.getWorkerSession(tid).
                callFunction(helper, name, args);
			
			String responseTypeName = response.getTypeName();
            dbg("helper type: " + helper.getTypeName());
            dbg("response type: " + responseTypeName);
            dbg("response contents: " 
                + response.getValueAsString());
			
			// continuation?
			if(responseTypeName.startsWith(CONTINUATION_CLASS_BASE_NAME + "@") || 
					responseTypeName.indexOf("::" + CONTINUATION_CLASS_BASE_NAME + "@") >= 0)
			{
				// get the list of queries
                String queryString = response.getMemberNamed(_sess,
                        CONTINUATION_QUERY_STRING_NAME).getValue().
                        getValueAsString();
                dbg("(" + tid + ") qstring: " + queryString);
                String[] qStrings = queryString.split("\\Q#\\E");
                int queriesLength = qStrings.length;
				args = new Value[queriesLength];
				
				for(int i = 0; i < queriesLength; i++)
				{
					// parse the query of form: ["queryName", ...queryArgs]
                    String[] qParts = qStrings[i].split("\\Q:\\E");
                    String queryName = hexDecode(qParts[0]);
                    int queryLength = qParts.length - 2;
					Value queryArgs[] = new Value[queryLength-1];
					
					for(int j = 1; j < queryLength; j++) {
                        String argStr = hexDecode(qParts[j+1]);
                        try {
                            queryArgs[j-1] = valueForPrimitive(
                                Double.valueOf(argStr), tid);
                        } catch (NumberFormatException e) {
                            queryArgs[j-1] = valueForPrimitive(argStr, tid);
                        }
                    }
					
					args[i] = valueForPrimitive(Value.UNDEFINED, tid);
					
					// special case continue queries (to be applied after all continuations finish)
                    dbg("resume code: " + queryName);
					if(queryName.equals("resume"))
						resumeCode = 'r';
					else if(queryName.equals("suspend"))
						resumeCode = 's';
					else if(queryName.equals("stepInto"))
						resumeCode = 'i';
					else if(queryName.equals("stepOut"))
						resumeCode = 'o';
					else if(queryName.equals("stepOver"))
						resumeCode = 'v';
					else
						args[i] = helperQuery(queryName, queryArgs, tid);
				}
				// morph into continuation.exec(...args)
				name = CONTINUATION_EXEC_NAME;
				helper = response; // helper <= continuation
			}
			else
				break;
		}
        dbg("resumeCode: " + resumeCode);
		switch(resumeCode)
		{
		default:
			warn("unrecognized resume code in callHelper");
			break;
		case 0: // explicit no-op
		case 's': // already suspended, so no-op!
			break;
		case 'r':
            dbg("resuming " + tid);
            _sess.getWorkerSession(tid).resume();
            _runningIsolates.put(tid, true);
			break;
		case 'i':
            dbg("(" + tid + ") stepInto");
			_sess.getWorkerSession(tid).stepInto();;
			break;
		case 'o':
            dbg("(" + tid + ") stepOut");
			_sess.getWorkerSession(tid).stepOut();
			break;
		case 'v':
            dbg("(" + tid + ") stepOver");
			_sess.getWorkerSession(tid).stepOver();
			break;
		}
		return response;
	}
	
	private byte[] valueToByteArray(Value val, int tid) throws PlayerDebugException, IOException, InterruptedException
	{
		if(val != null)
		{
			byte[] r;
			if(val.getTypeName().startsWith("flash.utils::ByteArray@"))
			{
                Value dbgHelper = getHelperForThread(tid);
				String hex = callHelper(dbgHelper, HELPER_BYTEARRAY_TO_HEX_NAME, new Value[] { val }, 0, tid).getValueAsString();
				r = new byte[hex.length()/2];
				for(int i = 0; i < r.length; i++)
					r[i] = (byte)Integer.parseInt(hex.substring(i*2, i*2+2), 16);
			}
			else
				r = val.getValueAsString().getBytes();
				
			return r;
		}
		return null;
	}
	
	// "list" routines -- lists are made up of 0 or more strings separated by commas
	// constituent strings have '\\' and ',' escaped (with a preceeding '\\')
	// we use these in certain places to minimize debug protocol traffic
	
	private String stringArrayToList(String[] a)
	{
		StringBuilder sb = new StringBuilder();
		
		for(String s: a)
		{
			if(sb.length() > 0)
				sb.append(',');
			sb.append(s.replaceAll("\\\\", "\\\\").replaceAll(",", "\\,"));
		}
		return sb.toString();
	}
	
	private String[] listToStringArray(String l)
	{
		StringBuilder sb = new StringBuilder();
		List<String> rl = new LinkedList<String>();
		
		for(int i = 0; i < l.length(); i++)
		{
			char ch = l.charAt(i);
			if(ch == ',')
			{
				rl.add(sb.toString());
				sb = new StringBuilder();
				continue;
			}
			else if(ch == '\\')
				sb.append(l.charAt(++i));
			else
				sb.append(ch);
		}
		rl.add(sb.toString());
		return rl.toArray(new String[0]);
	}
	
	Value valueArrayToArrayValue(Value[] v, int tid) throws PlayerDebugException
	{
		if(v.length != 1) // if length == 1, there's an ambiguity where Array might think v[0] is a length
		  return _sess.getWorkerSession(tid).callConstructor("Array", v);
		// so create and then push
		Value r = _sess.getWorkerSession(tid).callConstructor("Array", new Value[0]);
		_sess.getWorkerSession(tid).callFunction(r, "push", v );
		return r;	
	}

    private Value getHelperForThread(int isolateId) throws PlayerDebugException 
    {
        if (!_dbgHelpers.containsKey(isolateId)) {
            return null;
        }
        return _dbgHelpers.get(isolateId);
    }
	
	// handle a gdb command 
	private void gdbHandle(byte[] payload, int tid) throws NotConnectedException, NotSuspendedException, NoResponseException, IOException, InterruptedException, SuspendedException
	{
		// delegate to helper
		Value response = null;
        String payloadStr = new String(payload);
        long startTime = 0;
        if (s_perf_log_level > 0)
        {
            startTime = System.currentTimeMillis();
        }
        dbg("gdbHandle (" + tid + ")");
		try
		{
            Value dbgHelper = getHelperForThread(tid);

			// default to remaining suspended
            dbg("about to callHelper (" + tid + ")");
			response = callHelper(dbgHelper, HELPER_GDB_PACKET_HANDLER_NAME, new Value[] { valueForPrimitive(payloadStr, tid) }, 's', tid);
            dbg("response: " + response);
		} catch (PlayerDebugException e) {
			warn("PlayerDebugException delegating gdb packet to helper: " + e.toString());
		}
		if(response != null && response.getTypeName().equals("undefined")) {
            dbg("gdbHandle(" + tid + "): inferior running");
            _infRunning.put(tid, true);
            return;
        }
		try
		{
			byte[] r = valueToByteArray(response, tid);
			
			if(r != null)
			{
				log("gdb response", new String(r));
                dbg("sent gdb1: " + new String(r));
				_gdbServer.putPacket(r);
			}
		} catch (PlayerDebugException e) {
			warn("PlayerDebugException delegating gdb packet to helper: " + e.toString());
		}
        dbg("gdbHandle(" + tid + "): inferior suspended");
        dbg("packet: " + new String(payload));
        _infRunning.put(tid, false);
        if (s_perf_log_level > 0 && startTime != 0)
        {
            _packetLog.add(packetName(payloadStr), 
                System.currentTimeMillis() - startTime);
        }
	}

    // Creates a simplified name for a packet, for performance reporting.
    private static String packetName(String packet)
    {
        int len = 15;
        if (packet.startsWith("m"))
        {
            return "m";
        }
        else if (packet.startsWith("vFile"))
        {
            len = 11;
        }
        return packet.substring(0, Math.min(packet.length(), len));
    }

	// handle a gdb command, potentially affecting multiple threads
	private void gdbHandle(byte[] payload) throws NotConnectedException, NotSuspendedException, NoResponseException, IOException, InterruptedException, SuspendedException
	{
        String cmd = new String(payload);
        dbg("bridge got command: " + cmd);
        if(cmd.startsWith("vCont;"))
        {
            dbg("received vcont: " + cmd);
            String[] actions = cmd.substring(6).split(";");
            String response = "";
            String defaultAction = null;
            Map<Integer, String> threadsToActions = 
                new HashMap<Integer, String>();

            for (String action : actions) 
            {
                String threadStr = action.substring(1);
                int tid;
                if (threadStr.startsWith(":")) 
                {
                    tid = Integer.parseInt(threadStr.substring(1), 16);
                    threadsToActions.put(tid, action.substring(0, 1));
                } 
                else 
                {
                    defaultAction = action;
                }
            }

            if (_nonStop)
            {
                _gdbServer.putPacket("OK");
                for (Integer tid : _dbgHelpers.keySet()) 
                {
                    String action = defaultAction;
                    if (threadsToActions.containsKey(tid)) 
                    {
                        action = threadsToActions.get(tid);
                    } 
                    if (action != null)
                    {
                        if ("t".equals(action)) 
                        {
                            if (!_sess.getWorkerSession(tid).isSuspended())
                            {
                                try 
                                {
                                    _sess.getWorkerSession(tid).suspend();
                                }
                                catch (NoResponseException e)
                                {
                                    // We can safely ignore this. gdb won't
                                    // receive the async stop reply it wants,
                                    // and will correctly assume the thread
                                    // is still running.
                                }
                            }
                        } 
                        else 
                        {
                            gdbHandle(action.getBytes(), tid);
                        }

                    }
                }
            }
            else
            {
                String action = defaultAction;
                if (threadsToActions.containsKey(getCurrentGDBThread()))
                {
                    action = threadsToActions.get(getCurrentGDBThread());
                } 
                gdbHandle(action.getBytes(), getCurrentIsolate());
            }
        }
        else if (cmd.startsWith("vStopped"))
        {
            _notifier.sendNextNotification();
        }
        else if (cmd.startsWith("Hg"))
        {
            _currentIsolateId = Integer.parseInt(cmd.substring(2), 16);
            dbg("using thread " + _currentIsolateId);
            if (_currentIsolateId == 0 || _currentIsolateId == -1)
            {
                _currentIsolateId = 1;
            }
            _gdbServer.putPacket("OK");
        }
        else if (cmd.startsWith("T"))
        {
            int tid = Integer.parseInt(cmd.substring(1), 16);
            if (_dbgHelpers.containsKey(tid))
            {
                _gdbServer.putPacket("OK");
            }
            else
            {
                _gdbServer.putPacket("E01");
            }
        }
        else if (cmd.startsWith("qfThreadInfo"))
        {
            Set<Integer> tids = _dbgHelpers.keySet();
            StringBuilder res = new StringBuilder();
            Iterator<Integer> i = tids.iterator();

            if (_nonStop)
            {
                while (i.hasNext())
                {
                    res.append(Integer.toHexString(i.next()));
                    if (i.hasNext())
                    {
                        res.append(",");
                    }
                }
            }

            String resp = "l";
            if (res.length() > 0)
            {
                resp = "m" + res.toString();
            }
            _gdbServer.putPacket(resp);
        }
        else if (cmd.startsWith("qsThreadInfo"))
        {
            _gdbServer.putPacket("l");
        }
        else if (cmd.startsWith("qP") || cmd.startsWith("qThreadExtraInfo"))
        {
            // These packets are not implemented, but are caught here so that
            // gdbHandle does not try to delegate them to Actionscript, which
            // may not work if no isolates are currently suspended.
            // gdb may emit these packets in response to "info threads".
            _gdbServer.putPacket("");
        }
        else if (_nonStop && cmd.startsWith("?"))
        {
            _notifier.resendAllNotifications();
        }
        else if (cmd.startsWith("qRcmd"))
        {
            String cmdName = hexDecode(cmd.substring(6));
            boolean sendOK = false;
            if (cmdName.startsWith("monitor set-perf-log")) 
            {
                s_perf_log_level = Integer.valueOf(cmdName.substring(21));
                sendOK = true;
            }
            else if (cmdName.startsWith("monitor show-perf-log"))
            {
                StringBuilder str = new StringBuilder();
                str.append(_packetLog.toString() + "\n\n");
                str.append(_cmdLog.toString() + "\n");
                str.append(_eventLog.toString() + "\n");
                _gdbServer.putPacket(hexEncode(str.toString()));
            }
            else
            {
                gdbHandle(payload, getCurrentIsolate());
            }

            if (sendOK)
            {
                _gdbServer.putPacket("OK");
            }
        }
        else 
        {
            dbg("suspended? " +
                _sess.getWorkerSession(getCurrentIsolate()).isSuspended());
            gdbHandle(payload, getCurrentIsolate());
        }
	}

    private static void dbg(String msg)
    {
        if (false) {
            System.err.println(msg);
        }
    }

    private void dbg(String msg, boolean force)
    {
        if (force) {
            System.err.println(msg);
        }
    }
	
	private void warn(String msg)
	{
		log("warning", msg);
	}
	
	private void log(String type, String data)
	{
		if(s_logging)
			System.err.println(type + ": " + data);
	}
	
	private void log(DebugEvent event)
	{
		log(event.getClass().getSimpleName(), event.information);
	}
	
	private void trace(String msg)
	{
		if(!s_quiet)
			System.out.println(msg);
	}
	
	// are we currently in a debug helper function?
	// if helper is non-null, verify that's the current AS3 this
	// otherwise see if AS3 this looks like a debug helper
	public boolean inDebugHelper(Value helper, int tid)
	{
		try
		{
			Value thiz = _sess.getWorkerSession(tid).getValue(Value.THIS_ID);

			if(helper != null)
				return helper.getId() == thiz.getId();
			
			String typeName = thiz.getTypeName();
            String qName = _as3ns + HELPER_CLASS_FULLY_QUALIFIED_NAME;

			return typeName.startsWith(qName + "@");
		}
		catch(Exception e)
		{
		}
		return false;
	}
	
	private static final String VERSION_STRING = "Flascc Debug Bridge 1.0";
	private static final String HELPER_BYTEARRAY_TO_HEX_NAME = "hexBA";
	private static final String HOOK_REQUEST_CLASS_BASE_NAME = "AlcDbgHookRequest";
	private static final String THREAD_READY_CLASS_BASE_NAME = "AlcDbgThreadReadyNotification";
	private static final String HELPER_CLASS_FULLY_QUALIFIED_NAME = "::AlcDbgHelper";
	private static final String CONTINUATION_CLASS_BASE_NAME = "AlcDbgContinuation";
	private static final String CONTINUATION_QUERY_ARRAY_NAME = "queries";
	private static final String CONTINUATION_QUERY_STRING_NAME = "queryString";
	private static final String CONTINUATION_EXEC_NAME = "exec";
	private static final String HELPER_GDB_PACKET_HANDLER_NAME = "handleGDBPacket";
	private static final String HELPER_GDB_BREAK_HANDLER_NAME = "handleBreak";
	private static final String HELPER_DEBUG_HOOK_NAME = "hook";

    static String StringToHexString(String str) {
        StringBuilder sb = new StringBuilder();
        for (byte b : str.getBytes()) {
            sb.append(Integer.toHexString((int)b));
        }
        return sb.toString();
    }

    private static int numWorkerBreaks = 0;
	
	// run the bridge!
	public void run(int gdbPort, String as3ns) throws InterruptedException, IOException, NotConnectedException, NotSuspendedException, NoResponseException, SuspendedException, InProgressException, NotSupportedException, PlayerDebugException
	{
        _as3ns = as3ns;
		// Session is live at this point
		trace("Player connected");

		// general debug event queue
		BlockingQueue<DebugEvent> eventQ = new LinkedBlockingQueue<DebugEvent>();
		// copy session debug events into our queue
		SessDebugEventHelper sessDebugEventHelper  = new SessDebugEventHelper(_sess, eventQ);
		sessDebugEventHelper.start();
		_gdbServer = new GDBServer(gdbPort, eventQ);
		// does gdb think the inferior is running?
		boolean gdbInfRunning = false;
        _infRunning.put(1, false);
		// about to break as a result of an exception that is going to be caught?
		//boolean breakingFromExceptFaultThatWillBeCaught = false;
		// last exception's type name
		//String lastExceptionTypeName = "";
		// we break immediately -- ignore that one
		boolean firstBreakEvent = true;
		
		// we use this to catch flascc hooks early
        try {
            _sess.breakOnCaughtExceptions(true);
        } catch (NotSupportedException e) {
            e.printStackTrace();
            throw e;
        }

		for(;;)
		{
            long startTime = 0;
            if (s_perf_log_level > 0)
            {
                startTime = System.currentTimeMillis();;
            }
			DebugEvent event = eventQ.take();
            dbg("looking at event: " + event);
			
			if(event instanceof HelperThreadExitEvent)
			{
				HelperThreadExitEvent hte = (HelperThreadExitEvent)event;
				String msg = (hte.getThread() == _gdbServer) ? "GDB disconnected" :
					(hte.getThread() == sessDebugEventHelper) ? "Player disconnected" : "Unknown helper thread exited";
				log("HelperThreadExitEvent", msg);
				trace(msg);
				System.exit(0);
			}
			else if(event instanceof TraceEvent)
			{
				TraceEvent te = (TraceEvent)event;
				trace("[" + te.information + "]");
                dbg("[" + te.information + "]");
				//_gdbServer.putPacket("O" + StringToHexString(te.information + "\n"));
			}
			else if(event instanceof GDBConnectEvent)
			{
				trace("GDB connected");
			}
			else if(event instanceof SwfLoadedEvent)
			{
				trace("SWF loaded");
			}
			else if(event instanceof SwfUnloadedEvent)
			{
				trace("SWF unloaded");
			}
			else if(event instanceof ExceptionFault)
			{
				ExceptionFault ef = (ExceptionFault)event;
				log(ef);
				Value tv = ef.getThrownValue();
                int tid = ef.isolateId;
                dbg("exception from thread: " + tid);

				//lastExceptionTypeName = tv.getTypeName();
                _lastException.put(tid, tv.getTypeName());
                dbg("exception value: " + tv.getValueAsString());
				if(ef.willExceptionBeCaught()) {
                    dbg("except will be caught");
                    _breakingFromCaught.put(tid, true);
                }
					//breakingFromExceptFaultThatWillBeCaught = true;
			}
			else if(event instanceof BreakEvent)
			{
                BreakEvent be = (BreakEvent) event;
                int tid = be.isolateId;

                // When in all-stop mode, ignore breaks from isolates other
                // than the primordial isolate and the isolate corresponding
                // to the main C thread. 
                if (!_nonStop && tid != 1 && tid != _mainCThreadIsolate &&
                        _mainCThreadIsolate != -1) {
                    _sess.getWorkerSession(tid).resume();
                    continue;
                }

                _runningIsolates.put(tid, false);

                dbg("break from thread " + tid);
                dbg("suspended? " + _sess.getWorkerSession(tid).isSuspended());
                Value dbgHelper = null;
                int reason = -1;

                try {
                    // Can't call this due to fdb throwing exception
                    /*
                    Frame[] frames = _sess.getWorkerSession(tid).getFrames();

                    for (Frame f : frames) {
                        dbg(f.getCallSignature());
                    }
                    */

                    reason = _sess.getWorkerSession(tid).suspendReason();

                    /*
                    if (reason == 1) {
                        Location loc = frames[0].getLocation();
                        SourceFile f = loc.getFile();
                        int fid = (f == null) ? -1 : f.getId();
                        dbg("broke at " + fid + ":" + loc.getLine());
                    }
                    */
                    dbg("break reason: " + reason);
                } catch (Exception e) {
                    e.printStackTrace();
                }

                /* fdb can set breakpoints on non-primordial workers, but
                 * it cannot clear them. So if it hits a breakpoint on
                 * a non-primordial worker, skip it if it been cleared.
                 */
                if (tid != 1 && reason == SuspendReason.Breakpoint) 
                {
                    Frame[] frames = _sess.getWorkerSession(tid).getFrames();
                    boolean shouldContinue = false;
                    for (Location loc : _clearedBreakpoints) 
                    {
                        if (frames.length > 0 && locationsEqual(loc,
                                    frames[0].getLocation()))
                        {
                            _sess.getWorkerSession(tid).resume();
                            shouldContinue = true;
                            break;
                        }
                    }
                    if (shouldContinue) 
                    {
                        dbg("skipping breakpoint");
                        continue;
                    }
                }
                
                if (tid != 1 && reason == SuspendReason.Fault) 
                {
                    Frame[] frames = _sess.getWorkerSession(tid).getFrames();
                    if (frames.length > 0) 
                    {
                        if (isolateAppearsToBeTerminating(
                                frames[0].getCallSignature(),
                                _lastException.get(tid)))
                        {
                            _runningIsolates.put(tid, true);
                            _sess.getWorkerSession(tid).resume();
                            handleIsolateExit(tid);
                            continue;
                        }
                    }
                }

                if (!_dbgHelpers.isEmpty()) {
                    dbgHelper = getHelperForThread(tid);
                }
                if (dbgHelper == null) {
                    dbg("couldn't find helper");
                } else {
                    dbg("got a helper");
                }
                
                if (tid > 1 && !_brokenIsolates.contains(tid)) {
                    dbg("skipping first break from thread " + tid);
                    _brokenIsolates.add(tid);
                    _runningIsolates.put(tid, true);
                    _sess.getWorkerSession(tid).resume();
                    continue;
                }

                dbg("inf running on thread " + tid + ": " 
                    + _infRunning.get(tid));
				if(_breakingFromCaught.containsKey(tid) && _breakingFromCaught.get(tid)) // the debug hook will throw a (caught) exception to get our attention
				{
                    _breakingFromCaught.put(tid, false);
                    String lastExcept = _lastException.get(tid);
                    dbg("caught exception: " +
                        lastExcept);

					if((lastExcept.startsWith(HOOK_REQUEST_CLASS_BASE_NAME + "@") ||
							lastExcept.indexOf("::" + HOOK_REQUEST_CLASS_BASE_NAME + "@") >= 0) &&
							inDebugHelper(dbgHelper, tid)) // hook in a debug helper -- attach or service requests
					{
						log("debug hook", "");
                        dbg("calling debug hook");

						if(dbgHelper == null) // first time in the hook
						{
                            if (!_gdbServer.isAlive()) {
                                _gdbServer.start(); // start listening for gdb
                                trace("Listening for GDB connection");
                            }
							dbgHelper = _sess.getWorkerSession(tid).getValue(Value.THIS_ID);
                            _dbgHelpers.put(tid, dbgHelper);
                            dbg("helper for " + tid);
                            if (tid != 1 && _mainCThreadIsolate == -1) {
                                _mainCThreadIsolate = tid;
                            }

                            dbg("set up helper for " + tid);
						}
						try
						{
							// default to resuming
                            if (dbgHelper != null) {
                                callHelper(dbgHelper, HELPER_DEBUG_HOOK_NAME, new Value[0], 'r', tid);
                            }
						}
						catch (PlayerDebugException e)
						{
							warn("PlayerDebugException while calling debugger hook: " + e.toString());
                            _runningIsolates.put(tid, true);
							_sess.getWorkerSession(tid).resume(); // resume (with fingers crossed)
						}
					}
					else 
                    {
                        dbg("resuming thread " + tid);
                        _runningIsolates.put(tid, true);
                        _sess.getWorkerSession(tid).resume(); // resume
                        dbg("done resuming " + tid);
                    }
				}
				else if((_nonStop && _infRunning.get(tid)) || gdbInfRunning) // inform gdb if it thinks we're running
				{
                    dbg("turning off inferior on thread " + tid);
                    _infRunning.put(tid, false);
                    gdbInfRunning = false;
                    dbg("calling gdb from thread " + tid);

					try
					{
						// ask stub to deal with the break event
                        callHelper(dbgHelper, HELPER_GDB_BREAK_HANDLER_NAME, new Value[] { }, 0, tid);
					}
					catch (PlayerDebugException e)
					{
						warn("PlayerDebugException while signaling a break: " + e.toString());
					}
				}
				else // spurious break
				{
					if(!firstBreakEvent) // first one? we break on entry, so just continue
						warn("BreakEvent while inf isn't running");  // fingers crossed!
                    dbg("fell through and resuming " + tid);
                    _runningIsolates.put(tid, true);
                    _sess.getWorkerSession(tid).resume();
				}
				firstBreakEvent = false;
			}
			else if(event instanceof GDBPacketEvent)
			{
				GDBPacketEvent gevent = ((GDBPacketEvent)event);
				
				log(event);
                dbg("handling: " + 
                    new String(gevent.getPayload()));
                gdbHandle(gevent.getPayload());
                gdbInfRunning = _infRunning.get(_currentIsolateId);
                dbg("handled gdb event, inf: " + gdbInfRunning + " (" +
                    _currentIsolateId + ")");
			}
			else if(event instanceof GDBBreakEvent) 
            {
                dbg("gdbbreak " + getCurrentIsolate());
                _sess.getWorkerSession(getCurrentIsolate()).suspend();
			} 
            else if (event instanceof IsolateCreateEvent) 
            {
                IsolateCreateEvent ice = (IsolateCreateEvent) event;
                dbg("ICE: " + ice.isolate.getId());

                // Work around apparent race condition in fdb.
                // The call after this will sometimes throw an NPE
                // without this sleep.
                Thread.sleep(500);

                _sess.getWorkerSession(ice.isolate.getId()).
                    breakOnCaughtExceptions(true);

                for (Location loc : _breakpoints) 
                {
                    SourceFile sf = loc.getFile();
                    int fileID = (sf == null) ? -1 : sf.getId();
                    int lineNum = loc.getLine();
                    Location res = _sess.getWorkerSession(ice.isolate.getId()).
                        setBreakpoint(fileID, lineNum);
                    dbg("(" + ice.isolate.getId() + ")breakpoint set at " 
                        + fileID + ":" + lineNum + "(" + res + ")");
                }
                _infRunning.put(ice.isolate.getId(), true);
            }
            else if (event instanceof IsolateExitEvent) 
            {
                IsolateExitEvent iee = (IsolateExitEvent) event;
                dbg("IEE: " + iee.isolate.getId());
                handleIsolateExit(iee.isolate.getId());
            }
            else
				log(event);

            if (s_perf_log_level > 0 && startTime != 0)
            {
                String eventName = event.getClass().getName();
                eventName = eventName.substring(eventName.lastIndexOf(".") + 1);
                _eventLog.add(eventName,
                                System.currentTimeMillis() - startTime);
            }
		}
	}

    private void handleIsolateExit(int id)
    {
        dbg("thread " + id + " terminated");
        if (_currentIsolateId == id) 
        {
            _currentIsolateId = 1;
        }

        if (id == _mainCThreadIsolate) 
        {
            _mainCThreadIsolate = -2;
        }
        _dbgHelpers.remove(id);
    }

    private static boolean locationsEqual(Location loc1, Location loc2) 
    {
        if (loc1 == loc2) 
        {
            return true;
        } 
        else if (loc1 == null || loc2 == null) 
        {
            return false;
        } 
        else 
        {
			SourceFile sf1 = loc1.getFile();
			int fid1 = (sf1 == null) ? -1 : sf1.getId();
			int line1 = loc1.getLine();
			SourceFile sf2 = loc2.getFile();
			int fid2 = (sf2 == null) ? -1 : sf2.getId();
			int line2 = loc2.getLine();

            if (fid1 == -1 || fid2 == -1) 
            {
                return false;
            } 
            else 
            {
                return fid1 == fid2 && line1 == line2;
            }
        }
    }

	public static void run(Session s, int gdbPort, String as3ns) throws NotConnectedException, NotSuspendedException, NoResponseException, SuspendedException, InProgressException, NotSupportedException, InterruptedException, IOException, PlayerDebugException
	{
		new AlcDB(s).run(gdbPort, as3ns);
	}
	
	static boolean s_logging = false;
	static boolean s_quiet = false;
    static long s_perf_log_level = 0;
	
	public static void usage()
	{
		System.out.println("alcdb [-port NN] [-timeout NN] [-v] [-q] [-n NS]");
		System.exit(0);
	}
	
	public static void main(String[] args) {
		try
		{
			int port = 6666; // default port
			int timeout = 300000; // long (5 min) timeout by default
            String as3ns = "com.adobe.flascc";
			
			for(int argNo = 0; argNo < args.length; argNo++)
			{
				if(args[argNo].equals("-port"))
					port = Integer.valueOf(args[++argNo]);
				else if(args[argNo].equals("-v"))
					s_logging = true;
				else if(args[argNo].equals("-q"))
					s_quiet = true;
				else if(args[argNo].equals("-timeout"))
					timeout = Integer.valueOf(args[++argNo]);
                else if(args[argNo].equals("-n"))
                    as3ns = args[++argNo];
                else if (args[argNo].equals("-p"))
                    s_perf_log_level = Integer.valueOf(args[++argNo]);
				else
					usage();
			}

			SessionManager sm = Bootstrap.sessionManager();
			sm.setPreference(SessionManager.PREF_RESPONSE_TIMEOUT, timeout);
			sm.setPreference(SessionManager.PREF_GETVAR_RESPONSE_TIMEOUT, timeout);
			sm.setPreference(SessionManager.PREF_ACCEPT_TIMEOUT, timeout);

            // This values controls how long fdb will wait when attempting
            // to suspend a worker. It is set to 5 seconds because we want
            // to recover from attempts to suspend blocked threads
            // reasonably quickly, but we also want to allow a blocked thread
            // to wake up after waiting up to 3 seconds (as long as no other
            // thread is holding the thread arbitration lock).
            sm.setPreference(SessionManager.PREF_SUSPEND_WAIT, 5000);

            //sm.setPreference("$debug_message_file", 1);
			sm.startListening();
			
			Session s = sm.accept(null);
			
			sm.startListening();
			
			if(s.bind())
			{
                s.setPreference("$flash_script_timeout", 3600000);
				run(s, port, as3ns);
				s.unbind();
			}
		}
		catch(Exception e)
		{
			e.printStackTrace();
		}
	}

}
