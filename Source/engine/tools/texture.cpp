#include "texture.h"
#include "PL/pl_utils.h"

static inline uint32 strlen(char* str) 
{
	uint32 len = 0;
	while (*str != 0) 
	{
		str++;
		len++;
	}
	return len;
}

namespace BMP_FILE_FORMAT
{
	//Making sure to avoid packing. Size should be 14 bytes.
#pragma pack(push,bfh, 1)
	struct BitmapFileHeader
	{
		uint8   bitmap_type[2];
		int32   total_file_size;
		int16   reserved1;
		int16   reserved2;
		uint32	offset_bits;
	};
#pragma pack(pop,bfh)

	//Size should be 56 bytes
	struct BitmapDIBHeader
	{
		uint32    size_header;
		uint32    width;
		uint32    height;
		int16     planes;
		int16     bits_per_pixel;
		uint32    compression;
		uint32    buffer_size;
		uint32    ppm_x;
		uint32    ppm_y;
		uint32    clr_used;
		uint32    clr_important;
		//DO NOT MESS WITH MASK VALUES
		uint32    red_bitmask;
		uint32    green_bitmask;
		uint32    blue_bitmask;
		uint32    alpha_bitmask;
	};


	struct Bitmap
	{
		BitmapFileHeader bfh = {};
		BitmapDIBHeader bih = {};
		BitmapBuffer bitmap_buffer = {};
	};

	static void Setup_Bitmap(BitmapBuffer& bmp, uint32 width, uint32 height)
	{
		bmp.bytes_per_pixel = 4;
		bmp.height = height;
		bmp.width = width;
		bmp.buffer_memory = pl_buffer_alloc(bmp.size());
	}

	static bool Write_To_File(BitmapBuffer& bmb, const char* file_name)
	{
		Bitmap bmp;
		bmp.bitmap_buffer = bmb;
		bmp.bih.bits_per_pixel = 8 * bmp.bitmap_buffer.bytes_per_pixel;
		bmp.bih.planes = 1;
		bmp.bih.height = bmp.bitmap_buffer.height;
		bmp.bih.width = bmp.bitmap_buffer.width;
		bmp.bih.clr_important = 0;
		bmp.bih.clr_used = 0;
		bmp.bih.compression = 3;
		bmp.bih.buffer_size = bmp.bitmap_buffer.size();
		bmp.bih.size_header = sizeof(bmp.bih);
		bmp.bih.ppm_x = 197;
		bmp.bih.ppm_y = 39;
		bmp.bih.red_bitmask = 0x00FF0000;
		bmp.bih.green_bitmask = 0x0000FF00;
		bmp.bih.blue_bitmask = 0x000000FF;
		bmp.bih.alpha_bitmask = 0x00000000;

		bmp.bfh.bitmap_type[0] = 'B';
		bmp.bfh.bitmap_type[1] = 'M';
		bmp.bfh.offset_bits = 14 + sizeof(bmp.bih);
		bmp.bfh.reserved1 = 0;
		bmp.bfh.reserved2 = 0;
		bmp.bfh.total_file_size = 14 + sizeof(bmp.bih) + bmp.bitmap_buffer.size();

		void* file = 0;

		uint32 file_id = 0;
		int length = strlen((char*)file_name);

		char new_name[1024];
		pl_format_print(new_name, length + 8, "%s%c%u%s", file_name, '_', file_id, ".bmp");

		//Checking if another file exists with same name
		while (!pl_create_file(&file, new_name))
		{
			file_id++;
			pl_format_print(new_name, length + 8, "%s%c%u%s", file_name, '_', file_id, ".bmp");
		}
		if (file)
		{
			pl_append_to_file(file, &bmp.bfh, 14);
			pl_append_to_file(file,&bmp.bih, sizeof(bmp.bih));
			pl_append_to_file(file, bmp.bitmap_buffer.buffer_memory, bmp.bitmap_buffer.size());
			pl_close_file_handle(file);
		}
		return true;
	}

	//use if extracting this namespace for seperate BMP library, else defined in texture.h

	/*inline void Set_Pixel(const Vec3<uint8>& color, Texture& buffer, int32 x, int32 y)
	{
		if (buffer.file_type == TextureFileType::BMP)
		{
			Vec3<uint8> rgb_color = { color.z, color.y,color.x };
			ASSERT(((((uint32)x <= buffer.width) && (x >= 0)) && (((uint32)y <= buffer.height) && (y >= 0))));

			uint32* pixel = (uint32*)buffer.bmb.buffer_memory;
			pixel += y * buffer.bmb.width + x;
			*pixel = *(uint32*)&rgb_color.raw[0] & 0x00FFFFFF;
		}
	}*/
}

 bool Setup_Texture(Texture& tex, TextureFileType file_type, uint32 width, uint32 height)
{
	tex.file_type = file_type;
	if (tex.file_type == TextureFileType::BMP)
	{
		BMP_FILE_FORMAT::Setup_Bitmap(tex.bmb, width, height);
		return true;
	}
	else
	{
		return false;	//Bitmap initilization not defined for file type
		ASSERT(false);
	}
}

 bool Write_To_File(Texture& tex, const char* file_name)
{
	if (tex.file_type == TextureFileType::BMP)
	{
		return BMP_FILE_FORMAT::Write_To_File(tex.bmb, file_name);
	}
	else
	{
		return false;
		ASSERT(false); //File initilization not defined for file type
	}
}
