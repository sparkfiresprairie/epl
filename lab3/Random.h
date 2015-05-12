#include <random>

#if defined (_MSC_VER)
namespace epl {
extern std::default_random_engine random_generator;
extern std::function<double(void)> drand48;
}
#endif
