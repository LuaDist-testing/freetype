local image = require 'image'
local png = require 'png'
local freetype = require 'freetype.font'

local function round(n) return math.floor(n+0.5) end

-- load the fonts
local vergennes = freetype.new([[test/Vergennes.ttf]], 144, 0)
local narcissus = freetype.new([[test/NarcissusOpenSG.ttf]], 72, 0)

-- rasterize the glyphs
local function char(font, char)
	local img,parm = font:getglyph(string.byte(char), image.new)
	return {img=img, parm=parm}
end
local d = char(vergennes, 'D')
local o = char(narcissus, 'O')
local u = char(narcissus, 'U')
local b = char(narcissus, 'B')

-- compute text dimension
local dminx = d.parm.top - d.parm.glyphsize.height
local ominx = o.parm.top - o.parm.glyphsize.height
local uminx = u.parm.top - u.parm.glyphsize.height
local bminx = b.parm.top - b.parm.glyphsize.height
local minx = math.min(dminx, ominx, uminx, bminx)
local maxx = math.max(d.parm.top, o.parm.top, u.parm.top, b.parm.top)
local xadvance = round(d.parm.advance.x) + round(o.parm.advance.x) + round(u.parm.advance.x) + round(b.parm.advance.x)

-- create output image
local margin = 10
local height = maxx - minx + margin * 2
local width = xadvance + margin * 2
local img = image.new(height, width, 3, 8)

-- fill with white
for r=1,height do for c=1,width do for k=1,3 do
	img[{r,c,k}] = 255
end end end

-- helper function to write a character
local advance = 0
local function putc(char)
	local cimg,cparm = char.img,char.parm
	local h,w = cparm.glyphsize.height,cparm.glyphsize.width
	local left = cparm.left
	local top = cparm.top
	for r=1,h do
		for c=1,w do
			local a = cimg[{r,c,4}]
			local y = top - r
			local x = left + c
			local r2 = (height - margin) - (y - minx)
			local c2 = margin + advance + x
			img[{r2,c2,1}] = 255-a
			img[{r2,c2,2}] = 255-a
			img[{r2,c2,3}] = 255-a
		end
	end
	advance = advance + round(cparm.advance.x)
end

-- write the text
putc(d)
putc(o)
putc(u)
putc(b)

-- save the image
png.write("test/output.png", img)

-- compare the image with the reference
local a
for _,reference in ipairs{
	'test/reference-'..freetype._VERSION..'.png',
	'test/reference-'..freetype._VERSION:match('^%d+%.%d+')..'.png',
	'test/reference-'..freetype._VERSION:match('^%d+')..'.png',
	'test/reference.png',
} do
	local file = io.open(reference, 'rb')
	if file then
		a = file:read('*all')
		file:close()
		break
	end
end
local b = io.open('test/output.png', 'rb'):read('*all')
-- :TODO: only compare the pixmap, not the PNG meta-data
assert(a==b, "generated image do not match reference")

print("all tests passed successfully")

