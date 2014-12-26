#define STB_IMAGE_IMPLEMENTATION

#include "texture2d.hpp"
#include <string>

#ifdef TEXTURE2D_FROMFILE_STBI
#include <stb_image.h>
#endif

namespace gl
{
	Texture2D::Texture2D(std::uint32_t width, std::uint32_t height, TextureFormat format, std::int32_t numMipLevels, std::uint32_t numMSAASamples) :
		Texture(width, height, 1, format, numMipLevels, numMSAASamples)
	{
		GL_CALL(glCreateTextures, GL_TEXTURE_2D, 1, &m_textureHandle);
		if(m_numMSAASamples == 0)
			GL_CALL(glTextureStorage2D, m_textureHandle, m_numMipLevels, gl::TextureFormatToGLSizedInternal[static_cast<unsigned int>(format)], m_width, m_height);
		else
			GL_CALL(glTextureStorage2DMultisample, m_textureHandle, m_numMSAASamples, gl::TextureFormatToGLSizedInternal[static_cast<unsigned int>(format)], m_width, m_height, GL_FALSE);
	}

	Texture2D::Texture2D(std::uint32_t _width, std::uint32_t _height, TextureFormat _format, const void* _data, TextureSetDataFormat _dataFormat, TextureSetDataType _dataType, std::uint32_t _numMSAASamples) :
		Texture2D(_width, _height, _format, 1, _numMSAASamples)
	{
		GL_CALL(glTextureSubImage2D, m_textureHandle, 0, 0, 0, m_width, m_height,
				static_cast<GLenum>(_dataFormat), static_cast<GLenum>(_dataType), _data);
	}

#ifdef TEXTURE2D_FROMFILE
	std::unique_ptr<Texture2D> Texture2D::LoadFromFile(const std::string& _filename, bool _generateMipMaps, bool _sRGB)
	{
		int texSizeX = -1;
		int texSizeY = -1;

		int numComps = -1;
		stbi_uc* textureData = stbi_load(_filename.c_str(), &texSizeX, &texSizeY, &numComps, 4);
		if (!textureData)
		{
			GLHELPER_LOG_ERROR("Error loading texture \"" + _filename + "\".");
			return nullptr;
		}

		std::unique_ptr<Texture2D> newTex(new Texture2D(static_cast<std::uint32_t>(texSizeX), static_cast<std::uint32_t>(texSizeY), _sRGB ? gl::TextureFormat::SRGB8_ALPHA8 : gl::TextureFormat::RGB8, _generateMipMaps ? -1 : 1));
		newTex->SetData(0, TextureSetDataFormat::RGBA, TextureSetDataType::UNSIGNED_BYTE, textureData);
		if (_generateMipMaps)
			newTex->GenMipMaps();

		stbi_image_free(textureData);

		return std::move(newTex);
	}
#endif

	void Texture2D::SetData(std::uint32_t _mipLevel, TextureSetDataFormat _dataFormat, TextureSetDataType _dataType, const void* _data)
	{
		GLHELPER_ASSERT(_mipLevel < m_numMipLevels, "MipLevel " + std::to_string(_mipLevel) + " does not exist, texture has only " + std::to_string(m_numMipLevels) + " MipMapLevels");

		GL_CALL(glTextureSubImage2D, m_textureHandle, _mipLevel, 0, 0, m_width, m_height,
			static_cast<GLenum>(_dataFormat), static_cast<GLenum>(_dataType), _data);
	}

	void Texture2D::GenMipMaps()
	{
		GL_CALL(glGenerateTextureMipmap, m_textureHandle);
	}
}