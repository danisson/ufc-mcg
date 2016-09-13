#include "model.h"
#include <cstdio>

int main(int argc, char const *argv[]) {
	FILE* f = stdin;
	tnw::octree::Tree t = tnw::octree::make_from_file(f);
	printf("%s\n", t.serialize().c_str());
	return 0;
}
