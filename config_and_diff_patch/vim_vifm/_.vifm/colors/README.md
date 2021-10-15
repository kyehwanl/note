Various colorschemes for [vifm](http://vifm.info/).
- Astrell
- Dark Desert
- Default
- Desert
- G80
- Matrix
- Midnight Commander
- ph
- Solarized Dark
- Zenburn
- Zenburn_1 (juef's version)

The solarized-dark theme is based on [istib](https://github.com/istib)'s [version](https://github.com/istib/dotfiles/blob/master/vifm/vifm-colors).

# Get them #
If you would like to have just one theme you could download it via wget, for example:
`wget -P ~/.vifm/colors https://raw.githubusercontent.com/vifm/vifm-colors/master/solarized-dark`

If you prefer to download all themes you could set it up with git, and stay up to date.

`rm -rf ~/.vifm/colors`

`git clone https://github.com/vifm/vifm-colors ~/.vifm/colors`

To check for updates just type `git pull` in `~/.vifm/colors`.

# Preview them #
A preview of all color themes contained in this repository are available at [this vifm site](http://vifm.info/colorschemes.shtml).

# Set them #
Load with `:colorscheme theme-name` in vifm, or write `colorscheme theme-name` in vifm's configuration file `~/.vifm/vifmrc`.

If you have any color themes that are not in this repo, feel free to fork, add it and send me a pull request!


<highlight command>
  :highlight

       :hi[ghlight]
              will show information about all highlight groups in the  current
              directory.

       :hi[ghlight] clear
              will reset all highlighting to builtin defaults.

       :hi[ghlight] group-name
              will  show  information  on given highlight group of the primary
              color scheme.

       :hi[ghlight] group-name cterm=style | ctermfg=color | ctermbg=color
              sets  style  (cterm),  foreground  (ctermfg)  or/and  background
              (ctermbg)  parameters of highlight groups of the current primary
              color scheme.

       All style values as well as color names are case insensitive.

       Available style values (some of them can be combined):
        - bold
        - underline
        - reverse or inverse
        - standout
        - none

       Available group-name values:
        - Win - color of all windows (views, dialogs, menus)
        - Border - color of vertical parts of the border
        - TopLineSel - top line color of the current pane
        - TopLine - top line color of the other pane
        - CmdLine - the command line/status bar color
        - ErrorMsg - color of error messages in the status bar
        - StatusLine - color of the line above the status bar
        - WildMenu - color of the wild menu items
        - CurrLine - line at cursor position in active view
        - OtherLine - line at cursor position in inactive view
        - Selected - color of selected files
        - Directory - color of directories
        - Link - color of symbolic links in the views
        - BrokenLink - color of broken symbolic links
        - Socket - color of sockets
        - Device - color of block and character devices
        - Executable - color of executable files
        - Fifo - color of fifo pipes

       Available colors:
        - -1 or default or none - default or transparent
        - black   and lightblack
        - red     and lightred
        - green   and lightgreen
        - yellow  and lightyellow
        - blue    and lightblue
        - magenta and lightmagenta
        - cyan    and lightcyan
        - white   and lightwhite
        - 0-255 - corresponding colors from 256-color palette

       Light versions of colors are regular colors with  bold  attribute  set.
       So  order  of  arguments  of  :highlight  command is important and it's
       better to put "cterm" in front of others to prevent it from overwriting
       attributes set by "ctermfg" or "ctermbg" arguments.

       For  convenience of color scheme authors xterm-like names for 256 color
       palette   is   also   supported.    The   mapping   is    taken    from
       http://vim.wikia.com/wiki/Xterm256_color_names_for_console_Vim
       Duplicated entries were altered by adding  an  underscore  followed  by
       numerical suffix.

         0 Black                  86 Aquamarine1           172 Orange3
         1 Red                    87 DarkSlateGray2        173 LightSalmon3_2
         2 Green                  88 DarkRed_2             174 LightPink3
         3 Yellow                 89 DeepPink4_2           175 Pink3
         4 Blue                   90 DarkMagenta           176 Plum3
         5 Magenta                91 DarkMagenta_2         177 Violet
         6 Cyan                   92 DarkViolet            178 Gold3_2
         7 White                  93 Purple                179 LightGoldenrod3
         8 LightBlack             94 Orange4_2             180 Tan
         9 LightRed               95 LightPink4            181 MistyRose3
        10 LightGreen             96 Plum4                 182 Thistle3
        11 LightYellow            97 MediumPurple3         183 Plum2
        12 LightBlue              98 MediumPurple3_2       184 Yellow3_2
        13 LightMagenta           99 SlateBlue1            185 Khaki3
        14 LightCyan             100 Yellow4               186 LightGoldenrod2
        15 LightWhite            101 Wheat4                187 LightYellow3
        16 Grey0                 102 Grey53                188 Grey84
        17 NavyBlue              103 LightSlateGrey        189 LightSteelBlue1
        18 DarkBlue              104 MediumPurple          190 Yellow2
        19 Blue3                 105 LightSlateBlue        191 DarkOliveGreen1
        20 Blue3_2               106 Yellow4_2             192 DarkOliveGreen1_2
        21 Blue1                 107 DarkOliveGreen3       193 DarkSeaGreen1_2
        22 DarkGreen             108 DarkSeaGreen          194 Honeydew2
        23 DeepSkyBlue4          109 LightSkyBlue3         195 LightCyan1
        24 DeepSkyBlue4_2        110 LightSkyBlue3_2       196 Red1
        25 DeepSkyBlue4_3        111 SkyBlue2              197 DeepPink2
        26 DodgerBlue3           112 Chartreuse2_2         198 DeepPink1
        27 DodgerBlue2           113 DarkOliveGreen3_2     199 DeepPink1_2
        28 Green4                114 PaleGreen3_2          200 Magenta2_2
        29 SpringGreen4          115 DarkSeaGreen3         201 Magenta1
        30 Turquoise4            116 DarkSlateGray3        202 OrangeRed1
        31 DeepSkyBlue3          117 SkyBlue1              203 IndianRed1
        32 DeepSkyBlue3_2        118 Chartreuse1           204 IndianRed1_2
        33 DodgerBlue1           119 LightGreen_2          205 HotPink
        34 Green3                120 LightGreen_3          206 HotPink_2
        35 SpringGreen3          121 PaleGreen1            207 MediumOrchid1_2
        36 DarkCyan              122 Aquamarine1_2         208 DarkOrange
        37 LightSeaGreen         123 DarkSlateGray1        209 Salmon1
        38 DeepSkyBlue2          124 Red3                  210 LightCoral
        39 DeepSkyBlue1          125 DeepPink4_3           211 PaleVioletRed1
        40 Green3_2              126 MediumVioletRed       212 Orchid2
        41 SpringGreen3_2        127 Magenta3              213 Orchid1
        42 SpringGreen2          128 DarkViolet_2          214 Orange1
        43 Cyan3                 129 Purple_2              215 SandyBrown
        44 DarkTurquoise         130 DarkOrange3           216 LightSalmon1
        45 Turquoise2            131 IndianRed             217 LightPink1
        46 Green1                132 HotPink3              218 Pink1
        47 SpringGreen2_2        133 MediumOrchid3         219 Plum1
        48 SpringGreen1          134 MediumOrchid          220 Gold1
        49 MediumSpringGreen     135 MediumPurple2         221 LightGoldenrod2_2
        50 Cyan2                 136 DarkGoldenrod         222 LightGoldenrod2_3
        51 Cyan1                 137 LightSalmon3          223 NavajoWhite1
        52 DarkRed               138 RosyBrown             224 MistyRose1
        53 DeepPink4             139 Grey63                225 Thistle1
        54 Purple4               140 MediumPurple2_2       226 Yellow1
        55 Purple4_2             141 MediumPurple1         227 LightGoldenrod1
        56 Purple3               142 Gold3                 228 Khaki1
        57 BlueViolet            143 DarkKhaki             229 Wheat1
        58 Orange4               144 NavajoWhite3          230 Cornsilk1
        59 Grey37                145 Grey69                231 Grey100
        60 MediumPurple4         146 LightSteelBlue3       232 Grey3
        61 SlateBlue3            147 LightSteelBlue        233 Grey7
        62 SlateBlue3_2          148 Yellow3               234 Grey11
        63 RoyalBlue1            149 DarkOliveGreen3_3     235 Grey15
        64 Chartreuse4           150 DarkSeaGreen3_2       236 Grey19
        65 DarkSeaGreen4         151 DarkSeaGreen2         237 Grey23
        66 PaleTurquoise4        152 LightCyan3            238 Grey27
        67 SteelBlue             153 LightSkyBlue1         239 Grey30
        68 SteelBlue3            154 GreenYellow           240 Grey35
        69 CornflowerBlue        155 DarkOliveGreen2       241 Grey39
        70 Chartreuse3           156 PaleGreen1_2          242 Grey42
        71 DarkSeaGreen4_2       157 DarkSeaGreen2_2       243 Grey46
        72 CadetBlue             158 DarkSeaGreen1         244 Grey50
        73 CadetBlue_2           159 PaleTurquoise1        245 Grey54
        74 SkyBlue3              160 Red3_2                246 Grey58
        75 SteelBlue1            161 DeepPink3             247 Grey62
        76 Chartreuse3_2         162 DeepPink3_2           248 Grey66
        77 PaleGreen3            163 Magenta3_2            249 Grey70
        78 SeaGreen3             164 Magenta3_3            250 Grey74
        79 Aquamarine3           165 Magenta2              251 Grey78
        80 MediumTurquoise       166 DarkOrange3_2         252 Grey82
        81 SteelBlue1_2          167 IndianRed_2           253 Grey85
        82 Chartreuse2           168 HotPink3_2            254 Grey89
        83 SeaGreen2             169 HotPink2              255 Grey93
        84 SeaGreen1             170 Orchid
        85 SeaGreen1_2           171 MediumOrchid1

       There are two colors (foreground and  background)  and  only  one  bold
       attribute.   Thus  single  bold  attribute  affects  both  colors  when
       "reverse" attribute is used in vifm run inside terminal  emulator.   At
       the  same  time  linux native console can handle boldness of foreground
       and background colors independently, but for consistency with  terminal
       emulators  this is available only implicitly by using light versions of
       colors.  This behaviour might be changed in the future.

       Although vifm supports 256 colors in a sense they are supported  by  UI
       drawing  library,  whether  you  will be able to use all of them highly
       depends on your terminal.  To set up terminal properly, make sure  that
       $TERM  in  the  environment  you  run  vifm is set to name of 256-color
       terminal (on  *nixes  it  can  also  be  set  via  X  resources),  e.g.
       xterm-256color.   One  can  find  list  of  available terminal names by
       listing /usr/lib/terminfo/.  Number of  colors  supported  by  terminal
       with current settings can be checked via "tput colors" command.

       Here  is  the hierarchy of highlight groups, which you need to know for
       using transparency:
         StatusLine
           WildMenu
         Border
         CmdLine
           ErrorMsg
         Win
           Directory
           Link
           BrokenLink
           Socket
           Device
           Fifo
           Executable
             Selected
               CurrLine
               OtherLine
         TopLine
           TopLineSel

       "none" means default terminal color for highlight groups at  the  first
       level of the hierarchy and transparency for all others.
