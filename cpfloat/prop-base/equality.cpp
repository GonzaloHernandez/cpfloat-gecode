#include <cpfloat/prop-base/equality.hh>

namespace MPG {
  using namespace CPFloat;
  using namespace CPFloat::Prop;
  void equality(Gecode::Space& home, CPFloatVar x, CPFloatVar y) {
    if (home.failed()) return;

    CPFloatView left(x);
    CPFloatView right(y);
    GECODE_ES_FAIL((Equality<CPFloatView,CPFloatView>::post(home,left,right)));
  }
}
