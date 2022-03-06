#include <cpfloat/prop/k3b.hh>

namespace MPG {
using namespace CPFloat;
using namespace CPFloat::Prop;
void k3b(Gecode::Space& home, Constraint& cst) {
  if (home.failed()) return;
  std::cout << " *** Posting k3b constraint *** ";
  cst.print();
  std::cout << std::endl;
  GECODE_ES_FAIL((K3B::post(home,cst)));
}
}
