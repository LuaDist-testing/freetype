
------------------------------------------------------------------------------

index {
	name = 'LuaFreeType',
	header = [[A low level binding to the FreeType library]],
}

------------------------------------------------------------------------------

header()

chapter('about', "About", [[
[FreeType](http://www.freetype.org/) is a library to read and rasterize font files. This module is a thin wrapper around FreeType's C API.

## Support

All support is done through the [Lua mailing list](http://www.lua.org/lua-l.html).

Feel free to ask for further developments. I can't guarantee that I'll develop everything you ask, but I want my code to be as useful as possible, so I'll do my best to help you. You can also send me request or bug reports (for code and documentation) directly at [jerome.vuarand@gmail.com](mailto:jerome.vuarand@gmail.com).

## Credits

This module is written and maintained by [Jérôme Vuarand](mailto:jerome.vuarand@gmail.com).

It is available under an [MIT-style license](LICENSE.txt).

]])

chapter('installation', 'Installation', [[
LuaFreeType sources are available in its [Mercurial repository](http://hg.piratery.net/luafreetype/):

    hg clone http://hg.piratery.net/luafreetype/

Tarballs of the latest code can be downloaded directly from there: as [gz](http://hg.piratery.net/luafreetype/get/tip.tar.gz), [bz2](http://hg.piratery.net/luafreetype/get/tip.tar.bz2) or [zip](http://hg.piratery.net/luafreetype/get/tip.zip).

Finally, I published some rockspecs:

    luarocks install freetype
]])

chapter('manual', 'Manual', [=[
This module is a low level binding to FreeType, so the API stays as close as possible. Basically all FreeType functions with an `FT_` prefix are functions in the module without the prefix, and arguments and return values are very similar. In other words you should consult the [FreeType documentation](http://www.freetype.org/freetype2/documentation.html), apply some logic to adapt that to Lua, and eventually check this module sources to find peculiarities.

As the API is very similar to the FreeType C API, no detailed documentation will be maintained here. If that proves to be problematic, please let me know, and I'll see if I can improve that.

To use this module:

    local FT = require 'freetype'

Here I named the module `FT`, so that you can write `FT.New_Library` to keep your code similar to C code using the FreeType C API, in this case `FT_New_Library`. Of course you can name your local whatever you want.

Finally note that this binding is not automatically generated, so it is (very) incomplete so far. Please contact me if something you need is missing.
]=])

footer()

------------------------------------------------------------------------------

--[[
Copyright (c) Jérôme Vuarand

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
]]

-- vi: ts=4 sts=4 sw=4 noet
