<!---
#  This Source Code Form is subject to the terms of the Mozilla Public
#  License, v. 2.0. If a copy of the MPL was not distributed with this
#  file, You can obtain one at http://mozilla.org/MPL/2.0/.
--->

<cfsetting showDebugOutput="false"/>
<cfparam name="url.builder" default="compile"/>
<cfset buildbot_url = "http://10.171.22.12:8750">

<!--- Create the XML data to post. --->
<cfsavecontent variable="rpcRequestPacket">
<?xml version="1.0"?>
<methodCall>
    <methodName>isBuildSetActive</methodName>
        <params>
            <param>
                <value>
                    <array>
                        <data>
                            <value><string><cfoutput>#url.builder#</cfoutput></string></value>
                        </data>
                    </array>
                </value>
            </param>
        </params>
</methodCall>
</cfsavecontent>

<!---
 Post the XML data to catch page. We are going
 to post this value as an XML object will actually
 just post it as an XML body.
--->
<cfhttp url="#buildbot_url#/xmlrpc"
    method="POST"
    result="rpcResponse">
    <cfhttpparam type="XML"
        value="#rpcRequestPacket.Trim()#"/>
</cfhttp>

<cfset active = false>
<cfif rpcResponse.statusCode EQ "200 OK">
    <cfset response = xmlParse(rpcResponse.fileContent)>
    <cfset active = iif(response.methodResponse.params.param.value.boolean.XmlText EQ "1", true, false)>
</cfif>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
 "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">

<html xmlns="http://www.w3.org/1999/xhtml" lang="en" xml:lang="en">
<head>
    <link href="buildbot.css" rel="stylesheet" type="text/css" />
</head>

<body vlink="#800080"><cfif active>active<cfelse>idle</cfif></body>
