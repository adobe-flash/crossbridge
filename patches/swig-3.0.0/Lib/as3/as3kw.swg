/*
** Copyright 2012 Adobe Systems Incorporated. All Rights Reserved.
**
** You may copy, modify, distribute, and make derivative works based on
** this software, in source code or object code form, without
** restriction. If you distribute the software to others, you may do
** so according to the terms of your choice. This software is offered as
** is, without warranty of any kind.
**/

#ifndef AS3_AS3KW_SWG_
#define AS3_AS3KW_SWG_

/* Warnings for AS3 keywords */
#define AS3KW(x) %keywordwarn("'" `x` "' is an AS3 keyword, renaming to '_"`x`"'",rename="_%s")  `x`

/*
   from
   http://help.adobe.com/en_US/FlashPlatform/reference/actionscript/3/statements.html
   http://www.adobe.com/go/learn_as3_reservedwords_en
*/

AS3KW(as);
AS3KW(break);
AS3KW(case);
AS3KW(continue);
AS3KW(default);
AS3KW(do);
AS3KW(while);
AS3KW(else);
AS3KW(for);
AS3KW(in);
AS3KW(instanceof);
AS3KW(is);
AS3KW(each);	
AS3KW(if);	
AS3KW(label);
AS3KW(return);	
AS3KW(super);	
AS3KW(switch);	
AS3KW(throw);
AS3KW(try);	
AS3KW(finally);	
AS3KW(catch);	
AS3KW(with);
AS3KW(dynamic);	
AS3KW(final);	
AS3KW(internal);	
AS3KW(native);
AS3KW(override);	
AS3KW(private);	
AS3KW(protected);	
AS3KW(public);
AS3KW(static);	
AS3KW(class); 	
AS3KW(const);	
AS3KW(extends);
AS3KW(function); 	
AS3KW(implements);	
AS3KW(interface);
AS3KW(namespace);	
AS3KW(package);	
AS3KW(var);
AS3KW(import);	
AS3KW(include);	
AS3KW(use);	
AS3KW(AS3);
AS3KW(flash_proxy);	
AS3KW(object_proxy);	
AS3KW(false);	
AS3KW(null);
AS3KW(this);
AS3KW(true);

/* While these have special meaning, they are valid identifier names, so we
 * don't restrict their usage. (get and set are also very commonly used
 * by swig-generated code).
AS3KW(get);	
AS3KW(set);	
*/

#undef AS3KW

#endif //AS3_AS3KW_SWG_
