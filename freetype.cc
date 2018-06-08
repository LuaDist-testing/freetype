extern "C" {
#include <lua.h>
#include <lauxlib.h>
}
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_MODULE_H
#include FT_STROKER_H
#include FT_BITMAP_H

extern "C" {
#include "compat.h"
}

typedef struct _str_map_elem_t
{
	char const* name;
	FT_Int32 value;
} str_map_elem_t;

#define FT_CST(prefix, name) {#name, prefix##_##name}

#ifdef USE_VERBOSE_FT_CALLS_LOGGING
#	define LOG(name) printf(#name "\n")
#else
#	define LOG(name)
#endif

static const str_map_elem_t FT_LOAD_XXX[] = {
	FT_CST(FT_LOAD, DEFAULT),
	FT_CST(FT_LOAD, NO_SCALE),
	FT_CST(FT_LOAD, NO_HINTING),
	FT_CST(FT_LOAD, RENDER),
	FT_CST(FT_LOAD, NO_BITMAP),
	FT_CST(FT_LOAD, VERTICAL_LAYOUT),
	FT_CST(FT_LOAD, FORCE_AUTOHINT),
	FT_CST(FT_LOAD, CROP_BITMAP),
	FT_CST(FT_LOAD, PEDANTIC),
	FT_CST(FT_LOAD, IGNORE_GLOBAL_ADVANCE_WIDTH),
	FT_CST(FT_LOAD, NO_RECURSE),
	FT_CST(FT_LOAD, IGNORE_TRANSFORM),
	FT_CST(FT_LOAD, MONOCHROME),
	FT_CST(FT_LOAD, LINEAR_DESIGN),
	FT_CST(FT_LOAD, SBITS_ONLY),
	FT_CST(FT_LOAD, NO_AUTOHINT),
	{0, 0},
};

static const str_map_elem_t FT_RENDER_MODE_XXX[] = {
	FT_CST(FT_RENDER_MODE, NORMAL),
	FT_CST(FT_RENDER_MODE, LIGHT),
	FT_CST(FT_RENDER_MODE, MONO),
	FT_CST(FT_RENDER_MODE, LCD),
	FT_CST(FT_RENDER_MODE, LCD_V),
	{0, 0},
};

static const str_map_elem_t FT_PIXEL_MODE_XXX[] = {
	FT_CST(FT_PIXEL_MODE, NONE),
	FT_CST(FT_PIXEL_MODE, MONO),
	FT_CST(FT_PIXEL_MODE, GRAY),
	FT_CST(FT_PIXEL_MODE, GRAY2),
	FT_CST(FT_PIXEL_MODE, GRAY4),
	FT_CST(FT_PIXEL_MODE, LCD),
	FT_CST(FT_PIXEL_MODE, LCD_V),
	{0, 0},
};

static const str_map_elem_t FT_GLYPH_FORMAT_XXX[] = {
	FT_CST(FT_GLYPH_FORMAT, NONE),
	FT_CST(FT_GLYPH_FORMAT, COMPOSITE),
	FT_CST(FT_GLYPH_FORMAT, BITMAP),
	FT_CST(FT_GLYPH_FORMAT, OUTLINE),
	FT_CST(FT_GLYPH_FORMAT, PLOTTER),
	{0, 0},
};

static const str_map_elem_t FT_STROKER_LINECAP_XXX[] = {
	FT_CST(FT_STROKER_LINECAP, BUTT),
	FT_CST(FT_STROKER_LINECAP, ROUND),
	FT_CST(FT_STROKER_LINECAP, SQUARE),
	{0, 0},
};

static const str_map_elem_t FT_STROKER_LINEJOIN_XXX[] = {
	FT_CST(FT_STROKER_LINEJOIN, ROUND),
	FT_CST(FT_STROKER_LINEJOIN, BEVEL),
	FT_CST(FT_STROKER_LINEJOIN, MITER),
	{0, 0},
};

static const str_map_elem_t FT_KERNING_MODE_XXX[] = {
	FT_CST(FT_KERNING, DEFAULT),
	FT_CST(FT_KERNING, UNFITTED),
	FT_CST(FT_KERNING, UNSCALED),
	{0, 0},
};

static const str_map_elem_t FT_OUTLINE_XXX[] = {
	FT_CST(FT_OUTLINE, NONE),
	FT_CST(FT_OUTLINE, OWNER),
	FT_CST(FT_OUTLINE, EVEN_ODD_FILL),
	FT_CST(FT_OUTLINE, REVERSE_FILL),
	FT_CST(FT_OUTLINE, IGNORE_DROPOUTS),
	FT_CST(FT_OUTLINE, SMART_DROPOUTS),
	FT_CST(FT_OUTLINE, INCLUDE_STUBS),
	FT_CST(FT_OUTLINE, HIGH_PRECISION),
	FT_CST(FT_OUTLINE, SINGLE_PASS),
	{0, 0},
};

static const str_map_elem_t FT_FACE_FLAG_XXX[] = {
	FT_CST(FT_FACE_FLAG, SCALABLE),
	FT_CST(FT_FACE_FLAG, FIXED_SIZES),
	FT_CST(FT_FACE_FLAG, FIXED_WIDTH),
	FT_CST(FT_FACE_FLAG, SFNT),
	FT_CST(FT_FACE_FLAG, HORIZONTAL),
	FT_CST(FT_FACE_FLAG, VERTICAL),
	FT_CST(FT_FACE_FLAG, KERNING),
	FT_CST(FT_FACE_FLAG, FAST_GLYPHS),
	FT_CST(FT_FACE_FLAG, MULTIPLE_MASTERS),
	FT_CST(FT_FACE_FLAG, GLYPH_NAMES),
	FT_CST(FT_FACE_FLAG, EXTERNAL_STREAM),
	FT_CST(FT_FACE_FLAG, HINTER),
	FT_CST(FT_FACE_FLAG, CID_KEYED),
//	FT_CST(FT_FACE_FLAG, TRICKY),
//	FT_CST(FT_FACE_FLAG, COLOR),
	{0, 0},
};

int str_map_tovalue(const char* name, int* ret, const str_map_elem_t* map)
{
	const str_map_elem_t* it;
	for (it=map; it->name; ++it)
	{
		if (!strcmp(it->name, name))
		{
			*ret = it->value;
			return 1;
		}
	}
	return 0;
}

int str_map_toname(int value, const char** ret, const str_map_elem_t* map)
{
	const str_map_elem_t* it;
	for (it=map; it->name; ++it)
	{
		if (it->value==value)
		{
			*ret = it->name;
			return 1;
		}
	}
	return 0;
}

static int str_map_toflags(lua_State* L, int index, FT_Int32* ret, const str_map_elem_t* map)
{
	FT_Int32 flags = 0;
	lua_pushnil(L);
	while (lua_next(L, index) != 0)
	{
		const char* name = 0;
		if (lua_isstring(L, -1))
			name = lua_tostring(L, -1);
		else if (lua_isstring(L, -2) && lua_isboolean(L, -1) && lua_toboolean(L, -1))
			name = lua_tostring(L, -2);
		if (name)
		{
			const str_map_elem_t* it;
			for (it=map; it->name; ++it)
			{
				if (!strcmp(it->name, lua_tostring(L, -1)))
				{
					flags |= it->value;
					break;
				}
			}
			if (!it->name)
				return 0;
		}
		lua_pop(L, 1);
	}
	*ret = flags;
	return 1;
}

static void str_map_pushflags(lua_State* L, FT_Int32 flags, const str_map_elem_t* map)
{
	lua_newtable(L);
	const str_map_elem_t* it;
	for (it=map; it->name; ++it)
	{
		if (flags & it->value)
		{
			lua_pushstring(L, it->name);
			lua_pushboolean(L, 1);
			lua_settable(L, -3);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

#undef __FTERRORS_H__
#define FT_ERROR_START_LIST     static const ft_error_t ft_errors[] = {
#define FT_ERRORDEF( e, v, s )  { e, s },
#define FT_ERROR_END_LIST       { 0, 0 } };
typedef struct ft_error_t_
{
	int code;
	const char* name;
} ft_error_t;
#include FT_ERRORS_H

static int lua__pusherrorstring(lua_State* L, const char* err)
{
	lua_pushnil(L);
	lua_pushstring(L, err);
	return 2;
}

static int lua__pusherror(lua_State* L, FT_Error err)
{
	const ft_error_t* it;
	
	for (it=ft_errors; it->name; it++)
	{
		if (it->code==err)
		{
			return lua__pusherrorstring(L, it->name);
		}
	}
	return lua__pusherrorstring(L, "unknown freetype error");
}

static FT_Error lua__toerror(const char* str)
{
	const ft_error_t* it;
	
	for (it=ft_errors; it->name; it++)
	{
		if (!strcmp(str, it->name))
		{
			return it->code;
		}
	}
	return FT_Err_Ok;
}
/*
static int lua__throwerror(lua_State* L, FT_Error err)
{
	const ft_error_t* it;
	
	for (it=ft_errors; it->name; it++)
	{
		if (it->code==err)
		{
			return luaL_error(L, it->name);
		}
	}
	return luaL_error(L, "unknown freetype error");
}
*/
//////////////////////////////////////////////////////////////////////////////

// assign 'nowners' owners to the value on the top of the stack
// the owners are below the value
static void assign_owners(lua_State* L, int nowners)
{
	lua_insert(L, -1-nowners);
	lua_createtable(L, nowners, 0);
	for (int i=nowners; i>=1; --i)
	{
		lua_insert(L, -2);
		lua_rawseti(L, -2, i);
	}
	setuservalue(L, -2);
}

static void set_finalizer(lua_State* L, lua_CFunction finalizer, int nupvalues)
{
	lua_pushcclosure(L, finalizer, nupvalues);
	if (!lua_getmetatable(L, -1))
	{
		lua_createtable(L, 0, 1);
		lua_pushvalue(L, -1);
		lua_setmetatable(L, -3);
	}
	lua_insert(L, -2);
	lua_setfield(L, -2, "__gc");
	lua_pop(L, 1);
}
/*
static int class_tostring(lua_State* L)
{
	if (lua_getmetatable(L, 1))
	{
		lua_getfield(L, -1, "classname");
		lua_pushfstring(L, "%s: %p", lua_tostring(L, -1), lua_topointer(L, 1));
	}
	else
		lua_pushfstring(L, "%s: %p", luaL_typename(L, 1), lua_topointer(L, 1));
	return 1;
}

static void set_classname(lua_State* L, const char* name)
{
	if (!lua_getmetatable(L, -1))
	{
		lua_createtable(L, 0, 1);
		lua_pushvalue(L, -1);
		lua_setmetatable(L, -3);
	}
	lua_pushstring(L, name);
	lua_setfield(L, -2, "classname");
	lua_pushcfunction(L, class_tostring);
	lua_setfield(L, -2, "__gc");
	lua_pop(L, 1);
}
*/
template <typename T>
static T* lua__checkp(lua_State* L, int index)
{
	luaL_checktype(L, index, LUA_TUSERDATA);
	T* ud = (T*)lua_touserdata(L, index);
	return ud;
}

template <typename T>
static T lua__check(lua_State* L, int index)
{
	return *lua__checkp<T>(L, index);
}

template <typename T>
static T lua__to(lua_State* L, int index)
{
	T* ud = (T*)lua_touserdata(L, index);
	return *ud;
}

template <typename T>
static void lua__push(lua_State* L, T object, int nowners)
{
	T* ud = (T*)lua_newuserdata(L, sizeof(T));
	*ud = object;
	assign_owners(L, nowners);
}

template <typename T>
static T* lua__new(lua_State* L)
{
	return (T*)lua_newuserdata(L, sizeof(T));
}

template <typename T>
static int lua__isfixed(lua_State* L, int index, lua_Number one)
{
	return lua_type(L, index) == LUA_TNUMBER;
}

template <typename T>
static T lua__checkfixed(lua_State* L, int index, lua_Number one)
{
	luaL_checktype(L, index, LUA_TNUMBER);
	return (T)(lua_tonumber(L, index) * one);
}

template <typename T>
static T lua__tofixed(lua_State* L, int index, lua_Number one)
{
	return (T)(lua_tonumber(L, index) * one);
}

template <typename T>
static void lua__pushfixed(lua_State* L, const T& value, lua_Number one)
{
	lua_pushnumber(L, value / one);
}

template <>
void lua__push<FT_OutlineGlyph>(lua_State* L, FT_OutlineGlyph glyph, int nowners);

//////////////////////////////////////////////////////////////////////////////

template <>
void lua__pushfixed<FT_Vector>(lua_State* L, const FT_Vector& vector, lua_Number one)
{
	lua_newtable(L);
	lua_pushnumber(L, vector.x / one); lua_setfield(L, -2, "x");
	lua_pushnumber(L, vector.y / one); lua_setfield(L, -2, "y");
}

//////////////////////////////////////////////////////////////////////////////

static int generic__index(lua_State* L)
{
	lua_getmetatable(L, 1);                           // self, k, ..., mt
	lua_getfield(L, -1, "accessors");                 // self, k, ..., mt, accessors
	lua_replace(L, -2);                               // self, k, ..., accessors
	if (!lua_isnil(L, -1))
	{
		lua_pushvalue(L, 2);                          // self, k, ..., accessors, k
		lua_gettable(L, -2);                          // self, k, ..., accessors, accessor
		lua_replace(L, -2);                           // self, k, ..., accessor
	}
	if (!lua_isnil(L, -1))
	{
		lua_pushvalue(L, 1);                          // self, k, ..., accessor, glyphslot
		lua_call(L, 1, 1);                            // self, k, ..., v
	}
	return 1;
}

//////////////////////////////////////////////////////////////////////////////

static int lua__FT_Glyph_Metrics__horiBearingX(lua_State* L)
{
	FT_Glyph_Metrics* slot = lua__to<FT_Glyph_Metrics*>(L, 1);
	lua__pushfixed(L, slot->horiBearingX, 1<<6);
	return 1;
}

static int lua__FT_Glyph_Metrics__horiAdvance(lua_State* L)
{
	FT_Glyph_Metrics* slot = lua__to<FT_Glyph_Metrics*>(L, 1);
	lua__pushfixed(L, slot->horiAdvance, 1<<6);
	return 1;
}

static const luaL_Reg lua__FT_Glyph_Metrics__accessors[] = {
	{"horiBearingX", lua__FT_Glyph_Metrics__horiBearingX},
	{"horiAdvance", lua__FT_Glyph_Metrics__horiAdvance},
	{0, 0},
};

template <>
void lua__push<FT_Glyph_Metrics*>(lua_State* L, FT_Glyph_Metrics* slot, int nowners)
{
	FT_Glyph_Metrics** ud = (FT_Glyph_Metrics**)lua_newuserdata(L, sizeof(FT_Glyph_Metrics*));
	*ud = slot;
	assign_owners(L, nowners);
	lua_createtable(L, 0, 1); // mt
	lua_pushcfunction(L, generic__index);
	lua_setfield(L, -2, "__index");
	lua_newtable(L);
	setfuncs(L, lua__FT_Glyph_Metrics__accessors, 0);
	lua_setfield(L, -2, "accessors");
	lua_setmetatable(L, -2);
}


//////////////////////////////////////////////////////////////////////////////

static int lua__FT_GlyphSlot__advance(lua_State* L)
{
	FT_GlyphSlot slot = lua__to<FT_GlyphSlot>(L, 1);
	lua__pushfixed(L, slot->advance, 1<<6);
	return 1;
}

static int lua__FT_GlyphSlot__metrics(lua_State* L)
{
	FT_GlyphSlot slot = lua__to<FT_GlyphSlot>(L, 1);
	lua_pushvalue(L, 1);
	lua__push(L, &slot->metrics, 1);
	return 1;
}

static const luaL_Reg lua__FT_GlyphSlot__accessors[] = {
	{"advance", lua__FT_GlyphSlot__advance},
	{"metrics", lua__FT_GlyphSlot__metrics},
	{0, 0},
};

template <>
void lua__push<FT_GlyphSlot>(lua_State* L, FT_GlyphSlot slot, int nowners)
{
	FT_GlyphSlot* ud = (FT_GlyphSlot*)lua_newuserdata(L, sizeof(FT_GlyphSlot));
	*ud = slot;
	assign_owners(L, nowners);
	lua_createtable(L, 0, 1); // mt
	lua_pushcfunction(L, generic__index);
	lua_setfield(L, -2, "__index");
	lua_newtable(L);
	setfuncs(L, lua__FT_GlyphSlot__accessors, 0);
	lua_setfield(L, -2, "accessors");
	lua_setmetatable(L, -2);
}

//////////////////////////////////////////////////////////////////////////////

static int lua__FT_Bitmap__get_image(lua_State* L)
{
	FT_Bitmap* bitmap;
	int height;
	int width;
	int channels;
	int bit_depth;
	int rowsize;
	int theight;
	int twidth;
	int txoffset;
	int tyoffset;
	short num_grays;
	char pixel_mode;
	
	bitmap = lua__to<FT_Bitmap*>(L, 1);
	luaL_checktype(L, 2, LUA_TFUNCTION);
	
	theight = height = bitmap->rows;
	twidth = width = bitmap->width;
	txoffset = 0;
	tyoffset = 0;
	channels = 4;
	bit_depth = 8;
	rowsize = bitmap->pitch;
	num_grays = bitmap->num_grays;
	pixel_mode = bitmap->pixel_mode;
	switch (pixel_mode)
	{
	case FT_PIXEL_MODE_MONO: break;
	case FT_PIXEL_MODE_GRAY: break;
	default: {
		const char* pixel_mode_name;
		if (!str_map_toname(pixel_mode, &pixel_mode_name, FT_PIXEL_MODE_XXX))
		{
			lua_pushstring(L, "unsupported glyph bitmap pixel mode ");
			lua_pushnumber(L, pixel_mode);
			lua_concat(L, 2);
			return luaL_error(L, lua_tostring(L, -1));
		}
		else
		{
			lua_pushstring(L, "unsupported glyph bitmap pixel mode ");
			lua_pushstring(L, pixel_mode_name);
			lua_concat(L, 2);
			return luaL_error(L, lua_tostring(L, -1));
		}
	} }

	if (lua_istable(L, 3))
	{
		// indices 1 and 2 are theight and twidth
		lua_rawgeti(L, 3, 1);
		if (lua_isnumber(L, -1))
			theight = (int)lua_tonumber(L, -1);
		lua_pop(L, 1);
		lua_rawgeti(L, 3, 2);
		if (lua_isnumber(L, -1))
			twidth = (int)lua_tonumber(L, -1);
		lua_pop(L, 1);
		lua_getfield(L, 3, "theight");
		if (lua_isnumber(L, -1))
			theight = (int)lua_tonumber(L, -1);
		lua_pop(L, 1);
		lua_getfield(L, 3, "twidth");
		if (lua_isnumber(L, -1))
			twidth = (int)lua_tonumber(L, -1);
		lua_pop(L, 1);
		lua_getfield(L, 3, "txoffset");
		if (lua_isnumber(L, -1))
			txoffset = (int)lua_tonumber(L, -1);
		lua_pop(L, 1);
		lua_getfield(L, 3, "tyoffset");
		if (lua_isnumber(L, -1))
			tyoffset = (int)lua_tonumber(L, -1);
		lua_pop(L, 1);
	}
	
	lua_pushvalue(L, 2);
	lua_pushnumber(L, theight);
	lua_pushnumber(L, twidth);
	lua_pushnumber(L, channels);
	lua_pushnumber(L, bit_depth);
	lua_call(L, 4, 1);
	
	if (lua_isuserdata(L, -1))
	{
		int x, y, tx, ty;
		unsigned char* src;
		unsigned char* dst;
		src = (unsigned char*)bitmap->buffer;
		dst = (unsigned char*)lua_touserdata(L, -1);
		for (ty=0; ty<theight; ++ty) for (tx=0; tx<twidth; ++tx)
		{
			unsigned char val;
			x = tx - txoffset;
			y = ty - tyoffset;
			val = 0;
			if (y>=0 && y<height && x>=0 && x<width)
			{
				switch (pixel_mode)
				{
				case FT_PIXEL_MODE_GRAY:
					val = src[y*rowsize + x] * 255 / num_grays;
					break;
				case FT_PIXEL_MODE_MONO:
					val = (src[y*rowsize + x/8] >> (7-x%8)) * 255;
					break;
				}
			}
			dst[(ty*twidth + tx)*channels + 0] = 255;
			dst[(ty*twidth + tx)*channels + 1] = 255;
			dst[(ty*twidth + tx)*channels + 2] = 255;
			dst[(ty*twidth + tx)*channels + 3] = val;
		}
	}
	return 1;
}

static int lua__FT_Bitmap__index(lua_State* L)
{
	FT_Bitmap* bitmap = lua__to<FT_Bitmap*>(L, 1);
	if (lua_isstring(L, 2))
	{
		const char* name = lua_tostring(L, 2);
		if (!strcmp(name, "rows"))
		{
			lua_pushnumber(L, bitmap->rows);
			return 1;
		}
		else if (!strcmp(name, "width"))
		{
			lua_pushnumber(L, bitmap->width);
			return 1;
		}
		else if (!strcmp(name, "pitch"))
		{
			lua_pushnumber(L, bitmap->pitch);
			return 1;
		}
		/*
		else if (!strcmp(name, "buffer"))
		{
			lua_pushvalue(L, 1);
			lua__push(L, &bitmap->buffer, 1);
			return 1;
		}
		*/
		else if (!strcmp(name, "num_grays"))
		{
			lua_pushnumber(L, bitmap->num_grays);
			return 1;
		}
		else if (!strcmp(name, "pixel_mode"))
		{
			const char* pixel_mode;
			if (str_map_toname(bitmap->pixel_mode, &pixel_mode, FT_PIXEL_MODE_XXX))
				lua_pushstring(L, pixel_mode);
			else
				lua_pushnumber(L, bitmap->pixel_mode);
			return 1;
		}
		/*
		else if (!strcmp(name, "palette_mode"))
		{
			const char* palette_mode;
			if (str_map_toname(bitmap->palette_mode, &palette_mode, FT_PALETTE_MODE_XXX))
				lua_pushstring(L, palette_mode);
			else
				lua_pushnumber(L, bitmap->palette_mode);
			return 1;
		}
		else if (!strcmp(name, "palette"))
		{
			lua_pushvalue(L, 1);
			lua__push(L, &bitmap->palette, 1);
			return 1;
		}
		*/
		else if (!strcmp(name, "get_image"))
		{
			lua_pushcfunction(L, lua__FT_Bitmap__get_image);
			return 1;
		}
	}
	lua_pushnil(L);
	return 1;
}

template <>
void lua__push<FT_Bitmap*>(lua_State* L, FT_Bitmap* bitmap, int nowners)
{
	FT_Bitmap** ud = (FT_Bitmap**)lua_newuserdata(L, sizeof(FT_Bitmap*));
	*ud = bitmap;
	assign_owners(L, nowners);
	lua_createtable(L, 0, 1); // mt
	lua_pushcfunction(L, lua__FT_Bitmap__index);
	lua_setfield(L, -2, "__index");
	lua_setmetatable(L, -2);
}

//////////////////////////////////////////////////////////////////////////////

static int lua__FT_BitmapGlyph__index(lua_State* L)
{
	FT_BitmapGlyph glyph_bitmap = lua__to<FT_BitmapGlyph>(L, 1);
	if (lua_isstring(L, 2))
	{
		const char* name = lua_tostring(L, 2);
		if (!strcmp(name, "bitmap"))
		{
			FT_Bitmap* bitmap = &glyph_bitmap->bitmap;
			lua_pushvalue(L, 1);
			lua__push(L, bitmap, 1);
			return 1;
		}
		else if (!strcmp(name, "left"))
		{
			lua_pushnumber(L, glyph_bitmap->left);
			return 1;
		}
		else if (!strcmp(name, "top"))
		{
			lua_pushnumber(L, glyph_bitmap->top);
			return 1;
		}
		/*
		else if (!strcmp(name, "get_image"))
		{
			lua_pushcfunction(L, lua__FT_BitmapGlyph__get_image);
			return 1;
		}
		*/
	}
	lua_pushnil(L);
	return 1;
}

template <>
void lua__push<FT_BitmapGlyph>(lua_State* L, FT_BitmapGlyph glyph_bitmap, int nowners)
{
	FT_BitmapGlyph* ud = (FT_BitmapGlyph*)lua_newuserdata(L, sizeof(FT_BitmapGlyph));
	*ud = glyph_bitmap;
	assign_owners(L, nowners);
	lua_createtable(L, 0, 1); // mt
	lua_pushcfunction(L, lua__FT_BitmapGlyph__index);
	lua_setfield(L, -2, "__index");
	lua_setmetatable(L, -2);
}

//////////////////////////////////////////////////////////////////////////////

static int lua__FT_Glyph__index(lua_State* L)
{
	FT_Glyph glyph = lua__to<FT_Glyph>(L, 1);
	if (lua_isstring(L, 2))
	{
		const char* name = lua_tostring(L, 2);
		if (!strcmp(name, "library"))
		{
			lua_pushvalue(L, 1);
			lua__push(L, glyph->library, 1);
			return 1;
		}
		else if (!strcmp(name, "clazz"))
		{
			lua_pushvalue(L, 1);
			lua__push(L, glyph->clazz, 1);
			return 1;
		}
		else if (!strcmp(name, "format"))
		{
			const char* format;
			if (!str_map_toname(glyph->format, &format, FT_GLYPH_FORMAT_XXX))
				lua_pushnumber(L, glyph->format);
			else
				lua_pushstring(L, format);
			return 1;
		}
		else if (!strcmp(name, "advance"))
		{
			lua__pushfixed(L, glyph->advance, 1 << 16);
			return 1;
		}
	}
	lua_pushnil(L);
	return 1;
}

template <>
void lua__push<FT_Glyph>(lua_State* L, FT_Glyph glyph, int nowners)
{
	if (glyph->format == FT_GLYPH_FORMAT_OUTLINE)
	{
		FT_OutlineGlyph outlineGlyph = reinterpret_cast<FT_OutlineGlyph>(glyph);
		lua__push(L, outlineGlyph, nowners);
		return;
	}
	FT_Glyph* ud = (FT_Glyph*)lua_newuserdata(L, sizeof(FT_Glyph));
	*ud = glyph;
	assign_owners(L, nowners);
	lua_createtable(L, 0, 1); // mt
	lua_pushcfunction(L, lua__FT_Glyph__index);
	lua_setfield(L, -2, "__index");
	lua_setmetatable(L, -2);
}

//////////////////////////////////////////////////////////////////////////////

static int lua__FT_Outline__index(lua_State* L)
{
	FT_Outline* outline = lua__to<FT_Outline*>(L, 1);
	if (lua_isstring(L, 2))
	{
		const char* name = lua_tostring(L, 2);
		if (!strcmp(name, "n_contours"))
		{
			lua_pushnumber(L, outline->n_contours);
			return 1;
		}
		else if (!strcmp(name, "n_points"))
		{
			lua_pushnumber(L, outline->n_points);
			return 1;
		}
		else if (!strcmp(name, "points"))
		{
			lua_createtable(L, outline->n_points, 0);
			for (short i=0; i<outline->n_points; ++i)
			{
				lua__pushfixed(L, outline->points[i], 1<<6);
				// complement the vector with point tags
				lua_pushnumber(L, outline->tags[i]);
				lua_setfield(L, -2, "tag");
				lua_rawseti(L, -2, i+1);
			}
			return 1;
		}
		else if (!strcmp(name, "tags"))
		{
			lua_createtable(L, outline->n_points, 0);
			for (short i=0; i<outline->n_points; ++i)
			{
				lua_pushnumber(L, outline->tags[i]);
				lua_rawseti(L, -2, i+1);
			}
			return 1;
		}
		else if (!strcmp(name, "contours"))
		{
			lua_createtable(L, outline->n_contours, 0);
			for (short i=0; i<outline->n_contours; ++i)
			{
				lua_pushnumber(L, outline->contours[i]);
				lua_rawseti(L, -2, i+1);
			}
			return 1;
		}
		else if (!strcmp(name, "flags"))
		{
			str_map_pushflags(L, outline->flags, FT_OUTLINE_XXX);
			return 1;
		}
	}
	lua_pushnil(L);
	return 1;
}

template <>
void lua__push<FT_Outline*>(lua_State* L, FT_Outline* outline, int nowners)
{
	FT_Outline** ud = (FT_Outline**)lua_newuserdata(L, sizeof(FT_Outline*));
	*ud = outline;
	assign_owners(L, nowners);
	lua_createtable(L, 0, 1); // mt
	lua_pushcfunction(L, lua__FT_Outline__index);
	lua_setfield(L, -2, "__index");
	lua_setmetatable(L, -2);
}

//////////////////////////////////////////////////////////////////////////////

static int lua__FT_OutlineGlyph__index(lua_State* L)
{
	FT_OutlineGlyph glyph = lua__to<FT_OutlineGlyph>(L, 1);
	if (lua_isstring(L, 2))
	{
		const char* name = lua_tostring(L, 2);
		if (!strcmp(name, "outline"))
		{
			FT_Outline* outline;
			outline = &glyph->outline;
			lua_pushvalue(L, 1);
			lua__push(L, outline, 1);
			// set the glyph in the outline environment to prevent its collection1
			lua_createtable(L, 0, 1);
			lua_pushvalue(L, 1);
			lua_pushboolean(L, 1);
			lua_settable(L, -3);
			setuservalue(L, -2);
			return 1;
		}
	}
	// fall back to base class index metamethod
	lua_pushcfunction(L, lua__FT_Glyph__index);
	lua_insert(L, 1);
	lua_call(L, lua_gettop(L)-1, LUA_MULTRET);
	return lua_gettop(L);
}

template <>
void lua__push<FT_OutlineGlyph>(lua_State* L, FT_OutlineGlyph glyph, int nowners)
{
	FT_OutlineGlyph* ud = (FT_OutlineGlyph*)lua_newuserdata(L, sizeof(FT_OutlineGlyph));
	*ud = glyph;
	assign_owners(L, nowners);
	lua_createtable(L, 0, 1); // mt
	lua_pushcfunction(L, lua__FT_OutlineGlyph__index);
	lua_setfield(L, -2, "__index");
	lua_setmetatable(L, -2);
}

//////////////////////////////////////////////////////////////////////////////

static int lua__FT_Face__index(lua_State* L)
{
	FT_Face face = lua__to<FT_Face>(L, 1);
	if (lua_isstring(L, 2))
	{
		const char* name = lua_tostring(L, 2);
		if (!strcmp(name, "glyph"))
		{
			lua_pushvalue(L, 1);
			lua__push(L, face->glyph, 1);
			return 1;
		}
		else if (!strcmp(name, "face_flags"))
		{
			str_map_pushflags(L, face->face_flags, FT_FACE_FLAG_XXX);
			return 1;
		}
	}
	lua_pushnil(L);
	return 1;
}

template <>
void lua__push<FT_Face>(lua_State* L, FT_Face face, int nowners)
{
	FT_Face* ud = (FT_Face*)lua_newuserdata(L, sizeof(FT_Face));
	*ud = face;
	assign_owners(L, nowners);
	lua_createtable(L, 0, 1); // mt
	lua_pushcfunction(L, lua__FT_Face__index);
	lua_setfield(L, -2, "__index");
	lua_setmetatable(L, -2);
}

//////////////////////////////////////////////////////////////////////////////

typedef struct lua_Ref_ {
	lua_State* L;
	int ref;
} lua_Ref;

static void* alloc_with_lua(lua_State* L, size_t size)
{
	void* result;
	
	result = lua_newuserdata(L, size);
	lua_pushlightuserdata(L, result);
	lua_insert(L, -2);
	lua_settable(L, LUA_REGISTRYINDEX);

	return result;
}

static void free_with_lua(lua_State* L, void* ptr)
{
	lua_pushlightuserdata(L, ptr);
	lua_pushnil(L);
	lua_settable(L, LUA_REGISTRYINDEX);
}

//////////////////////////////////////////////////////////////////////////////

static int freetype_gc(lua_State* L)
{
	FT_Library* plibrary = lua__checkp<FT_Library>(L, 1);
	LOG(FT_Done_FreeType);
	FT_Done_FreeType(*plibrary);
	*plibrary = 0;
	return 0;
}

static int lua__FT__Init_FreeType(lua_State* L)
{
	FT_Error err;
	FT_Library library = 0;
	
	LOG(FT_Init_FreeType);
	err = FT_Init_FreeType(&library);
	if (err)
		return lua__pusherror(L, err);

	lua__push(L, library, 0);
	set_finalizer(L, freetype_gc, 0);
	return 1;
}
#if 0
static int lua__FT__New_Face(lua_State* L)
{
	FT_Error err;
	FT_Library library;
	const char* filepathname;
	FT_Long face_index;
	FT_Face face = 0;
	
	library = lua__check<FT_Library>(L, 1);
	filepathname = luaL_checkstring(L, 2);
	face_index = (FT_Long)luaL_checknumber(L, 3);
	
	LOG(FT_New_Face);
	err = FT_New_Face(library, filepathname, face_index, &face);
	if (err)
		return lua__pusherror(L, err);

	lua__push(L, face);
	return 1;
}

static void Face_with_ref_Finalizer(void* object)
{
	FT_Face face;
	lua_Ref* ref;
	lua_State* L;
	
	face = (FT_Face)object;
	ref = (lua_Ref*)face->generic.data;
	L = ref->L;
	luaL_unref(L, LUA_REGISTRYINDEX, ref->ref);
	free_with_lua(L, ref);
	
	face->generic.data = 0;
	face->generic.finalizer = 0;
}

static int lua__FT__New_Memory_Face(lua_State* L)
{
	FT_Error err;
	FT_Library library;
	const FT_Byte* file_base;
	size_t string_size;
	FT_Long file_size;
	FT_Long face_index;
	FT_Face face = 0;
	lua_Ref* ref;
	
	library = lua__check<FT_Library>(L, 1);
	file_base = (const FT_Byte*)luaL_checklstring(L, 2, &string_size);
	file_size = string_size;
	face_index = (FT_Long)luaL_checknumber(L, 3);
	
	LOG(FT_New_Memory_Face);
	err = FT_New_Memory_Face(library, file_base, file_size, face_index, &face);
	if (err)
		return lua__pusherror(L, err);
	
	ref = (lua_Ref*)alloc_with_lua(L, sizeof(lua_Ref));
	ref->L = L;
	lua_pushvalue(L, 2);
	ref->ref = luaL_ref(L, LUA_REGISTRYINDEX);
	face->generic.data = (void*)ref;
	face->generic.finalizer = Face_with_ref_Finalizer;

	lua__push(L, face);
	return 1;
}
#endif
static unsigned long FT_LuaFileStream_IoFunc(FT_Stream stream, unsigned long offset, unsigned char* buffer, unsigned long count)
{
	lua_Ref* ref;
	ref = (lua_Ref*)stream->descriptor.pointer;
	if (ref)
	{
		lua_State* L;
		unsigned long result;
		
		L = ref->L;
		if (lua_checkstack(L, 6))
		{
			// get file
			lua_rawgeti(L, LUA_REGISTRYINDEX, ref->ref);
			lua_pushvalue(L, -1);
			// get seek method
			lua_getfield(L, -1, "seek");
			// push self
			lua_pushvalue(L, -2);
			// push args
			lua_pushliteral(L, "set");
			lua_pushnumber(L, offset);
			// call, returns pos,nil or nil,error
			lua_call(L, 3, 0);
			// get read method
			lua_getfield(L, -1, "read");
			// swap with self
			lua_insert(L, -2);
			// push args
			lua_pushnumber(L, count);
			// call, returns data,nil or nil,error
			lua_call(L, 2, 1);
			// handle error
			if (lua_type(L, -1)==LUA_TSTRING)
			{
				// check output
				result = (unsigned long)rawlen(L, -1); /* :FIXME: handle overflow */
				// copy read bytes
				memcpy(buffer, lua_tostring(L, -1), result);
			}
			else
				result = 0;
			lua_pop(L, 1);
			return result;
		}
	}
	return 0;
}

static void FT_LuaFileStream_CloseFunc(FT_Stream stream)
{
	lua_Ref* ref;
	ref = (lua_Ref*)stream->descriptor.pointer;
	if (ref)
	{
		lua_State* L;
		L = ref->L;
		lua_rawgeti(L, LUA_REGISTRYINDEX, ref->ref);
		lua_getfield(L, -1, "close");
		lua_insert(L, -2); /* swap file object and close method */
		lua_call(L, 1, 0);
		luaL_unref(L, LUA_REGISTRYINDEX, ref->ref);
		free_with_lua(L, ref);
		stream->size = 0;
		stream->pos = 0;
		stream->descriptor.pointer = 0;
	}
}

static FT_Error FT_Stream_OpenLuaFile(FT_Stream stream, lua_State* L, int index)
{
	lua_Ref* ref;
	// compute size
	// get seek
	lua_getfield(L, index, "seek");
	// seek to end
	lua_pushvalue(L, -1);
	lua_pushvalue(L, index);
	lua_pushliteral(L, "end");
	lua_pushnumber(L, 0);
	lua_call(L, 3, 1);
	stream->size = (unsigned long)lua_tonumber(L, -1); /* :FIXME: handle overflow */
	lua_pop(L, 1);
	// seek to beginning
	lua_pushvalue(L, index);
	lua_pushliteral(L, "set");
	lua_pushnumber(L, 0);
	lua_call(L, 3, 0);
	
	ref = (lua_Ref*)alloc_with_lua(L, sizeof(lua_Ref));
	ref->L = L;
	lua_pushvalue(L, index);
	ref->ref = luaL_ref(L, LUA_REGISTRYINDEX);
	
	stream->descriptor.pointer = (void*)ref;
	stream->pathname.pointer = 0;
	stream->pos = 0;
	
	stream->read = FT_LuaFileStream_IoFunc;
	stream->close = FT_LuaFileStream_CloseFunc;
	
	return FT_Err_Ok;
}

static FT_Stream FT_Stream_NewLuaFile(lua_State* L, int index)
{
	FT_Stream result;
	FT_Error error;
	
	result = (FT_Stream)alloc_with_lua(L, sizeof(FT_StreamRec));
	memset(result, 0, sizeof(*result));
	LOG(FT_Stream_OpenLuaFile);
	error = FT_Stream_OpenLuaFile(result, L, index);

	if (error != FT_Err_Ok)
	{
		free_with_lua(L, result);
		return 0;
	}
	
	return result;
}

static void FT_Stream_DeleteLuaFile(lua_State* L, FT_Stream stream)
{
	FT_Stream_CloseFunc close;
	close = stream->close;
	close(stream);
	free_with_lua(L, stream);
}

typedef struct LuaFile_based_Face_generic_data_ {
	lua_State* L;
	FT_Stream stream;
} LuaFile_based_Face_generic_data;

static void LuaFile_based_Face_Finalizer(void* object)
{
	FT_Face face;
	LuaFile_based_Face_generic_data* data;
	lua_State* L;
	
	face = (FT_Face)object;
	data = (LuaFile_based_Face_generic_data*)face->generic.data;
	L = data->L;
	LOG(FT_Stream_DeleteLuaFile);
	FT_Stream_DeleteLuaFile(L, data->stream);
	free_with_lua(L, data);
	
	face->generic.data = 0;
	face->generic.finalizer = 0;
}

static int face_gc(lua_State* L)
{
	FT_Face* pface = lua__checkp<FT_Face>(L, 1);
	LOG(FT_Done_Face);
	FT_Done_Face(*pface);
	*pface = 0;
	return 0;
}

static int lua__FT__Open_Face(lua_State* L)
{
	FT_Error err;
	FT_Library library;
	FT_Open_Args args;
	FT_Stream stream;
	FT_Long face_index;
	FT_Face face = 0;
	LuaFile_based_Face_generic_data* data;
	
	library = lua__check<FT_Library>(L, 1);
	luaL_checktype(L, 2, LUA_TTABLE);
	face_index = (FT_Long)luaL_checknumber(L, 3);
	
	lua_getfield(L, 2, "stream");
	if (lua_isnil(L, -1))
		return luaL_argerror(L, 2, "table should contain a 'stream' field");
	memset(&args, 0, sizeof(args));
	args.flags |= FT_OPEN_STREAM;
	LOG(FT_Stream_NewLuaFile);
	stream = FT_Stream_NewLuaFile(L, lua_gettop(L));
	if (!stream)
		return luaL_argerror(L, 2, "could not create stream");
	args.stream = stream;
	
	LOG(FT_Open_Face);
	err = FT_Open_Face(library, &args, face_index, &face);
	if (err) {
		LOG(FT_Stream_DeleteLuaFile);
		FT_Stream_DeleteLuaFile(L, stream);
		return lua__pusherror(L, err);
	}
	if (face->generic.finalizer) {
		LOG(FT_Stream_DeleteLuaFile);
		FT_Stream_DeleteLuaFile(L, stream);
		return luaL_error(L, "new face already has a finalizer");
	}
	
	data = (LuaFile_based_Face_generic_data*)alloc_with_lua(L, sizeof(LuaFile_based_Face_generic_data));
	data->L = L;
	data->stream = stream;
	face->generic.data = data;
	face->generic.finalizer = LuaFile_based_Face_Finalizer;
	
	lua_pushvalue(L, 1);
	lua__push(L, face, 1);
	set_finalizer(L, face_gc, 0);
	return 1;
}

static int lua__FT__Set_Char_Size(lua_State* L)
{
	FT_Error err;
	FT_Face face;
	FT_F26Dot6 char_width;
	FT_F26Dot6 char_height;
	FT_UInt horz_resolution;
	FT_UInt vert_resolution;
	
	face = lua__check<FT_Face>(L, 1);
	char_width = (FT_F26Dot6)(luaL_checknumber(L, 2)*64);
	char_height = (FT_F26Dot6)(luaL_checknumber(L, 3)*64);
	horz_resolution = (FT_UInt)luaL_checknumber(L, 4);
	vert_resolution = (FT_UInt)luaL_checknumber(L, 5);
	
	LOG(FT_Set_Char_Size);
	err = FT_Set_Char_Size(face, char_width, char_height, horz_resolution, vert_resolution);
	if (err)
		return lua__pusherror(L, err);
	
	lua_pushboolean(L, 1);
	return 1;
}
#if 0
static int lua__FT__Done_FreeType(lua_State* L)
{
	FT_Error err;
	FT_Library library;
	
	library = lua__check<FT_Library>(L, 1);
	
	LOG(FT_Done_FreeType);
	err = FT_Done_FreeType(library);
	if (err)
		return lua__pusherror(L, err);
	
	lua_pushboolean(L, 1);
	return 1;
}

static int lua__FT__Done_Library(lua_State* L)
{
	FT_Error err;
	FT_Library library;
	
	library = lua__check<FT_Library>(L, 1);
	
	LOG(FT_Done_Library);
	err = FT_Done_Library(library);
	if (err)
		return lua__pusherror(L, err);
	
	lua_pushboolean(L, 1);
	return 1;
}

static int lua__FT__Done_Face(lua_State* L)
{
	FT_Error err;
	FT_Face face;
	
	face = lua__check<FT_Face>(L, 1);
	
	LOG(FT_Done_Face);
	err = FT_Done_Face(face);
	if (err)
		return lua__pusherror(L, err);
	
	lua_pushboolean(L, 1);
	return 1;
}

static int lua__FT__Done_Glyph(lua_State* L)
{
	FT_Glyph glyph;
	
	glyph = lua__check<FT_Glyph>(L, 1);
	
	LOG(FT_Done_Glyph);
	FT_Done_Glyph(glyph);
	
	lua_pushboolean(L, 1);
	return 1;
}
#endif
static int lua__FT__Get_Char_Index(lua_State* L)
{
	FT_UInt index;
	FT_Face face;
	FT_ULong charcode;
	
	face = lua__check<FT_Face>(L, 1);
	charcode = (FT_ULong)luaL_checknumber(L, 2);
	
	LOG(FT_Get_Char_Index);
	index = FT_Get_Char_Index(face, charcode);
	if (index==0)
		return lua__pusherrorstring(L, "undefined character code");
	
	lua_pushnumber(L, index);
	return 1;
}

static int lua__FT__Load_Glyph(lua_State* L)
{
	FT_Error err;
	FT_Face face;
	FT_UInt glyph_index;
	FT_Int32 load_flags;
	
	face = lua__check<FT_Face>(L, 1);
	glyph_index = (FT_UInt)luaL_checknumber(L, 2);
	luaL_checktype(L, 3, LUA_TTABLE);
	if (!str_map_toflags(L, 3, &load_flags, FT_LOAD_XXX))
		return luaL_argerror(L, 3, "set contains an unknown value");
	
	LOG(FT_Load_Glyph);
	err = FT_Load_Glyph(face, glyph_index, load_flags);
	if (err)
		return lua__pusherror(L, err);
	
	lua_pushboolean(L, 1);
	return 1;
}

static int glyph_gc(lua_State* L)
{
	FT_Glyph* pglyph = lua__checkp<FT_Glyph>(L, 1);
	LOG(FT_Done_Glyph);
	FT_Done_Glyph(*pglyph);
	*pglyph = 0;
	return 0;
}

static int lua__FT__Get_Glyph(lua_State* L)
{
	FT_Error err;
	FT_GlyphSlot slot;
	FT_Glyph glyph;
	
	slot = lua__check<FT_GlyphSlot>(L, 1);
	
	LOG(FT_Get_Glyph);
	err = FT_Get_Glyph(slot, &glyph);
	if (err)
		return lua__pusherror(L, err);
	
	lua_pushvalue(L, 1);
	lua__push(L, glyph, 1);
	set_finalizer(L, glyph_gc, 0);
	return 1;
}

static int lua__FT__Glyph_To_Bitmap(lua_State* L)
{
	FT_Error err;
	FT_BitmapGlyph glyph_bitmap;
	FT_Glyph glyph;
	FT_Render_Mode render_mode;
	FT_Vector origin;
	FT_Vector* porigin;
	FT_Bool destroy;
	
	glyph = lua__check<FT_Glyph>(L, 1);
	if (!str_map_tovalue(luaL_checkstring(L, 2), (int*)&render_mode, FT_RENDER_MODE_XXX))
		return typeerror(L, 2, "enum");
	if (lua_isnoneornil(L, 3))
		porigin = 0;
	else if (lua_istable(L, 3))
	{
		lua_getfield(L, 3, "x");
		if (!lua_isnumber(L, -1))
			return luaL_argerror(L, 3, "field x is not a number");
		origin.x = lua__tofixed<FT_Pos>(L, -1, 1 << 6);
		lua_pop(L, 1);
		lua_getfield(L, 3, "y");
		if (!lua_isnumber(L, -1))
			return luaL_argerror(L, 3, "field y is not a number");
		origin.y = lua__tofixed<FT_Pos>(L, -1, 1 << 6);
		lua_pop(L, 1);
		porigin = &origin;
	}
	else
		return typeerror(L, 3, "table");
	destroy = lua_toboolean(L, 4);
	
	LOG(FT_Glyph_To_Bitmap);
	err = FT_Glyph_To_Bitmap(&glyph, render_mode, porigin, destroy);
	if (err)
		return lua__pusherror(L, err);
	
	glyph_bitmap = (FT_BitmapGlyph)glyph;
	
	lua_pushvalue(L, 1);
	lua__push(L, glyph_bitmap, 1);
	return 1;
}

static int stroker_gc(lua_State* L)
{
	FT_Stroker* pstroker = lua__checkp<FT_Stroker>(L, 1);
	LOG(FT_Stroker_Done);
	FT_Stroker_Done(*pstroker);
	*pstroker = 0;
	return 0;
}

static int lua__FT__Stroker_New(lua_State* L)
{
	FT_Error err;
	FT_Library library;
	FT_Stroker stroker = 0;
	
	library = lua__check<FT_Library>(L, 1);
	
	LOG(FT_Stroker_New);
	err = FT_Stroker_New(library, &stroker);
	if (err)
		return lua__pusherror(L, err);
	
	lua_pushvalue(L, 1);
	lua__push(L, stroker, 1);
	if (!lua_getmetatable(L, -1))
	{
		lua_createtable(L, 0, 1);
		lua_pushvalue(L, -1);
		lua_setmetatable(L, -3);
	}
	lua_pushcclosure(L, stroker_gc, 0);
	lua_setfield(L, -2, "__gc");
	lua_pop(L, 1);
	return 1;
}

static int lua__FT__Stroker_Set(lua_State* L)
{
	FT_Stroker stroker;
	FT_Fixed radius;
	FT_Stroker_LineCap line_cap;
	FT_Stroker_LineJoin line_join;
	FT_Fixed miter_limit = 0;
	
	stroker = lua__check<FT_Stroker>(L, 1);
	radius = lua__checkfixed<FT_Fixed>(L, 2, 1 << 6);
	if (!str_map_tovalue(luaL_checkstring(L, 3), (int*)&line_cap, FT_STROKER_LINECAP_XXX))
		return typeerror(L, 3, "enum");
	if (!str_map_tovalue(luaL_checkstring(L, 4), (int*)&line_join, FT_STROKER_LINEJOIN_XXX))
		return typeerror(L, 4, "enum");
	if (lua__isfixed<FT_Fixed>(L, 5, 1 << 16))
		miter_limit = lua__tofixed<FT_Fixed>(L, 5, 1 << 16);
	
	LOG(FT_Stroker_Set);
	FT_Stroker_Set(stroker, radius, line_cap, line_join, miter_limit);
	
	return 0;
}

static int lua__FT__Stroker_ParseOutline(lua_State* L)
{
	FT_Stroker stroker;
	FT_Outline* outline;
	FT_Bool opened;
	FT_Error err;
	
	stroker = lua__check<FT_Stroker>(L, 1);
	outline = lua__check<FT_Outline*>(L, 2);
	opened = lua_toboolean(L, 3) ? 1 : 0;
	
	LOG(FT_Stroker_ParseOutline);
	err = FT_Stroker_ParseOutline(stroker, outline, opened);
	if (err)
		return lua__pusherror(L, err);
	
	lua_pushboolean(L, 1);
	return 1;
}

static int lua__FT__Stroker_GetCounts(lua_State* L)
{
	FT_Stroker stroker;
	FT_UInt num_points;
	FT_UInt num_contours;
	FT_Error err;
	
	stroker = lua__check<FT_Stroker>(L, 1);
	
	LOG(FT_Stroker_GetCounts);
	err = FT_Stroker_GetCounts(stroker, &num_points, &num_contours);
	if (err)
		return lua__pusherror(L, err);
	
	lua_pushnumber(L, num_points);
	lua_pushnumber(L, num_contours);
	return 2;
}

static int outline_gc(lua_State* L)
{
	FT_Library library = lua__check<FT_Library>(L, lua_upvalueindex(1));
	FT_Outline** poutline = lua__checkp<FT_Outline*>(L, 1);
	LOG(FT_Outline_Done);
	FT_Outline_Done(library, *poutline);
	*poutline = 0;
	return 0;
}

static int lua__FT__Outline_New(lua_State* L)
{
	FT_Library library;
	FT_UInt num_points;
	FT_Int num_contours;
	FT_Outline* outline;
	FT_Error err;
	
	library = lua__check<FT_Library>(L, 1);
	num_points = (FT_UInt)luaL_checknumber(L, 2);
	num_contours = (FT_Int)luaL_checknumber(L, 3);
	
	outline = lua__new<FT_Outline>(L);
	
	LOG(FT_Outline_New);
	err = FT_Outline_New(library, num_points, num_contours, outline);
	if (err)
		return lua__pusherror(L, err);
	
	lua_pushvalue(L, 1); // library
	lua__push(L, outline, 2);
	lua_pushvalue(L, 1); // library
	set_finalizer(L, outline_gc, 1);
	return 1;
}

static int lua__FT__Stroker_Export(lua_State* L)
{
	FT_Stroker stroker;
	FT_UInt num_points;
	FT_UInt num_contours;
	FT_Library library;
	FT_Error err;
	
	FT_Outline* outline;
	
	stroker = lua__check<FT_Stroker>(L, 1);
	// get library from stroker environment
	getuservalue(L, 1);
	lua_rawgeti(L, -1, 1);
	lua_replace(L, -2);
	library = lua__check<FT_Library>(L, lua_gettop(L)); // :FIXME: lua__check should only be used for function arguments
	
	LOG(FT_Stroker_GetCounts);
	err = FT_Stroker_GetCounts(stroker, &num_points, &num_contours);
	if (err)
		return lua__pusherror(L, err);
	
	outline = lua__new<FT_Outline>(L);
	
	LOG(FT_Outline_New);
	err = FT_Outline_New(library, num_points, num_contours, outline);
	if (err)
		return lua__pusherror(L, err);
	
	// :NOTE: FT_Outline_New sets n_points/n_contours to the size of the
	// associated arrays, while FT_Stroker_Export assumes that's the number of
	// elements currently in use. So set them to 0.
	outline->n_points = 0;
	outline->n_contours = 0;
	
	LOG(FT_Stroker_Export);
	FT_Stroker_Export(stroker, outline);
	lua_pushvalue(L, -2); // library
	lua__push(L, outline, 2);
	lua_pushvalue(L, -2); // library
	set_finalizer(L, outline_gc, 1);
	return 1;
}

static int bitmap_gc(lua_State* L)
{
	FT_Library library = lua__check<FT_Library>(L, lua_upvalueindex(1));
	FT_Bitmap** pbitmap = lua__checkp<FT_Bitmap*>(L, 1);
	LOG(FT_Bitmap_Done);
	FT_Bitmap_Done(library, *pbitmap);
	*pbitmap = 0;
	return 0;
}

static int lua__FT__Bitmap_New(lua_State* L)
{
	FT_Bitmap* bitmap;
	
	lua__check<FT_Library>(L, 1);
	
	bitmap = lua__new<FT_Bitmap>(L);
	
	LOG(FT_Bitmap_New);
	FT_Bitmap_New(bitmap);
	
	lua_pushvalue(L, 1);
	lua__push(L, bitmap, 2);
	lua_pushvalue(L, 1);
	set_finalizer(L, bitmap_gc, 1);
	return 1;
}
#if 0
static int lua__FT__Bitmap_Done(lua_State* L)
{
	FT_Library library;
	FT_Bitmap* bitmap;
	
	library = lua__check<FT_Library>(L, 1);
	bitmap = lua__check<FT_Bitmap*>(L, 2);
	
	LOG(FT_Bitmap_Done);
	FT_Bitmap_Done(library, bitmap);
	
	return 0;
}

static int lua__FT__Outline_Done(lua_State* L)
{
	FT_Library library;
	FT_Outline* outline;
	
	library = lua__check<FT_Library>(L, 1);
	outline = lua__check<FT_Outline*>(L, 2);
	
	LOG(FT_Outline_Done);
	FT_Outline_Done(library, outline);
	
	return 0;
}

static int lua__FT__Stroker_Done(lua_State* L)
{
	FT_Stroker stroker;
	
	stroker = lua__check<FT_Stroker>(L, 1);
	
	LOG(FT_Stroker_Done);
	FT_Stroker_Done(stroker);
	
	return 0;
}
#endif
static int lua__FT__Outline_Get_Bitmap(lua_State* L)
{
	FT_Library library;
	FT_Outline* outline;
	FT_Bitmap* bitmap;
	FT_Error err;
	
	library = lua__check<FT_Library>(L, 1);
	outline = lua__check<FT_Outline*>(L, 2);
	bitmap = lua__check<FT_Bitmap*>(L, 3);
	
	LOG(FT_Outline_Get_Bitmap);
	err = FT_Outline_Get_Bitmap(library, outline, bitmap);
	if (err)
		return lua__pusherror(L, err);
	
	lua_pushboolean(L, 1);
	return 1;
}

static int lua__FT__Glyph_Stroke(lua_State* L)
{
	FT_Error err;
	FT_Glyph glyph_stroke;
	FT_Glyph glyph;
	FT_Stroker stroker;
	FT_Bool destroy;
	
	glyph = lua__check<FT_Glyph>(L, 1);
	stroker = lua__check<FT_Stroker>(L, 2);
	destroy = lua_toboolean(L, 3) ? 1 : 0;
	
	LOG(FT_Glyph_Stroke);
	err = FT_Glyph_Stroke(&glyph, stroker, destroy);
	if (err)
		return lua__pusherror(L, err);
	
	glyph_stroke = glyph;
	
	lua_pushvalue(L, 1);
	lua__push(L, glyph_stroke, 1);
	return 1;
}

static int lua__FT__Glyph_StrokeBorder(lua_State* L)
{
	FT_Error err;
	FT_Glyph glyph_stroke;
	FT_Glyph glyph;
	FT_Stroker stroker;
	FT_Bool inside;
	FT_Bool destroy;
	
	glyph = lua__check<FT_Glyph>(L, 1);
	stroker = lua__check<FT_Stroker>(L, 2);
	inside = lua_toboolean(L, 3) ? 1 : 0;
	destroy = lua_toboolean(L, 4) ? 1 : 0;
	
	LOG(FT_Glyph_Stroke);
	err = FT_Glyph_StrokeBorder(&glyph, stroker, inside, destroy);
	if (err)
		return lua__pusherror(L, err);
	
	glyph_stroke = glyph;
	
	lua_pushvalue(L, 1);
	lua__push(L, glyph_stroke, 1);
	return 1;
}

static int lua__FT__Get_Kerning(lua_State* L)
{
	FT_Error err;
	FT_Face face;
	FT_UInt left_glyph;
	FT_UInt right_glyph;
	FT_UInt kern_mode;
	FT_Vector kerning;
	
	face = lua__check<FT_Face>(L, 1);
	left_glyph = (FT_UInt)luaL_checknumber(L, 2);
	right_glyph = (FT_UInt)luaL_checknumber(L, 3);
	if (!str_map_tovalue(luaL_checkstring(L, 4), (int*)&kern_mode, FT_KERNING_MODE_XXX))
		return typeerror(L, 4, "enum");
	
	err = FT_Get_Kerning(face, left_glyph, right_glyph, kern_mode, &kerning);
	if (err)
		return lua__pusherror(L, err);
	
	lua__pushfixed(L, kerning, 1<<6);
	return 1;
}

static int lua__FT__Outline_MoveToFunc(const FT_Vector* to, void* user)
{
	lua_State* L = (lua_State*)user;
	lua_settop(L, 2);
	lua_getfield(L, 2, "move_to");
	if (lua_type(L, -1) != LUA_TFUNCTION)
	{
		lua_pop(L, 1);
		return FT_Err_Unimplemented_Feature;
	}
	lua__pushfixed(L, *to, 1 << 6);
	if (lua_pcall(L, 1, 1, 1))
	{
		lua_replace(L, 2);
		lua_pushnil(L);
		lua_replace(L, 1);
		return -1;
	}
	else if (lua_type(L, -1) == LUA_TSTRING)
	{
		int result;
		result = lua__toerror(lua_tostring(L, -1));
		lua_pop(L, 1);
		return result;
	}
	else
	{
		lua_pop(L, 1);
		return 0;
	}
}

static int lua__FT__Outline_LineToFunc(const FT_Vector* to, void* user)
{
	lua_State* L = (lua_State*)user;
	lua_settop(L, 2);
	lua_getfield(L, 2, "line_to");
	if (lua_type(L, -1) != LUA_TFUNCTION)
	{
		lua_pop(L, 1);
		return FT_Err_Unimplemented_Feature;
	}
	lua__pushfixed(L, *to, 1 << 6);
	if (lua_pcall(L, 1, 1, 1))
	{
		lua_replace(L, 2);
		lua_pushnil(L);
		lua_replace(L, 1);
		return -1;
	}
	else if (lua_type(L, -1) == LUA_TSTRING)
	{
		int result;
		result = lua__toerror(lua_tostring(L, -1));
		lua_pop(L, 1);
		return result;
	}
	else
	{
		lua_pop(L, 1);
		return 0;
	}
}

static int lua__FT__Outline_ConicToFunc(const FT_Vector* control, const FT_Vector* to, void* user)
{
	lua_State* L = (lua_State*)user;
	lua_settop(L, 2);
	lua_getfield(L, 2, "conic_to");
	if (lua_type(L, -1) != LUA_TFUNCTION)
	{
		lua_pop(L, 1);
		return FT_Err_Unimplemented_Feature;
	}
	lua__pushfixed(L, *control, 1 << 6);
	lua__pushfixed(L, *to, 1 << 6);
	if (lua_pcall(L, 2, 1, 1))
	{
		lua_replace(L, 2);
		lua_pushnil(L);
		lua_replace(L, 1);
		return -1;
	}
	else if (lua_type(L, -1) == LUA_TSTRING)
	{
		int result;
		result = lua__toerror(lua_tostring(L, -1));
		lua_pop(L, 1);
		return result;
	}
	else
	{
		lua_pop(L, 1);
		return 0;
	}
}

static int lua__FT__Outline_CubicToFunc(const FT_Vector* control1, const FT_Vector* control2, const FT_Vector* to, void* user)
{
	lua_State* L = (lua_State*)user;
	lua_settop(L, 2);
	lua_getfield(L, 2, "cubic_to");
	if (lua_type(L, -1) != LUA_TFUNCTION)
	{
		lua_pop(L, 1);
		return FT_Err_Unimplemented_Feature;
	}
	lua__pushfixed(L, *control1, 1 << 6);
	lua__pushfixed(L, *control2, 1 << 6);
	lua__pushfixed(L, *to, 1 << 6);
	if (lua_pcall(L, 3, 1, 1))
	{
		lua_replace(L, 2);
		lua_pushnil(L);
		lua_replace(L, 1);
		return -1;
	}
	else if (lua_type(L, -1) == LUA_TSTRING)
	{
		int result;
		result = lua__toerror(lua_tostring(L, -1));
		lua_pop(L, 1);
		return result;
	}
	else
	{
		lua_pop(L, 1);
		return 0;
	}
}

static int lua__FT__Outline_Decompose(lua_State* L)
{
	FT_Error err;
	FT_Outline* outline;
	FT_Outline_Funcs func_interface;
	lua_State* thread;
	
	outline = lua__check<FT_Outline*>(L, 1);
	luaL_checktype(L, 2, LUA_TTABLE);
	lua_getfield(L, 2, "shift");
	if (lua_isnil(L, -1))
		func_interface.shift = 0;
	else
		func_interface.shift = (int)lua_tonumber(L, -1); /* :TODO: handle overflow */
	lua_pop(L, 1);
	lua_getfield(L, 2, "delta");
	if (lua_isnil(L, -1))
		func_interface.delta = 0;
	else
		func_interface.delta = lua__tofixed<FT_Pos>(L, -1, 1 << 6);
	lua_pop(L, 1);
	func_interface.move_to = lua__FT__Outline_MoveToFunc;
	func_interface.line_to = lua__FT__Outline_LineToFunc;
	func_interface.conic_to = lua__FT__Outline_ConicToFunc;
	func_interface.cubic_to = lua__FT__Outline_CubicToFunc;
	
	thread = lua_newthread(L);
	lua_getglobal(thread, "debug");
	lua_getfield(thread, 1, "traceback");
	lua_replace(thread, 1);
	lua_pushvalue(L, 2);
	lua_xmove(L, thread, 1);
	
	err = FT_Outline_Decompose(outline, &func_interface, thread);
	if (err)
	{
		if (lua_isnil(thread, 1))
		{
			lua_pushvalue(thread, 2);
			lua_xmove(thread, L, 1);
			return lua_error(L);
		}
		else
			return lua__pusherror(L, err);
	}
	
	lua_pushboolean(L, 1);
	return 1;
}

static int lua__FT__Library_Version(lua_State* L)
{
	FT_Library library;
	FT_Int major;
	FT_Int minor;
	FT_Int patch;
	
	library = lua__check<FT_Library>(L, 1);
	
	FT_Library_Version(library, &major, &minor, &patch);
	
	lua_pushnumber(L, (lua_Number)major);
	lua_pushnumber(L, (lua_Number)minor);
	lua_pushnumber(L, (lua_Number)patch);
	return 3;
}

//////////////////////////////////////////////////////////////////////////////

static const luaL_Reg FT_functions[] = {
	{"Init_FreeType", lua__FT__Init_FreeType},
	{"Open_Face", lua__FT__Open_Face},
	{"Set_Char_Size", lua__FT__Set_Char_Size},
	{"Get_Char_Index", lua__FT__Get_Char_Index},
	{"Load_Glyph", lua__FT__Load_Glyph},
	{"Get_Glyph", lua__FT__Get_Glyph},
	{"Stroker_New", lua__FT__Stroker_New},
	{"Stroker_Set", lua__FT__Stroker_Set},
	{"Stroker_ParseOutline", lua__FT__Stroker_ParseOutline},
	{"Stroker_GetCounts", lua__FT__Stroker_GetCounts},
	{"Outline_New", lua__FT__Outline_New},
	{"Stroker_Export", lua__FT__Stroker_Export},
	{"Bitmap_New", lua__FT__Bitmap_New},
	{"Outline_Get_Bitmap", lua__FT__Outline_Get_Bitmap},
	{"Glyph_To_Bitmap", lua__FT__Glyph_To_Bitmap},
	{"Glyph_Stroke", lua__FT__Glyph_Stroke},
	{"Glyph_StrokeBorder", lua__FT__Glyph_StrokeBorder},
	{"Get_Kerning", lua__FT__Get_Kerning},
	{"Outline_Decompose", lua__FT__Outline_Decompose},
	{"Library_Version", lua__FT__Library_Version},
#if 0
	{"New_Face", lua__FT__New_Face},
	{"New_Memory_Face", lua__FT__New_Memory_Face},
	{"Done_FreeType", lua__FT__Done_FreeType},
	{"Done_Library", lua__FT__Done_Library},
	{"Done_Face", lua__FT__Done_Face},
	{"Done_Glyph", lua__FT__Done_Glyph},
	{"Bitmap_Done", lua__FT__Bitmap_Done},
	{"Outline_Done", lua__FT__Outline_Done},
	{"Stroker_Done", lua__FT__Stroker_Done},
#endif
	{0, 0},
};

extern "C" LUAFREETYPE_API int luaopen_module(lua_State* L)
{
	lua_newtable(L);
	setfuncs(L, FT_functions, 0);
	return 1;
}

