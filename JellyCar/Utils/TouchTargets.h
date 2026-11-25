#ifndef TouchTargets_H
#define TouchTargets_H

#include <glm/glm.hpp>
#include <vector>

// Areas of the screen that react to a touch. A state clears and adds them while it
// draws, and looks up the touched one while it handles input.
class TouchTargets
{
private:

	struct Target
	{
		int id;
		int x, y, w, h;
	};

	std::vector<Target> _targets;

public:

	static const int None = -1;

	void Clear()
	{
		_targets.clear();
	}

	void Add(int id, int x, int y, int w, int h)
	{
		Target target = { id, x, y, w, h };
		_targets.push_back(target);
	}

	// The last added target is on top
	int Find(glm::vec2 point)
	{
		for (size_t i = _targets.size(); i-- > 0;)
		{
			const Target& t = _targets[i];
			if (point.x >= t.x && point.x < t.x + t.w && point.y >= t.y && point.y < t.y + t.h)
				return t.id;
		}

		return None;
	}
};

#endif
