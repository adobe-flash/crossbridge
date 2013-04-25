/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package SWF
{
    public final class Constants
    {
        public static const TAG_End : uint = 0;
        public static const TAG_ShowFrame : uint = 1;
        public static const TAG_DefineShape : uint = 2;
        public static const TAG_FreeCharacter : uint = 3;
        public static const TAG_PlaceObject : uint = 4;
        public static const TAG_RemoveObject : uint = 5;
        public static const TAG_DefineBits : uint = 6;
        public static const TAG_DefineButton : uint = 7;
        public static const TAG_JPEGTables : uint = 8;
        public static const TAG_SetBackgroundColor : uint = 9;
        public static const TAG_DefineFont : uint = 10;
        public static const TAG_DefineText : uint = 11;
        public static const TAG_DoAction : uint = 12;
        public static const TAG_DefineFontInfo : uint = 13;
        public static const TAG_DefineSound : uint = 14;
        public static const TAG_StartSound : uint = 15;
        public static const TAG_StopSound : uint = 16;
        public static const TAG_DefineButtonSound : uint = 17;
        public static const TAG_SoundStreamHead : uint = 18;
        public static const TAG_SoundStreamBlock : uint = 19;
        public static const TAG_DefineBitsLossless : uint = 20;
        public static const TAG_DefineBitsJPEG2 : uint = 21;
        public static const TAG_DefineShape2 : uint = 22;
        public static const TAG_DefineButtonCxform : uint = 23;
        public static const TAG_Protect : uint = 24;
        public static const TAG_PathsArePostScript : uint = 25;
        public static const TAG_PlaceObject2 : uint = 26;
        public static const TAG_RemoveObject2 : uint = 28;
        public static const TAG_SyncFrame : uint = 29;
        public static const TAG_FreeAll : uint = 31;
        public static const TAG_DefineShape3 : uint = 32;
        public static const TAG_DefineText2 : uint = 33;
        public static const TAG_DefineButton2 : uint = 34;
        public static const TAG_DefineBitsJPEG3 : uint = 35;
        public static const TAG_DefineBitsLossless2 : uint = 36;
        public static const TAG_DefineEditText : uint = 37;
        public static const TAG_DefineVideo : uint = 38;
        public static const TAG_DefineSprite : uint = 39;
        public static const TAG_NameCharacter : uint = 40;
        public static const TAG_ProductInfo : uint = 41;
        public static const TAG_DefineTextFormat : uint = 42;
        public static const TAG_FrameLabel : uint = 43;
        public static const TAG_DefineBehavior : uint = 44;
        public static const TAG_SoundStreamHead2 : uint = 45;
        public static const TAG_DefineMorphShape : uint = 46;
        public static const TAG_FrameTag : uint = 47;
        public static const TAG_DefineFont2 : uint = 48;
        public static const TAG_GenCommand : uint = 49;
        public static const TAG_DefineCommandObj : uint = 50;
        public static const TAG_CharacterSet : uint = 51;
        public static const TAG_FontRef : uint = 52;
        public static const TAG_DefineFunction : uint = 53;
        public static const TAG_PlaceFunction : uint = 54;
        public static const TAG_GenTagObject : uint = 55;
        public static const TAG_ExportAssets : uint = 56;
        public static const TAG_ImportAssets : uint = 57;
        public static const TAG_EnableDebugger : uint = 58;
        public static const TAG_DoInitAction : uint = 59;
        public static const TAG_DefineVideoStream : uint = 60;
        public static const TAG_VideoFrame : uint = 61;
        public static const TAG_DefineFontInfo2 : uint = 62;
        public static const TAG_DebugID : uint = 63;
        public static const TAG_EnableDebugger2 : uint = 64;
        public static const TAG_ScriptLimits : uint = 65;
        public static const TAG_SetTabIndex : uint = 66;
        // public static const TAG_DefineShape4 : uint = 67;
        // public static const TAG_DefineMorphShape2 : uint = 68;
        public static const TAG_FileAttributes : uint = 69;
        public static const TAG_PlaceObject3 : uint = 70;
        public static const TAG_ImportAssets2 : uint = 71;
        public static const TAG_DoABC : uint = 72;
        public static const TAG_DefineFontAlignZones : uint = 73;
        public static const TAG_CSMSettings : uint = 74;
        public static const TAG_DefineFont3 : uint = 75;
        public static const TAG_SymbolClass : uint = 76;
        public static const TAG_Metadata : uint = 77;
        public static const TAG_DefineScalingGrid : uint = 78;
        public static const TAG_DefineDeviceVideo : uint = 79;
        public static const TAG_DoABC2 : uint = 82;
        public static const TAG_DefineShape4 : uint = 83;
        public static const TAG_DefineMorphShape2 : uint = 84;
        public static const TAG_PlaceImagePrivate : uint = 85;
        public static const TAG_DefineSceneAndFrameLabelData : uint = 86;
        public static const TAG_DefineBinaryData : uint = 87;
        public static const TAG_DefineFontName : uint = 88;
        public static const TAG_StartSound2 : uint = 89;
        public static const TAG_DefineBitsJPEG64 : uint = 90;
        public static const TAG_DefineFont4 : uint = 91;
    
        public static const tagNames:Array = [
        "End",                  // 00
        "ShowFrame",            // 01
        "DefineShape",          // 02
        "FreeCharacter",        // 03
        "PlaceObject",          // 04
        "RemoveObject",         // 05
        "DefineBits",           // 06
        "DefineButton",         // 07
        "JPEGTables",           // 08
        "SetBackgroundColor",   // 09

        "DefineFont",           // 10
        "DefineText",           // 11
        "DoAction",             // 12
        "DefineFontInfo",       // 13

        "DefineSound",          // 14
        "StartSound",           // 15
        "StopSound",            // 16

        "DefineButtonSound",    // 17

        "SoundStreamHead",      // 18
        "SoundStreamBlock",     // 19

        "DefineBitsLossless",   // 20
        "DefineBitsJPEG2",      // 21

        "DefineShape2",         // 22
        "DefineButtonCxform",   // 23

        "Protect",              // 24

        "PathsArePostScript",   // 25

        "PlaceObject2",         // 26
        "27 (invalid)",         // 27
        "RemoveObject2",        // 28

        "SyncFrame",            // 29
        "30 (invalid)",         // 30
        "FreeAll",              // 31

        "DefineShape3",         // 32
        "DefineText2",          // 33
        "DefineButton2",        // 34
        "DefineBitsJPEG3",      // 35
        "DefineBitsLossless2",  // 36
        "DefineEditText",       // 37

        "DefineVideo",          // 38

        "DefineSprite",         // 39
        "NameCharacter",        // 40
        "ProductInfo",          // 41
        "DefineTextFormat",     // 42
        "FrameLabel",           // 43
        "DefineBehavior",       // 44
        "SoundStreamHead2",     // 45
        "DefineMorphShape",     // 46
        "FrameTag",             // 47
        "DefineFont2",          // 48
        "GenCommand",           // 49
        "DefineCommandObj",     // 50
        "CharacterSet",         // 51
        "FontRef",              // 52

        "DefineFunction",       // 53
        "PlaceFunction",        // 54

        "GenTagObject",         // 55

        "ExportAssets",         // 56
        "ImportAssets",         // 57

        "EnableDebugger",       // 58

        "DoInitAction",         // 59
        "DefineVideoStream",    // 60
        "VideoFrame",           // 61

        "DefineFontInfo2",      // 62
        "DebugID",              // 63
        "EnableDebugger2",      // 64
        "ScriptLimits",         // 65

        "SetTabIndex",          // 66

        "DefineShape4",         // 67
        "DefineMorphShape2",    // 68

        "FileAttributes",       // 69

        "PlaceObject3",         // 70
        "ImportAssets2",        // 71

        "DoABC",                // 72
        "DefineFontAlignZones", // 73
        "CSMSettings",          // 74
        "DefineFont3",          // 75
        "SymbolClass",          // 76
        "Metadata",             // 77
        "DefineScalingGrid",    // 78
        "DefineDeviceVideo",    // 79
        "80 (invalid)",         // 80
        "81 (invalid)",         // 81
        "DoABC2",               // 82
        "DefineShape4",         // 83
        "DefineMorphShape2",    // 84
        "PlaceImagePrivate",    // 85
        "DefineSceneAndFrameLabelData", // 86
        "DefineBinaryData",     // 87
        "DefineFontName",       // 88
        "StartSound2",           // 89
        "DefineBitsJPEG64",     // 90
        "DefineFont4",          // 91
    ]

    }
}
