/* See LICENSE file for copyright and license details. */

/* appearance */
static unsigned int borderpx  = 1;        /* border pixel of windows */
static unsigned int snap      = 32;       /* snap pixel */

static const unsigned int gappih    = 20;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 10;       /* vert inner gap between windows */
static const unsigned int gappoh    = 10;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 30;       /* vert outer gap between windows and screen edge */
static       int          smartgaps = 0;        /* 1 means no outer gap when there is only one window */

static       int showbar        = 1;        /* 0 means no bar */
static       int topbar         = 1;        /* 0 means bottom bar */
static const int vertpadbar     = 10;       /* vertical padding for statusbar */
static const int horizpadbar    = 10;       /* horizontal padding for statusbar */
static       int floatposgrid_x = 5;        /* float grid columns */
static       int floatposgrid_y = 5;        /* float grid rows */

static char font[]            = "SauceCodePro Nerd Font:size=10";
static char dmenufont[]       = "SauceCodePro Nerd Font:size=10";
static const char *fonts[]          = { font };
static char normbgcolor[]           = "#222222";
static char normbordercolor[]       = "#444444";
static char normfgcolor[]           = "#bbbbbb";
static char selfgcolor[]            = "#eeeeee";
static char selbordercolor[]        = "#005577";
static char selbgcolor[]            = "#005577";
static char *colors[][3] = {
       /*               fg           bg           border   */
       [SchemeNorm] = { normfgcolor, normbgcolor, normbordercolor },
       [SchemeSel]  = { selfgcolor,  selbgcolor,  selbordercolor  },
};

typedef struct {
    const char *name;
    const void *cmd;
} Sp;
const char *spcmd1[] = {"st", "-n", "spterm", "-g", "120x34", NULL };
const char *spcmd2[] = {"keepassxc", NULL };
const char *spcmd3[] = {"qalculate-gtk", "--class=Qalculate", NULL };

static Sp scratchpads[] = {
    /* name          cmd  */
    {"spterm",      spcmd1},
    {"keepassxc",   spcmd2},
    {"spcalc",      spcmd3},
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
    /* xprop(1):
     *	WM_CLASS(STRING) = instance, class
     *	WM_NAME(STRING) = title
     */
    /* class      instance     title       tags mask     isfloating  floatpos               monitor */
    { "Gimp",     NULL,        NULL,       0,            1,          NULL,                  -1 },
    { "Firefox",  NULL,        NULL,       1 << 8,       0,          NULL,                  -1 },
    { NULL,       "spterm",	   NULL,	   SPTAG(0),     1,          NULL,                  -1 },
    { NULL,		  "keepassxc", NULL,	   SPTAG(1),     0,          NULL,                  -1 },
    { "Qalculate", NULL,       NULL,       SPTAG(2),     1,          "50% 50% 50% 50%",     -1 },
};

/* layout(s) */
static float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static int nmaster     = 1;    /* number of clients in master area */
static int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

#define FORCE_VSPLIT 1  /* nrowgrid layout: force two clients to always split vertically */
#include "vanitygaps.c"

static const Layout layouts[] = {
    /* symbol     arrange function */
    { "[]=",      tile },    /* Default: Master on left, slaves on right */
    { "TTT",      bstack },  /* Master on top, slaves on bottom */

    { "[@]",      spiral },  /* Fibonacci spiral */
    { "[\\]",     dwindle }, /* Decreasing in size right and leftward */

    { "[D]",      deck },    /* Master on left, slaves in monocle-like mode on right */
    { "[M]",      monocle }, /* All windows on top of each other */

    { "|M|",      centeredmaster },         /* Master in middle, slaves on sides */
    { ">M>",      centeredfloatingmaster }, /* Same but master floats */

//    { "===",      bstackhoriz },
//    { "HHH",      grid },
//    { "###",      nrowgrid },
//    { "---",      horizgrid },
//    { ":::",      gaplessgrid },

    { "><>",      NULL },    /* no layout function means floating behavior */
    { NULL,       NULL },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG)												\
    &((Keychord){1, {{MODKEY, KEY}},								view,           {.ui = 1 << TAG} }), \
    &((Keychord){1, {{MODKEY|ControlMask, KEY}},					toggleview,     {.ui = 1 << TAG} }), \
    &((Keychord){1, {{MODKEY|ShiftMask, KEY}},						tag,            {.ui = 1 << TAG} }), \
    &((Keychord){1, {{MODKEY|ControlMask|ShiftMask, KEY}},			toggletag,      {.ui = 1 << TAG} }),

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

#define STATUSBAR "dwmblocks"

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbordercolor, "-sf", selfgcolor, NULL };
static const char *roficmd[] = { "rofi", "-modi", "drun", "-show", "drun", "-config", "~/.config/rofi/rofidmenu.rasi", NULL };
static const char *termcmd[]  = { "st", NULL };

/*
 * Xresources preferences to load at startup
 */
ResourcePref resources[] = {
        { "font",               STRING,  &font },
        { "dmenufont",          STRING,  &dmenufont },
        { "normbgcolor",        STRING,  &normbgcolor },
        { "normbordercolor",    STRING,  &normbordercolor },
        { "normfgcolor",        STRING,  &normfgcolor },
        { "selbgcolor",         STRING,  &selbgcolor },
        { "selbordercolor",     STRING,  &selbordercolor },
        { "selfgcolor",         STRING,  &selfgcolor },
        { "borderpx",           INTEGER, &borderpx },
        { "snap",               INTEGER, &snap },
        { "showbar",            INTEGER, &showbar },
        { "topbar",             INTEGER, &topbar },
        { "nmaster",            INTEGER, &nmaster },
        { "resizehints",        INTEGER, &resizehints },
        { "mfact",              FLOAT,   &mfact },
};

#include <X11/XF86keysym.h>
#include "shiftview.c"

static const Keychord *keychords[] = {
    /* Keys                                                function        argument */
    /* Open rofi to lauch a program */
    &((Keychord){1, {{MODKEY, XK_p}},                      spawn,          { .v = roficmd } }),

    /* Start a terminal */
    &((Keychord){1, {{MODKEY|ShiftMask, XK_Return}},       spawn,          { .v = termcmd } }),

    /* Toggle the bar on and off */
    &((Keychord){1, {{MODKEY, XK_b}},                      togglebar,      { 0 } }),

    /* Rotate the stack around */
    &((Keychord){1, {{MODKEY, XK_period}},                 rotatestack,    { .i = +1 } }),
    &((Keychord){1, {{MODKEY, XK_comma}},                  rotatestack,    { .i = -1 } }),

    /* Rotate focus around the stack */
    &((Keychord){1, {{MODKEY, XK_apostrophe}},             focusstack,     { .i = +1 } }),
    &((Keychord){1, {{MODKEY, XK_semicolon}},              focusstack,     { .i = -1 } }),

    /* change number of windows in master area */
    &((Keychord){1, {{MODKEY, XK_i}},                      incnmaster,     { .i = +1 } }),
    &((Keychord){1, {{MODKEY, XK_d}},                      incnmaster,     { .i = -1 } }),

    /* Move vertical divider between master and stack left and right */
    &((Keychord){1, {{MODKEY, XK_Left}},                   setmfact,       { .f = -0.05 } }),
    &((Keychord){1, {{MODKEY, XK_Right}},                  setmfact,       { .f = +0.05 } }),

    /* Move active window from stack to master */
    &((Keychord){1, {{MODKEY, XK_Return}},                 zoom,           { 0 } }),

    /* increase/decrease gaps */
    &((Keychord){1, {{MODKEY, XK_z}},                      incrgaps,       { .i = +3 } }),
    &((Keychord){1, {{MODKEY|ShiftMask, XK_z}},            incrgaps,       { .i = -3 } }),

    /* Toggle/set default gaps */
    &((Keychord){1, {{MODKEY, XK_g}},                      togglegaps,     { 0 } }),
    &((Keychord){1, {{MODKEY|ShiftMask, XK_g}},            defaultgaps,    { 0 } }),

    /* Switch to previously viewed tag */
    &((Keychord){1, {{MODKEY, XK_Tab}},                    view,           { 0 } }),

    /* Switch to next/previous tag */
    &((Keychord){1, {{MODKEY|ShiftMask, XK_t}},            shiftview,      { .i = -1 } }),
    &((Keychord){1, {{MODKEY, XK_t}},                      shiftview,      { .i = +1 } }),

    &((Keychord){1, {{MODKEY|ShiftMask, XK_c}},            killclient,     { 0 } }),

    /* Layouts */
    &((Keychord){2, {{MODKEY, XK_l}, {0, XK_t}},           setlayout,      { .v = &layouts[0] } }), /* tile */
    &((Keychord){2, {{MODKEY, XK_l}, {0, XK_b}},           setlayout,      { .v = &layouts[1] } }), /* bstack */
    &((Keychord){2, {{MODKEY, XK_l}, {0, XK_s}},           setlayout,      { .v = &layouts[2] } }), /* spiral */
    &((Keychord){2, {{MODKEY, XK_l}, {0, XK_d}},           setlayout,      { .v = &layouts[3] } }), /* dwindle */
    &((Keychord){2, {{MODKEY, XK_l}, {0, XK_k}},           setlayout,      { .v = &layouts[4] } }), /* deck */
    &((Keychord){2, {{MODKEY, XK_l}, {0, XK_m}},           setlayout,      { .v = &layouts[5] } }), /* monocle */
    &((Keychord){2, {{MODKEY, XK_l}, {0, XK_c}},           setlayout,      { .v = &layouts[6] } }), /* centered master */
    &((Keychord){2, {{MODKEY, XK_l}, {0, XK_f}},           setlayout,      { .v = &layouts[7] } }), /* centered floating master */

    /* Floating Layout */
    &((Keychord){1, {{MODKEY, XK_space}},                  setlayout,      { 0 } }),

    /* Make window floating */
    &((Keychord){1, {{MODKEY|ShiftMask, XK_space}},        togglefloating, { 0 } }),

    /* Toggle sticky window */
    &((Keychord){1, {{MODKEY|ShiftMask, XK_s}},            togglesticky,   { 0 } }),

    /* Toggle fullscreen */
    &((Keychord){1, {{MODKEY, XK_f}},                      togglefullscr,  { 0 } }),

    /* View all the windows on all the tags (tag 0 = all windows) */
    &((Keychord){1, {{MODKEY, XK_0}},                      view,           { .ui = ~0 } }),

    /* Make focused window appear on all tags */
    /* &((Keychord){1, {{MODKEY|ShiftMask, XK_0}},            tag,            { .ui = ~0 } }), */

    /* Rotate focus between monitors */
    /* &((Keychord){1, {{MODKEY, XK_comma}},                  focusmon,       { .i = -1 } }), */
    /* &((Keychord){1, {{MODKEY, XK_period}},                 focusmon,       { .i = +1 } }), */

    /* &((Keychord){1, {{MODKEY|ShiftMask, XK_comma}},        tagmon,         { .i = -1 } }), */
    /* &((Keychord){1, {{MODKEY|ShiftMask, XK_period}},       tagmon,         { .i = +1 } }), */

    /* Audio */
    &((Keychord){1, {{0, XF86XK_AudioMute}},               spawn,          SHCMD("wpctl set-mute @DEFAULT_AUDIO_SINK@ toggle; kill -44 $(pidof dwmblocks)") }),
    &((Keychord){1, {{0, XF86XK_AudioRaiseVolume}},        spawn,          SHCMD("wpctl set-volume @DEFAULT_AUDIO_SINK@ 2%+; kill -44 $(pidof dwmblocks)") }),
    &((Keychord){1, {{0, XF86XK_AudioLowerVolume}},        spawn,          SHCMD("wpctl set-volume @DEFAULT_AUDIO_SINK@ 2%-; kill -44 $(pidof dwmblocks)") }),

    /* Brightness */
    &((Keychord){1, {{0, XF86XK_MonBrightnessUp}},         spawn,          { .v = (const char*[]){ "xbacklight", "-inc", "5", NULL } } }),
    &((Keychord){1, {{0, XF86XK_MonBrightnessDown}},       spawn,          { .v = (const char*[]){ "xbacklight", "-dec", "5", NULL } } }),

    /* Scratchpad terminal */
    &((Keychord){1, {{MODKEY, XK_grave}},                  togglescratch,  { .ui = 0 } }),

    /* Scratchpad keepass */
    &((Keychord){2, {{MODKEY, XK_s}, {0, XK_k}},           togglescratch,  { .ui = 1 } }), /* keepass */

    /* Scratchpad calculator */
    &((Keychord){2, {{MODKEY, XK_s}, {0, XK_c}},           togglescratch,  { .ui = 2 } }), /* calculator */

    /* Quit DWM */
    &((Keychord){1, {{MODKEY|ShiftMask, XK_q}},            quit,           { 0 } }),

    /* Tags */
    TAGKEYS(                        XK_1,                      0)
    TAGKEYS(                        XK_2,                      1)
    TAGKEYS(                        XK_3,                      2)
    TAGKEYS(                        XK_4,                      3)
    TAGKEYS(                        XK_5,                      4)
    TAGKEYS(                        XK_6,                      5)
    TAGKEYS(                        XK_7,                      6)
    TAGKEYS(                        XK_8,                      7)
    TAGKEYS(                        XK_9,                      8)
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
    /* click                event mask      button          function        argument */
    { ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
    { ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
    { ClkWinTitle,          0,              Button2,        zoom,           {0} },
    { ClkStatusText,        0,              Button1,        sigstatusbar,   {.i = 1} },
    { ClkStatusText,        0,              Button2,        sigstatusbar,   {.i = 2} },
    { ClkStatusText,        0,              Button3,        sigstatusbar,   {.i = 3} },
    { ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
    { ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
    { ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
    { ClkTagBar,            0,              Button1,        view,           {0} },
    { ClkTagBar,            0,              Button3,        toggleview,     {0} },
    { ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
    { ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

/* signal definitions */
/* signum must be greater than 0 */
/* trigger signals using `xsetroot -name "fsignal:<signum>"` */
static Signal signals[] = {
    /* signum       function        argument  */
    { 9,            quit,           {0} },
};
