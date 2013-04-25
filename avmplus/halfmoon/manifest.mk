# ***** BEGIN LICENSE BLOCK *****
# Version: MPL 2.0/GPL 2.0/LGPL 2.1
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Software distributed under the License is distributed on an "AS IS" basis,
# WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
# for the specific language governing rights and limitations under the
# License.
#
# The Original Code is [Open Source Virtual Machine].
#
# The Initial Developer of the Original Code is
# Adobe System Incorporated.
# Portions created by the Initial Developer are Copyright (C) 2005-2006
# the Initial Developer. All Rights Reserved.
#
# Contributor(s):
#
# Alternatively, the contents of this file may be used under the terms of
# either the GNU General Public License Version 2 or later (the "GPL"), or
# the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
# in which case the provisions of the GPL or the LGPL are applicable instead
# of those above. If you wish to allow use of your version of this file only
# under the terms of either the GPL or the LGPL, and not to allow others to
# use your version of this file under the terms of the MPL, indicate your
# decision by deleting the provisions above and replace them with the notice
# and other provisions required by the GPL or the LGPL. If you do not delete
# the provisions above, a recipient may use your version of this file under
# the terms of any one of the MPL, the GPL or the LGPL.
#
# ***** END LICENSE BLOCK *****

# keep files in case-insenstive alphabetical order.
avmplus_CXXSRCS := $(avmplus_CXXSRCS) \
  $(curdir)/hm-abcbuilder.cpp \
  $(curdir)/hm-abcgraph.cpp \
  $(curdir)/hm-cleaner.cpp \
  $(curdir)/hm-constraints.cpp \
  $(curdir)/hm-dead.cpp \
  $(curdir)/hm-debug.cpp \
  $(curdir)/hm-deoptimizer.cpp \
  $(curdir)/hm-bailouts.cpp \
  $(curdir)/hm-dominatortree.cpp \
  $(curdir)/hm-exec.cpp \
  $(curdir)/hm-identityanalyzer.cpp \
  $(curdir)/hm-inline.cpp \
  $(curdir)/hm-instrfactory.cpp \
  $(curdir)/hm-instrgraph.cpp \
  $(curdir)/hm-interpreter.cpp \
  $(curdir)/hm-liremitter.cpp \
  $(curdir)/hm-models.cpp \
  $(curdir)/hm-prettyprint.cpp \
  $(curdir)/hm-profiler.cpp \
  $(curdir)/hm-schedulers.cpp \
  $(curdir)/hm-specializer.cpp \
  $(curdir)/hm-stubs.cpp \
  $(curdir)/hm-templatebuilder.cpp \
  $(curdir)/hm-typeanalyzer.cpp \
  $(curdir)/hm-typeinference.cpp \
  $(curdir)/hm-types.cpp \
  $(curdir)/hm-valnum.cpp \
  $(curdir)/hm-check.cpp \
  $(curdir)/hm-main.cpp \
  $(curdir)/hm-jitmanager.cpp \
  $(curdir)/hm-jitwriter.cpp \
  $(curdir)/profiler/profiler-methodprofile.cpp \
  $(curdir)/profiler/profiler-liremitter.cpp \
  $(curdir)/profiler/profiler-profiledstate.cpp
  
  $(NULL)
