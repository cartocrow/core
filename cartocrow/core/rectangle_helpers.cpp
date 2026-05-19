/*
Copyright (C) 2026  TU Eindhoven

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "rectangle_helpers.h"

namespace cartocrow {
Corner opposite(Corner corner) {
	return static_cast<Corner>((corner + 2) % 4);
}

bool is_horizontal(Side side) {
	return side % 2 == 1;
}

Corner mirror_corner(Corner corner, bool vertical) {
	Corner mirrored;
	switch(corner) {
	case BL:
		mirrored = vertical ? TL : BR;
		break;
	case BR:
		mirrored = vertical ? TR : BL;
		break;
	case TR:
		mirrored = vertical ? BR : TL;
		break;
	case TL:
		mirrored = vertical ? BL : TR;
		break;
	}
	return mirrored;
}

Side next_side(const Side& side) {
	return static_cast<Side>((side + 1) % 4);
}
}