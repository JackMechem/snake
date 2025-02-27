project('snake', 'cpp', 'c', version: '0.0.1', default_options : ['cpp_std=c++20'])

ncursesdep = dependency('ncurses')

srcfiles = ['src/main.cpp', 'src/snake.cpp', 'include/game.hpp', 'include/snake.hpp']

executable('snake', srcfiles, dependencies: [ncursesdep])
