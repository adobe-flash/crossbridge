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
import com.adobe.flash.abc.*;
import com.adobe.flash.abc.visitors.*;
import com.adobe.flash.abc.semantics.*;

    // translate burned-in Flascc namespaces to the specified namespace
    public class FlasccNamespaceTranslator extends com.adobe.flash.abc.ABCParser.ConstantPoolTranslator
    {
        public static final String flasccNS = "com.adobe.flascc";

        String _namespace;

        public FlasccNamespaceTranslator(String namespace)
        {
                _namespace = namespace;
        }

        @Override
        public Namespace translateNamespace(Namespace namespace)
        {
                if(namespace != null && namespace.getName() != null)
                {
                        String nsString = namespace.getName();
                        if(nsString.equals("C_Run")) // C_Run => new namespace
                                return new Namespace(namespace.getKind(), _namespace);
                        else if(nsString.startsWith("C_Run_")) // C_Run => new namespace
                                return new Namespace(namespace.getKind(), _namespace + "_" + nsString.substring(7));

                        // replace com.adobe.flascc w/ new namespace (partials too)
                        if(nsString.equals(flasccNS) || nsString.startsWith(flasccNS + "."))
                                return new Namespace(namespace.getKind(), _namespace + nsString.substring(flasccNS.length()));
                }
                return namespace;
        }

        @Override
        public Name translateName(Name name)
        {
                return name;
        }
    }
