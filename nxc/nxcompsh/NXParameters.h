/**************************************************************************/
/*                                                                        */
/* Copyright (c) 2001,2002 NoMachine, http://www.nomachine.com.           */
/*                                                                        */
/* NXPROXY, NX protocol compression and NX extensions to this software    */
/* are copyright of NoMachine. Redistribution and use of the present      */
/* software is allowed according to terms specified in the file LICENSE   */
/* which comes in the source distribution.                                */
/*                                                                        */
/* Check http://www.nomachine.com/licensing.html for applicability.       */
/*                                                                        */
/* NX and NoMachine are trademarks of Medialogic S.p.A.                   */
/*                                                                        */
/* All rigths reserved.                                                   */
/*                                                                        */
/**************************************************************************/


#ifndef NX_PARAMETERS_H
#define NX_PARAMETERS_H


#define NX_Undefined                0
#define NX_SessionDirectory         1
#define NX_PersonalDirectory        2   /*  HOME/.nx  */
#define NX_SessionId                3
#define NX_SessionTempDirectory     4   /*  HOME/.nx/temp/process_pid  */
#define NX_CookieTempFilePath       5

/*
 *   NXSSH
 */

#define NX_SshPath                 10
#define NX_SshName                 11
#define NX_SshUsername             12
#define NX_HostName                13
#define NX_HostPort                14
#define NX_SshKeyPath              15   /* SSH key for connection path */
#define NX_SshLogPath              16
#define NX_EncryptionPort          17

#define NX_Ssh_FirstCustomOption   80
#define NX_Ssh_LastCustomOption    99



/*
 *   NXSERVER
 */

#define NX_Hello                   100



#define NX_Username                101
#define NX_Password                102
#define NX_PasswordMD5             103
#define NX_ServerOptions           104
#define NX_ServerVersion           105

#define NX_Server_FirstParameter   110

#define NX_SessionName             110
#define NX_SessionCookie           111
#define NX_SessionType             112  /* unix-kde, unix-gnome, unix-console, windows, vnc, unix-application */
#define NX_ApplicationPath         113  /* applicantion path (only if type=unix-application) */
#define NX_CacheSizeInMemory       114  /* 256k, 1m, 2m, 4m, 8m */
#define NX_LinkSpeed               115  /* modem, isdn, adsl, wan, lan */
#define NX_EnableBackingstore      116  /* 0,1 */
#define NX_DesktopGeometry         117  /* WIDTHxHEIGTH+OFFSET_W+OFFSET_H */
#define NX_Keyboard                118
#define NX_EnableEncryption        119  /* 0,1 */
#define NX_RestoreCache            120  /* 0,1 */
#define NX_EnableTcpNoDelay        121  /* 0,1 */
#define NX_EnableStreamCompression 122  /* 0,1 */
#define NX_EnableMedia             123  /* 0,1 */
#define NX_MediaDriver             124  /* esd, artsd */
#define NX_EnableSamba             125  /* 0,1 */
#define NX_AgentServer             126  /* (only if type=windows or type=vnc) */
#define NX_AgentUser               127  /* (only if type=windows) */
#define NX_AgentPassword           128  /* (only if type=windows or type=vnc) */
#define NX_ImageCompressionType    129  /* -1,0,1,2,3 (depend for SessionType)
                                       SessionType:
                                          - unix:    -1 disable compression
                                                      0 default compression
                                                      1 jpeg compression
                                                      2 png compression

                                          - windows:  0 default compression

                                          - vnc:      0 default compression
                                                      1 Hextile
                                                      2 Tight
                                                      3 Tight + enable jpeg quality
                                       */
#define NX_ImageCompressionLevel   130  /* -1 (disabled), 0..9 (usually for jpeg quality) */
#define NX_EnableLowBandwidth      131  /* 0,1 (only for type=windows) */
#define NX_EnableVirtualDesktop    132  /* 0,1 */
#define NX_EnableTaint             133  /* 0,1 */
#define NX_EnableRender            134  /* 0,1 */
#define NX_UseSessionId            135  /* session id usable by application */
#define NX_Keymap                  136  /* For windows */
#define NX_CacheSizeOnDisk         137  /* 0, 2, 4, 8, 16, 32, 64, 128 MB */

/* pref_node_id */

#define NX_Server_LastParameter    137


/*
 *   NXServer Answers
 */

/* NX_SessionId */
/* NX_SessionType */
/* NX_ProxyCookie */
/* NX_ProxyServer */
/* NX_ProxyPort */
/* NX_ProxyCache */
/* NX_UseEncryption */

#define NX_AgentCookie             201

/*
 *   NXPROXY (temp)
 */

#define NX_ProxyPath               300
#define NX_ProxyName               301
#define NX_ProxyLibraryPath        302
#define NX_ProxyUseOptionsFile     303

#define NX_ProxyServer             310
#define NX_ProxyPort               311

#define NX_ProxyMode               320  /* S, C */
/*      NX_EncryptionPort                (-P) encryption port */
#define NX_ProxyDisplay            321  /* (-D) display */
#define NX_ProxyLogPath            322
#define NX_ProxyOptions            323
#define NX_ProxyOptionsPath        324

#define NX_Proxy_FirstParameter    350

#define NX_ProxySessionName        350
#define NX_ProxyCookie             351  /* returned from server */
#define NX_ProxyRoot               352
#define NX_ProxySessionId          353  /* returned from server */
#define NX_ProxyListen             354  /* returned from server */
#define NX_ProxyStream             355
#define NX_ProxyConnect            356  /* returned from server */
#define NX_ProxyNoDelay            357
#define NX_ProxyTaint              358
#define NX_ProxySamba              359
#define NX_ProxyMediaPort          360
#define NX_ProxyRender             361
#define NX_ProxyCacheType          362  /* not useful since proxy 1.2.2 */

#define NX_Proxy_LastParameter     356

/*
 *   XServer
 */

#define NX_XServerPath             400
#define NX_XServerName             401

#define NX_XServerAuthorityFile    410
#define NX_XServerWindowName       411
#define NX_XServerPort             412

#define NX_XServer_FirstCustomOption  450
#define NX_XServer_LastCustomOption   499

/*
-auth <file authority>
-emulate3buttons
-agent
-hide
-noreset
-name <window name>
-fullscreen or -screen 0 WIDTHxHEIGHT
port (:0)
*/


/*
 *   Samba
 */

#define NX_SambaCommand            501

#endif /* NX_PARAMETERS_H */


