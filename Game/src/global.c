//-------------------------------------------------------------------------
/*
Copyright (C) 1996, 2003 - 3D Realms Entertainment

This file is part of Duke Nukem 3D version 1.5 - Atomic Edition

Duke Nukem 3D is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
aint32_t with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

Original Source: 1996 - Todd Replogle
Prepared for public release: 03/21/2003 - Charlie Wiederhold, 3D Realms
*/
//-------------------------------------------------------------------------

#if WIN32
	#include <fcntl.h>
#endif

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "duke3d.h"
#include "global.h"
#include "names.h"
#include "platform.h"
#include "util_lib.h"

#include <errno.h>
#include <time.h>

char  *mymembuf = NULL;
uint8_t  MusicPtr[72000];


crc32_t crc32lookup[] = {
		// good:
		{ CRC_BASE_GRP_SHAREWARE_13,	"SHAREWARE 1.3D", 11035779 },
		{ CRC_BASE_GRP_FULL_13,			"FULL 1.3D     ", 26524524 },
		{ CRC_BASE_GRP_PLUTONIUM_14,	"PLUTONIUM 1.4 ", 44348015 },
		{ CRC_BASE_GRP_ATOMIC_15,		"ATOMIC 1.5    ", 44356548 },
		// unknown:
		{ 0,							"HACK/UNKNOWN  ", 0}
	};

uint8_t  conVersion = 13;
uint8_t  grpVersion = 0;

// FIX_00015: Backward compliance with older demos (down to demos v27, 28, 116 and 117 only)

// For BYTEVERSION diff, 27/116 vs 28/117 see extras\duke3d.h vs source\duke3d.h
// from the official source code release. 

int BYTEVERSION_27 = 27; // 1.3 under 1.4 Plutonium. Not supported anymore
int BYTEVERSION_116 = 116; // 1.4 Plutonium. Not supported anymore

int BYTEVERSION_28 = 28; // 1.3 under 1.5 engine
int BYTEVERSION_117 = 117; // 1.5 Atomic

int BYTEVERSION_29 = 29; // 1.3 under xDuke v19.6.
int BYTEVERSION_118 = 118; // 1.5 Atomic under xDuke v19.6.

int BYTEVERSION_1_3 = 1; // for 1.3 demos (Not compatible)

int BYTEVERSION			= 119; // xDuke v19.7

short global_random = 0;
short neartagsector = 0, neartagwall = 0, neartagsprite = 0;

int32_t gc = 0,neartaghitdist = 0,lockclock = 0,max_player_health = 0,max_armour_amount = 0,max_ammo_amount[MAX_WEAPONS] = {};

// int32_t temp_data[MAXSPRITES][6];
struct weaponhit hittype[MAXSPRITES] = {};
short spriteq[1024] = {},spriteqloc = 0,spriteqamount=64;

struct animwalltype animwall[MAXANIMWALLS] = {};
short numanimwalls = 0;
int32_t *animateptr[MAXANIMATES] = {}, animategoal[MAXANIMATES] = {}, animatevel[MAXANIMATES] = {}, animatecnt = 0;
// int32_t oanimateval[MAXANIMATES];
short animatesect[MAXANIMATES] = {};
int32_t msx[2048] = {},msy[2048] = {};
short cyclers[MAXCYCLERS][6] = {},numcyclers = 0;

char  fta_quotes[NUMOFFIRSTTIMEACTIVE][64] = {};

uint8_t  tempbuf[2048] = {};
uint8_t packbuf[576] = {};

char  buf[80] = {};

short camsprite = 0;
short mirrorwall[64] = {}, mirrorsector[64] = {}, mirrorcnt = 0;

int current_menu = 0;

uint8_t  betaname[80] = {};

char level_names[44][33] = {};
char level_file_names[44][128] = {};
int32_t partime[44] = {},designertime[44] = {};
char  volume_names[4][33] = { "L.A. MELTDOWN", "LUNAR APOCALYPSE", "SHRAPNEL CITY", "" }; // Names are not in 1.3 con files. MUST be in code.
char  skill_names[5][33] = { "PIECE OF CAKE", "LET'S ROCK", "COME GET SOME", "DAMN I'M GOOD", "" };

volatile int32_t checksume;
int32_t soundsiz[NUM_SOUNDS] = {};

short soundps[NUM_SOUNDS] = {},soundpe[NUM_SOUNDS] = {},soundvo[NUM_SOUNDS] = {};
uint8_t  soundm[NUM_SOUNDS] = {},soundpr[NUM_SOUNDS] = {};
char  sounds[NUM_SOUNDS][14] = {};

short title_zoom;

SAMPLE Sound[ NUM_SOUNDS ] = {};
SOUNDOWNER SoundOwner[NUM_SOUNDS][4] = {};

uint8_t  numplayersprites = 0,earthquaketime = 0;

int32_t fricxv = 0,fricyv = 0;
struct player_orig po[MAXPLAYERS] = {};
struct player_struct ps[MAXPLAYERS] = {};
struct user_defs ud = {};

uint8_t  pus = 0, pub = 0;
uint8_t  syncstat = 0, syncval[MAXPLAYERS][MOVEFIFOSIZ] = {};
int32_t syncvalhead[MAXPLAYERS] = {}, syncvaltail = 0, syncvaltottail = 0;

input sync[MAXPLAYERS] = {}, loc = {};
input recsync[RECSYNCBUFSIZ] = {};
int32_t avgfvel = 0, avgsvel = 0, avgavel = 0, avghorz = 0, avgbits = 0;


input inputfifo[MOVEFIFOSIZ][MAXPLAYERS] = {};
input recsync[RECSYNCBUFSIZ];

int32_t movefifosendplc = 0;

  //Multiplayer syncing variables
short screenpeek = 0;
int32_t movefifoend[MAXPLAYERS] = {};


    //Game recording variables

uint8_t  playerreadyflag[MAXPLAYERS] = {},ready2send = 0;
uint8_t  playerquitflag[MAXPLAYERS] = {};
int32_t vel = 0, svel = 0, angvel = 0, horiz = 0, ototalclock = 0, respawnactortime=768, respawnitemtime=768, groupfile;

int32_t script[MAXSCRIPTSIZE] = {},*scriptptr = NULL,*insptr = NULL,*labelcode = NULL,labelcnt = 0;
int32_t*actorscrptr[MAXTILES] = {},*parsing_actor = NULL;
char  *label = NULL,*textptr = NULL,error = NULL,warning = NULL;
uint8_t killit_flag = 0;
uint8_t  *music_pointer = NULL;
uint8_t  actortype[MAXTILES] = {};


uint8_t  display_mirror = 0,typebuflen = 0;
char typebuf[41] = {};

char  music_fn[4][11][13] = {};
uint8_t music_select = 0;
char  env_music_fn[4][13] = {};
uint8_t  rtsplaying = 0;


short weaponsandammosprites[15] = {
        RPGSPRITE,
        CHAINGUNSPRITE,
        DEVISTATORAMMO,
        RPGAMMO,
        RPGAMMO,
        JETPACK,
        SHIELD,
        FIRSTAID,
        STEROIDS,
        RPGAMMO,
        RPGAMMO,
        RPGSPRITE,
        RPGAMMO,
        FREEZESPRITE,
        FREEZEAMMO
    };

int32_t impact_damage = 0;

        //GLOBAL.C - replace the end "my's" with this
int32_t myx = 0, omyx = 0, myxvel = 0, myy = 0, omyy = 0, myyvel = 0, myz = 0, omyz = 0, myzvel = 0;
short myhoriz = 0, omyhoriz = 0, myhorizoff = 0, omyhorizoff = 0;
short myang = 0, omyang = 0, mycursectnum = 0, myjumpingcounter = 0,frags[MAXPLAYERS][MAXPLAYERS] = {};

uint8_t  myjumpingtoggle = 0, myonground = 0, myhardlanding = 0, myreturntocenter = 0;
int8_t multiwho = 0, multipos = 0, multiwhat = 0, multiflag = 0;

int32_t fakemovefifoplc = 0,movefifoplc = 0;
int32_t myxbak[MOVEFIFOSIZ] = {}, myybak[MOVEFIFOSIZ] = {}, myzbak[MOVEFIFOSIZ] = {};
int32_t myhorizbak[MOVEFIFOSIZ] = {},dukefriction = 0xcc00, show_shareware = 0;

short myangbak[MOVEFIFOSIZ] = {};
char  myname[2048] = "XDUKE";
uint8_t  camerashitable = 0,freezerhurtowner = 0,lasermode = 0;
// CTW - MODIFICATION
// uint8_t  networkmode = 255, movesperpacket = 1,gamequit = 0,playonten = 0,everyothertime;
uint8_t  networkmode = 255, movesperpacket = 1,gamequit = 0,everyothertime = 0;
// CTW END - MODIFICATION
int32_t numfreezebounces=3,rpgblastradius = 0,pipebombblastradius = 0,tripbombblastradius = 0,shrinkerblastradius = 0,morterblastradius = 0,bouncemineblastradius = 0,seenineblastradius = 0;
STATUSBARTYPE sbar = {};

int32_t myminlag[MAXPLAYERS] = {}, mymaxlag = 0, otherminlag = 0, bufferjitter = 1;
short numclouds = 0,clouds[128] = {},cloudx[128] = {},cloudy[128] = {};
int32_t cloudtotalclock = 0,totalmemory = 0;
int32_t numinterpolations = 0, startofdynamicinterpolations = 0;
int32_t oldipos[MAXINTERPOLATIONS] = {};
int32_t bakipos[MAXINTERPOLATIONS] = {};
int32_t *curipos[MAXINTERPOLATIONS] = {};

int _argc = 0;
char  **  _argv = NULL;

// portability stuff.  --ryan.
// A good portion of this was ripped from GPL'd Rise of the Triad.  --ryan.

void FixFilePath(char  *filename)
{
#if UNIX
    uint8_t  *ptr;
    uint8_t  *lastsep = filename;

    if ((!filename) || (*filename == '\0'))
        return;

    if (access(filename, F_OK) == 0)  /* File exists; we're good to go. */
        return;

    for (ptr = filename; 1; ptr++)
    {
        if (*ptr == '\\')
            *ptr = PATH_SEP_CHAR;

        if ((*ptr == PATH_SEP_CHAR) || (*ptr == '\0'))
        {
            uint8_t  pch = *ptr;
            struct dirent *dent = NULL;
            DIR *dir;

            if ((pch == PATH_SEP_CHAR) && (*(ptr + 1) == '\0'))
                return; /* eos is pathsep; we're done. */

            if (lastsep == ptr)
                continue;  /* absolute path; skip to next one. */

            *ptr = '\0';
            if (lastsep == filename) {
                dir = opendir((*lastsep == PATH_SEP_CHAR) ? ROOTDIR : CURDIR);
                
                if (*lastsep == PATH_SEP_CHAR) {
                    lastsep++;
                }
            } 
            else
            {
                *lastsep = '\0';
                dir = opendir(filename);
                *lastsep = PATH_SEP_CHAR;
                lastsep++;
            }

            if (dir == NULL)
            {
                *ptr = PATH_SEP_CHAR;
                return;  /* maybe dir doesn't exist? give up. */
            }

            while ((dent = readdir(dir)) != NULL)
            {
                if (strcasecmp(dent->d_name, lastsep) == 0)
                {
                    /* found match; replace it. */
                    strcpy(lastsep, dent->d_name);
                    break;
                }
            }

            closedir(dir);
            *ptr = pch;
            lastsep = ptr;

            if (dent == NULL)
                return;  /* no match. oh well. */

            if (pch == '\0')  /* eos? */
                return;
        }
    }
#endif
}

#if WIN32
int _dos_findfirst(uint8_t  *filename, int x, struct find_t *f)
{
    int32_t rc = _findfirst(filename, &f->data);
    f->handle = rc;
    if (rc != -1)
    {
        strncpy(f->name, f->data.name, sizeof (f->name) - 1);
        f->name[sizeof (f->name) - 1] = '\0';
        return(0);
    }
    return(1);
}

int _dos_findnext(struct find_t *f)
{
    int rc = 0;
    if (f->handle == -1)
        return(1);   /* invalid handle. */

    rc = _findnext(f->handle, &f->data);
    if (rc == -1)
    {
        _findclose(f->handle);
        f->handle = -1;
        return(1);
    }

    strncpy(f->name, f->data.name, sizeof (f->name) - 1);
    f->name[sizeof (f->name) - 1] = '\0';
    return(0);
}

#elif defined(UNIX) || defined(PLATFORM_MACOSX)
int _dos_findfirst(char  *filename, int x, struct find_t *f)
{
    char  *ptr;

    if (strlen(filename) >= sizeof (f->pattern))
        return(1);

    strcpy(f->pattern, filename);
    FixFilePath(f->pattern);
    ptr = strrchr(f->pattern, PATH_SEP_CHAR);

    if (ptr == NULL)
    {
        ptr = filename;
        f->dir = opendir(CURDIR);
    }
    else
    {
        *ptr = '\0';
        f->dir = opendir(f->pattern);
        memmove(f->pattern, ptr + 1, strlen(ptr + 1) + 1);
    }

    return(_dos_findnext(f));
}


static int check_pattern_nocase(const char  *x, const char  *y)
{
    if ((x == NULL) || (y == NULL))
        return(0);  /* not a match. */

    while ((*x) && (*y))
    {
        if (*x == '*')
        {
            x++;
            while (*y != '\0')
            {
                if (toupper((int) *x) == toupper((int) *y))
                    break;
                y++;
            }
        }

        else if (*x == '?')
        {
            if (*y == '\0')
                return(0);  /* anything but EOS is okay. */
        }

        else
        {
            if (toupper((int) *x) != toupper((int) *y))
                return(0);  /* not a match. */
        }

        x++;
        y++;
    }

    return(*x == *y);  /* it's a match (both should be EOS). */
}

int _dos_findnext(struct find_t *f)
{
    struct dirent *dent;

    if (f->dir == NULL)
        return(1);  /* no such dir or we're just done searching. */

    while ((dent = readdir(f->dir)) != NULL)
    {
        if (check_pattern_nocase(f->pattern, dent->d_name))
        {
            if (strlen(dent->d_name) < sizeof (f->name))
            {
                strcpy(f->name, dent->d_name);
                return(0);  /* match. */
            }
        }
    }

    closedir(f->dir);
    f->dir = NULL;
    return(1);  /* no match in whole directory. */
}
#else
#error please define for your platform.
#endif

void _dos_getdate(struct dosdate_t *date)
{
	time_t curtime = time(NULL);
	struct tm *tm;
	
	if (date == NULL) {
		return;
	}
	
	memset(date, 0, sizeof(struct dosdate_t));
	
	if ((tm = localtime(&curtime)) != NULL) {
		date->day = tm->tm_mday;
		date->month = tm->tm_mon + 1;
		date->year = tm->tm_year + 1900;
		date->dayofweek = tm->tm_wday + 1;
	}
}

int FindDistance2D(int ix, int iy)
{
  int   t;

  ix= abs(ix);        /* absolute values */
  iy= abs(iy);

  if (ix<iy)
  {
     int tmp = ix;
     ix = iy;
     iy = tmp;
  }

  t = iy + (iy>>1);

  return (ix - (ix>>5) - (ix>>7)  + (t>>2) + (t>>6));
}

int FindDistance3D(int ix, int iy, int iz)
{
   int   t;

   ix= abs(ix);           /* absolute values */
   iy= abs(iy);
   iz= abs(iz);

   if (ix<iy)
   {
     int tmp = ix;
     ix = iy;
     iy = tmp;
   }

   if (ix<iz)
   {
     int tmp = ix;
     ix = iz;
     iz = tmp;
   }

   t = iy + iz;

   return (ix - (ix>>4) + (t>>2) + (t>>3));
}
#include "SDL2/SDL.h"
void Error (int errorType, const char *error, ...)
{
   va_list argptr;

   SDL_Quit();

   //FCS: http://duke3d.m-klein.com is obscolete :/ !
   /*
   if(errorType==EXIT_FAILURE)
	   printf("ERROR: Please copy that screen and visit http://duke3d.m-klein.com for report:\n");
	else
		printf("http://duke3d.m-klein.com\n");
	*/


   va_start (argptr, error);
   vprintf(error, argptr);
   va_end (argptr);

   //printf("Press any key to continue...\n");

	// FIX_00043: Nicer exit on error. Ask the user to hit a key on exits and error exits.
   //getch();

   exit (errorType);
}

void write2disk(int line, char * cfilename, char  *filename2write, char  *message)
{
	// usage: write2disk(__LINE__, __FILE__, "c:\temp\my_dbug_file.txt", uint8_t * msg);

	int i, k=0;
	char  filename[2048];
	FILE *pFile;

	for(i=0; cfilename[i]; i++)
	{
		if(cfilename[i]=='\\')
		{
			i++;
			k = 0;
		}
		filename[k++]=(cfilename[i]=='.')?0:cfilename[i];
	}
	pFile = fopen(filename2write,"a");
	fprintf(pFile,"%-4d %-5s %s", line, filename, message);
	fclose(pFile);
}

int32 SafeOpenAppend (const char  *_filename, int32 filetype)
{
	int	handle;
    char  filename[MAX_PATH];
    
    strncpy(filename, _filename, sizeof (filename));
    filename[sizeof (filename) - 1] = '\0';
    FixFilePath(filename);

#if (defined WIN32)
    handle = open(filename,O_RDWR | O_BINARY | O_CREAT | O_APPEND );
#else
	handle = open(filename,O_RDWR | O_BINARY | O_CREAT | O_APPEND , S_IREAD | S_IWRITE);
#endif

	if (handle == -1)
		Error (EXIT_FAILURE, "Error opening for append %s: %s",filename,strerror(errno));

	return handle;
}

boolean SafeFileExists ( const char  * _filename )
{
    char  filename[MAX_PATH];
    strncpy(filename, _filename, sizeof (filename));
    filename[sizeof (filename) - 1] = '\0';
    FixFilePath(filename);

#if( defined WIN32)
    return(access(filename, 6) == 0);
#else
    return(access(filename, F_OK) == 0);
#endif
}


int32 SafeOpenWrite (const char  *_filename, int32 filetype)
{
	int	handle;
    char  filename[MAX_PATH];
    strncpy(filename, _filename, sizeof (filename));
    filename[sizeof (filename) - 1] = '\0';
    FixFilePath(filename);

#if (defined WIN32)
    handle = open(filename,O_RDWR | O_BINARY | O_CREAT | O_TRUNC );
#else
	handle = open(filename,O_RDWR | O_BINARY | O_CREAT | O_TRUNC
	, S_IREAD | S_IWRITE);
#endif

	if (handle == -1)
		Error (EXIT_FAILURE, "Error opening %s: %s",filename,strerror(errno));

	return handle;
}




int32 SafeOpenRead (const char  *_filename, int32 filetype)
{
	int	handle;
    char  filename[MAX_PATH];
    strncpy(filename, _filename, sizeof (filename));
    filename[sizeof (filename) - 1] = '\0';
    FixFilePath(filename);

	handle = open(filename,O_RDONLY | O_BINARY);

	if (handle == -1)
		Error (EXIT_FAILURE, "Error opening %s: %s",filename,strerror(errno));

	return handle;
}


void SafeRead (int32 handle, void *buffer, int32 count)
{
	unsigned	iocount;

	while (count)
	{
		iocount = count > 0x8000 ? 0x8000 : count;
		if (read (handle,buffer,iocount) != (int)iocount)
			Error (EXIT_FAILURE, "File read failure reading %ld bytes",count);
		buffer = (void *)( (byte *)buffer + iocount );
		count -= iocount;
	}
}


void SafeWrite (int32 handle, void *buffer, int32 count)
{
	unsigned	iocount;

	while (count)
	{
		iocount = count > 0x8000 ? 0x8000 : count;
		if (write (handle,buffer,iocount) != (int)iocount)
			Error (EXIT_FAILURE, "File write failure writing %ld bytes",count);
		buffer = (void *)( (byte *)buffer + iocount );
		count -= iocount;
	}
}

void SafeWriteString (int handle, char  * buffer)
{
	unsigned	iocount;

   iocount=strlen(buffer);
	if (write (handle,buffer,iocount) != (int)iocount)
			Error (EXIT_FAILURE, "File write string failure writing %s\n",buffer);
}

void *SafeMalloc (int32_t size)
{
	void *ptr;

    ptr = malloc(size);

	if (!ptr)
      Error (EXIT_FAILURE, "SafeMalloc failure for %lu bytes",size);

	return ptr;
}

void SafeRealloc (void **x, int32 size)
{
	void *ptr;

    ptr = realloc(*x, size);

	if (!ptr)
      Error (EXIT_FAILURE, "SafeRealloc failure for %lu bytes",size);

    *x = ptr;
}

void *SafeLevelMalloc (int32_t size)
{
	void *ptr;

    ptr = malloc(size);

	if (!ptr)
      Error (EXIT_FAILURE, "SafeLevelMalloc failure for %lu bytes",size);

	return ptr;
}

void SafeFree (void * ptr)
{
   if ( ptr == NULL )
      Error (EXIT_FAILURE, "SafeFree : Tried to free a freed pointer\n");

    free(ptr);

}

short	SwapShort (short l)
{
	byte	b1,b2;

	b1 = l&255;
	b2 = (l>>8)&255;

	return (b1<<8) + b2;
}

short	KeepShort (short l)
{
	return l;
}


int32_t	Swapint32_t (int32_t l)
{
	byte	b1,b2,b3,b4;

	b1 = l&255;
	b2 = (l>>8)&255;
	b3 = (l>>16)&255;
	b4 = (l>>24)&255;

	return ((int32_t)b1<<24) + ((int32_t)b2<<16) + ((int32_t)b3<<8) + b4;
}

int32_t	Keepint32_t (int32_t l)
{
	return l;
}


#undef KeepShort
#undef KeepLong
#undef SwapShort
#undef SwapLong

void SwapIntelLong(int32_t *l)
{
    *l = IntelLong(*l);
}

void SwapIntelShort(short *s)
{
    *s = IntelShort(*s);
}

void SwapIntelLongArray(int32_t *l, int num)
{
    while (num--) {
        SwapIntelLong(l);
        l++;
    }
}

void SwapIntelShortArray(short *s, int num)
{
    while (num--) {
        SwapIntelShort(s);
        s++;
    }
}


/* 
  Copied over from Wolf3D Linux: http://www.icculus.org/wolf3d/
  Modified for ROTT.
  Stolen for Duke3D, too.
 */
 
#if UNIX
/*uint8_t  *strlwr(uint8_t  *s)
{
	uint8_t  *p = s;
	
	while (*p) {
		*p = tolower(*p);
		p++;
	}
	
	return s;
}

uint8_t  *strupr(uint8_t  *s)
{
	uint8_t  *p = s;
	
	while (*p) {
		*p = toupper(*p);
		p++;
	}
	
	return s;
}*/
	
uint8_t  *itoa(int value, uint8_t  *string, int radix)
{
	switch (radix) {
		case 10:
			sprintf(string, "%d", value);
			break;
		case 16:
			sprintf(string, "%x", value);
			break;
		default:
			STUBBED("unknown radix");
			break;
	}
	
	return string;
}

uint8_t  *ltoa(int32_t value, uint8_t  *string, int radix)
{
	switch (radix) {
		case 10:
			sprintf(string, "%d", value);
			break;
		case 16:
			sprintf(string, "%x", value);
			break;
		default:
			STUBBED("unknown radix");
			break;
	}
	
	return string;
}

uint8_t  *ultoa(uint32_t value, uint8_t  *string, int radix)
{
	switch (radix) {
		case 10:
			sprintf(string, "%u", value);
			break;
		case 16:
			sprintf(string, "%ux", value);
			break;
		default:
			STUBBED("unknown radix");
			break;
	}
	
	return string;
}
#endif

char  ApogeePath[256];

int setup_homedir (void)
{
#if UNIX
	int err;

	snprintf (ApogeePath, sizeof (ApogeePath), "%s/.duke3d/", getenv ("HOME"));

	//err = mkdir (ApogeePath, S_IRWXU);
	err = mkdir (ApogeePath);
	if (err == -1 && errno != EEXIST)
	{
		fprintf (stderr, "Couldn't create preferences directory: %s\n", 
				strerror (errno));
		return -1;
	}
#else
    sprintf(ApogeePath, ".%s", PATH_SEP_STR);
#endif

	return 0;
}


uint8_t    CheckParm (char  *check)
{
    int i;
    for (i = 1; i < _argc; i++)
    {
        if ((*(_argv[i]) == '-') && (strcmpi(_argv[i] + 1, check) == 0))
            return(i);
    }

    return(0);
}


static void (*shutdown_func)(void) = NULL;

void RegisterShutdownFunction( void (* shutdown) (void) )
{
    shutdown_func = shutdown;
}

void Shutdown(void)
{
    if (shutdown_func != NULL)
    {
        shutdown_func();
        shutdown_func = NULL;
    }
}


