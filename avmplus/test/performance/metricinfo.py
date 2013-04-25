# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# This file contains information about the different performance metrics
# It is a python file that is imported into runtests.py
# Only one variable, metric_info is to be defined in this file.  
# metric_info is a dictionary with the primary key being the metric name whose value is another
# dictionary.  

# This secondary dictionary MUST define the following (string) keys:
#       best    :   when analyzing multiple iterations, how is the "best" value calculated
#                   valid values are one of [ min | max | mean | median ]
#                   note that these are NOT strings, but method names

# The following are optional (string) keys:
#       desc    :   A string description of the metric
#       name    :   Display this name instead of the metric name
#       unit    :   Metric Unit
#       largerIsFaster : Boolean indicating whether larger values are considered
#                        to be faster.  Defaults to False

# If a test reports a metric not defined in the metric_info dictionary, min is used as the default

import sys
# add parent dir to python module search path
sys.path.append('..')
from util.runtestUtils import mean, median

metric_info = {
        'time': {
            'best':min,
            'unit':'milliseconds',
        },
        'compile_time': {
            'best':min,
            'unit':'seconds',
        },
        'memory':{
            'best':max,
            'unit':'k',
        },
        'size':{
            'best':min,
            'unit':'bytes',
        },
        'v8':   {
            'best':max, 
            'desc': 'custom v8 normalized metric (hardcoded in the test)',
            'largerIsFaster':True
        },
        'iterations/second':{
            'best':max,
            'largerIsFaster':True,
        },
        
        # steps is a metric output by the avm when compiled with --enable-count-steps
        'steps':{
            'best':mean,
            'desc':'internal steps reported by vm composed of call_count+loop_count.  See Bug 568933 for details'
        },
        # vprof / perfm metrics
        'vprof-compile-time': {
            'best':min,
            'name':'vprof: compile (time)'
        },
        'vprof-code-size' : {
            'best':min,
            'name':'vprof: code size (bytes)'
        },
        'vprof-verify-time' : {
            'best':min,
            'name':'vprof: verify & IR gen (time)'
        },
        'vprof-ir-bytes': {
            'best':min,
            'name':'vprof: mir/lir bytes'
        },
        'vprof-ir-time': {
            'best':min,
            'name':'vprof: mir/lir (# of inst)'
        },
        'vprof-count': {
            'best':min,
            'name':'vprof: count'
        }
}
