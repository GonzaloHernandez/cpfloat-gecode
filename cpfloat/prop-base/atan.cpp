#include <cpfloat/prop-base/atan.hh>

namespace MPG {
  using namespace CPFloat;
  using namespace CPFloat::Prop;
  void atan(Gecode::Space& home, CPFloatVar x, CPFloatVar y) {
    if (home.failed()) return;
    
    CPFloatView left(x);
    CPFloatView right(y);
    GECODE_ES_FAIL((Atan<CPFloatView,CPFloatView>::post(home,left,right)));
  }  
}
