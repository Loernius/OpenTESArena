#ifndef RENDER_TEXTURE_UTILS_H
#define RENDER_TEXTURE_UTILS_H

// Common texture handles allocated by a renderer for a user when they want a new texture in the
// internal renderer format.

using ObjectTextureID = int; // For all scene geometry (voxels/entities/sky/particles).
using UiTextureID = int; // Used with all UI textures.

class Renderer;

class ScopedObjectTextureRef
{
private:
	ObjectTextureID id;
	Renderer *renderer;
	int width, height;

	void setDims();
public:
	ScopedObjectTextureRef(ObjectTextureID id, Renderer &renderer);
	ScopedObjectTextureRef();
	ScopedObjectTextureRef(ScopedObjectTextureRef &&other);
	~ScopedObjectTextureRef();

	ScopedObjectTextureRef &operator=(ScopedObjectTextureRef &&other);

	void init(ObjectTextureID id, Renderer &renderer);

	ObjectTextureID get() const;
	int getWidth() const;
	int getHeight() const;

	// Texture updating functions. The returned pointer allows for changing any texels in the texture.
	uint32_t *lockTexels();
	void unlockTexels();
};

class ScopedUiTextureRef
{
private:
	UiTextureID id;
	Renderer *renderer;
	int width, height;

	void setDims();
public:
	ScopedUiTextureRef(UiTextureID id, Renderer &renderer);
	ScopedUiTextureRef();
	ScopedUiTextureRef(ScopedUiTextureRef &&other);
	~ScopedUiTextureRef();

	ScopedUiTextureRef &operator=(ScopedUiTextureRef &&other);

	void init(UiTextureID id, Renderer &renderer);

	UiTextureID get() const;
	int getWidth() const;
	int getHeight() const;

	// Texture updating functions. The returned pointer allows for changing any texels in the texture.
	uint32_t *lockTexels();
	void unlockTexels();
};

#endif
