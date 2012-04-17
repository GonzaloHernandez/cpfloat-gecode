#include <cpfloat/prop-advanced/hc4.hh>

namespace MPG {
using namespace CPFloat;
using namespace CPFloat::Prop;
void hc4(Gecode::Space& home, Constraint& cst) {
  if (home.failed()) return;
  std::cout << " *** Posting hc4 constraint *** ";
  cst.print();
  std::cout << std::endl;
  GECODE_ES_FAIL((HC4::post(home,cst)));
}
}
