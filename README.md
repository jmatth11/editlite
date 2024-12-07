# Editlite

A simple text editor for learning purposes.

I'm hoping to have simple features of displaying text, scrolling, editing, searching.
I'd also like to have it dynamically load long lines and files so I don't have to load the entire thing in memory at once.

Not sure how configurable I will allow it, but I will try to make it customizable with the basic features.

## Demo

https://github.com/jmatth11/editlite/assets/5776125/ccb72b91-3f02-45f0-9bcf-7f2b041c1613

TODO
- [x] navigating the document.
- [x] Loading more of the file in memory when navigating.
- [x] Edit the document.
    - [x] Handle newlines properly.
    - [x] Fix bugs with certain edge cases on deleting to previous line and moving to newline
- [x] Implement "command" mode.
    - [x] Support plugins with command mode.
- [ ] Save file.
- [ ] Command to load another file.
- [ ] Allow load in of config file to adjust settings.
- [ ] Searching through document.
