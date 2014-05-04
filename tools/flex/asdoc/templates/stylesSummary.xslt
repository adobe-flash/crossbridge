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
	<xsl:output method="xml" version="1.0" encoding="UTF-8" indent="yes"/>
	<xsl:param name="ditaFileDir" select="''"/>
	<xsl:template match="/">
		<allClasses>
			<xsl:apply-templates select="//apiItemRef">
				<xsl:sort select="@href" order="ascending"/>
			</xsl:apply-templates>
		</allClasses>
	</xsl:template>
	<xsl:template match="apiItemRef">
		<xsl:variable name="ditaFileName">
			<xsl:value-of select="concat($ditaFileDir,@href)"/>
		</xsl:variable>
		<xsl:for-each select="document($ditaFileName)/apiPackage">
			<apiPackage id="{@id}">
				<apiName>
					<xsl:value-of select="./apiName"/>
				</apiName>
				<xsl:apply-templates select="apiClassifier"/>
			</apiPackage>
		</xsl:for-each>
	</xsl:template>
	<xsl:template match="apiClassifier">
		<apiClassifier id="{@id}">
			<apiName>
				<xsl:value-of select="apiName/."/>
			</apiName>
			<xsl:apply-templates select="apiClassifierDetail"/>
			<xsl:apply-templates select="prolog/asMetadata"/>
		</apiClassifier>
	</xsl:template>
	<xsl:template match="apiClassifierDetail">
		<apiClassifierDetail>
			<xsl:apply-templates select="apiClassifierDef"/>
		</apiClassifierDetail>
	</xsl:template>
	<xsl:template match="prolog/asMetadata">
		<prolog>
			<asMetadata>
				<xsl:apply-templates select="styles"/>
			</asMetadata>
		</prolog>
	</xsl:template>
	<xsl:template match="styles">
		<styles>
			<xsl:apply-templates select="node()"/>
		</styles>
	</xsl:template>
	<xsl:template match="apiClassifierDef">
		<apiClassifierDef>
			<xsl:apply-templates select="node()"/>
		</apiClassifierDef>
	</xsl:template>
	<xsl:template match="node()">
		<xsl:copy copy-namespaces="no">
			<xsl:copy-of select="@*"/>
			<xsl:apply-templates select="node()"/>
		</xsl:copy>
	</xsl:template>
</xsl:stylesheet>
