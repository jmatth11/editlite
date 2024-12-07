# Editlite

A simple text editor for learning purposes.

One of the main goals of this project is to make it easier to open large text files. Currently it incrementally loads an opened file as you scroll through it.
But I'd like to try and implement a "memory constrained" mode where it only loads in a specified buffer length while doing operations on the file.

I've only been testing this on linux so far, so probably won't work anywhere else until I can find time to support other platforms.

## Functionality

Currently supports:
- Viewing a file.
- Editing/saving files.
- incrementally loading a file that you are viewing (so it's not all in memory at once)
- file traversing to a different file.
- searching for text within the currently loaded file.
- plugins to add extra functionality.
- some vim motions
- configuration through toml file

## Setup

This project relies on SDL2 and a small toml library for config files.
The setup is straight forward, run a script to install dependencies and then run the Makefile


```bash
./install_deps.sh
make
```

## Running

Run the program with no arguments or with a file to initially load in.
```bash
./bin/editlite
```

```bash
# with a file
./bin/editlite ./path/to/file.txt
```

## Usage

I'm mainly reusing vim movements at the moment, but I only have a very small subset implemented.

Supported movements:
- h/j/k/l (left,down,up,right)
- CTRL-d/u (down half page, up half page)
- 0/$ (beginning of line, end of line)
- i/a/I/A (insert at cursor, insert after cursor,insert beginning of line, insert at the end of the line)
- : (command mode -- this is different from vim, it lists the plugins you have and hitting enter on one triggers it's action)
- g (just a single g goes to top of file)

- Within File Menu
    - h/j/k/l
    - enter to select file
- Within Find Plugin
    - Type to enter the word you want to find
    - CTRL-n/p/y (navigate down the list, navigate up the list, confirm choice)

## Configuration

Configuration is done with a toml file at `$HOME/.editlite.toml`

The default location for plugins are located in `$HOME/.config/editlite/`

Example file:
```toml
[main]
win_height=720
win_width=1024
fps=120
font_point=64
font_scale=0.3
cursor_color=[170,170,170,119]
font_color=[255,255,255,255]
font_file="/home/<username>/.config/editlite/resources/RobotoMono-Regular.ttf"
plugin_path="/home/<username>/.config/editlite"
plugins=["plugins/files/libfiles.so", "plugins/find/libfind.so"]
```

## Plugins

Plugins are still in flux but you can find examples of them in `plugins/` directory.

Plugins are done in C or anything that can generate a C structured shared library.

Current API:
```c
/**
 * Optional
 * Entry point for setting up your plugin.
 * This is called at launch.
 */
bool setup(struct plugin_interface*);
/**
 * !! Mandatory !!
 * Called when the user executes your plugin from the command menu.
 * This is when you can perform an action or request certain actions with dispatch calls.
 */
bool action(struct plugin_interface *);
/**
 * !! Mandatory !!
 * Called when displaying the name of your plugin.
 */
void get_display_prompt(const char **out);
/**
 * Optional
 * If your plugin needs to render things to the screen.
 * This gets called on every frame update.
 */
bool render(struct display *, struct display_dim *);
/**
 * Optional
 * If your plugin needs to listen for SDL events.
 * This gets called on every frame update.
 */
bool event(SDL_Event *, struct display *, struct display_dim *);
/**
 * Optional
 * Entry point for cleaning up your plugin.
 * This gets called when the user is exiting the app.
 */
bool cleanup(struct plugin_interface*);
```

## Demo

Basic usage: editing, file traversing, and searching through file.

https://github.com/jmatth11/editlite/assets/5776125/bb694295-c093-403a-8afd-08701b4fd3f3


TODO
- [x] navigating the document.
- [x] Loading more of the file in memory when navigating.
- [x] Edit the document.
    - [x] Handle newlines properly.
    - [x] Fix bugs with certain edge cases on deleting to previous line and moving to newline
- [x] Implement "command" mode.
    - [x] Support plugins with command mode.
- [x] Save file.
- [x] Allow load in of config file to adjust settings.
- [x] Command to load another file.
- [x] allow rendering callback for plugins.
- [x] Searching through document.
- [x] Add "home" path to look for config and plugins.
- [ ] Implement some more basic vim motions.
    - [x] implement jumping with ctrl-d/u
    - [ ] jump between words
- [ ] Prompt to add filename to unnamed pages.
- [ ] Run in "memory limited" mode for memory constrained systems.
- [ ] Support Windows
- [ ] Update Makefile to make a release bundle
