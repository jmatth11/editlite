# Editlite

A simple text editor for learning purposes.

Currently supports:
    - Viewing a file.
    - Editing/saving files.
    - incrementally loading a file that you are viewing (so it's not all in memory at once)
    - file traversing to a different file.
    - searching for text within the currently loaded file.
    - plugins to add extra functionality.
    - some vim motions
    - configuration through toml file

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
- [ ] Implement some more basic vim motions.
    - [x] implement jumping with ctrl-d/u
    - [ ] jump between words
- [x] Searching through document.
- [ ] Add "home" path to look for config and plugins.
- [ ] Prompt to add filename to unnamed pages.
- [ ] Run in "memory limited" mode for memory constrained systems.
- [ ] allow rendering callback for plugins.
