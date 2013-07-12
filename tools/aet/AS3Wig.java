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
import java.util.*;

import com.adobe.flash.abc.*;
import com.adobe.flash.abc.visitors.*;
import com.adobe.flash.abc.semantics.*;

import static com.adobe.flash.abc.ABCConstants.*;

public class AS3Wig {

	static int CONSTR_VARARGS = 10;
	static String outName;
	static PrintStream outHeader, outImpl;
	
	static String moveToNS(String nsNew, String nsCur)
	{
		// TODO optimize for partial overlaps?
		String s[];
		s = nsCur.split("\\:\\:");
		for(int i = s.length - 1; i >= 0; i--)
			if(s[i].length() != 0)
				outHeader.println("} //" + s[i]);
		s = nsNew.split("\\:\\:");
		for(int i = 0; i < s.length; i++)
			if(s[i].length() != 0)
				outHeader.println("namespace " + s[i] + " {");
		return nsNew;
	}
	
	static String namespaceToString(Namespace ns, String sep, boolean finalSep)
	{
		String s[] = ns.getName().split("\\.");
		if(s.length == 0 || s.length == 1 && s[0].equals(""))
			return "";
		String r = s[0];
		for(int i = 1; i < s.length; i++)
			r = r + sep + s[i];
		return r + (finalSep ? sep : "");
	}
	
	static String namespaceToString(Namespace ns, String sep)
	{
		return namespaceToString(ns, sep, false);
	}

	static String namespaceToString(Namespace ns, boolean finalColons)
	{
		return namespaceToString(ns, "::", finalColons);
	}

	static String namespaceToString(Namespace ns)
	{
		return namespaceToString(ns, "::", false);
	}
	
	static Set<String> getReserved()
	{
		// TODO all c++ reserved words + other junk we want to prevent from becoming names
		Set<String> set = new HashSet<String>();
		set.add("_id");
		set.add("int");
		set.add("void");
		set.add("_new");
		set.add("namespace");
		set.add("union");
		set.add("register");
		set.add("struct");
		set.add("vector");
		set.add("BIG_ENDIAN");
		set.add("LITTLE_ENDIAN");
		set.add("_BIG_ENDIAN");
		set.add("_LITTLE_ENDIAN");
		return set;
		
	}
	
	static boolean publicName(Name n)
	{
		if(n.getQualifiers().length() != 1)
			return false;
		switch(n.getSingleQualifier().getKind())
		{
		case CONSTANT_Namespace:
		case CONSTANT_PackageNs:
			break;
		default:
			return false;
		}
		return true;
	}
	
	static class Uniquer
	{
		Map<Object, Set<String>> _used  = new HashMap<Object, Set<String>>();
		Map<Object, Map<String, String>> _mapping = new HashMap<Object, Map<String, String>>();
		Set<String> _rsvd;
		
		public Uniquer(Set<String> rsvd)
		{
			_rsvd = rsvd;
		}
		
		public Uniquer()
		{
			_rsvd = getReserved();
		}
		
		public String get(Object ns, String s)
		{
			if(!_mapping.containsKey(ns) || !_mapping.get(ns).containsKey(s))
				return unique(ns, s);
			return _mapping.get(ns).get(s);
		}
		
		public String unique(Object ns, String s)
		{
			Set<String> used;
			Map<String, String> mapping;
			if(!_used.containsKey(ns))
			{
				used = new HashSet<String>();
				used.addAll(_rsvd);
				mapping = new HashMap<String, String>();
				_mapping.put(ns, mapping);
			}
			else
			{
				used = _used.get(ns);
				mapping = _mapping.get(ns);
			}
			
			String u = s;
			
			if(u != null && u.startsWith("AS3_")) // reserve anything starting w/ AS3_ since we use it in a lot of macros
				u = "_" + u;
			while(used.contains(u))
				u = "_" + u;
			used.add(u);
			mapping.put(s, u);
			return u;
		}
	}
	
	static Uniquer uniquer = new Uniquer();
	
	static class TypeDesc
	{
		public final Namespace namespace;
		public final String name;
		public final String as3name;
		
		public TypeDesc(Namespace namespace, String name)
		{
			this.namespace = namespace;
			this.name = uniquer.get(namespace, name);
			this.as3name = name;
		}
		
		public TypeDesc(Name name)
		{
			if(name == null)
			{
				this.namespace = null;
				this.name = null;
				this.as3name = null;
			}
			else
			{
				this.namespace = name.getSingleQualifier();
				this.name = uniquer.get(this.namespace, name.getBaseName());
				this.as3name = name.getBaseName();
			}
		}

		public boolean isBuiltin(String s)
		{
			return (as3name != null && as3name.equals(s)) && (namespace != null && namespace.getName().equals(""));
		}

		public boolean isVoid()
		{
			return isBuiltin("void");
		}
		
		public boolean isScalar()
		{
			if(name == null || namespace == null || !namespace.getName().equals(""))
				return false;
			return as3name.equals("int")
				|| as3name.equals("uint")
				|| as3name.equals("Number")
				|| as3name.equals("Boolean");
		}
		
		public String toString()
		{
			return toString(true);
		}
		
		public String toString(boolean useScalar)
		{
			if(namespace == null || name == null)
				return "AS3_NS::var";
			
			if(as3name.equals("void") && namespace.getName().equals(""))
				return "void";

			if(useScalar && namespace.getName().equals(""))
			{
				if(as3name.equals("int"))
					return "int";
				if(as3name.equals("uint"))
					return "unsigned";
				if(as3name.equals("Number"))
					return "double";
				if(as3name.equals("Boolean"))
					return "bool";
			}
			
			String nss = namespaceToString(namespace);
			if(nss.length() == 0)
				return "AS3_NS::" + name;
			else
				return "AS3_NS::" + nss + "::" + name;
		}
	}
	
	static class FuncDesc
	{
		public final Namespace namespace;
		public final String name;
		public final String as3name;
		public final TypeDesc retType;
		public final TypeDesc[] argTypes;
		public final String[] argDefaults;
		public final boolean isVarArgs;
		
		private static String pooledValueToString(TypeDesc type, PooledValue value)
		{
			switch(value.getKind())
			{
			case CONSTANT_Int:
				return type.isScalar() ? value.getIntegerValue().toString() : "AS3_NS::internal::new_int(" + value.getIntegerValue() + ")";
			case CONSTANT_UInt:
				return type.isScalar() ? value.getLongValue().toString() : "AS3_NS::internal::new_uint(" + value.getLongValue() + ")";
			case CONSTANT_Double:
				{
					Double d = value.getDoubleValue();
					String v = d.isNaN() ? "(0./0.)" : d.isInfinite() ? ((double)d < 0) ? "(-1./0.)" : "(1./0.)" : value.getDoubleValue().toString();
					return type.isScalar() ? v : "AS3_NS::internal::new_Number(" + v + ")";
				}
			case CONSTANT_Utf8:
				return type.isBuiltin("String") ? "\"" + value.getStringValue()  + "\"" : "AS3_NS::internal::new_String(\"" + value.getStringValue() + "\")"; // TODO escape quotes?
			case CONSTANT_True:
				return type.isScalar() ? "true" : "AS3_NS::internal::_true";
			case CONSTANT_False:
				return type.isScalar() ? "false" : "AS3_NS::internal::_false";
			case CONSTANT_Undefined:
				return type.isScalar() ? "AS3_NS::Number::NaN" : "AS3_NS::internal::_undefined";
			case CONSTANT_Null:
				return "*(" + type.toString() + "*)&AS3_NS::internal::_null";//<" + type.toString() + ">()";
			default: // Namespace
				return "AS3_NS::internal::new_Namespace(\"" + value.getNamespaceValue().getName() + "\")";
			}
		}

		public FuncDesc(Namespace namespace, String name, TypeDesc retType, TypeDesc[] argTypes, String[] argDefaults, boolean isVarArgs)
		{
			this.namespace = namespace;
			this.name = uniquer.get(namespace, name);
			this.as3name = name;
			this.retType = retType;
			this.argTypes = argTypes;
			this.argDefaults = argDefaults;
			this.isVarArgs = isVarArgs;
		}
		
		public FuncDesc(Namespace namespace, String name, MethodInfo mi)
		{
			this.namespace = namespace;
			this.name = uniquer.get(namespace, name);
			this.as3name = name;
			this.retType = new TypeDesc(mi.getReturnType());
			Vector<Name> argTypeV = mi.getParamTypes();
			this.argTypes = new TypeDesc[argTypeV.size()];
			for(int i = 0; i < this.argTypes.length; i++)
				this.argTypes[i] = new TypeDesc(argTypeV.get(i));

			this.argDefaults = new String[this.argTypes.length];

			Vector<PooledValue> defValues = mi.getDefaultValues();
			int j = defValues.size()-1;

			for(int i = this.argTypes.length-1; i >= 0; i--, j--)
				this.argDefaults[i] = (j >= 0) ? (" = " + pooledValueToString(this.argTypes[i], defValues.get(j))) : "";
			this.isVarArgs = (mi.getFlags() & NEED_REST) != 0;
		}
		
		public FuncDesc(TypeDesc type, MethodInfo mi) // constructor
		{
			boolean isDate = type.isBuiltin("Date"); // HACK for Date!!!
			this.namespace = null;
			this.name = "_new";
			this.as3name = null;
			this.retType = type;
			Vector<Name> argTypeV = mi.getParamTypes();

			if(isDate) // HACK!
			{
				this.argTypes = new TypeDesc[0];
				this.argDefaults = new String[0];
			}
			else
			{
				this.argTypes = new TypeDesc[argTypeV.size()];
				for(int i = 0; i < this.argTypes.length; i++)
					this.argTypes[i] = new TypeDesc(argTypeV.get(i));

				this.argDefaults = new String[this.argTypes.length];

				Vector<PooledValue> defValues = mi.getDefaultValues();
				int j = defValues.size()-1;

				for(int i = this.argTypes.length-1; i >= 0; i--, j--)
					this.argDefaults[i] = (j >= 0) ? (" = " + pooledValueToString(this.argTypes[i], defValues.get(j))) : "";
			}
			this.isVarArgs = ((mi.getFlags() & NEED_REST) != 0) || isDate /* HACK! Date construction is a hack in the VM -- see Date.as */;
		}
		
		public int argsSize(ClassDesc cd, int numConstructorVA)
		{
			int size = (cd != null) ? 4 : 0; // this
			size += (retType.isScalar() || retType.isVoid()) ? 0 : 4; // hidden ptr for returning non-scalar
			for(int i = 0; i < argTypes.length; i++)
			{
				if(argTypes[i].isBuiltin("Number"))
					size += 8;
				else
					size += 4;
			}
			if(hasByteArrayArg() || cd != null && cd.isByteArray())
				size += 4; // ramPos
			if(isVarArgs)
			{
				if(as3name != null)
					size += 8; // restCount, rest
				size += 4 * numConstructorVA;
			}
			
			return size;
		}
		
		public boolean hasByteArrayArg()
		{
			for(int i = 0; i < argTypes.length; i++)
				if(argTypes[i].as3name != null && argTypes[i].as3name.equals("ByteArray") &&
						argTypes[i].namespace.getName().equals("flash.utils"))
					return true;
			return false;
		}
		
		public String getArgList(ClassDesc cd, boolean types, boolean argNames, int numConstructorVA)
		{
			String result = "";
			
			for(int i = 0; i < argTypes.length; i++)
			{
				if(result.length() != 0)
					result = result + ",";
				if(types)
					result = result + " " + argTypes[i];
				if(argNames) {
					result = result + " arg" + i;
				} else {
					// FIXME: clang does not support default values for forward delclared types. Disable them for now
					// result = result + argDefaults[i];
				}
			}
			if(isVarArgs)
			{
				if(as3name == null) // constructor
				{
					for(int i = 0; i < numConstructorVA; i++)
					{
						if(result.length() != 0)
							result = result + ",";
						if(types)
							result = result + " var";
						if(argNames)
							result = result + " rest" + i;
					}
				}
				else
				{
					if(result.length() != 0)
						result = result + ",";
					if(types)
					{
						if(argNames)
							result = result + " int restCount, var *rest";
						else
							result = result + " int = 0, var * = NULL";
					}
					else
						result = result + " restCount, rest";
				}
			}
			if(hasByteArrayArg() || cd != null && cd.isByteArray())
			{
				if(result.length() != 0)
					result = result + ",";
				if(types)
				{
					if(argNames)
						result = result + " void *ramPos";
					else
						result = result + " void * = NULL";
				}
				else
					result = result + " ramPos";
			}
			return result;
		}
		
		public String toString()
		{
			String result = this.retType + " " + this.name + "(";
			for(int i = 0; i < argTypes.length; i++)
			{
				if(i != 0)
					result += ", ";
				result += argTypes[i];
			}
			if(isVarArgs)
			{
				if(argTypes.length != 0)
					result += ", ";
				result += "...";
			}
			result += ");\n";
			return result;
		}
	}

	static class PropDesc implements Comparable<PropDesc>
	{
		public final Namespace namespace;
		public final String name;
		public final String as3name;
		public final TypeDesc type;
		public final int flags;
		
		public static final int FL_getter = 1;
		public static final int FL_setter = 2;
		public static final int FL_const = 4;
		
		public PropDesc(Namespace namespace, String name, TypeDesc type, int flags)
		{
			this.namespace = namespace;
			this.name = uniquer.get(namespace, name);
			this.as3name = name;
			this.type = type;
			this.flags = flags;
		}
		
		public boolean canGet()
		{
			return (flags & FL_setter) == 0 || (flags & FL_getter) != 0;
		}
		
		public boolean canSet()
		{
			return (flags & FL_setter) != 0 || (flags & (FL_getter | FL_const)) == 0;
		}
		
		public String toString()
		{
			String result = "";
			if((flags & FL_const) != 0)
				result += "const ";
			if((flags & FL_getter) != 0)
				result += type + " get " + name + ";\n";
			if((flags & FL_setter) != 0)
				result += type + " set " + name + ";\n";
			if((flags & (FL_getter | FL_setter)) == 0)
				result += type + " " + name + ";\n";
			return result;	
		}

		@Override
		public int compareTo(PropDesc pdOther) {
			int cmp = namespace.toString().compareTo(pdOther.namespace.toString());
			if(cmp != 0)
				return cmp;
			cmp = name.compareTo(pdOther.name);
			return cmp;
		}
		
		static PropDesc[] merge(PropDesc[] properties)
		{
			// merge getters/setters
			Arrays.sort(properties);
			
			List<PropDesc> mergedProps = new ArrayList<PropDesc>();
			
			for(PropDesc pd: properties)
			{
				int lastIndex = mergedProps.size()-1;
				// identical to last? merge
				if(lastIndex >= 0 && pd.compareTo(mergedProps.get(lastIndex)) == 0)
				{
					PropDesc last = mergedProps.get(lastIndex);
					mergedProps.set(lastIndex, new PropDesc(pd.namespace, pd.name, pd.type, pd.flags | last.flags));
				}
				else
					mergedProps.add(pd);
			}
			return mergedProps.toArray(new PropDesc[0]);
		}
		
	}
	
	static class ClassDesc
	{
		public final Namespace namespace;
		public final String name;
		public final String as3name;
		
		public final TypeDesc superType;
		
		public final FuncDesc[] classMethods;
		public final PropDesc[] classProps;
		public final FuncDesc constructor;
		public final FuncDesc[] instanceMethods;
		public final PropDesc[] instanceProps;
		
		public ClassDesc(Namespace namespace, String name, TypeDesc superType, FuncDesc[] classMethods, PropDesc[] classProps, FuncDesc constructor, FuncDesc[] instanceMethods, PropDesc[] instanceProps)
		{
			this.namespace = namespace;
			this.name = uniquer.get(namespace, name);
			this.as3name = name;
			this.superType = superType;
			this.classMethods = classMethods;
			this.classProps = PropDesc.merge(classProps);
			this.constructor = constructor;
			this.instanceMethods = instanceMethods;
			this.instanceProps = PropDesc.merge(instanceProps);
		}
		
		public boolean isByteArray()
		{
			return as3name != null && as3name.equals("ByteArray") &&
					namespace.getName().equals("flash.utils");
		}
		

		public String toString()
		{
			String result = "class " + name + " {\n";
			for(int i = 0; i < classMethods.length; i++)
				result += "static " + classMethods[i];
			result += "static {\n";
			for(int i = 0; i < classProps.length; i++)
				result += classProps[i];
			result += "}\n";
			result += constructor;
			for(int i = 0; i < instanceMethods.length; i++)
				result += instanceMethods[i];
			for(int i = 0; i < instanceProps.length; i++)
				result += instanceProps[i];
			result += "};\n";
			return result;
		}
	}

	static class ABCDesc
	{
		public final ClassDesc[] classes;
		public final FuncDesc[] functions;
		public final PropDesc[] properties;
		
		public ABCDesc(ClassDesc[] classes, FuncDesc[] functions, PropDesc[] properties)
		{
			this.classes = classes;
			this.functions = functions;
			this.properties = PropDesc.merge(properties);
		}
		
		public String toString()
		{
			String result = "";
			
			for(ClassDesc cd: classes)
				result += cd;
			for(FuncDesc fd: functions)
				result += fd;
			for(PropDesc pd: properties)
				result += pd;
			return result;
		}
	}
	
	static class GenTraitsDesc extends NilTraitsVisitor
	{
		List<FuncDesc> _functions;
		List<PropDesc> _properties;
		
		public GenTraitsDesc(List<FuncDesc> functions, List<PropDesc> properties)
		{
			_functions = functions;
			_properties = properties;
		}
		
		@Override
		public ITraitVisitor visitMethodTrait(int kind, Name name, int dispId, MethodInfo method) {
			if(publicName(name))
				// TODO special case Function?
				switch(kind)
				{
				case TRAIT_Getter:
					_properties.add(new PropDesc(name.getSingleQualifier(), name.getBaseName(), new TypeDesc(method.getReturnType()), PropDesc.FL_getter));
					break;
				case TRAIT_Setter:
					_properties.add(new PropDesc(name.getSingleQualifier(), name.getBaseName(), new TypeDesc(method.getParamTypes().get(0)), PropDesc.FL_setter));
					break;
				case TRAIT_Method:
					_functions.add(new FuncDesc(name.getSingleQualifier(), name.getBaseName(), method));
					break;
				}
			return super.visitMethodTrait(kind, name, dispId, method);
		}
		
		@Override
		public ITraitVisitor visitSlotTrait(int kind, Name name, int slotId, Name slotType, Object slotValue) {
			if(publicName(name))
				// TODO special case Function?
				_properties.add(new PropDesc(name.getSingleQualifier(), name.getBaseName(), new TypeDesc(slotType), (kind == TRAIT_Const) ? PropDesc.FL_const : 0));
			return super.visitSlotTrait(kind, name, slotId, slotType, slotValue);
		}
	}
	static class GenClassDesc extends NilClassVisitor
	{
		InstanceInfo _ii;
		ClassInfo _ci;
		List<ClassDesc> _list;
		List<FuncDesc> _classMethods  = new ArrayList<FuncDesc>();
		List<PropDesc> _classProps = new ArrayList<PropDesc>();
		FuncDesc _constructor;
		List<FuncDesc> _instanceMethods  = new ArrayList<FuncDesc>();
		List<PropDesc> _instanceProps = new ArrayList<PropDesc>();

		public GenClassDesc(InstanceInfo ii, ClassInfo ci, List<ClassDesc> list)
		{
			_ii = ii;
			_ci = ci;
			_list = list;
			_constructor = ii.isInterface() ? null : new FuncDesc(new TypeDesc(_ii.name), _ii.iInit);
		}
		
		@Override
		public void visitEnd() {
			if(publicName(_ii.name))
				_list.add(new ClassDesc(_ii.name.getSingleQualifier(), _ii.name.getBaseName(), new TypeDesc(_ii.superName), _classMethods.toArray(new FuncDesc[0]), _classProps.toArray(new PropDesc[0]),
						_constructor, _instanceMethods.toArray(new FuncDesc[0]), _instanceProps.toArray(new PropDesc[0])));
		}
		@Override
		public ITraitsVisitor visitClassTraits() {
			return new GenTraitsDesc(_classMethods, _classProps);
		}
		@Override
		public ITraitsVisitor visitInstanceTraits() {
			return new GenTraitsDesc(_instanceMethods, _instanceProps);
		}
	}
	
	static class GenScriptDesc extends NilScriptVisitor
	{
		List<FuncDesc> _functions;
		List<PropDesc> _properties;
		
		public GenScriptDesc(List<FuncDesc> functions, List<PropDesc> properties)
		{
			_functions = functions;
			_properties = properties;
		}
		
		@Override
		public ITraitsVisitor visitTraits() {
			return new GenTraitsDesc(_functions, _properties);
		}
	}
	
	static class GenABCDesc extends NilABCVisitor
	{
		List<ABCDesc> _list;
		List<ClassDesc> _classes = new ArrayList<ClassDesc>();
		List<FuncDesc> _functions = new ArrayList<FuncDesc>();
		List<PropDesc> _properties = new ArrayList<PropDesc>();
		
		public GenABCDesc(List<ABCDesc> list)
		{
			_list = list;
		}
		
		@Override
		public void visitEnd() {
			_list.add(new ABCDesc(_classes.toArray(new ClassDesc[0]), _functions.toArray(new FuncDesc[0]), _properties.toArray(new PropDesc[0])));
		}

		@Override
		public void visit(int arg0, int arg1) {
		}

		@Override
		public IClassVisitor visitClass(InstanceInfo ii, ClassInfo ci) {
			return new GenClassDesc(ii, ci, _classes);
		}

		@Override
		public IScriptVisitor visitScript() {
			return new GenScriptDesc(_functions, _properties);
		}
		
		@Override
		public void visitPooledName(Name n) {
			if(n.getKind() == CONSTANT_Qname || n.getKind() == CONSTANT_QnameA)
				uniquer.unique(n.getSingleQualifier(), n.getBaseName());
			super.visitPooledName(n);
		}
		
		@Override
		public IMethodVisitor visitMethod(MethodInfo minfo) {
			return null;
		}
	}
	
	static void usage()
	{
        System.err.println("Usage: as3wig -i input.abc -header output.h -impl output.cpp");
        System.exit(-1);
	}
	
	static List<String> getAsmForArgList(FuncDesc fd, boolean imeth, int rsvdRegs, int constructorVA)
	{
		List<String> result = new ArrayList<String>();
		result.add(null);
		String argList = "";

		if(imeth)
		{
			result.add("_id");
			rsvdRegs++; // reserve one for "this"
		}
		for(int j = 0; j < fd.argTypes.length + constructorVA; j++)
		{
			if(!argList.equals(""))
				argList += ", ";
			if(j < fd.argTypes.length)
			{
				if(fd.argTypes[j].isScalar())
				{
					result.add("arg" + j);
					argList += "%" + (j + rsvdRegs);
				}
				else
				{
					result.add("AS3_GET_ID(arg" + j + ")");
					argList += "as3_id2rcv[%" + (j + rsvdRegs) + "].val";
				}
			}
			else
			{
				result.add("AS3_GET_ID(rest" + (j - fd.argTypes.length) + ")");
				argList += "as3_id2rcv[%" + (j - fd.argTypes.length + rsvdRegs) + "].val";
			}
		}
		result.set(0, argList);
		return result;
	}

	static void genAsm(String asm, String[] outs, String[] ins)
	{
		outHeader.print("\tinline_as3 (\"" + asm + "\"");
		if(outs != null)
		{
			for(int i = 0; i < outs.length; i++)
			{
				outHeader.print((i == 0) ? " : " : ", ");
				outHeader.print ("\"=r\"(" + outs[i] + ")");
			}
		}
		if(ins != null)
		{
			if(outs == null)
				outHeader.print(" : ");
			for(int i = 0; i < ins.length; i++)
			{
				outHeader.print((i == 0) ? " : " : ", ");
				outHeader.print ("\"r\"(" + ins[i] + ")");
			}
		}
		outHeader.println(");");
	}
	
	static void genMethodImpl(FuncDesc fd, ClassDesc cd, String setup, String thiz, String call, int constructorVA)
	{
		String asm1 = "; try { ";
		String asm2 = "; %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }";
		String[] outs;
		outHeader.println("\tunsigned eid;");
		if(fd.as3name != null && fd.retType.isScalar())
		{
			outHeader.println("\t" + fd.retType + " res;");
			outs = new String[] { "eid", "res" };
			asm1 += "%1 = ";
		}
		else if(!fd.retType.isVoid())
		{
			outHeader.println("\tunsigned id;");
			outs = new String[] { "eid", "id" };
			asm1 += "%1 = as3_valAcquireId(";
			asm2 = ")" + asm2;
		}
		else
			outs = new String[] { "eid" };

		int rsvdRegs = fd.retType.isVoid() ? 1 : 2;
		List<String> args = getAsmForArgList(fd, cd != null, rsvdRegs, constructorVA);
		String argList = args.remove(0);
		if(fd.as3name != null && fd.isVarArgs) // handle non-constructor var args
		{
			outHeader.println("\tvar va = AS3_NS::internal::new_Array(restCount, rest);");
			call = call + ".apply";
			String rcv = thiz.equals("") ? "undefined" : thiz;
			argList = rcv + ", [" + argList + "].concat(as3_id2rcv[%" + (args.size() + rsvdRegs) + "].val)";
			args.add("AS3_GET_ID(va)");
		}
		if(fd.hasByteArrayArg() || cd != null && cd.isByteArray()) // handle settinng ram.position for any method that takes a ByteArray
		{
			setup = "ram.position = %" + (args.size() + rsvdRegs) + "; " + setup;
			args.add("ramPos");
		}
		genAsm(setup + asm1 + thiz + call + "(" + argList + ")" + asm2, outs, args.toArray(new String[0]));
		outHeader.println("\tif(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; }");
		if(!fd.retType.isVoid())
		{
			if(!fd.retType.isScalar() || fd.as3name == null) // constructors always return non-scalar
			{
				outHeader.println("\t" + fd.retType.toString(false) + " res;");
				outHeader.println("\tAS3_GET_ID(res) = id;");
			}
			outHeader.println("\treturn res;");
		}
	}
	
	static void genABCClasses(ClassDesc[] classes)
	{
		String curNS = "";
		
		outHeader.println("#ifndef AS3_IMPL // class interfaces");
		outHeader.println("#pragma GCC diagnostic ignored \"-Wdiv-by-zero\" push");
		for(ClassDesc cd: classes)
		{
			curNS = moveToNS(namespaceToString(cd.namespace), curNS);
			// fwd decl
			outHeader.println("class " + cd.name + ";");
		}
		for(ClassDesc cd: classes)
		{
			curNS = moveToNS(namespaceToString(cd.namespace), curNS);
			String def = (cd.name.equals("_int") || cd.name.equals("uint")) ? "kZero" : 
				cd.name.equals("Number") ? "kNaN" :
				cd.name.equals("Boolean") ? "kFalse" :
				"kNull";
			// class decl
			outHeader.println("class " + cd.name + " : public " + cd.superType + " {");
			outHeader.println("protected:");
			outHeader.println("\texplicit " + cd.name + "(unsigned id) : " + cd.superType + "(id) {}");
			outHeader.println("public:");
			// default constructor
			// copy constructor
			// var constructor
			// assignment operator
			outHeader.println("\tAS3_BASE_DEF(" + cd.name + ", " + def + ");");
			// internal inner class
			outHeader.println("\tclass internal {");
			for(PropDesc pd: cd.instanceProps)
				outHeader.println("\t\tAS3_IPROP" + (pd.canSet() ? "_SET" : "") + (pd.canGet() ? "_GET" : "") + "_DEF(" + pd.type + ", " + pd.name + ");");
			// public stuff
			outHeader.println("\tpublic:");
			// get at the class' class closure
			outHeader.println("\t\tstatic Class getClosure();");
			// class properties
			for(PropDesc pd: cd.classProps)
				outHeader.println("\t\tAS3_PROP" + (pd.canSet() ? "_SET" : "") + (pd.canGet() ? "_GET" : "") + "_DEF(" + pd.type + ", " + pd.name + ");");
			// now one class to hold all of the property helper classes as members
			outHeader.println("\t\tclass instance : public " + cd.superType + "::internal::instance {");
			outHeader.println("\t\tpublic:");
			for(PropDesc pd: cd.instanceProps)
				outHeader.println("\t\t\tAS3_IPROP(" + pd.name + ");");
			// instance methods
			for(FuncDesc fd: cd.instanceMethods)
				outHeader.println("\t\t\tAS3_METH_DEF(" + fd.retType + ", " + fd.name + ", (" + fd.getArgList(cd, true, false, 0) + "));");
			outHeader.println("\t\t};");
			outHeader.println("\t};");
			// now weird-o operator -> that returns an instance *
			outHeader.println("\tinternal::instance *operator->() { return (internal::instance *)this; }");
			// static properties
			for(PropDesc pd: cd.classProps)
				outHeader.println("\tstatic AS3_PROP(" + pd.name + ");");
			// static methods
			for(FuncDesc fd: cd.classMethods)
				outHeader.println("\tstatic AS3_METH_DEF(" + fd.retType + ", " + fd.name + ", (" + fd.getArgList(null, true, false, 0) + "));");
			// now constructor
			if(cd.constructor != null)
				for(int i = 0; i <= (cd.constructor.isVarArgs ? CONSTR_VARARGS : 0); i++)
					outHeader.println("\tAS3_CONSTR_DEF(" + cd.name + ", (" + cd.constructor.getArgList(cd, true, false, i) + "));");
			if(cd.namespace.getName().equals("")) // a few special constructors
			{
				if(cd.as3name.equals("String"))
				{
					outHeader.println("\tstatic String _new(const char *, int len = -1);");
					outHeader.println("\tString(const char *);");
				}
				else if(cd.as3name.equals("Array"))
					outHeader.println("\tstatic Array _new(int, var *);");
				else if(cd.as3name.equals("int"))
					outHeader.println("\tstatic _int _new(int);");
				else if(cd.as3name.equals("uint"))
					outHeader.println("\tstatic uint _new(unsigned);");
				else if(cd.as3name.equals("Number"))
					outHeader.println("\tstatic Number _new(double);");
				else if(cd.as3name.equals("Boolean"))
					outHeader.println("\tstatic Boolean _new(bool);");
				else if(cd.as3name.equals("Function"))
                                        outHeader.println("\tstatic Function _new(var (*fun)(void *data, var args), void *data);");

			}
			outHeader.println("};");
		}
		curNS = moveToNS("", curNS);
		outHeader.println("#pragma GCC diagnostic ignored \"-Wdiv-by-zero\" pop");
		outHeader.println("#else // class implementations");
		for(ClassDesc cd: classes)
		{
			// copy constructor
			// var constructor
			// assignment operator
			outHeader.println("AS3_BASE_IMPL(" + namespaceToString(cd.namespace, true) + ", " + cd.name + ", \"" + namespaceToString(cd.namespace, ".", true) + "\", \"" + cd.as3name + "\");");
			for(PropDesc pd: cd.instanceProps)
				outHeader.println("AS3_IPROP" + (pd.canSet() ? "_SET" : "") + (pd.canGet() ? "_GET" : "") + (pd.type.isScalar() ? "_SIMPL" : "_IMPL") + "(" + pd.type + ", " + pd.name + ", " + namespaceToString(cd.namespace, true) + ", " + cd.name + ", \"" + pd.as3name + "\");");
			for(PropDesc pd: cd.classProps)
				outHeader.println("AS3_PROP" + (pd.canSet() ? "_SET" : "") + (pd.canGet() ? "_GET" : "") + (pd.type.isScalar() ? "_SIMPL" : "_IMPL") + "(" + pd.type + ", " + pd.name + ", " + namespaceToString(cd.namespace, true) + cd.name + ", \"" + namespaceToString(cd.namespace, ".", true) + cd.as3name + "." + pd.as3name + "\", \"" + namespaceToString(cd.namespace, ".", true) + cd.as3name + "\");");
			for(FuncDesc fd: cd.instanceMethods)
			{
				outHeader.println("AS3_IMETH_IMPL(" + fd.retType + ", " + namespaceToString(cd.namespace, true) + ", " + cd.name + ", " + fd.name + ", (" + fd.getArgList(cd, true, true, 0) + "), {");
				genMethodImpl(fd, cd,
						"import " + namespaceToString(cd.namespace, ".", true) + cd.as3name + "; " +
						"var thiz:" + cd.as3name + " = as3_id2rcv[%" + (fd.retType.isVoid() ? "1" : "2") + "].val",
						"thiz",  "." + fd.as3name,
						0);
				outHeader.println("}, " + fd.argsSize(cd, 0) + ")");
			}
			for(FuncDesc fd: cd.classMethods)
			{
				outHeader.println("AS3_METH_IMPL(" + fd.retType + ", " + namespaceToString(cd.namespace, true) + cd.name + "::" + fd.name + ", (" + fd.getArgList(null, true, true, 0) + "), {");
				genMethodImpl(fd, null,
						"import " + namespaceToString(cd.namespace, ".", true) + cd.as3name,
						cd.as3name, "." + fd.as3name,
						0);
				outHeader.println("}, " + fd.argsSize(null, 0) + ")");
			}
			if(cd.constructor != null)
				for(int i = 0; i <= (cd.constructor.isVarArgs ? CONSTR_VARARGS : 0); i++)
				{
					outHeader.println("AS3_CONSTR_IMPL(" + namespaceToString(cd.namespace, true) + ", " + cd.name + ", (" + cd.constructor.getArgList(cd, true, true, i) + "), {");
					genMethodImpl(cd.constructor, null,
							"import " + namespaceToString(cd.namespace, ".", true) + cd.as3name,
							"", "new " + cd.as3name,
							i);
					outHeader.println("}, " + cd.constructor.argsSize(null, i) + ")");
				}
			if(cd.namespace.getName().equals("")) // a few special constructors
			{
				if(cd.as3name.equals("String"))
				{
					outHeader.println("String String::_new(const char *sz, int len) { return AS3_NS::internal::new_String(sz, len); }");
					outHeader.println("String::String(const char *sz) { *this = AS3_NS::internal::new_String(sz, -1); }");
				}
				else if(cd.as3name.equals("Array"))
					outHeader.println("Array Array::_new(int n, var *elems) { return AS3_NS::internal::new_Array(n, elems); }");
				else if(cd.as3name.equals("int"))
					outHeader.println("_int _int::_new(int n) { return AS3_NS::internal::new_int(n); }");
				else if(cd.as3name.equals("uint"))
					outHeader.println("uint uint::_new(unsigned n) { return AS3_NS::internal::new_uint(n); }");
				else if(cd.as3name.equals("Number"))
					outHeader.println("Number Number::_new(double n) { return AS3_NS::internal::new_Number(n); }");
				else if(cd.as3name.equals("Boolean"))
					outHeader.println("Boolean Boolean::_new(bool b) { return AS3_NS::internal::new_Boolean(b); }");
				else if(cd.as3name.equals("Function"))
					outHeader.println("Function Function::_new(var (*fun)(void *data, var args), void *data) { return AS3_NS::internal::new_Function(fun, data); }");
			}
		}
		outHeader.println("#endif // classes ");
	}
	
	static void genABCFunctions(FuncDesc[] functions)
	{
		String curNS = "";
		
		outHeader.println("#ifndef AS3_IMPL // function interfaces");
		// function prototypes
		for(FuncDesc fd: functions)
		{
			curNS = moveToNS(namespaceToString(fd.namespace), curNS);
			outHeader.println("AS3_METH_DEF(" + fd.retType + ", " + fd.name + ", (" + fd.getArgList(null, true, false, 0) + "));");
		}
		curNS = moveToNS("", curNS);
		outHeader.println("#else // function implementations");
		for(FuncDesc fd: functions) // function implementations
		{
			outHeader.println("AS3_METH_IMPL(" + fd.retType + ", " + namespaceToString(fd.namespace, true) + fd.name + ", (" + fd.getArgList(null, true, true, 0) + "), {");
			genMethodImpl(fd, null,
					"import " + namespaceToString(fd.namespace, ".", true) + fd.as3name,
					"", fd.as3name,
					0);
			outHeader.println("}, " + fd.argsSize(null, 0) + ")");
		}
		outHeader.println("#endif // functions");
	}
	
	static void genABCProps(PropDesc[] properties)
	{
		String curNS = "";

		outHeader.println("#ifndef AS3_IMPL // property interfaces");
		for(PropDesc pd: properties)
		{
			curNS = moveToNS(namespaceToString(pd.namespace), curNS);
			outHeader.println("namespace internal {");
			outHeader.println("\tAS3_PROP" + (pd.canSet() ? "_SET" : "") + (pd.canGet() ? "_GET" : "") + "_DEF(" + pd.type + ", " + pd.name + ");");
			outHeader.println("} //internal\n");
			outHeader.println("extern AS3_PROP(" + pd.name + ");");
		}
		curNS = moveToNS("", curNS);
		outHeader.println("#else // property implementations");
		for(PropDesc pd: properties)
		{
			String ns = namespaceToString(pd.namespace, false);
			String cns = "AS3_NS";
			if(!ns.equals(""))
				cns = cns + "::" + ns;
			outHeader.println("AS3_PROP" + (pd.canSet() ? "_SET" : "") + (pd.canGet() ? "_GET" : "") + (pd.type.isScalar() ? "_SIMPL_" : "_IMPL_") + "(" + pd.type + ", " + pd.name + ", " + cns + ", \"" + namespaceToString(pd.namespace, ".", true) + pd.as3name + "\", \"" + namespaceToString(pd.namespace, ".", true) + pd.as3name + "\");");
			outHeader.println("AS3_PROP_IMPL(" + pd.name + ", " + cns + ");");
		}
		outHeader.println("#endif // properties");
	}
	
	static void genABCDesc(ABCDesc desc)
	{
		genABCClasses(desc.classes);
		genABCFunctions(desc.functions);
		genABCProps(desc.properties);
	}
	
	public static void main(String[] args) throws IOException
	{
		if(args.length == 0)
			usage();

		InputStream is = System.in;
		Boolean builtins = false;

		for (int i = 0; i < args.length; i++) {
            if (args[i].equals("-i"))
                is = new FileInputStream(new File(args[++i]));
            else if (args[i].equals("-builtins"))
            	builtins = true;
            else if (args[i].equals("-o")) {
            	outName = args[++i];
            	if(builtins)
            		outHeader = new PrintStream(new FileOutputStream(new File(outName + ".h")));
            	else
                	outHeader = new PrintStream(new FileOutputStream(new File(outName + "_internal.h")));
                outImpl = new PrintStream(new FileOutputStream(new File(outName + ".cpp")));
            } else
                usage();
        }

		try
		{
	        ByteArrayOutputStream bas = new ByteArrayOutputStream();
	        byte[] chunk = new byte[0x100000];
	        int read;
	
	        while((read = is.read(chunk)) > 0)
	                bas.write(chunk, 0, read);
	
	        ABCParser parser = new ABCParser(bas.toByteArray());
	        bas.reset();
	        List<ABCDesc> abcDescs = new ArrayList<ABCDesc>();
	        GenABCDesc genDesc = new GenABCDesc(abcDescs);
	        parser.parseABC(genDesc);

	        for(ABCDesc desc: abcDescs)
	        	genABCDesc(desc);

	        if(!builtins) {
	    	    PrintStream realHeader = new PrintStream(new FileOutputStream(new File(outName + ".h")));

	        	File as3wigheader = new File(new File(new File(new File(AS3Wig.class.getProtectionDomain().getCodeSource().getLocation().getPath()).getParentFile().getParentFile(), "include"), "AS3++"), "AS3Wig.h");
	        	FileInputStream as3ppHeader = new FileInputStream(as3wigheader);

	        	BufferedReader br = new BufferedReader(new FileReader(as3wigheader));
				String line = null;
				while((line = br.readLine()) != null)
				{
					realHeader.println(line.replace("<AS3WigIncludes.h>", "\""+outName+"_internal.h\""));
				}

				outImpl.println("#include <Flash++.h>");
				outImpl.println("#define AS3_IMPL");
		        outImpl.println("#include \""+outName+".h\"");
	        }
	    }
	    catch(Exception e)
	    {
	            e.printStackTrace(System.err);
	            System.exit(-1);
	    }
	}
}
