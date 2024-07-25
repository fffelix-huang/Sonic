#include <algorithm>
#include <iostream>
#include <vector>

#include "bench/perft.h"
#include "chess/all.h"
#include "utils/bits.h"
#include "utils/random.h"
#include "utils/small_vector.h"
#include "utils/strings.h"
#include "uci.h"
#include "search.h"

int main(int argc, char* argv[]) {
	using namespace std;
	using namespace sonic;
	init_attacks();
	uci_loop();
	return 0;
}
