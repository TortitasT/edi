## Todo

- [x] Draw buffer text in multiple lines
- [x] Cursor indicator
- [x] Vim Modes
- [x] Separate globals in its own file
- [x] Vim Mode indicator
- [ ] Scrolling
- [ ] Soft Wrapping
- [ ] Commands
- [ ] Open files
- [ ] Save files
- [ ] Vim motions
- [ ] Config via some small language, lua?
  - [Running a Lua Script from C](https://www.oreilly.com/library/view/creating-solid-apis/9781491986301/ch01.html)
- [ ] Mouse support
- [ ] Line numbers
- [ ] Line signs (the char before the line number)
- [ ] Themes
- [ ] Treesitter
- [ ] LSP

### Bugs

- [ ] When on last character of line you cannot go up
- [ ] When there are two newlines the cursor will jump up awkwardly
- [ ] When you type a longer line then type two short ones, put cursor at the
      end of the first one and go down one, it will go down two lines
- [x] When you type two newlines one shows as a weird character

### Optimizations

- [ ] Type_In_Buffer function probably has a horrible implementation

## Dependencies

```bash
sudo pacman -S cmake sdl2 sdl2_ttf
```

## Resources

- https://github.com/aminosbh/sdl2-ttf-sample
- https://cmake.org/cmake/help/v3.16/guide/tutorial/index.html
