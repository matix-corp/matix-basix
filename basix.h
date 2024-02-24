//******************
// 
// basix.h
// 
//******************
// presentation :
// 
// Basix is a little project containing base functions for Matix.
// It can also be use in any projects.
//

#pragma once

#include <bitset>
#include <codecvt>
#include <filesystem>
#include <fstream>
#include <intrin.h>
#include <iostream>
#include <locale>
#include <math.h>
#include <sstream>
#include <string>
#include <vector>
#include <zlib/zlib.h>

// ZLib mandatory stuff
#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#  include <fcntl.h>
#  include <io.h>
#  define SET_BINARY_MODE(file) _setmode(_fileno(file), O_BINARY)
#else
#  define SET_BINARY_MODE(file)
#endif

// The namespace "basix" is used to simplify the all.
namespace basix
{
	// Static bool, usefull in debugging to tell to Basix if the "print" function works or not
	static bool _can_print = true;

	// Variables used to define a float separation character
	static std::string _float_character = "";
	static std::string _non_float_character = "";

	// Return the size of a number
	inline int sign(float number) { return number < 0 ? -1 : (number == 0 ? 0 : 1); };

	// Return the value of "_can_print".
	inline bool can_print() { return _can_print; };

	// Cut a string in a vector where there are the "cut" part
	inline std::vector<std::string> cut_string(std::string string, std::string cut, bool erase_blank = false)
	{
		std::string last_string = ""; // String since the last cut
		std::string last_string_cut = ""; // String of the "cut" size which allows to know where to cut
		std::vector<std::string> result = std::vector<std::string>();
		for (int i = 0; i < string.size(); i++) // Browse the string char by char
		{
			last_string_cut += string[i];
			if (last_string_cut.size() > cut.size()) // If the string which allows to know where to cut is too long, cut him
			{
				last_string_cut = last_string_cut.substr(1, cut.size());
			}

			if (last_string_cut == cut) // If the string which allows to know where to cut is equal to the part to cut, do a cut
			{
				std::string final_string = last_string.substr(0, last_string.size() - (cut.size() - 1));
				if (erase_blank)
				{
					if (final_string != "")
					{
						result.push_back(final_string);
					}
				}
				else
				{
					result.push_back(final_string);
				}
				last_string = "";
				last_string_cut = "";
			}
			else
			{
				last_string += string[i];
			}
		}

		if (last_string.size() > 0) { result.push_back(last_string); } // Add the last non-cutted element
		return result;
	};

	// Delete each element into an unused binary list
	inline void delete_binary(std::vector<char*>& datas)
	{
		for (int i = 0; i < datas.size(); i++) delete datas[i];
		datas.clear();
	}

	// Return the datas about a file (assuming the file exists).
	inline struct stat file_datas(std::string path) { struct stat sb; bool result = (stat(path.c_str(), &sb) == 0); return sb; };

	// Returns if a file exists.
	inline bool file_exists(std::string path) { struct stat sb; bool result = (stat(path.c_str(), &sb) == 0); return result; };

	// Join a vector of string into one string.
	inline std::string join_string(std::vector<std::string> strings, std::string separation)
	{
		std::string result = "";
		for (int i = 0; i < strings.size(); i++)
		{
			result += strings[i]; // Concatenate each strings
			if (i != strings.size() - 1)
			{
				result += separation; // Add the separation between strings
			}
		}

		return result;
	}

	// Normalize a value between "min" and "max" included
	inline double normalize_value(double number, double min, double max)
	{
		double to_add = (max - min) + 1;
		while (number < min) number += to_add;
		while (number > max) number -= to_add;
		return number;
	}

	// Apply the Paeth function to a left, above and upper left values
	inline float paeth_function(float left, float above, float upper_left)
	{
		float p = left + above - upper_left;
		float pa = abs(p - left);
		float pb = abs(p - above);
		float pc = abs(p - upper_left);
		if (pa <= pb && pa <= pc) return left;
		else if (pb <= pc) return above;
		else return upper_left;
	}

	// Returns if a path is a directory or not
	inline bool path_is_directory(std::string path)
	{
		return ((file_datas(path).st_mode & S_IFDIR) == S_IFDIR);
	};

	// Print the "message" something in the console, coming from "sender" of type "type".
	inline void print(std::string type, std::string sender, std::string message)
	{
		if (can_print())
		{
			std::cout << message << std::endl;
		}
	};

	// Return the content of a file.
	inline std::string read_file(std::string path)
	{
		std::string file_content;
		std::ifstream file;

		// ensure ifstream objects can throw exceptions:
		file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			file.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<char>));
			file.open(path);
			std::stringstream stream; stream << file.rdbuf();
			file.close();
			file_content = stream.str();
		}
		catch (std::ifstream::failure e) { print("Error", "System", "The file \"" + path + "\" can't be opened."); }

		return file_content;
	}

	// Return the content of a file in binary with vector of char
	inline void read_file_binary(std::string path, std::vector<char*>& datas, std::vector<unsigned int> size, unsigned int start_pos = 0)
	{
		std::string file_content;
		std::ifstream file;
		// ensure ifstream objects can throw exceptions:
		file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			file.open(path, std::ios::binary);
			file.seekg(start_pos, file.beg);
			for (int i = 0; i < datas.size(); i++)
			{
				file.read(datas[i], size[i]);
			}
			file.close();
		}
		catch (std::ifstream::failure e) { print("Error", "System", "The file \"" + path + "\" can't be opened, error -> " + e.what() + "."); }
	};

	// Return the content of a file in binary with a char array
	inline void read_file_binary(std::string path, char* datas, std::vector<unsigned int> size, unsigned int start_pos = 0)
	{
		std::string file_content;
		std::ifstream file;
		// ensure ifstream objects can throw exceptions:
		file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			file.open(path, std::ios::binary);
			file.seekg(start_pos, file.beg);
			for (int i = 0; i < size.size(); i++)
			{
				file.read(datas, size[i]);
			}
			file.close();
		}
		catch (std::ifstream::failure e) { print("Error", "System", "The file \"" + path + "\" can't be opened, error -> " + e.what() + "."); }
	};

	// Replace a string in an another string.
	inline std::string replace(std::string str, std::string to_replace, std::string new_str)
	{
		return join_string(cut_string(str, to_replace), new_str);
	}

	// Change the value of the static "_can_print" function.
	inline void set_can_print(bool can_print) { _can_print = can_print; };

	// Convert a string to a float
	inline float string_to_float(std::string str)
	{
		if (_float_character == "")
		{
			_float_character = ".";
			_non_float_character = ",";
			if (std::stod("0.25") != 0.25) // Test if the "." is the separation character
			{
				_float_character = ",";
				_non_float_character = ".";
			}
		}

		return std::stod(replace(str, _non_float_character, _float_character));
	}

	// Swap a buffer
	template<typename T>
	inline std::vector<T> swap_vector(std::vector<T> v)
	{
		for (int i = 0; i < floor((float)v.size() / 2.0); i++)
		{
			T temp = v[i];
			v[i] = v[v.size() - (i + 1)];
			v[i + 1] = temp;
		}
		return v;
	};

	// Write something in a file.
	inline void write_in_file(std::string path, std::string to_write, std::ios::openmode opening_mode = std::ios::out)
	{
		std::ofstream file;
		file.exceptions(std::ofstream::failbit | std::ofstream::badbit);
		try
		{
			file.open(path, opening_mode);
			file << to_write;
			file.close();
		}
		catch (std::ofstream::failure e)
		{
			print("Error", "System", "The file \"" + path + "\" can't be written in error -> " + e.what() + ".");
		}
	}

	// Write binary data in a file
	inline void write_in_file_binary(std::string path, std::vector<char*> to_write, std::ios::openmode opening_mode = std::ios::out | std::ios::binary)
	{
		std::ofstream file;
		file.exceptions(std::ofstream::failbit | std::ofstream::badbit);
		try
		{
			file.open(path, opening_mode);
			for(int i = 0;i<to_write.size();i++)
			{
				file.write(to_write[i], sizeof(i));
			}
			file.close();
		}
		catch (std::ofstream::failure e)
		{
			print("Error", "System", "The file \"" + path + "\" can't be written in error -> " + e.what() + ".");
		}
	}

	// Return the content of a directory in a vector of string.
	inline std::vector<std::string> directory_content(std::string path, bool sub_directory = false)
	{
		std::vector<std::string> result;
		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			std::string path = entry.path().string();
			if (sub_directory && path_is_directory(path) && file_exists(path))
			{
				std::vector<std::string> sub = directory_content(path, true);
				for (int i = 0; i < sub.size(); i++) result.push_back(sub[i]);
			}
			else
			{
				result.push_back(path);
			}
		}
		return result;
	};

	// Struct representing datas about a PNG chunk
	struct PNG_Chunk {
		// Positionf of the chunk in the file
		unsigned int position = 0;
		// Name of the chunk
		std::string name = "";
		// Size of the chunk
		unsigned int size = 0;
	};

	// Struct representing a pixel in a PNG image
	struct PNG_Pixel {
		// Alpha value of the pixel
		unsigned char alpha = 0;
		// Blue value of the pixel
		unsigned char blue = 0;
		// Green value of the pixel
		unsigned char green = 0;
		// Red value of the pixel
		unsigned char red = 0;
	};

	class PNG_Image
	{
		// Class representing a PNG image handler
	public:
		// PNG_Image constructor
		PNG_Image() { fill(0, 0, 0); };
		// PNG_Image copy constructor
		PNG_Image(PNG_Image& copy) : PNG_Image()
		{
			a_bit_depth = copy.a_bit_depth;
			a_color_type = copy.a_color_type;
			a_compression_method = copy.a_compression_method;
			a_filter_method = copy.a_filter_method;
			a_height = copy.a_height;
			a_interlace_method = copy.a_interlace_method;
			a_path = copy.a_path;
			a_physical_height_ratio = copy.a_physical_height_ratio;
			a_physical_unit = copy.a_physical_unit;
			a_physical_width_ratio = copy.a_physical_width_ratio;
			a_pixels = copy.a_pixels;
			a_width = copy.a_width;
		};
		// Return the data of the image in a unsigned char*
		unsigned char* data()
		{
			// Create the base variables to create the datas
			unsigned char components = 4;
			unsigned char* datas = new unsigned char[components * get_width() * get_height()];

			for (int i = 0; i < get_height(); i++)
			{
				for (int j = 0; j < get_width(); j++) // Create the datas
				{
					unsigned int position = (i * get_width() + j) * components;
					datas[position] = ((unsigned char)get_pixel(j, i).red);
					if (components > 1)
					{
						datas[position + 1] = ((unsigned char)get_pixel(j, i).green);
						if (components > 2)
						{
							datas[position + 2] = ((unsigned char)get_pixel(j, i).blue);
							if (components > 3)
							{
								datas[position + 3] = ((unsigned char)get_pixel(j, i).alpha);
							}
						}
					}
				}
			}

			return datas;
		}
		// Delete the pixels in the memory
		void free_memory()
		{
			if (a_last_processed_line_pixel != 0)
			{
				delete a_last_processed_line_pixel;
				a_last_processed_line_pixel = 0;
			}

			if (a_pixels != 0)
			{
				for (int i = 0; i < a_pixels->size(); i++)
				{
					delete (*a_pixels)[i];
					(*a_pixels)[i] = 0;
				}
				delete a_pixels;
				a_pixels = 0;
			}

			if (a_processed_pixel != 0)
			{
				delete a_processed_pixel;
				a_processed_pixel = 0;
			}
		};
		// Fill the image with one color
		void fill(unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha = 255)
		{
			free_memory();
			a_last_processed_line_pixel = new std::vector<PNG_Pixel>();
			a_pixels = new std::vector<std::vector<PNG_Pixel>*>();
			a_processed_pixel = new std::vector<PNG_Pixel*>();
			for (int i = 0; i < get_width(); i++)
			{
				std::vector<PNG_Pixel>* line = new std::vector<PNG_Pixel>();
				for (int j = 0; j < get_height(); j++)
				{
					PNG_Pixel pixel;
					pixel.red = red;
					pixel.green = green;
					pixel.blue = blue;
					pixel.alpha = alpha;
					line->push_back(pixel);
				}
				a_pixels->push_back(line);
			}
		};
		// Flip the image on the X axis
		inline void flip_x() { a_flipped_x = !a_flipped_x; };
		// Flip the image on the Y axis
		inline void flip_y() { a_flipped_y = !a_flipped_y; };
		// Get every chunks into a PNG image
		std::vector<PNG_Chunk> get_all_chunks_from_path(std::string path)
		{
			std::vector<PNG_Chunk> to_return = std::vector<PNG_Chunk>();
			if (file_exists(path) && !path_is_directory(path))
			{
				// Create the necessary things to read the PNG file
				std::vector<char*> header = std::vector<char*>();
				std::vector<PNG_Chunk> idat_chunk = std::vector<PNG_Chunk>();
				std::string name = "";
				std::vector<unsigned int> size = std::vector<unsigned int>();
				unsigned int size_offset = 0;

				// Check each chunks in the file
				while (name != "IEND")
				{
					header.clear();
					size.clear();
					name = "";
					header.push_back((char*)(new unsigned int(0)));
					size.push_back(sizeof(unsigned int));
					for (int i = 0; i < 4; i++)
					{
						header.push_back((new char(0)));
						size.push_back(sizeof(char));
					}
					read_file_binary(path, header, size, 8 + size_offset);
					unsigned int chunk_size = *((int*)header[0]);
					chunk_size = _byteswap_ulong(chunk_size);
					for (int i = 0; i < 4; i++)
					{
						name += *header[i + 1];
					}
					delete_binary(header);
					PNG_Chunk chunk;
					chunk.position = size_offset + 16;
					chunk.name = name;
					chunk.size = chunk_size;
					size_offset += chunk_size + 12;
					to_return.push_back(chunk);

					if (name == "pHYs")
					{
						load_pHYS_from_path(path, chunk);
					}
					else if (name == "IDAT" && is_loadable())
					{
						idat_chunk.push_back(chunk);
					}
					else if (name == "sRGB")
					{
						load_sRGB_from_path(path, chunk);
					}
					else if (name == "PLTE" || name == "bKGD") // Not implemented yet
					{
						a_loadable = false;
					}
				}

				// Load IDAT chunks
				if (idat_chunk.size() > 0 && is_loadable())
				{
					a_error_load = load_IDAT_from_path(path, idat_chunk);
					if (a_error_load != 1) a_loadable = false;
				}
			}
			return to_return;
		};
		// Return the pixel located in a position in the image
		PNG_Pixel get_pixel(unsigned short x, unsigned short y)
		{
			PNG_Pixel to_return;
			if (x >= 0 && x < get_width() && y >= 0 && y < get_height())
			{
				to_return = (*(*a_pixels)[normalized_x(x)])[normalized_y(y)];
			}
			return to_return;
		}
		// Load the base data of an image from a path
		bool load_base_from_path(std::string path)
		{
			if (file_exists(path) && !path_is_directory(path))
			{
				// Create the necessary things to read the PNG file
				std::vector<char*> header = std::vector<char*>();
				std::vector<unsigned int> size = std::vector<unsigned int>();
				for (int i = 0; i < 8; i++)
				{
					header.push_back((char*)(new unsigned char(0)));
					size.push_back(sizeof(unsigned char));
				}
				read_file_binary(path, header, size);

				// Check if the signature is correct (137 80 78 71 13 10 26 10 for png files)
				float signature[8];
				signature[0] = 137;
				signature[1] = 80;
				signature[2] = 78;
				signature[3] = 71;
				signature[4] = 13;
				signature[5] = 10;
				signature[6] = 26;
				signature[7] = 10;
				for (int i = 0; i < header.size(); i++)
				{
					if (signature[i] != *((unsigned char*)header[i])) return false;
				}
				delete_binary(header);

				// Check the first chunk of the file
				// Get the size of the chunk
				size.clear();
				header.push_back((char*)(new unsigned int(0)));
				size.push_back(sizeof(unsigned int));
				read_file_binary(path, header, size, 8);
				unsigned int chunk_size = *((int*)header[0]);
				chunk_size = _byteswap_ulong(chunk_size);
				delete_binary(header);
				if (chunk_size != 13) return false;
				// Get the datas of the chunk
				size.clear();
				for (int i = 0; i < 2; i++)
				{
					header.push_back((char*)(new unsigned int(0)));
					size.push_back(sizeof(unsigned int));
				}
				for (int i = 0; i < 5; i++)
				{
					header.push_back((char*)(new unsigned char(0)));
					size.push_back(sizeof(unsigned char));
				}
				read_file_binary(path, header, size, 16);
				unsigned int chunk_height = *((int*)header[1]);
				a_height = _byteswap_ulong(chunk_height);
				unsigned int chunk_width = *((int*)header[0]);
				a_width = _byteswap_ulong(chunk_width);
				a_bit_depth = *(header[2]);
				a_color_type = *(header[3]);
				a_compression_method = *(header[4]);
				a_filter_method = *(header[5]);
				a_interlace_method = *(header[6]);
				delete_binary(header);

				fill(0, 0, 0);
			}
			else
			{
				return false;
			}
		}
		// Load the image from a path
		bool load_from_path(std::string path)
		{
			if (load_base_from_path(path))
			{
				get_all_chunks_from_path(path);
			}
			else
			{
				return false;
			}
		};
		// Load a IDAT chunk grom a path
		char load_IDAT_from_path(std::string path, std::vector<PNG_Chunk> chunk)
		{
			if (file_exists(path) && !path_is_directory(path))
			{
				// Get the size of the chunks
				unsigned int current_size = 0;
				for (int i = 0; i < chunk.size(); i++)
				{
					current_size += chunk[i].size;
				}

				// Create the necessary things to read the PNG file
				char* header = new char[current_size];
				char* header_part = new char[current_size];
				std::vector<unsigned int> size = std::vector<unsigned int>();
				unsigned int total_size = 0;
				current_size = 0;

				// Read into the chunk
				for (int i = 0; i < chunk.size(); i++)
				{
					size.clear();
					size.push_back(chunk[i].size);
					read_file_binary(path, header_part, size, chunk[i].position);
					total_size += chunk[i].size;

					for (int j = 0; j < chunk[i].size; j++)
					{
						(header[current_size + j]) = (header_part[j]);
					}
					current_size += chunk[i].size;
					delete[] header_part;
					header_part = new char[a_chunk_size];
				}
				delete[] header_part;
				header_part = 0;

				// Set binary mode
				(void)SET_BINARY_MODE(stdin);
				(void)SET_BINARY_MODE(stdout);

				// Define compression variables
				unsigned char components = 4;
				int level = get_compression_method();
				int ret = 0;
				unsigned have = 0;
				z_stream strm;
				unsigned int out_size = (get_width() * get_height() * components) + get_height();
				unsigned char* out = new unsigned char[out_size];

				// Create compression ENV
				strm.zalloc = Z_NULL;
				strm.zfree = Z_NULL;
				strm.opaque = Z_NULL;
				strm.avail_in = 0;
				strm.next_in = Z_NULL;
				ret = inflateInit(&strm);
				if (ret != Z_OK) return ret;
				strm.avail_in = total_size;
				strm.next_in = (Bytef*)(header);
				bool stream_end = false;

				// Uncompress data
				do
				{
					// Set output
					strm.avail_out = a_chunk_size * chunk.size();
					strm.next_out = out;

					// Do the decompression
					ret = inflate(&strm, Z_NO_FLUSH);
					if (ret == Z_STREAM_ERROR)
					{
						(void)inflateEnd(&strm);
						delete[] header;
						delete[] out;
						return -1;
					}

					switch (ret)
					{
						case Z_NEED_DICT:
							ret = Z_DATA_ERROR;
							break;
						case Z_MEM_ERROR:
							(void)inflateEnd(&strm);
							delete[] header;
							delete[] out;
							return -2;
						case Z_STREAM_END:
							stream_end = true;
							break;
					}
				} while (strm.avail_out == 0 && !stream_end);
				have = strm.total_out;
				(void)inflateEnd(&strm);

				// Process data
				a_processed_pixel->clear();
				a_processing_last_position = a_processed_data + 1;
				unsigned char component_size = 4;
				unsigned short data_number = have - get_height();
				PNG_Pixel last_pixel;
				unsigned int part = -1;
				unsigned int y_pixel = 0;
				for (int i = 0; i < out_size; i++)
				{
					if (part >= 0 && part < get_width() * component_size)
					{
						unsigned char component = part % component_size;
						if (component == 0) // Apply red component
						{
							a_processed_data++;
							a_processed_pixel->push_back(_get_current_pixel_processed());
							_get_current_pixel_processed()->red = (unsigned char)(out[i]);
						}
						else if (component == 1) // Apply green component
						{
							_get_current_pixel_processed()->green = (unsigned char)(out[i]);
						}
						else if (component == 2) // Apply blue component
						{
							_get_current_pixel_processed()->blue = (unsigned char)(out[i]);
						}
						else if (component == 3) // Apply alpha component
						{
							_get_current_pixel_processed()->alpha = (unsigned char)(out[i]);
						}
						part++;
					}
					else
					{
						if ((*a_processed_pixel).size() > 0)
						{
							if (a_filter_type == 1) // Apply sub filtering
							{
								for (int i = 1; i < a_processed_pixel->size(); i++)
								{
									PNG_Pixel final_pixel;
									PNG_Pixel pixel = *(*a_processed_pixel)[i - 1];
									final_pixel.red = normalize_value((*a_processed_pixel)[i]->red + pixel.red, 0, 255);
									final_pixel.green = normalize_value((*a_processed_pixel)[i]->green + pixel.green, 0, 255);
									final_pixel.blue = normalize_value((*a_processed_pixel)[i]->blue + pixel.blue, 0, 255);
									final_pixel.alpha = normalize_value((*a_processed_pixel)[i]->alpha + pixel.alpha, 0, 255);
									(*(*a_processed_pixel)[i]) = final_pixel;
								}
							}
							else if (a_filter_type == 2 && a_last_processed_line_pixel->size() > 0) // Apply up filtering
							{
								for (int i = 0; i < a_processed_pixel->size(); i++)
								{
									PNG_Pixel final_pixel;
									PNG_Pixel pixel = (*a_last_processed_line_pixel)[i];
									final_pixel.red = normalize_value((*a_processed_pixel)[i]->red + pixel.red, 0, 255);
									final_pixel.green = normalize_value((*a_processed_pixel)[i]->green + pixel.green, 0, 255);
									final_pixel.blue = normalize_value((*a_processed_pixel)[i]->blue + pixel.blue, 0, 255);
									final_pixel.alpha = normalize_value((*a_processed_pixel)[i]->alpha + pixel.alpha, 0, 255);
									(*(*a_processed_pixel)[i]) = final_pixel;
								}
							}
							else if (a_filter_type == 3) // Apply average filtering
							{
								if (a_last_processed_line_pixel->size() > 0)
								{
									for (int i = 0; i < a_processed_pixel->size(); i++)
									{
										PNG_Pixel final_pixel;
										PNG_Pixel pixel1;
										if (i == 0)
										{
											pixel1.red = 0;
											pixel1.green = 0;
											pixel1.blue = 0;
											pixel1.alpha = 0;
										}
										else
										{
											pixel1 = *(*a_processed_pixel)[i - 1];
										}
										PNG_Pixel pixel2 = (*a_last_processed_line_pixel)[i];
										final_pixel.red = normalize_value((*a_processed_pixel)[i]->red + floor(((float)pixel1.red + (float)pixel2.red) / 2.0), 0, 255);
										final_pixel.green = normalize_value((*a_processed_pixel)[i]->green + floor(((float)pixel1.green + (float)pixel2.green) / 2.0), 0, 255);
										final_pixel.blue = normalize_value((*a_processed_pixel)[i]->blue + floor(((float)pixel1.blue + (float)pixel2.blue) / 2.0), 0, 255);
										final_pixel.alpha = normalize_value((*a_processed_pixel)[i]->alpha + floor(((float)pixel1.alpha + (float)pixel2.alpha) / 2.0), 0, 255);
										(*(*a_processed_pixel)[i]) = final_pixel;
									}
								}
							}
							else if (a_filter_type == 4 && a_pixels->size() > 0) // Apply paeth filtering
							{
								for (int i = 0; i < a_processed_pixel->size(); i++)
								{
									PNG_Pixel final_pixel;
									if (i == 0)
									{
										PNG_Pixel pixel = (*a_last_processed_line_pixel)[i];
										unsigned char final_red = pixel.red; // Calculate final colors
										unsigned char final_green = pixel.green;
										unsigned char final_blue = pixel.blue;
										unsigned char final_alpha = pixel.alpha;
										final_pixel.red = normalize_value((*(*a_processed_pixel)[i]).red + final_red, 0, 255);
										final_pixel.green = normalize_value((*(*a_processed_pixel)[i]).green + final_green, 0, 255);
										final_pixel.blue = normalize_value((*(*a_processed_pixel)[i]).blue + final_blue, 0, 255);
										final_pixel.alpha = normalize_value((*(*a_processed_pixel)[i]).alpha + final_alpha, 0, 255);
									}
									else
									{
										PNG_Pixel pixel2 = (*a_last_processed_line_pixel)[i];
										PNG_Pixel pixel3 = (*a_last_processed_line_pixel)[i - 1];
										PNG_Pixel pixel1 = *(*a_processed_pixel)[i - 1];
										final_pixel.red = normalize_value((*(*a_processed_pixel)[i]).red + paeth_function(pixel1.red, pixel2.red, pixel3.red), 0, 255);
										final_pixel.green = normalize_value((*(*a_processed_pixel)[i]).green + paeth_function(pixel1.green, pixel2.green, pixel3.green), 0, 255);
										final_pixel.blue = normalize_value((*(*a_processed_pixel)[i]).blue + paeth_function(pixel1.blue, pixel2.blue, pixel3.blue), 0, 255);
										final_pixel.alpha = normalize_value((*(*a_processed_pixel)[i]).alpha + paeth_function(pixel1.alpha, pixel2.alpha, pixel2.alpha), 0, 255);
									}
									(*(*a_processed_pixel)[i]) = final_pixel;
								}
							}
						}

						a_filter_type = out[i];
						part = 0;

						a_last_processed_line_pixel->clear();
						for (int i = 0; i < a_processed_pixel->size(); i++)
						{
							a_last_processed_line_pixel->push_back(*(*a_processed_pixel)[i]);
						}

						a_processed_pixel->clear();
					}
				}

				if (a_processed_pixel != 0 && a_processed_pixel->size() > 0)
				{
					if (a_filter_type == 1) // Apply sub filtering
					{
						for (int i = 1; i < a_processed_pixel->size(); i++)
						{
							PNG_Pixel final_pixel;
							PNG_Pixel pixel = *(*a_processed_pixel)[i - 1];
							final_pixel.red = normalize_value(pixel.red - (*(*a_processed_pixel)[i]).red, 0, 255);
							final_pixel.green = normalize_value(pixel.green - (*(*a_processed_pixel)[i]).green, 0, 255);
							final_pixel.blue = normalize_value(pixel.blue - (*(*a_processed_pixel)[i]).blue, 0, 255);
							final_pixel.alpha = normalize_value(pixel.alpha - (*(*a_processed_pixel)[i]).alpha, 0, 255);
							(*(*a_processed_pixel)[i]) = final_pixel;
						}
					}
					else if (a_filter_type == 2 && a_last_processed_line_pixel->size() > 0) // Apply up filtering
					{
						for (int i = 0; i < a_processed_pixel->size(); i++)
						{
							PNG_Pixel final_pixel;
							PNG_Pixel pixel = (*a_last_processed_line_pixel)[i];
							final_pixel.red = normalize_value(pixel.red - (*(*a_processed_pixel)[i]).red, 0, 255);
							final_pixel.green = normalize_value(pixel.green - (*(*a_processed_pixel)[i]).green, 0, 255);
							final_pixel.blue = normalize_value(pixel.blue - (*(*a_processed_pixel)[i]).blue, 0, 255);
							final_pixel.alpha = normalize_value(pixel.alpha - (*(*a_processed_pixel)[i]).alpha, 0, 255);
							(*(*a_processed_pixel)[i]) = final_pixel;
						}
					}
					else if (a_filter_type == 3) // Not implemented yet
					{
						delete[] header;
						delete[] out;
						return -3;
					}
					else if (a_filter_type == 4 && a_pixels->size() > 0) // Apply paeth filtering
					{
						for (int i = 0; i < a_processed_pixel->size(); i++)
						{
							PNG_Pixel final_pixel;
							if (i == 0)
							{
								PNG_Pixel pixel = (*a_last_processed_line_pixel)[i];
								unsigned char final_red = pixel.red; // Calculate final colors
								unsigned char final_green = pixel.green;
								unsigned char final_blue = pixel.blue;
								unsigned char final_alpha = pixel.alpha;
								final_pixel.red = normalize_value((*(*a_processed_pixel)[i]).red + final_red, 0, 255);
								final_pixel.green = normalize_value((*(*a_processed_pixel)[i]).green + final_green, 0, 255);
								final_pixel.blue = normalize_value((*(*a_processed_pixel)[i]).blue + final_blue, 0, 255);
								final_pixel.alpha = normalize_value((*(*a_processed_pixel)[i]).alpha + final_alpha, 0, 255);
							}
							else
							{
								PNG_Pixel pixel2 = (*a_last_processed_line_pixel)[i];
								PNG_Pixel pixel3 = (*a_last_processed_line_pixel)[i - 1];
								PNG_Pixel pixel1 = *(*a_processed_pixel)[i - 1];
								final_pixel.red = normalize_value((*(*a_processed_pixel)[i]).red + paeth_function(pixel1.red, pixel2.red, pixel3.red), 0, 255);
								final_pixel.green = normalize_value((*(*a_processed_pixel)[i]).green + paeth_function(pixel1.green, pixel2.green, pixel3.green), 0, 255);
								final_pixel.blue = normalize_value((*(*a_processed_pixel)[i]).blue + paeth_function(pixel1.blue, pixel2.blue, pixel3.blue), 0, 255);
								final_pixel.alpha = normalize_value((*(*a_processed_pixel)[i]).alpha + paeth_function(pixel1.alpha, pixel2.alpha, pixel2.alpha), 0, 255);
							}
							(*(*a_processed_pixel)[i]) = final_pixel;
						}
					}
				}

				// Free memory
				delete[] header;
				delete[] out;
			}
			else
			{
				return -4;
			}
			return 1;
		};
		// Load the pHYS chunk from a path
		bool load_pHYS_from_path(std::string path, PNG_Chunk chunk)
		{
			if (file_exists(path) && !path_is_directory(path) && chunk.name == "pHYs" && chunk.size == 9)
			{
				// Create the necessary things to read the PNG file
				std::vector<char*> header = std::vector<char*>();
				std::vector<unsigned int> size = std::vector<unsigned int>();

				// Read into the chunk
				for (int i = 0; i < 2; i++)
				{
					header.push_back((char*)(new unsigned int(0)));
					size.push_back(sizeof(unsigned int));
				}
				header.push_back((new char(0)));
				size.push_back(sizeof(char));
				read_file_binary(path, header, size, chunk.position);
				unsigned int physical_height = *((unsigned int*)header[1]);
				a_physical_height_ratio = _byteswap_ulong(physical_height);
				unsigned int physical_width = *((unsigned int*)header[0]);
				a_physical_width_ratio = _byteswap_ulong(physical_width);
				a_physical_unit = *header[2];
				delete_binary(header);
			}
			else
			{
				return false;
			}
		};
		// Load the sRGB chunk from a path
		bool load_sRGB_from_path(std::string path, PNG_Chunk chunk)
		{
			if (file_exists(path) && !path_is_directory(path) && chunk.name == "sRGB" && chunk.size == 1)
			{
				// Create the necessary things to read the PNG file
				std::vector<char*> header = std::vector<char*>();
				std::vector<unsigned int> size = std::vector<unsigned int>();

				// Read into the chunk
				header.push_back((new char(0)));
				size.push_back(sizeof(char));
				read_file_binary(path, header, size, chunk.position);
				a_srgb_value = (*header[0]);

				return true;
			}
			else
			{
				return false;
			}
		}
		// Return the x parameter normalized according to the image
		inline unsigned short normalized_x(unsigned short x)
		{
			if (is_flipped_y()) x = get_width() - (x + 1);
			return x;
		}
		// Return the y parameter normalized according to the image
		inline unsigned short normalized_y(unsigned short y)
		{
			if (is_flipped_x()) y = get_height() - (y + 1);
			return y;
		}
		// Set the color of a pixel
		void set_pixel(unsigned short x, unsigned short y, PNG_Pixel pixel)
		{
			(*(*a_pixels)[normalized_x(x)])[normalized_y(y)] = pixel;
		}
		// Set the alpha value of a pixel
		void set_pixel_alpha(unsigned short x, unsigned short y, unsigned char alpha)
		{
			(*(*a_pixels)[normalized_x(x)])[normalized_y(y)].alpha = alpha;
		}
		// Set the blue value of a pixel
		void set_pixel_blue(unsigned short x, unsigned short y, unsigned char blue)
		{
			(*(*a_pixels)[normalized_x(x)])[normalized_y(y)].blue = blue;
		}
		// Set the green value of a pixel
		void set_pixel_green(unsigned short x, unsigned short y, unsigned char green)
		{
			(*(*a_pixels)[normalized_x(x)])[normalized_y(y)].green = green;
		}
		// Set the red value of a pixel
		void set_pixel_red(unsigned short x, unsigned short y, unsigned char red)
		{
			(*(*a_pixels)[normalized_y(x)])[normalized_y(y)].red = red;
		}
		// PNG_Image destructor
		~PNG_Image() { free_memory(); };

		// Getters and setters
		inline unsigned int get_bit_depht() { return a_bit_depth; };
		inline unsigned int get_color_type() { return a_color_type; };
		inline PNG_Pixel* _get_current_pixel_processed(unsigned int offset = 0) { return &(*(*a_pixels)[_get_current_x_processing(offset)])[_get_current_y_processing(offset)]; };
		inline unsigned int _get_current_x_processing(unsigned int offset = 0) { return (a_processed_data - offset) % get_width(); };
		inline unsigned int _get_current_y_processing(unsigned int offset = 0) { return floor(((float)a_processed_data - (float)offset) / (float)get_width()); };
		inline unsigned int get_compression_method() { return a_compression_method; };
		inline char get_error_load() { return a_error_load; };
		inline unsigned int get_filter_method() { return a_filter_method; };
		inline unsigned int get_height() { return a_height; };
		inline unsigned int get_interlace_method() { return a_interlace_method; };
		inline std::string get_path() { return a_path; };
		inline unsigned int get_physical_height_ratio() { return a_physical_height_ratio; };
		inline unsigned int get_physical_unit() { return a_physical_unit; };
		inline unsigned int get_physical_width_ratio() { return a_physical_width_ratio; };
		inline unsigned int get_width() { return a_width; };
		inline bool is_flipped_x() { return a_flipped_x; };
		inline bool is_flipped_y() { return a_flipped_y; };
		inline unsigned bool is_loadable() { return a_loadable; };
	private:
		// Bit depth of the image
		unsigned int a_bit_depth = 0;
		// Size of a chunk to decode an image
		unsigned int a_chunk_size = pow(2, 24) - 1;
		// Color type of the image
		unsigned int a_color_type = 0;
		// Compression method of the image
		unsigned int a_compression_method = 0;
		// Error during the loading (1 = normal)
		char a_error_load = 1;
		// Filter method of the image
		unsigned int a_filter_method = 0;
		// Filter type of the image
		unsigned char a_filter_type = 0;
		// If the image is flipped by the X axis
		bool a_flipped_x = false;
		// If the image is flipped by the Y axis
		bool a_flipped_y = false;
		// Height of the image
		unsigned int a_height = 0;
		// Interlace method of the image
		unsigned int a_interlace_method = 0;
		// Last processed pixels during the process
		std::vector<PNG_Pixel>* a_last_processed_line_pixel = 0;
		// If the image can be loaded or not
		bool a_loadable = true;
		// Path of the image
		std::string a_path = "";
		// Physical height of the image
		unsigned int a_physical_height_ratio = 0;
		// Physical unit of the image (0 = unknow, 1 = meter)
		unsigned int a_physical_unit = 0;
		// Physical width of the image
		unsigned int a_physical_width_ratio = 0;
		// Pixel of the image
		std::vector<std::vector<PNG_Pixel>*>* a_pixels = 0;
		// Number of data processed
		unsigned int a_processed_data = -1;
		// Processed pixels during the process
		std::vector<PNG_Pixel*>* a_processed_pixel = 0;
		// Starting position of the last processing
		unsigned int a_processing_last_position = 0;
		// Value of the sRGB chunk
		unsigned char a_srgb_value = 0;
		// Width of the image
		unsigned int a_width = 0;
	};
}