// Game_Music_Emu 0.5.2. http://www.slack.net/~ant/

#include "Music_Emu.h"

#if !GME_DISABLE_STEREO_DEPTH
#include "Effects_Buffer.h"
#endif
#include "blargg_endian.h"
#include <string.h>
#include <ctype.h>

/* Copyright (C) 2003-2006 Shay Green. This module is free software; you
can redistribute it and/or modify it under the terms of the GNU Lesser
General Public License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version. This
module is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
details. You should have received a copy of the GNU Lesser General Public
License along with this module; if not, write to the Free Software Foundation,
Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA */

#include "blargg_source.h"

#ifndef GME_TYPE_LIST

// Default list of all supported game music types (copy this to blargg_config.h
// if you want to modify it)
#define GME_TYPE_LIST \
	gme_ay_type,\
	gme_gbs_type,\
	gme_gym_type,\
	gme_hes_type,\
	gme_kss_type,\
	gme_nsf_type,\
	gme_nsfe_type,\
	gme_sap_type,\
	gme_spc_type,\
	gme_vgm_type,\
	gme_vgz_type

#endif

static gme_type_t const gme_type_list_ [] = { GME_TYPE_LIST, 0 };

gme_type_t const* gme_type_list()
{
	return gme_type_list_;
}

const char* gme_identify_header( void const* header )
{
	switch ( get_be32( header ) )
	{
		case BLARGG_4CHAR('Z','X','A','Y'):  return "AY";
		case BLARGG_4CHAR('G','B','S',0x01): return "GBS";
		case BLARGG_4CHAR('G','Y','M','X'):  return "GYM";
		case BLARGG_4CHAR('H','E','S','M'):  return "HES";
		case BLARGG_4CHAR('K','S','C','C'):
		case BLARGG_4CHAR('K','S','S','X'):  return "KSS";
		case BLARGG_4CHAR('N','E','S','M'):  return "NSF";
		case BLARGG_4CHAR('N','S','F','E'):  return "NSFE";
		case BLARGG_4CHAR('S','A','P',0x0D): return "SAP";
		case BLARGG_4CHAR('S','N','E','S'):  return "SPC";
		case BLARGG_4CHAR('V','g','m',' '):  return "VGM";
	}
	return "";
}

static void to_uppercase( const char* in, int len, char* out )
{
	for ( int i = 0; i < len; i++ )
	{
		if ( !(out [i] = toupper( in [i] )) )
			return;
	}
	*out = 0; // extension too long
}

gme_type_t gme_identify_extension( const char* extension_ )
{
	char const* end = strrchr( extension_, '.' );
	if ( end )
		extension_ = end + 1;
	
	char extension [6];
	to_uppercase( extension_, sizeof extension, extension );
	
	for ( gme_type_t const* types = gme_type_list_; *types; types++ )
		if ( !strcmp( extension, (*types)->extension_ ) )
			return *types;
	return 0;
}

gme_err_t gme_identify_file( const char* path, gme_type_t* type_out )
{
	*type_out = gme_identify_extension( path );
	// TODO: don't examine header if file has extension?
	if ( !*type_out )
	{
		char header [4];
		GME_FILE_READER in;
		RETURN_ERR( in.open( path ) );
		RETURN_ERR( in.read( header, sizeof header ) );
		*type_out = gme_identify_extension( gme_identify_header( header ) );
	}
	return 0;   
}

gme_err_t gme_open_data( void const* data, long size, Music_Emu** out, long sample_rate )
{
	require( (data || !size) && out );
	*out = 0;
	
	gme_type_t file_type = 0;
	if ( size >= 4 )
		file_type = gme_identify_extension( gme_identify_header( data ) );
	if ( !file_type )
		return gme_wrong_file_type;
	
	Music_Emu* emu = gme_new_emu( file_type, sample_rate );
	CHECK_ALLOC( emu );
	
	gme_err_t err = gme_load_data( emu, data, size );
	
	if ( err )
		delete emu;
	else
		*out = emu;
	
	return err;
}

gme_err_t gme_open_file( const char* path, Music_Emu** out, long sample_rate )
{
	require( path && out );
	*out = 0;
	
	GME_FILE_READER in;
	RETURN_ERR( in.open( path ) );
	
	char header [4];
	int header_size = 0;
	
	gme_type_t file_type = gme_identify_extension( path );
	if ( !file_type )
	{
		header_size = sizeof header;
		RETURN_ERR( in.read( header, sizeof header ) );
		file_type = gme_identify_extension( gme_identify_header( header ) );
	}
	if ( !file_type )
		return gme_wrong_file_type;
	
	Music_Emu* emu = gme_new_emu( file_type, sample_rate );
	CHECK_ALLOC( emu );
	
	// optimization: avoids seeking/re-reading header
	Remaining_Reader rem( header, header_size, &in );
	gme_err_t err = emu->load( rem );
	in.close();
	
	if ( err )
		delete emu;
	else
		*out = emu;
	
	return err;
}

Music_Emu* gme_new_emu( gme_type_t type, long rate )
{
	if ( type )
	{
		if ( rate == gme_info_only )
			return type->new_info();
		
		Music_Emu* me = type->new_emu();
		if ( me )
		{
		#if !GME_DISABLE_STEREO_DEPTH
			if ( type->flags_ & 1 )
			{
				me->effects_buffer = BLARGG_NEW Effects_Buffer;
				if ( me->effects_buffer )
					me->set_buffer( me->effects_buffer );
			}
			
			if ( !(type->flags_ & 1) || me->effects_buffer )
		#endif
			{
				if ( !me->set_sample_rate( rate ) )
				{
					check( me->type() == type );
					return me;
				}
			}
			delete me;
		}
	}
	return 0;
}

gme_err_t gme_load_file( Music_Emu* me, const char* path ) { return me->load_file( path ); }

gme_err_t gme_load_data( Music_Emu* me, void const* data, long size )
{
	Mem_File_Reader in( data, size );
	return me->load( in );
}

gme_err_t gme_load_custom( Music_Emu* me, gme_reader_t func, long size, void* data )
{
	Callback_Reader in( func, size, data );
	return me->load( in );
}

void gme_delete( Music_Emu* me ) { delete me; }

gme_type_t gme_type( Music_Emu const* me ) { return me->type(); }

const char* gme_warning( Music_Emu* me ) { return me->warning(); }

int gme_track_count( Music_Emu const* me ) { return me->track_count(); }

const char* gme_track_info( Music_Emu const* me, track_info_t* out, int track )
{
	return me->track_info( out, track );
}

void gme_set_stereo_depth( Music_Emu* me, double depth )
{
#if !GME_DISABLE_STEREO_DEPTH
	if ( me->effects_buffer )
		STATIC_CAST(Effects_Buffer*,me->effects_buffer)->set_depth( depth );
#endif
}

void*     gme_user_data      ( Music_Emu const* me )                { return me->user_data(); }
void      gme_set_user_data  ( Music_Emu* me, void* new_user_data ) { me->set_user_data( new_user_data ); }
void      gme_set_user_cleanup(Music_Emu* me, gme_user_cleanup_t func ) { me->set_user_cleanup( func ); }

gme_err_t gme_start_track    ( Music_Emu* me, int index )           { return me->start_track( index ); }
gme_err_t gme_play           ( Music_Emu* me, long n, short* p )    { return me->play( n, p ); }
void      gme_set_fade       ( Music_Emu* me, long start_msec )     { me->set_fade( start_msec ); }
int       gme_track_ended    ( Music_Emu const* me )                { return me->track_ended(); }
long      gme_tell           ( Music_Emu const* me )                { return me->tell(); }
gme_err_t gme_seek           ( Music_Emu* me, long msec )           { return me->seek( msec ); }
int       gme_voice_count    ( Music_Emu const* me )                { return me->voice_count(); }
void      gme_ignore_silence ( Music_Emu* me, int disable )         { me->ignore_silence( disable != 0 ); }
void      gme_set_tempo      ( Music_Emu* me, double t )            { me->set_tempo( t ); }
void      gme_mute_voice     ( Music_Emu* me, int index, int mute ) { me->mute_voice( index, mute != 0 ); }
void      gme_mute_voices    ( Music_Emu* me, int mask )            { me->mute_voices( mask ); }
void      gme_set_equalizer  ( Music_Emu* me, gme_equalizer_t const* eq ) { me->set_equalizer( *eq ); }
gme_equalizer_t gme_equalizer( Music_Emu const* me )                { return me->equalizer(); }
const char** gme_voice_names ( Music_Emu const* me )                { return me->voice_names(); }
