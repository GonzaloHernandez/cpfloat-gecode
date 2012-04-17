#include <cpfloat/prop-base/power.hh>

namespace MPG {
  using namespace CPFloat;
  using namespace CPFloat::Prop;
  void power(Gecode::Space& home, CPFloatVar x, int e, CPFloatVar y) {
    if (home.failed()) return;
    
    CPFloatView left(x);
    CPFloatView right(y);
    GECODE_ES_FAIL((Power<CPFloatView,CPFloatView>::post(home,left,e,right)));
  }  
}
