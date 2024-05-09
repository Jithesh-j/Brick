
#ifndef GLAZY_TEXTURE
#define GLAZY_TEXTURE

#include "glazy_common.h"



namespace glazy {

	class Texture {
		
	public:
		struct RGBA8 {
			unsigned char r;
			unsigned char g;
			unsigned char b;
			unsigned char a;
		};

	private:

		GLuint id;

		void construct(RGBA8 *data, size_t width, size_t height, bool mipmap);

	public:


		Texture(std::string file_path, bool mipmap);
		Texture(std::vector<RGBA8> &data, size_t width, size_t height, bool mipmap);
		Texture(Texture&) = delete;
		~Texture();
		operator GLuint();

	};

	class Sampler {

		GLuint id;

	public:

		Sampler();
		Sampler(Sampler&) = delete;
		~Sampler();

	};


}


#endif

