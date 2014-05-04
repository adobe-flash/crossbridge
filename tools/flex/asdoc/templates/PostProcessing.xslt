<?xml version="1.0" encoding="UTF-8"?>
<!--

  Licensed to the Apache Software Foundation (ASF) under one or more
  contributor license agreements.  See the NOTICE file distributed with
  this work for additional information regarding copyright ownership.
  The ASF licenses this file to You under the Apache License, Version 2.0
  (the "License"); you may not use this file except in compliance with
  the License.  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

-->
<xsl:stylesheet version="2.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="xml" version="1.0" encoding="UTF-8" indent="no"/>
	<xsl:param name="ditaFileDir" select="''"/>
	<xsl:variable name="classheader" select="document('ClassHeader.xml')"/>
	<xsl:variable name="fieldSummary" select="document('PP_fieldSummary.xml')"/>
	<xsl:variable name="methodSummary" select="document('pp_methodSummary.xml')"/>
	<xsl:template match="node()">
		<xsl:copy copy-namespaces="no">
			<xsl:copy-of select="@*"/>
			<xsl:apply-templates select="node()"/>
		</xsl:copy>
	</xsl:template>
	<!-- For FieldSummary -->
	<xsl:template match="apiName[parent::apiValue[not(shortdesc)] and parent::apiValue/apiValueDetail/apiValueDef/apiInheritDoc]">
		<xsl:copy-of select="."/>
		<xsl:variable name="name" select="."/>
		<xsl:variable name="apiID" select="ancestor::apiClassifier/@id"/>
		<xsl:choose>
			<xsl:when test="$classheader//apiPackage/apiClassifier[@id=$apiID]">
				<xsl:for-each select="$classheader//apiPackage/apiClassifier[@id=$apiID]/apiClassifierDetail/Inheritancelist/Inheritance/Inherit/@id">
					<xsl:variable name="baseID" select="."/>
					<xsl:choose>
						<xsl:when test="$fieldSummary//apiPackage/apiClassifier[@id=$baseID]/apiOperation[apiName = $name]/shortdesc">
							<xsl:copy-of select="$fieldSummary//apiPackage/apiClassifier[@id=$baseID]/apiOperation[apiName = $name]/shortdesc"/>
						</xsl:when>
					</xsl:choose>
				</xsl:for-each>
			</xsl:when>
			<xsl:otherwise>
				<xsl:message terminate="no">
					<xsl:text>WARNING : Short Description not present for </xsl:text>
					<xsl:value-of select="parent::apiValue/@id"/>
				</xsl:message>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	<xsl:template match="apiValueDef[parent::apiValueDetail[not(apiDesc)] and parent::apiValueDetail/apiValueDef/apiInheritDoc]">
		<xsl:copy-of select="."/>
		<xsl:variable name="name" select="parent::apiValueDetail/parent::apiValue/apiName"/>
		<xsl:variable name="apiID" select="ancestor::apiClassifier/@id"/>
		<xsl:choose>
			<xsl:when test="$classheader//apiPackage/apiClassifier[@id=$apiID]">
				<xsl:for-each select="$classheader//apiPackage/apiClassifier[@id=$apiID]/apiClassifierDetail/Inheritancelist/Inheritance/Inherit/@id">
					<xsl:variable name="baseID" select="."/>
					<xsl:choose>
						<xsl:when test="$fieldSummary//apiPackage/apiClassifier[@id=$baseID]/apiOperation[apiName = $name]/apiValueDetail/apiDesc">
							<xsl:copy-of select="$fieldSummary//apiPackage/apiClassifier[@id=$baseID]/apiOperation[apiName = $name]/apiValueDetail/apiDesc"/>
						</xsl:when>
					</xsl:choose>
				</xsl:for-each>
			</xsl:when>
			<xsl:otherwise>
				<xsl:message terminate="no">
					<xsl:text>WARNING : api Description not present for </xsl:text>
					<xsl:value-of select="parent::apiValueDetail/parent::apiValue/@id"/>
				</xsl:message>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	<!-- For MethodSummary -->
	<xsl:template match="apiName[parent::apiOperation[not(shortdesc)] and parent::apiOperation/apiOperationDetail/apiOperationDef/apiInheritDoc]">
		<xsl:copy-of select="."/>
		<xsl:variable name="name" select="."/>
		<xsl:variable name="apiID" select="ancestor::apiClassifier/@id"/>
		<xsl:choose>
			<xsl:when test="$classheader//apiPackage/apiClassifier[@id=$apiID]">
				<xsl:for-each select="$classheader//apiPackage/apiClassifier[@id=$apiID]/apiClassifierDetail/Inheritancelist/Inheritance/Inherit/@id">
					<xsl:variable name="baseID" select="."/>
					<xsl:choose>
						<xsl:when test="$methodSummary//apiPackage/apiClassifier[@id=$baseID]/apiOperation[apiName = $name]/shortdesc">
							<xsl:copy-of select="$methodSummary//apiPackage/apiClassifier[@id=$baseID]/apiOperation[apiName = $name]/shortdesc"/>
						</xsl:when>
					</xsl:choose>
				</xsl:for-each>
			</xsl:when>
			<xsl:otherwise>
				<xsl:message terminate="no">
					<xsl:text>WARNING : Short Description not present for </xsl:text>
					<xsl:value-of select="parent::apiOperation/@id"/>
				</xsl:message>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	<xsl:template match="apiOperationDef[parent::apiOperationDetail[not(apiDesc) or apiDesc[normalize-space(.)=' ']] and parent::apiOperationDetail/apiOperationDef/apiInheritDoc]">
		<xsl:copy-of select="."/>
		<xsl:variable name="name" select="parent::apiOperationDetail/parent::apiOperation/apiName"/>
		<xsl:variable name="apiID" select="ancestor::apiClassifier/@id"/>
		<xsl:choose>
			<xsl:when test="$classheader//apiPackage/apiClassifier[@id=$apiID]">
				<xsl:for-each select="$classheader//apiPackage/apiClassifier[@id=$apiID]/apiClassifierDetail/Inheritancelist/Inheritance/Inherit/@id">
					<xsl:variable name="baseID" select="."/>
					<xsl:choose>
						<xsl:when test="$methodSummary//apiPackage/apiClassifier[@id=$baseID]/apiOperation[apiName = $name]/apiOperationDetail/apiDesc">
							<xsl:copy-of select="$methodSummary//apiPackage/apiClassifier[@id=$baseID]/apiOperation[apiName = $name]/apiOperationDetail/apiDesc"/>
						</xsl:when>
					</xsl:choose>
				</xsl:for-each>
			</xsl:when>
			<xsl:otherwise>
				<xsl:message terminate="no">
					<xsl:text>WARNING : api Description not present for </xsl:text>
					<xsl:value-of select="parent::apiOperationDetail/parent::apiOperation/@id"/>
				</xsl:message>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	<xsl:template match="shortdesc[normalize-space(.)= ' ']"/>
	<xsl:template match="shortdesc[2]"/>
	<xsl:template match="apiDesc[normalize-space(.) =' ']"/>
	<xsl:template match="apiDesc[2]"/>
</xsl:stylesheet>
