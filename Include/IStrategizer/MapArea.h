#ifndef MAPAREA_H
#define MAPAREA_H

#include "SharedResource.h"
#include "Vector2.h"

namespace IStrategizer
{
	class MapArea :  public SharedResource
	{
	public:
		MapArea() : m_pos(Vector2::Null()), m_width(-1), m_height(-1) {}
		MapArea(Vector2 p_pos, int p_width, int p_height) :
		  m_pos(p_pos), m_width(p_width), m_height(p_height) {}

		Vector2 Pos() const { return m_pos; }
		int Width() const { return m_width; }
		int Height() const { return m_height; }
		bool IsNull();
		static MapArea& Null();

	protected:
		bool Acquire();
		bool Release();

	private:
		Vector2 m_pos;
		int		m_width;
		int		m_height;
	};
}
#endif // MAPAREA_H