<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="xml" encoding="utf-8" indent="yes"/>
	<xsl:template match="*">
		<xsl:element name="{name(.)}">
			<xsl:apply-templates select="@*"/>
			<xsl:if test="child::* or child::text()">
				<xsl:apply-templates/>
			</xsl:if>
		</xsl:element>
	</xsl:template>
	<xsl:template match="@*">
		<xsl:attribute name="{name(.)}"><xsl:value-of select="."/></xsl:attribute>
	</xsl:template>
	<xsl:template match="processing-instruction()">
		<xsl:copy-of select="."/>
	</xsl:template>
	<xsl:template match="comment()">
		<xsl:comment>
			<xsl:value-of select="."/>
		</xsl:comment>
	</xsl:template>
</xsl:stylesheet>