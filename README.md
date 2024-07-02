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

Normal traversing of file and editing.

https://github.com/jmatth11/editlite/assets/5776125/ccb72b91-3f02-45f0-9bcf-7f2b041c1613

Using plugins loaded through dynamic libraries to handle commands (save/quit/files).

https://github.com/jmatth11/editlite/assets/5776125/589983fb-8f68-4c3b-b5cd-6a51018833d9


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
- [ ] Run in "memory limited" mode for memory constrained systems.
- [ ] allow rendering callback for plugins.
