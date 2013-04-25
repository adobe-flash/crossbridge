#!/bin/bash
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.
(set -o igncr) 2>/dev/null && set -o igncr; # comment is needed


##
# Bring in the environment variables
##
. ./environment.sh


##
# Calculate the change number and change id
##
. ../all/util-calculate-change.sh $1

errors=""
# check to see if we're running under Jenkins
if [ "$JENKINS_HOME" != "" ]; then
    covdatadir=$WS/objdir/shell
else
    covdatadir=$buildsdir/${change}-${changeid}/$platform
fi
for file in $covdatadir/*.cov
do
    export COVFILE=$file
    echo
    echo "coverage file: $COVFILE"
    cd ${basedir}
    $bullseyedir/covdir -q -m $coverage_exclude_regions
    fnpct=`$bullseyedir/covdir -q -m $coverage_exclude_regions | grep Total | awk '{print $6}'`
    cdpct=`$bullseyedir/covdir -q -m $coverage_exclude_regions | grep Total | awk '{print $11}'`

    echo "message: total function coverage:           $fnpct"
    echo "message: total condition/decision coverage: $cdpct"

    # download current historical files
    basename=`basename $file | awk -F. '{print $1}'`
    ${basedir}/build/buildbot/slaves/all/util-download.sh $http_coverage/${basename}-recentfn.csv $covdatadir/${basename}-recentfn.csv
    ${basedir}/build/buildbot/slaves/all/util-download.sh $http_coverage/${basename}-recentbc.csv $covdatadir/${basename}-recentbc.csv
    ${basedir}/build/buildbot/slaves/all/util-download.sh $http_coverage/${basename}-milestonefn.baseline.csv $covdatadir/${basename}-milestonefn.csv
    ${basedir}/build/buildbot/slaves/all/util-download.sh $http_coverage/${basename}-milestonebc.baseline.csv $covdatadir/${basename}-milestonebc.csv
    ${basedir}/build/buildbot/slaves/all/util-download.sh $http_coverage/${basename}-milestone-missingfn.csv $covdatadir/${basename}-milestone-missingfn.csv

    # parse code coverage data and generate csv reports
    ${basedir}/build/buildbot/slaves/all/util-parse-codecoverage.py --covfile=$file --build=$change
    result=$?
    if [ "$result" != "0" ]
    then
        echo "message: error ${basedir}/build/buildbot/slaves/all/util-parse-codecoverage.py failed"
        errors="$file FAILED $errors"
    else
        echo "uploading csv data"
        ${basedir}/build/buildbot/slaves/all/util-upload-scp-mozilla.sh $file ${scp_coverage}/${basename}.cov
        ${basedir}/build/buildbot/slaves/all/util-upload-scp-mozilla.sh $covdatadir/${basename}-info.csv ${scp_coverage}/${basename}-info.csv
        ${basedir}/build/buildbot/slaves/all/util-upload-scp-mozilla.sh $covdatadir/${basename}-summaryfn.csv ${scp_coverage}/${basename}-summaryfn.csv
        ${basedir}/build/buildbot/slaves/all/util-upload-scp-mozilla.sh $covdatadir/${basename}-summarybc.csv ${scp_coverage}/${basename}-summarybc.csv
        ${basedir}/build/buildbot/slaves/all/util-upload-scp-mozilla.sh $covdatadir/${basename}-recentfn.csv ${scp_coverage}/${basename}-recentfn.csv
        ${basedir}/build/buildbot/slaves/all/util-upload-scp-mozilla.sh $covdatadir/${basename}-recentbc.csv ${scp_coverage}/${basename}-recentbc.csv
        ${basedir}/build/buildbot/slaves/all/util-upload-scp-mozilla.sh $covdatadir/${basename}-milestonefn.csv ${scp_coverage}/${basename}-milestonefn.csv
        ${basedir}/build/buildbot/slaves/all/util-upload-scp-mozilla.sh $covdatadir/${basename}-milestonebc.csv ${scp_coverage}/${basename}-milestonebc.csv
        ${basedir}/build/buildbot/slaves/all/util-upload-scp-mozilla.sh $covdatadir/${basename}-missingfn.csv ${scp_coverage}/${basename}-missingfn.csv
        ${basedir}/build/buildbot/slaves/all/util-upload-scp-mozilla.sh $covdatadir/${basename}-missingfn-diffs.csv ${scp_coverage}/${basename}-missingfn-diffs.csv
    
        ${basedir}/build/buildbot/slaves/all/util-upload-ftp-asteam.sh $file $ftp_asteam/$branch/$change-${changeid}/coverage/${basename}.cov
        ${basedir}/build/buildbot/slaves/all/util-upload-ftp-asteam.sh $covdatadir/${basename}-missingfn-diffs.csv $ftp_asteam/$branch/$change-${changeid}/coverage/${basename}-missingfn-diffs.csv
        ${basedir}/build/buildbot/slaves/all/util-upload-ftp-asteam.sh $covdatadir/${basename}-missingfn.csv $ftp_asteam/$branch/$change-${changeid}/coverage/${basename}-missingfn.csv
        ${basedir}/build/buildbot/slaves/all/util-upload-ftp-asteam.sh $covdatadir/${basename}-summaryfn.csv $ftp_asteam/$branch/$change-${changeid}/coverage/${basename}-summaryfn.csv
        ${basedir}/build/buildbot/slaves/all/util-upload-ftp-asteam.sh $covdatadir/${basename}-summarybc.csv $ftp_asteam/$branch/$change-${changeid}/coverage/${basename}-summarybc.csv
        ${basedir}/build/buildbot/slaves/all/util-upload-ftp-asteam.sh $covdatadir/${basename}-recentfn.csv $ftp_asteam/$branch/$change-${changeid}/coverage/${basename}-recentfn.csv
        ${basedir}/build/buildbot/slaves/all/util-upload-ftp-asteam.sh $covdatadir/${basename}-recentbc.csv $ftp_asteam/$branch/$change-${changeid}/coverage/${basename}-recentbc.csv
        ${basedir}/build/buildbot/slaves/all/util-upload-ftp-asteam.sh $covdatadir/${basename}-milestonefn.csv $ftp_asteam/$branch/$change-${changeid}/coverage/${basename}-milestonefn.csv
        ${basedir}/build/buildbot/slaves/all/util-upload-ftp-asteam.sh $covdatadir/${basename}-milestonebc.csv $ftp_asteam/$branch/$change-${changeid}/coverage/${basename}-milestonebc.csv
    fi
done

if [ "$errors" != "" ]
then
    echo "message: code coverage processing failed"
    exit 1
else
    echo "message: finished code coverage processing"
    exit 0
fi
