project('snake', 'cpp', 'c', version: '0.0.1')

ncursesdep = dependency('ncurses')

srcfiles = ['src/main.cpp', 'src/snake.cpp', 'include/game.hpp', 'include/snake.hpp']

executable('snake', srcfiles, dependencies: [ncursesdep], install: true)
