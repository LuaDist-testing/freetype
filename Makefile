ifeq ($(OS),Windows_NT)
DLLEXT=dll
else
DLLEXT=so
endif

LUA_VERSION=5.2

PREFIX?=/usr
INSTALL_LUA=$(PREFIX)/share/lua/$(LUA_VERSION)
INSTALL_BIN=$(PREFIX)/lib/lua/$(LUA_VERSION)
CPPFLAGS=-Wall -O2 "-DLUAFREETYPE_API=__attribute__((visibility (\"default\")))"
CFLAGS=-fPIC
CPPFLAGS+=-I/usr/include/lua$(LUA_VERSION) $$(pkg-config --cflags freetype2)
LDFLAGS=-fvisibility=hidden $$(pkg-config --libs freetype2)

build:freetype.$(DLLEXT)

clean:
	rm -f freetype.$(DLLEXT)

install:build
	install -d $(INSTALL_BIN)
	install freetype.$(DLLEXT) $(INSTALL_BIN)

uninstall:
	rm -f $(INSTALL_BIN)/freetype.$(DLLEXT)

freetype.so: CPPFLAGS+=-Dluaopen_module=luaopen_freetype

%.$(DLLEXT): %.cc compat.o
	$(LINK.c) -shared $^ $(LOADLIBES) $(LDLIBS) -o $@

test:build
	@lua test.lua

.PHONY:build clean install test

# Copyright (c) Jérôme Vuarand
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

