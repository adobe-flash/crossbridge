/*
 * Copyright (c) 1989, 1990, 1991 by the University of Washington
 *
 * For copying and distribution information, please see the file
 * <copyright.h>.
 */

#include <copyright.h>
#include <perrno.h>
#include <stdio.h>

/* This file and perrno.h should always be updated simultaneously */

int	perrno = 0;
int	pwarn = 0;
char	p_err_string[P_ERR_STRING_SZ];
char	p_warn_string[P_ERR_STRING_SZ];

char	*p_err_text[256] = {
    /*   0 */ "Success (prospero)",
    /*   1 */ "Port unknown (dirsend)",
    /*   2 */ "Can't open local UDP port (dirsend)",
    /*   3 */ "Can't resolve hostname (dirsend)",
    /*   4 */ "Unable to send entire message (dirsend)",
    /*   5 */ "Timed out (dirsend)",
    /*   6 */ "Recvfrom failed (dirsend)",
    /*   7 */ "",    /*   8 */ "",    /*   9 */ "",    /*  10 */ "",
    /*  11 */ "Sendto failed (reply)",
    /*  12 */ "",    /*  13 */ "",    /*  14 */ "",    /*  15 */ "",
    /*  16 */ "",    /*  17 */ "",    /*  18 */ "",    /*  19 */ "",
    /*  20 */ "",
    /*  21 */ "Link already exists (vl_insert)",
    /*  22 */ "Link with same name already exists (vl_insert)",
    /*  23 */ "",    /*  24 */ "",
    /*  25 */ "Link already exists (ul_insert)",
    /*  26 */ "Replacing existing link (ul_insert)",
    /*  27 */ "Previous entry not found in dir->ulinks (ul_insert)",
    /*  28 */ "",    /*  29 */ "",    /*  30 */ "",    /*  31 */ "",
    /*  32 */ "",    /*  33 */ "",    /*  34 */ "",    /*  35 */ "",
    /*  36 */ "",    /*  37 */ "",    /*  38 */ "",    /*  39 */ "",
    /*  40 */ "",
    /*  41 */ "Temporary not found (rd_vdir)",
    /*  42 */ "Namespace not closed with object (rd_vdir)",
    /*  43 */ "Alias for namespace not defined (rd_vdir)",
    /*  44 */ "Specified namespace not found (rd_vdir)",
    /*  45 */ "",    /*  46 */ "",    /*  47 */ "",    /*  48 */ "",
    /*  49 */ "",    /*  50 */ "",
    /*  51 */ "File access method not supported (pfs_access)",
    /*  52 */ "",    /*  53 */ "",    /*  54 */ "",
    /*  55 */ "Pointer to cached copy - delete on close (pmap_cache)",
    /*  56 */ "Unable to retrieve file (pmap_cache)",
    /*  57 */ "",    /*  58 */ "",    /*  59 */ "",    /*  60 */ "",
    /*  61 */ "Directory already exists (mk_vdir)",
    /*  62 */ "Link with same name already exists (mk_vdir)",
    /*  63 */ "",    /*  64 */ "",
    /*  65 */ "Not a virtual system (vfsetenv)",
    /*  66 */ "Can't find directory (vfsetenv)",
    /*  67 */ "",    /*  68 */ "",    /*  69 */ "",    /*  70 */ "",
    /*  71 */ "Link already exists (add_vlink)",
    /*  72 */ "Link with same name already exists (add_vlink)",
    /*  73 */ "",    /*  74 */ "",    /*  75 */ "",    /*  76 */ "",
    /*  77 */ "",    /*  78 */ "",    /*  79 */ "",    /*  80 */ "",
    /*  81 */ "",    /*  82 */ "",    /*  83 */ "",    /*  84 */ "",
    /*  85 */ "",    /*  86 */ "",    /*  87 */ "",    /*  88 */ "",
    /*  89 */ "",    /*  90 */ "",    /*  91 */ "",    /*  92 */ "",
    /*  93 */ "",    /*  94 */ "",    /*  95 */ "",    /*  96 */ "",
    /*  97 */ "",    /*  98 */ "",    /*  99 */ "",    /* 100 */ "",
    /* 101 */ "",    /* 102 */ "",    /* 103 */ "",    /* 104 */ "",
    /* 105 */ "",    /* 106 */ "",    /* 107 */ "",    /* 108 */ "",
    /* 109 */ "",    /* 110 */ "",    /* 111 */ "",    /* 112 */ "",
    /* 113 */ "",    /* 114 */ "",    /* 115 */ "",    /* 116 */ "",
    /* 117 */ "",    /* 118 */ "",    /* 119 */ "",    /* 120 */ "",
    /* 121 */ "",    /* 122 */ "",    /* 123 */ "",    /* 124 */ "",
    /* 125 */ "",    /* 126 */ "",    /* 127 */ "",    /* 128 */ "",
    /* 129 */ "",    /* 130 */ "",    /* 131 */ "",    /* 132 */ "",
    /* 133 */ "",    /* 134 */ "",    /* 135 */ "",    /* 136 */ "",
    /* 137 */ "",    /* 138 */ "",    /* 139 */ "",    /* 140 */ "",
    /* 141 */ "",    /* 142 */ "",    /* 143 */ "",    /* 144 */ "",
    /* 145 */ "",    /* 146 */ "",    /* 147 */ "",    /* 148 */ "",
    /* 149 */ "",    /* 150 */ "",    /* 151 */ "",    /* 152 */ "",
    /* 153 */ "",    /* 154 */ "",    /* 155 */ "",    /* 156 */ "",
    /* 157 */ "",    /* 158 */ "",    /* 159 */ "",    /* 160 */ "",
    /* 161 */ "",    /* 162 */ "",    /* 163 */ "",    /* 164 */ "",
    /* 165 */ "",    /* 166 */ "",    /* 167 */ "",    /* 168 */ "",
    /* 169 */ "",    /* 170 */ "",    /* 171 */ "",    /* 172 */ "",
    /* 173 */ "",    /* 174 */ "",    /* 175 */ "",    /* 176 */ "",
    /* 177 */ "",    /* 178 */ "",    /* 179 */ "",    /* 180 */ "",
    /* 181 */ "",    /* 182 */ "",    /* 183 */ "",    /* 184 */ "",
    /* 185 */ "",    /* 186 */ "",    /* 187 */ "",    /* 188 */ "",
    /* 189 */ "",    /* 190 */ "",    /* 191 */ "",    /* 192 */ "",
    /* 193 */ "",    /* 194 */ "",    /* 195 */ "",    /* 196 */ "",
    /* 197 */ "",    /* 198 */ "",    /* 199 */ "",    /* 200 */ "",
    /* 201 */ "",    /* 202 */ "",    /* 203 */ "",    /* 204 */ "",
    /* 205 */ "",    /* 206 */ "",    /* 207 */ "",    /* 208 */ "",
    /* 209 */ "",    /* 210 */ "",    /* 211 */ "",    /* 212 */ "",
    /* 213 */ "",    /* 214 */ "",    /* 215 */ "",    /* 216 */ "",
    /* 217 */ "",    /* 218 */ "",    /* 219 */ "",    /* 220 */ "",
    /* 221 */ "",    /* 222 */ "",    /* 223 */ "",    /* 224 */ "",
    /* 225 */ "",    /* 226 */ "",    /* 227 */ "",    /* 228 */ "",
    /* 229 */ "",
    /* 230 */ "File not found (prospero)",
    /* 231 */ "Directory not found (prospero)",
    /* 232 */ "Symbolic links nested too deep (prospero)",
    /* 233 */ "Environment not initialized - source vfsetup.source then run vfsetup",
    /* 234 */ "Can't traverse an external file (prospero)",
    /* 235 */ "Forwarding chain is too long (prospero)",
    /* 236 */ "",    /* 237 */ "",    /* 238 */ "",    /* 239 */ "",
    /* 240 */ "",    /* 241 */ "",
    /* 242 */ "Authentication required (prospero server)",
    /* 243 */ "Not authorized (prospero server)",
    /* 244 */ "Not found (prospero server)",
    /* 245 */ "Bad version number (prospero server)",
    /* 246 */ "Not a directory (prospero server)",
    /* 247 */ "Already exists (prospero server)",
    /* 248 */ "Link with same name already exists (prospero server)",
    /* 249 */ "",    /* 250 */ "",
    /* 251 */ "Command not implemented on server (dirsrv)",
    /* 252 */ "Bad format for response (dirsrv)",
    /* 253 */ "Protocol error (prospero server)",
    /* 254 */ "Unspecified server failure (prospero server)",
    /* 255 */ "Generic Failure (prospero)"};

char	*p_warn_text[256] = {
    /*   0 */ "No warning",
    /*   1 */ "You are using an old version of this program",
    /*   2 */ "From server",
    /*   3 */ "Unrecognized line in response from server",
  /* 4-254 */ "", "", "", "", "", "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
    /* 255 */ ""};

#ifndef ARCHIE

void perrmesg(char *prefix,int no,char *text)
    {
	fprintf(stderr,"%s%s%s%s\n", (prefix ? prefix : ""),
		(no ? p_err_text[no] : p_err_text[perrno]),
		((text ? (*text ? " - " : "") : 
		  (!no && *p_err_string ? " - " : ""))),
		(text ? text : (no ? "" : p_err_string)));
    }

void sperrmesg(char *buf,char *prefix,int no,char *text)
    {
	sprintf(buf,"%s%s%s%s\n", (prefix ? prefix : ""),
		(no ? p_err_text[no] : p_err_text[perrno]),
		((text ? (*text ? " - " : "") : 
		  (!no && *p_err_string ? " - " : ""))),
		(text ? text : (no ? "" : p_err_string)));
    }

void pwarnmesg(char *prefix,int no,char *text)
    {
	fprintf(stderr,"%s%s%s%s\n", (prefix ? prefix : ""),
		(no ? p_warn_text[no] : p_warn_text[pwarn]),
		((text ? (*text ? " - " : "") : 
		  (!no && *p_warn_string ? " - " : ""))),
		(text ? text : (no ? "" : p_warn_string)));
    }

void spwarnmesg(char *buf,char *prefix,int no,char *text)
    {
	sprintf(buf,"%s%s%s%s\n", (prefix ? prefix : ""),
		(no ? p_warn_text[no] : p_warn_text[pwarn]),
		((text ? (*text ? " - " : "") : 
		  (!no && *p_warn_string ? " - " : ""))),
		(text ? text : (no ? "" : p_warn_string)));
    }
#endif
