/* -*- Mode: Java; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* Usage:
 *
 *    avmshell apivergen.abc -- ... api-versions.xml
 *
 * All arguments except the last are ignored.
 */

/*
    THEORY OF OPERATION

    API Versioning is about manipulating public namespaces in builtin code
    such that some names are hidden, depending on the version of the observer.

    One way to look at product version relations is as an partial ordering of
    versions where v1 < v2 if every API defined in v1 is defined in v2.
    e.g.:

    FP_9_0 < FP_10_0 < FP_10_0_32 < FP_10_1
    AIR_1_0 < AIR_1_5 < AIR_1_5_1 < AIR_1_5_2 < AIR_2_0
    FP_9_0 < AIR_1_0
    FP_10_0 < AIR_1_5
    FP_10_0_32 < AIR_1_5_2
    FP_10_1 < AIR_2_0
    
    (It is understood that x < y and y < z implies x < z.)

    Since versions are a partial ordering, it is possible to (topologically) sort
    the versions so that if v1 < v2 then ord(v1) < ord(v2). E.g.:

    0 - FP_9_0
    1 - AIR_1_0
    2 - FP_10_0
    3 - AIR_1_5
    4 - AIR_1_5_1
    5 - FP_10_0_32
    6 - AIR_1_5_2
    7 - FP_10_1
    8 - AIR_2_0

    The wrinkle in this system is that while, as a matter of policy, every FP API
    becomes an AIR API in the corresponding release, most AIR APIs never become
    Flash APIs but some APIs are introduced in an AIR version and then (perhaps
    much) later in a Flash version. So, every versioned API is marked with the
    version(s) in which it was introduced, e.g.

        [API(AIR_1_0), API(FP_10_0_32)] 

    We address this issue by separating the versions into multiple "series".
    (Currently we only make use of two (Flash vs AIR) but an arbitrary number
    is possible.) 
    
    Versions are specified by by XML of the form:

    <api>
        <series name="AIR">
            <version name="AIR_1_0"/>
            <version name="AIR_1_5"/>
            <version name="AIR_1_5_1"/>
            <version name="AIR_1_5_2"/>
            <version name="AIR_2_0"/>
        </series>
        <series name="FP">
            <version name="FP_10_0"/>
            <version name="FP_10_0_32"/>
            <version name="FP_10_1"/>
        </series>
        <ordering>
            <set sub="FP_10_0" super="AIR_1_5"/>
            <set sub="FP_10_0_32" super="AIR_1_5_2"/>
            <set sub="FP_10_1" super="AIR_2_0"/>
        </ordering>
    </api>
    
    Each "series" defines an increasing set of APIs, implicitly in
    the listed order. Each successive version in a given series contains
    all the APIs in all previous versions.
    
    The "ordering" section allows you to define how versions in disjoint
    series relate to each other, by specifying that a given "super" contains
    all the APIs in "sub".
    
    In the example above, we know that AIR_1_5 contains all the APIs in AIR_1_0
    (from the implicit order in the series), and also contains all the APIs in 
    FP_10_0 (from an explicit ordering-set directive).
    
    When we process this XML, we topologically sort based on these constraints,
    so that we end up with a set of integer ids that conform to ordering described
    above; in the previous example, ord(AIR_1_5) > ord(AIR_1_0) and also 
    ord(AIR_1_5) > ord(FP_10_0). (The relationship between AIR_1_0 and FP_10_0 is unspecified).
    
    The gotcha here is that it's only meaningful to compare versions in the same series; 
    we do this by having a single "active" series at a time, and if we encounter
    version numbers that aren't in the active series, we translate them into the 
    next-highest version in the active series. To simplify this, we always emit
    two "special" versions:
    
        VM_ALLVERSIONS, which means "every version is allowed to see this name"
        VM_INTERNAL, which means "only VM_INTERNAL is allowed to see this name"
    
    crucially, these two are defined to be members of all series, which always allows
    us to promote an invalid-series-version into VM_INTERNAL. For the sample above we'd
    end up with

        enum ApiVersion {
            kApiVersion_VM_ALLVERSIONS = 0,
            kApiVersion_AIR_1_0 = 1,
            kApiVersion_FP_10_0 = 2,
            kApiVersion_AIR_1_5 = 3,
            kApiVersion_AIR_1_5_1 = 4,
            kApiVersion_FP_10_0_32 = 5,
            kApiVersion_AIR_1_5_2 = 6,
            kApiVersion_FP_10_1 = 7,
            kApiVersion_AIR_2_0 = 8,
            kApiVersion_VM_INTERNAL = 9
        };
    
    For the example above, if the active series is AIR and we encounter
    FP_10_0, we promote it to AIR_1_5 (because that's the smallest version in the active
    series that contains all of the APIs in FP_10_0). If the active series is FP and we encounter
    AIR_1_0, we promote it to VM_INTERNAL (because there's no version in the active series
    that contains all of the APIs in AIR_1_0).
    
    
    Important things to remember:
    
    -- *only* NS_Public Namespaces are ever versioned; all other Namespaces are
        required to be VM_ALLVERSIONS.
    
    -- *only* Namespaces in builtin code are ever versioned; code loaded at runtime
        will tag its namespaces as necessary when creating a namespace in a versioned URI.
    
    -- When initialized, an AvmCore is locked into a given series; in Flash and AIR this is 
        fixed at compiletime, but avmshell allows for switching at startup time via a commandline switch.
        There is no way to safely change series after an AvmCore is created, however.
    
    -- versioned namespace lookup is not commutative; an MNHT contains a version that means
        "minimum version that is allowed to see this name", while the name being looked
        up contains "the version that is trying to see this name".
    
    -- VM_ALLVERSIONS is always 0. VM_INTERNAL will expand as versions are added.
    
*/


package apivergen 
{
    import avmplus.*;

    var DO_NOT_EDIT =
        "/* DO NOT EDIT THIS FILE!  It was generated by utils/apivergen.abc. */\n\n";

    var LICENSE_BLOCK = 
        "/* This Source Code Form is subject to the terms of the Mozilla Public" +
        " * License, v. 2.0. If a copy of the MPL was not distributed with this" +
        " * file, You can obtain one at http://mozilla.org/MPL/2.0/. */" +
        "\n";
    
    const VM_ALLVERSIONS:String = "VM_ALLVERSIONS";
    const VM_INTERNAL:String = "VM_INTERNAL";

    function process(fname:String) 
    {
        var api:XML = new XML(File.read(fname));
        
        var frag = {};
        frag.series_names = [];
        frag.series_first = [];
        frag.series_latest = [];
        
        var all_series:Object = {};
        var versions:Object = {};

        versions[VM_ALLVERSIONS] = ({name: VM_ALLVERSIONS, series: all_series, subs:[], visited: false});
        versions[VM_INTERNAL] = ({name: VM_INTERNAL, series: all_series, subs:[], visited: false});

        for each (var p in api..series) 
        {
            var pn:String = p.@name;
            if (frag.series_names.indexOf(pn) >= 0)
            {
                throw Error("illegal duplicate name" + pn);
            }
            frag.series_names.push(pn);
            if (frag.series_names.length > 32)
            {
                throw Error("We only support a maximum of 32 series currently");
            }
            
            all_series[pn] = true;
            
            var prev = versions[VM_ALLVERSIONS];
            var cur = null;
            var series_versions:Array = [];
            var this_series = {};
            this_series[pn] = true;
            for each (var v in p..version) 
            {
                var n:String = v.@name;
                if (n in versions)
                {
                    throw Error("illegal duplicate name" + n);
                }
                if (n == VM_INTERNAL || n == VM_ALLVERSIONS)
                {
                    throw Error("reserved name " + n);
                }
                cur = {name: n, series: this_series, subs:[], visited: false};
                series_versions.push(cur);
                cur.subs.push(prev.name);
                prev = cur;
            }

            versions[VM_INTERNAL].subs.push(cur.name);
           
            frag.series_first.push(series_versions[0].name);
            frag.series_latest.push(series_versions[series_versions.length-1].name);
            for (var i:int = 0; i < series_versions.length; ++i)
            {
                versions[series_versions[i].name] = series_versions[i];
            }
        }

        for each (var v in api.ordering..set) 
        {
            var sub:String = v.@sub;
            var supe:String = v.@["super"];
            if (!(sub in versions))
            {
                throw Error("illegal version "+sub);
            }
            if (!(supe in versions))
            {
                throw Error("illegal version "+supe);
            }
            versions[supe].subs.push(sub);
        }

        for (var name in versions)
        {
            versions[name].subs.sort();
        }
        
        frag.ordered_versions = [];

        function visit(name:String):void
        {
            var node = versions[name];
            if (node.visited == false)
            {
                node.visited = true;
                for (var i:int = 0; i < node.subs.length; ++i)
                {
                    visit(node.subs[i]);
                }
                node.value = frag.ordered_versions.length;
                frag.ordered_versions.push(node);
            }
        }
        visit(VM_INTERNAL);

        function add_subs(name:String, all_subs:Object):void
        {
            all_subs[name] = true;
            var node = versions[name];
            for (var i:int = 0; i < node.subs.length; ++i)
            {
                var sub = node.subs[i];
                add_subs(sub, all_subs);
            }
        }
        for (var name in versions)
        {
            versions[name].all_subs = {};
            add_subs(name, versions[name].all_subs);
        }

        return frag;
    }

    /*
      emit c code
    */

    function emith(frag) 
    {
        var out = DO_NOT_EDIT + LICENSE_BLOCK;
        out += "#ifndef api_versions_H_\n";
        out += "#define api_versions_H_\n";
        out += "\n";
        out += "namespace avmplus {\n";
        out += "\n";
        out += "enum ApiVersionSeries {\n";
        var c = frag.series_names;
        for ( var i=0 ; i < c.length ; i++ )
        {
            out += "    kApiVersionSeries_" + c[i];
            if (i < c.length-1)
                out += ",";
            out += "\n";
        }
        out += "};\n";
        out += "\n";
        out += "enum {\n";
        out += "    kApiVersionSeries_count = " + c.length + "\n";
        out += "};\n";
        out += "\n";
        out += "enum ApiVersion {\n";
        var c = frag.ordered_versions;
        for ( var i=0 ; i < c.length ; i++ )
        {
            out += "    kApiVersion_" + c[i].name + " = " + (c[i].value);
            if (i < c.length-1)
                out += ",";
            out += "\n";
        }
        out += "};\n";
        out += "\n";
        // these aren't valid ApiVersion, it goes in a different enum
        out += "enum {\n";
        out += "    kApiVersion_count = " + (frag.ordered_versions.length) + "\n";
        out += "};\n";
        out += "\n";
        out += "extern const char* const kApiVersionNames[kApiVersion_count];\n";
        out += "extern uint32_t const kApiVersionSeriesMembership[kApiVersion_count];\n";
        out += "extern ApiVersion const kApiVersionSeriesTransfer[kApiVersion_count][kApiVersionSeries_count];\n";
        out += "extern ApiVersion const kApiVersionFirst[kApiVersionSeries_count];\n";
        out += "extern ApiVersion const kApiVersionLatest[kApiVersionSeries_count];\n";
        out += "\n";
        out += "}";
        out += "\n";
        out += "#endif // api_versions_H_\n";
        return out;
    }

    function emitcpp(frag) 
    {
        var out = DO_NOT_EDIT + LICENSE_BLOCK;
        out += "namespace avmplus {\n";
        out += "\n";
        out += "const char* const kApiVersionNames[kApiVersion_count] = {\n";
        var c = frag.ordered_versions;
        for ( var i=0 ; i < c.length ; i++ )
        {
            out += '    "' + c[i].name + '"';
            if (i < c.length - 1)
               out += ", ";
            out += "\n";
        }
        out += "};\n";
        out += "\n";
        out += "uint32_t const kApiVersionSeriesMembership[kApiVersion_count] = {\n";
        var c = frag.ordered_versions;
        for (var i=0 ; i < c.length; i++)
        {
            var s:String = "";
            for (var j=0 ; j < frag.series_names.length; j++)
            {
                if (frag.series_names[j] in c[i].series)
                {
                    if (s.length > 0)
                        s += "|";
                    s += '(1<<kApiVersionSeries_' + frag.series_names[j] + ')';
                }
            }
            out += "    " + s;
            if (i < c.length-1)
                out += ",";
            out += '\n';
        }
        out += "};\n";
        out += "\n";

        out += "// If a version is in an inactive series, what is the closest superset in the active series?\n";
        out += "ApiVersion const kApiVersionSeriesTransfer[kApiVersion_count][kApiVersionSeries_count] = {\n";
        var c = frag.ordered_versions;
        for (var i=0 ; i < c.length; i++)
        {
            out += "    { ";
            for (var j=0 ; j < frag.series_names.length; j++)
            {
                var cur_series_name = frag.series_names[j];
                // Find the smallest version in the given series that has us as a subset.
                for (var k:int = i; k < c.length; k++)
                {
                    if ((cur_series_name in c[k].series) && (c[i].name in c[k].all_subs))
                    {
                        out += "kApiVersion_" + c[k].name;
                        break;
                    }
                }
                if (k == c.length)
                {
                    throw Error("impossible");
                }
                if (j < frag.series_names.length - 1)
                    out += ", ";
            }
            out += ' }';
            if (i < c.length-1)
                out += ",";
            out += " // " + c[i].name;
            out += "\n";
        }
        out += "};\n";
        out += "\n";

        out += "ApiVersion const kApiVersionFirst[kApiVersionSeries_count] = {\n";
        var c = frag.series_first;
        for (var i=0 ; i < c.length; i++)
        {
            out += '    kApiVersion_' + c[i];
            if (i < c.length-1)
                out += ",";
            out += "\n";
        }
        out += "};\n";
        out += "\n";
        out += "ApiVersion const kApiVersionLatest[kApiVersionSeries_count] = {\n";
        var c = frag.series_latest;
        for (var i=0 ; i < c.length; i++)
        {
            out += '    kApiVersion_' + c[i];
            if (i < c.length-1)
                out += ",";
            out += "\n";
        }
        out += "};\n";
        out += "\n";
        out += "}";
        out += "\n";

        return out;
    }

    /*
      emit java
    */
    function emitj(frag) {
        var out = DO_NOT_EDIT + LICENSE_BLOCK;
        out += "\n";
        out += "package adobe.abc;\n";
        out += "\n";
        out += "public class APIVersions {\n";
        out += "\n";

        var c = frag.series_names;
        for ( var i=0 ; i < c.length ; i++ )
        {
            out += "public static final int kApiVersionSeries_" + c[i] + " = " + i + ";\n";
        }
        out += "\n";
        
        out += "public static final int kApiVersionSeries_count = " + c.length + ";\n";
        out += "\n";

        var c = frag.ordered_versions;
        for ( var i=0 ; i < c.length ; i++ )
        {
            out += "public static final int kApiVersion_" + c[i].name + " = " + c[i].value + ";\n";
        }
        out += "\n";

        out += "public static final int kApiVersion_count = " + frag.ordered_versions.length + ";\n";
        out += "\n";
        out += "public static final String kApiVersionNames[]= {\n";
        var c = frag.ordered_versions;
        for ( var i=0 ; i < c.length ; i++ )
        {
            out += '    "' + c[i].name + '"';
            if (i < c.length - 1)
               out += ", ";
            out += "\n";
        }
        out += "};\n";
        out += "\n";

        out += "public static final int kApiVersionSeriesMembership[] = {\n";
        var c = frag.ordered_versions;
        for (var i=0 ; i < c.length; i++)
        {
            var s:String = "";
            for (var j=0 ; j < frag.series_names.length; j++)
            {
                if (frag.series_names[j] in c[i].series)
                {
                    if (s.length > 0)
                        s += "|";
                    s += '(1<<kApiVersionSeries_' + frag.series_names[j] + ')';
                }
            }
            out += "    " + s;
            if (i < c.length-1)
                out += ",";
            out += '\n';
        }
        out += "};\n";
        out += "\n";

        out += "public static final int kApiVersionSeriesTransfer[][] = {\n";
        var c = frag.ordered_versions;
        for (var i=0 ; i < c.length; i++)
        {
            out += "    { ";
            for (var j=0 ; j < frag.series_names.length; j++)
            {
                var cur_series_name = frag.series_names[j];
                // Find the smallest version in the given series that has us as a subset.
                for (var k:int = i; k < c.length; k++)
                {
                    if ((cur_series_name in c[k].series) && (c[i].name in c[k].all_subs))
                    {
                        out += "kApiVersion_" + c[k].name;
                        break;
                    }
                }
                if (k == c.length)
                {
                    throw Error("impossible");
                }
                if (j < frag.series_names.length - 1)
                    out += ", ";
            }
            out += ' }';
            if (i < c.length-1)
                out += ",";
            out += " // " + c[i].name;
            out += "\n";
        }
        out += "};\n";
        out += "\n";

        out += "public static final int kApiVersionFirst[] = {\n";
        var c = frag.series_first;
        for (var i=0 ; i < c.length; i++)
        {
            out += '    kApiVersion_' + c[i];
            if (i < c.length-1)
                out += ",";
            out += "\n";
        }
        out += "};\n";
        out += "\n";

        out += "public static final int kApiVersionLatest[] = {\n";
        var c = frag.series_latest;
        for (var i=0 ; i < c.length; i++)
        {
            out += '    kApiVersion_' + c[i];
            if (i < c.length-1)
                out += ",";
            out += "\n";
        }
        out += "};\n";
        out += "\n";

        out += "};";
        out += "\n";
        return out;
    }
    
    /*
      emit actionscript
    */
    function emitas(frag) 
    {
        var out = DO_NOT_EDIT + LICENSE_BLOCK;
        var c = frag.ordered_versions;
        // don't emit VM_ALLVERSIONS -- it's pointless to tag
        // something with it, so don't encourage people to do so by providing it
        // (660 is the magic base that asc.jar uses to mark; we account for that
        // when we read in via MIN_API_MARK)
        for ( var i=1 ; i < c.length ; i++ )
            out += "CONFIG const " + c[i].name + " = " + (c[i].value + 660) + ";\n";
        return out;
    }

    /*
      get file name from the command-line and go. ignore all but the last 
      argument, which is treated as the file name
    */

    var argv = System.argv;
    var fname = argv[argv.length-1];
    var frags = process(fname);
    File.write("api-versions.h", emith(frags));
    File.write("api-versions.cpp", emitcpp(frags));
    File.write("api-versions.as", emitas(frags));
    File.write("api-versions.java", emitj(frags));
}
