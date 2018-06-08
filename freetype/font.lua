local _NAME = ...
local _M = {}

local io = require 'io'
local math = require 'math'
local FT = require 'freetype'

local font_methods = {}
local font_mt = {__index=font_methods}

local library = assert(FT.Init_FreeType())

do
	local a,b,c = FT.Library_Version(library)
	_M._VERSION = a..'.'..b..'.'..c
end

function _M.new(name, height, stroke_radius)
	local self = setmetatable({
		name=name,
		height=height,
	}, font_mt)
	self.face = assert(FT.Open_Face(library, {stream=assert(io.open(name, "rb"))}, 0))
--	self.face = assert(FT.New_Memory_Face(library, assert(assert(io.open(name, "rb")):read"*a"), 0))
	FT.Set_Char_Size(self.face, self.height, self.height, 96, 96)
	if stroke_radius then
		local stroker = assert(FT.Stroker_New(library))
		self.stroker = stroker
		-- BUTT, ROUND, SQUARE
		-- ROUND, BEVEL, MITER
		FT.Stroker_Set(stroker, stroke_radius, 'SQUARE', 'MITER', 1)
	end
	return self
end

local function glyph_to_bitmap(glyph_slot, glyph, newimage)
	local bitmap_glyph = assert(FT.Glyph_To_Bitmap(glyph, 'NORMAL', nil))
	local bitmap = bitmap_glyph.bitmap
	
	local glyphsize = {height=bitmap.rows, width=bitmap.width}
	local texturesize = {
		height = 2 ^ math.ceil(math.log(glyphsize.height) / math.log(2)),
		width = 2 ^ math.ceil(math.log(glyphsize.width) / math.log(2)),
	}

--	log.Begin("get_image")
	local image = assert(bitmap:get_image(newimage, {texturesize.height, texturesize.width}))
--	log.End()
	
	return image, {
		glyphsize = glyphsize,
		texturesize = texturesize,
		advance = glyph_slot.advance,
		left = bitmap_glyph.left,
		top = bitmap_glyph.top,
	}
end

function font_methods:getglyph(char, newimage)
	local charcode = FT.Get_Char_Index(self.face, char)
	if not charcode then
		return nil,"character code "..char.." is not present in font "..self.name
	end
	
	assert(FT.Load_Glyph(self.face, charcode, {'DEFAULT', 'NO_HINTING', 'NO_AUTOHINT'}))
	
	local glyph_slot = self.face.glyph
	local glyph = assert(FT.Get_Glyph(glyph_slot))
	local stroker = self.stroker
	local outside,inside
	if stroker then
		outside = assert(FT.Glyph_StrokeBorder(glyph, stroker, false))
	--	inside = assert(FT.Glyph_StrokeBorder(glyph, stroker, true))
		inside = glyph
	else
		outside = glyph
		inside = nil
	end
	local oimage,oparams = glyph_to_bitmap(glyph_slot, outside, newimage)
	local iimage,iparams
	if inside then
		iimage,iparams = glyph_to_bitmap(glyph_slot, inside, newimage)
	end
	return oimage,oparams,iimage,iparams
end

function font_methods:getkerning(leftchar, rightchar)
	local leftcharcode = FT.Get_Char_Index(self.face, leftchar)
	if not leftcharcode then
		return nil,"character code "..leftchar.." is not present in font "..self.name
	end
	local rightcharcode = FT.Get_Char_Index(self.face, rightchar)
	if not rightcharcode then
		return nil,"character code "..rightchar.." is not present in font "..self.name
	end
	return FT.Get_Kerning(self.face, leftcharcode, rightcharcode, 'DEFAULT')
end

return _M
