noinst_DATA= \
	lambdamanual.html \
	trans_xml_for_cat.pl \
	user_manual_style.css \
	lambda.html

lambdamanual.html: ${srcdir}/lambdamanual_src.html ${srcdir}/trans_xml_for_cat.pl
	${srcdir}/trans_xml_for_cat.pl <${srcdir}/lambdamanual_src.html >lambdamanual.html

EXTRA_DIST=\
	lambdamanual_src.html \
	trans_xml_for_cat.pl \
	user_manual_style.css \
	lambda.html

CLEANFILES=lambdamanual.html
